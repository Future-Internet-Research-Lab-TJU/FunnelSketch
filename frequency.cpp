#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "./Funnelsketch.h"

using namespace std;

#define key_len 8
#define memTot 250 * 1024

#define mem_in_32bit 1 * 1024
#define mem_in_16bit 115 * 1024
#define bs_tot memTot

vector<string> packets;
unordered_map<string, int> freq;

void fileReader(const char* filename)
{
	packets.clear();
	freq.clear();
	ifstream fin(filename, ios::binary);
	ios::sync_with_stdio(false);																							

	char key[13];
	while(1)
	{
		fin.read(key, 13);
		if (fin.gcount() < 13) break;
		string str0(key, 8);
		packets.push_back(str0);
		string str1(key, key_len);
		freq[str1]++;
	}
	fin.close();

	cout << freq.size() << " flows, " << packets.size() << " packets read" << endl;
}


void test_funnel(int mem_16bit, int mem_32bit, int mem_tot)
{
	int w_u = MEMORY_PROP * memTot / 0.5 / 3;
    int w = ((1 - MEMORY_PROP) * memTot - 0.5 * 1024) / (8 + 2 + COUNTER_PER_BUCKET_8BIT + COUNTER_PER_BUCKET_16BIT * 2) / 3;
	
	FunnelSketch fs(100, w, w_u);

	for (auto key : packets)
		fs.insert(key, key_len, 1);

	double ARE = 0, AAE = 0;

	for (auto pr : freq)
	{
		int est_val = fs.query(pr.first, key_len);
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}

	ARE /= freq.size(), AAE /= freq.size();
	cout << "funnel\t" << "ARE：" << ARE << "\t" <<"AAE：" <<AAE << endl;	
}



int main()
{
	fstream input("interest_trace.txt");
	string filename;
	while(getline(input, filename))
	{
		// cout << "\n数据集" << filename << "的frequency测试：" << endl;
		fileReader(filename.c_str());
		test_funnel(mem_in_16bit, mem_in_32bit, bs_tot);
	}

	cout << "\nSuccessfully exit!" << endl;
	
	return 0;
}
