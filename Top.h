#ifndef _TOP_H
#define _TOP_H

#include <cstring>
#include <algorithm>
#include <math.h>
#include <iostream>
#include"BOBHash32.h"

using namespace std;
#define d_t 3

class Top
{
protected:
	uint32_t w;
	uint8_t* A[d_t];
	BOBHash32* bobhash[d_t];
	int idx_pre[d_t];
	
public:
	Top() {}
	Top(uint32_t w_d) { init(w_d); }
	~Top() { clear(); }

	void init(uint32_t w_d)
	{
		w = w_d;
		for (int i = 0; i < d_t; ++i)
		{
			A[i] = new uint8_t[w_d];
			memset(A[i], 0, w_d * sizeof(uint8_t));
			bobhash[i] = new BOBHash32(750 + i);
		}
	}

	void clear()
	{
		for (int i = 0; i < d_t; ++i)
		{
			delete[] A[i];
			if (bobhash[i]) delete bobhash[i];
		}

	}

	uint8_t insert(const string& key, uint16_t key_len, uint32_t f, uint32_t* hashval)
	{
		uint32_t min_t = UINT32_MAX;

		for (int i = 0; i < d_t; ++i)
		{
			hashval[i] = bobhash[i]->run(key.c_str(), key_len);
			idx_pre[i] = hashval[i] % w;
			min_t = (A[i][idx_pre[i]] < min_t) ? A[i][idx_pre[i]] : min_t;					
		}

		if (min_t == 15)
		{
			return 1;
		}

		else
		{
			for (int i = 0; i < d_t; ++i)
			{
				A[i][idx_pre[i]] += (A[i][idx_pre[i]] == min_t) ? f : 0;					
			}

			return 0;
		}
	}
	
	uint32_t query(const string& key, uint16_t key_len, uint32_t* hashval)
	{
		uint32_t val_t = UINT32_MAX;

		for (int i = 0; i < d_t; ++i)
		{
			hashval[i] = bobhash[i]->run(key.c_str(), key_len);
			idx_pre[i] = hashval[i] % w;
			val_t = (A[i][idx_pre[i]] < val_t) ? A[i][idx_pre[i]] : val_t;					
		}

		if (val_t == 15)
		{	
			return 16;
		}

		else
		{
			return val_t;
		}
	}
};
#endif