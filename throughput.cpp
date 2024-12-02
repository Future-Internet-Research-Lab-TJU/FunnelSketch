#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "Funnelsketch.h"
using namespace std;
using namespace std::chrono;

const int WARM_UP_TIME = 20;
const int RUN_TIME = 20;

#define key_len 8
#define memTot 500 * 1024
#define mem_in_32bit 1 * 1024
#define mem_in_16bit 115 * 1024


vector<string> packets;
unordered_map<string, int> freq;

void fileReader(const char *filename)
{
	packets.clear();
	freq.clear();
	ifstream fin(filename, ios::binary);
	ios::sync_with_stdio(false);

	char key[13];
	while (1)
	{
		fin.read(key, 13);
		if (fin.gcount() < 13)
			break;
		string str0(key, 8);
		packets.push_back(str0);
		string str1(key, key_len);
		freq[str1]++;
	}
	fin.close();

	cout << freq.size() << " flows, " << packets.size() << " packets read" << endl;
}

void test_funnel(double mem_16bit, double mem_32bit, double mem_tot, double& insert_time, double& query_time)
{
	int w_u = MEMORY_PROP * memTot / 0.5 / 3;
    int w = ((1 - MEMORY_PROP) * memTot - 0.5 * 1024) / (8 + 2 + COUNTER_PER_BUCKET_8BIT + COUNTER_PER_BUCKET_16BIT * 2) / 3;
	FunnelSketch fs(100, w, w_u);

	steady_clock::time_point t1 = steady_clock::now();
	for (auto key : packets)
		fs.insert(key, key_len, 1);
	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
	insert_time = time_span1.count();

	steady_clock::time_point t3 = steady_clock::now();
	for (auto pr : freq)
		int est_val = fs.query(pr.first, key_len);
	steady_clock::time_point t4 = steady_clock::now();
	duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
	query_time = time_span2.count();
	
}

void test_fs(double memory_in_16bit, double memory_in_32bit, double mem_tot)
{
	double ave_insert_time = 0, ave_query_time = 0;
	for (int i = 0; i < WARM_UP_TIME + RUN_TIME; ++i)
	{
		double insert_time, query_time;
		test_funnel(memory_in_16bit, memory_in_32bit, mem_tot, insert_time, query_time);
		if (i >= WARM_UP_TIME)
			ave_insert_time += insert_time, ave_query_time += query_time;
	}
	ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
	double insert_throughput = packets.size() / ave_insert_time / 1e6;
	double query_throughput = freq.size() / ave_query_time / 1e6;
	cout << "funnel\t" << insert_throughput << "\t" << query_throughput << "" << endl;
}

int main()
{
	fstream input("interest_trace.txt");
	string filename;
	while (getline(input, filename))
	{
		fileReader(filename.c_str());

		std::cout << "sketch\tinsert\t\tquery" << endl;
		test_fs(mem_in_16bit, mem_in_32bit, memTot);
	}
	std::cout << "\nSuccessfully exit!" << endl;
	return 0;
}
