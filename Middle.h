#ifndef _MIDDLE_H
#define _MIDDLE_H

#include <cstring>
#include <algorithm>
#include <math.h>
#include <iostream>
#include"BOBHash32.h"
#include"param.h"


using namespace std;
#define d_m 3

int idx;
int idx_sk[d_m];
int idx_ar[d_m];

typedef pair<string, uint16_t> KV_72; 

struct middle{
	KV_72 kv_72;
	uint8_t _8bit[COUNTER_PER_BUCKET_8BIT];
	uint16_t _16bit[COUNTER_PER_BUCKET_16BIT];
};

class Middle
{
protected:
	uint32_t w;
	middle* A[d_m];
	BOBHash32* bobhash[d_m];
	int row;
	int index;

public:
	Middle() {}
	Middle(uint32_t w_d) { init(w_d);}
	~Middle() { clear(); }

	void init(uint32_t w_d)
	{
		w = w_d;
		for (int i = 0; i < d_m; ++i)
		{
			A[i] = new middle[w_d];
			memset(A[i], 0, w_d * sizeof(middle));
			bobhash[i] = new BOBHash32(750 + i);
		}
	}

	void clear()
	{
		for (int i = 0; i < d_m; ++i)
		{
			delete[] A[i];
			if (bobhash[i]) delete bobhash[i];
		}
	}
	
	uint8_t insert(const string& key, uint16_t key_len, uint32_t f, uint32_t* hashval)
	{
		
		uint32_t min_m_1 = UINT32_MAX;

		
		for (int i = 0; i < d_m; ++i)
		{
			idx_sk[i] = hashval[i] % w;
			idx_ar[i] = hashval[i]  % COUNTER_PER_BUCKET_8BIT;
			min_m_1 = (A[i][idx_sk[i]]._8bit[idx_ar[i]] < min_m_1) ? A[i][idx_sk[i]]._8bit[idx_ar[i]] : min_m_1;
		}

		if (min_m_1 < 255)
		{
			for (int i = 0; i < d_m; ++i)
			{	
				A[i][idx_sk[i]]._8bit[idx_ar[i]] += (A[i][idx_sk[i]]._8bit[idx_ar[i]] == min_m_1) ? f : 0;	
			}

			return 0;
		}

		else
		{
			
			for (int i = 0; i < d_m; ++i)
			{
				if (A[i][idx_sk[i]].kv_72.second == 0)
				{ 
					A[i][idx_sk[i]].kv_72.first = key;
					A[i][idx_sk[i]].kv_72.second += f;

					return 0;
				}

				if (A[i][idx_sk[i]].kv_72.first == key)
				{	
					if (A[i][idx_sk[i]].kv_72.second < 65535)
					{
						A[i][idx_sk[i]].kv_72.second += f;

						return 0;
					}

					else
					{
						return 1;
					}		
				}				
			}

			
		   uint32_t min_m_2 = UINT32_MAX;	
			for(int i = 0; i < d_m; ++i)
			{
				idx_ar[i] = idx % COUNTER_PER_BUCKET_16BIT;
				min_m_2 = (A[i][idx_sk[i]]._16bit[idx_ar[i]] < min_m_2) ? A[i][idx_sk[i]]._16bit[idx_ar[i]] : min_m_2;
			}

			if (min_m_2 < 65535)
			{	
				for (int i = 0; i < d_m; ++i)
				{	
					A[i][idx_sk[i]]._16bit[idx_ar[i]] += (A[i][idx_sk[i]]._16bit[idx_ar[i]] == min_m_2) ? f : 0;	
				}

				uint32_t min_8bit = UINT32_MAX;

				for (int i = 0; i < d_m; ++i)
				{
					if (A[i][idx_sk[i]].kv_72.second < min_8bit)
					{
						min_8bit = A[i][idx_sk[i]].kv_72.second;
						index = idx_sk[i];
						row = i;
					}
				}

				if (min_m_2  + f > THRESHOLD_REPLACEMENT * min_8bit)
				{	
					string swap_key = A[row][index].kv_72.first;
					A[row][index].kv_72.first = key;
					A[row][index].kv_72.second += min_m_2;

					for (int i = 0; i < d_m; ++i)
					{	
						hashval[i] = bobhash[i]->run(swap_key.c_str(), key_len);
						idx_sk[i] = hashval[i] % w;
						idx_ar[i] = hashval[i]  % COUNTER_PER_BUCKET_16BIT;
						A[i][idx_sk[i]]._16bit[idx_ar[i]] += min_8bit;
					} 
				}
				return 0;

			}

			else
			{
				return 1;
			}

		}

	}	

	uint32_t query(const string& key, uint16_t key_len, uint32_t* hashval)
	{
		uint32_t val = UINT32_MAX;
		uint32_t min_m_2 = UINT32_MAX;

		for(int i = 0; i < d_m; ++i) 
		{
			idx_sk[i] = hashval[i] % w;
			idx_ar[i] = hashval[i]  % COUNTER_PER_BUCKET_8BIT;
			val = A[i][idx_sk[i]]._8bit[idx_ar[i]] < val ? A[i][idx_sk[i]]._8bit[idx_ar[i]] : val;
		}

		if (val == 255)
		{
			for (int i = 0; i < d_m; ++i)
			{
				if (A[i][idx_sk[i]].kv_72.first == key)
				{	
					if (A[i][idx_sk[i]].kv_72.second < 65535)
					{
						return A[i][idx_sk[i]].kv_72.second + 270;
					}

					else
					{
						return 65536;
					}
				}
			}

			for(int i = 0; i < d_m; ++i)
			{
				idx_ar[i] = idx % COUNTER_PER_BUCKET_16BIT;
				min_m_2 = (A[i][idx_sk[i]]._16bit[idx_ar[i]] < min_m_2) ? A[i][idx_sk[i]]._16bit[idx_ar[i]] : min_m_2;
			}

			if(min_m_2 < 65535)
			{   
				return min_m_2 + 270;
			}

			else
			{
				return 65536;
			}
			
		}

		else
		{
			return val + 15;
		}
	}
};
#endif