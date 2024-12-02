#ifndef _FUNNELSKETCH_H
#define _FUNNELSKETCH_H
#include<iostream>
#include <fstream>
#include "Bottom.h"
#include "Middle.h"
#include "Top.h"

using namespace std;

uint32_t val_m;
uint32_t val_t;
uint8_t judge_m; 
uint8_t judge_t; 
uint32_t hashval[3];

class FunnelSketch
{
public:
	HashTable_32* hashtable_32 = NULL;
	Middle* middle = NULL;
	Top* top = NULL;
	

    FunnelSketch(){};
	FunnelSketch(int _w_of_HT_32, int w_d, int w_u)
	{
		init(_w_of_HT_32, w_d, w_u);
	}
	~FunnelSketch(){clear();}
	void init(int _w_of_HT_32, int w_d, int w_u)
	{
		hashtable_32 = new HashTable_32(_w_of_HT_32);
		middle = new Middle(w_d);
		top = new Top(w_u);
	}
	void clear()
	{
		if (hashtable_32) delete hashtable_32;
		if (middle) delete middle;
		if (top) delete top;
	}

    void insert(const string& key, uint16_t key_len, uint32_t f)
	{
        judge_t = top->insert(key, key_len, f, hashval);
		
		if (judge_t == 0)
		{
			return;
		}
		
		else
		{
			val_m = middle->insert(key, key_len, f, hashval);

			if (judge_t == 0)
			{
				return;
			}

			else
			{
				hashtable_32->insert(hashval, f, judge_m - 1);
			}
		}

	}

	uint32_t query(const string& key, uint16_t key_len)
	{
		judge_t = top->query(key, key_len, hashval);

		if (val_t < 15)
		{
			
			return val_t;
		}

		else
		{
			val_m = middle->query(key, key_len, hashval);
		}

		if (val_m < 65535)
		{
			return val_m;
			
		}

		else
		{
			return hashtable_32->query(hashval, 0);
		}
	}

};
#endif