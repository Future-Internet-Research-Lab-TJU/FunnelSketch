#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <string>
#include "./Funnelsketch.h"
using namespace std;
using namespace std::chrono;

#define REP_TIME 100
#define thCnt 0.03 / 100
#define key_len 8
#define memTot 250 * 1024

#define mem_in_32bit 1 * 1024
#define mem_in_16bit 115 * 1024
#define bs_tot memTot



vector<string> packets;
unordered_map<string, int> freq;
vector<string> hhitter_key;
vector<uint32_t> hhitter_size;
vector<uint32_t> int_hitter_key;

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

// 判断某个元素是否存在vector中
bool search(vector<string> insVector, string target)
{
    auto it = find(insVector.begin(), insVector.end(), target);
    if (it != insVector.end())
        return true;
    return false;
}

void test_funnel(int mem_16bit, int mem_32bit, int mem_tot)
{
    int w_u = MEMORY_PROP * memTot / 0.5 / 3;
    int w = ((1 - MEMORY_PROP) * memTot - 0.5 * 1024) / (8 + 2 + COUNTER_PER_BUCKET_8BIT + COUNTER_PER_BUCKET_16BIT * 2) / 3;
    FunnelSketch fs(100, w, w_u);

    vector<string> fs_res;
    double time;
    for (auto key : packets)
        fs.insert(key, key_len, 1);

    fs_res.clear();

    steady_clock::time_point t1 = steady_clock::now();
    for (auto pr : freq)
    {
        int est_val = fs.query(pr.first, key_len);
        if (est_val >= thCnt * packets.size())
            fs_res.push_back(pr.first);
    }

    steady_clock::time_point t2 = steady_clock::now();
    duration<double> time_span1 = duration_cast<duration<double> >(t2 - t1);
    time = time_span1.count();
    double Precision = 0, Recall = 0, F1_Score = 0, ARE = 0;
    double bs_hh_num = 0, std_hh_num = 0;
    for (int i = 0; i < hhitter_key.size(); i++)
    {
        int est_val = fs.query(hhitter_key[i], key_len);
        int real_val = hhitter_size[i];
        ARE += (double)abs(est_val - real_val) / real_val;
    }
    ARE /= hhitter_key.size();
    for (auto hkey : fs_res)
    {
        bool judge_hh = search(hhitter_key, hkey);
        if (judge_hh)
            bs_hh_num++;
    }
    Precision = bs_hh_num / fs_res.size();
    for (auto hkey : hhitter_key)
    {
        bool judge_hh = search(fs_res, hkey);
        if (judge_hh)
            std_hh_num++;
    }
    Recall = std_hh_num / hhitter_key.size();
    F1_Score = (2 * Precision * Recall) / (Precision + Recall);
    cout << "funnel\t"
         << "Precision: " << Precision << "\tRecall: " << Recall << "\tF1-Score: " << F1_Score << "\tARE: " << ARE << "\tdelay: " << time * 1000 << "ms" << endl;
}


int main()
{
    fstream input("interest_trace.txt");
    string filename;
    BOBHash32 *bobhash;
    int H, FP;
    while (getline(input, filename))
    {
        fileReader(filename.c_str());
        hhitter_key.clear();
        hhitter_size.clear();
        for (auto pr : freq)
        {
            if (pr.second >= thCnt * packets.size())
            {
                H = bobhash->run(pr.first.c_str(), pr.first.size());
                FP = (H >> 16);

                hhitter_key.push_back(pr.first); 
                hhitter_size.push_back(pr.second);
            }
        }

        cout << "nums of heavy hitters is：" << hhitter_key.size() << endl;
        test_funnel(mem_in_16bit, mem_in_32bit, bs_tot);
    }
}
