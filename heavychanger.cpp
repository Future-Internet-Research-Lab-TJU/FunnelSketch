#include<iostream>
#include<fstream>
#include<set>
#include<unordered_map>
#include <stdexcept>
#include<vector>
#include<algorithm>
#include "./FunnelSketch.h"
using namespace std;

#define hc_percnt 0.03 / 100
#define key_len 8
#define memTot 250 * 1024

#define mem_in_32bit 1 * 1024
#define mem_in_16bit 115 * 1024
#define bs_tot memTot


vector<string> packets1;
vector<string> packets2;
unordered_map<string, int> freq1;
unordered_map<string, int> freq2;
vector<string> hc;
double th;


bool search(vector<string> insVector, string target) 
{
  auto it = find(insVector.begin(), insVector.end(), target);
  if(it != insVector.end())
    return true;
  return false ;
}
void fileReader(const char* filename, vector<string>& packets, unordered_map<string, int>& freq)
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
	FunnelSketch fs_1(100, w, w_u);
    FunnelSketch fs_2(100, w, w_u);

    for (auto key : packets1)
        fs_1.insert(key, key_len, 1);
    for (auto key : packets2)
        fs_2.insert(key, key_len, 1);
    double Precision = 0, Recall = 0, F1_Score = 0;
    set<string> fs_all_item;
    vector<string> fs_hc;
 
    for (auto pr : freq1)
	{
		int est_val = fs_1.query(pr.first, key_len);
		if (est_val >= 255)
		{
		 	fs_all_item.insert(pr.first);
		}
    }

        for (auto pr : freq2)
	{
		int est_val = fs_2.query(pr.first, key_len);
		if (est_val >= 255)
		{
		 	fs_all_item.insert(pr.first);
		}
    }

    for (auto it : fs_all_item)
    {
        int t1_size = fs_1.query(it, key_len);
        int t2_size = fs_2.query(it, key_len);
        uint32_t change = abs(t1_size - t2_size);
        if (change >= th) fs_hc.push_back(it);
    }
    double num1 = 0, num2 = 0;
    for (auto it : fs_hc)
    {
        bool judge = search(hc, it);
        if (judge) num1++;
    }
    Precision = num1 / fs_hc.size(); 
    for (auto it : hc)
    {
        bool judge = search(fs_hc, it);
        if (judge) num2++;
    }
    Recall = num2 / hc.size();
    F1_Score = 2 * Precision * Recall / (Precision + Recall);

  cout << "funnel\t" << "Precision：" << Precision << " Recall: " << Recall <<" F1-score: " << F1_Score << endl;
}

int main()
{
    fileReader("trace_data/1.dat", packets1, freq1);
    fileReader("trace_data/3.dat", packets2, freq2);
    set<string> all_item;
    for (auto pr : freq1)
        all_item.insert(pr.first);
    for (auto pr : freq2)
        all_item.insert(pr.first);
    uint32_t tot_change = 0;
    int t1_size;
    int t2_size;
    for (auto it : all_item)
    {
        try
        {
            auto tmp_num = freq1.at(it);
            t1_size = tmp_num;
        }
        catch(out_of_range err)
        {
            t1_size = 0;
        }

        try
        {
            auto tmp_num = freq2.at(it);
            t2_size = tmp_num;
        }
        catch(out_of_range err)
        {
            t2_size = 0;
        }
        tot_change += abs(t1_size - t2_size);
    }
    th = tot_change * hc_percnt;
    hc.clear();
    uint32_t tmp_change;
    for (auto it : all_item)
    {
        try
        {
            auto tmp_num = freq1.at(it);
            t1_size = tmp_num;
        }
        catch(out_of_range err)
        {
            t1_size = 0;
        }

        try
        {
            auto tmp_num = freq2.at(it);
            t2_size = tmp_num;
        }
        catch(out_of_range err)
        {
            t2_size = 0;
        }   
        tmp_change = abs(t1_size - t2_size);
        if (tmp_change >= th) hc.push_back(it);
    }
    test_funnel(mem_in_16bit, mem_in_32bit, bs_tot);
}