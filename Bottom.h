#ifndef _BOTTOM_H
#define _BOTTOM_H

#include<cstring>
#include "BOBHash32.h"
#include <algorithm>
using namespace std;

class HashTable_32
{
public:
	int w;
	uint32_t idx;
	uint32_t val;
    uint32_t* hashTable32;

    HashTable_32(){};
    HashTable_32(int _w)
    {
        init(_w);
    }
    ~HashTable_32()
    {
        clear();
    }

    void init(int _w)
    {
        w = _w;
        hashTable32 = new uint32_t[w];
		memset(hashTable32, 0, sizeof(uint32_t) * w);	
    }
    void clear()
    {
        if (hashTable32) delete[] hashTable32;
    }

	void insert(uint32_t* hashval, uint32_t f, int judge_cm) 
    {
        idx = hashval[judge_cm] % w;
		hashTable32[idx] += f; 
    }

	uint32_t query(uint32_t* hashval, int val_cm) 
    {
		idx = hashval[val_cm] % w;
		val = hashTable32[idx];
		return val + 65805;
    }

};

#endif
