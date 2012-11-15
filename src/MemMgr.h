// 
// 
// 
#ifndef _MEMMGR_H_
#define _MEMMGR_H_

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>

using std::map;
using std::vector;


// memory unit base class
class MemUnitBase
{
public:
	MemUnitBase();
	~MemUnitBase();

	bool Acquire()
	{
		// lock
		if (!bUsed)
		{
			bUsed = true;
			return true;
		}
		
		return false;
	}

	// implement by subclass
	virtual void MemCpy(char* pSrcBuff, int nSrcLen){}
	virtual void MemCpyTo(char* pDstcBuff, int nDstLen){}
	virtual char* GetMem(){}

	int ReturnBack()
	{
		nLen = 0;
		bUsed = false;

		return 0;
	}

public:
	int nLen;

private:
	bool bUsed;
};


// Fix size memory unit, 1KB
class MemUnit1KB : public MemUnitBase
{
public:
	virtual void MemCpy(char* pSrcBuff, int nSrcLen)
	{
		memcpy(szBuff, pSrcBuff, nSrcLen);
		nLen = nSrcLen;
	}

	virtual void MemCpyTo(char* pDstcBuff, int nDstLen)
	{
		if (nDstLen > nLen)
		{
			memcpy(pDstcBuff, szBuff, nLen);
		}
		else
		{
			memcpy(pDstcBuff, szBuff, nDstLen);
		}
	}

	virtual char* GetMem()
	{
		return szBuff;
	}

public:
	char szBuff[1024];
};


// Fix size memory unit, 2KB
class MemUnit2KB : public MemUnitBase
{
public:
	virtual void MemCpy(char* pSrcBuff, int nSrcLen)
	{
		memcpy(szBuff, pSrcBuff, nSrcLen);
		nLen = nSrcLen;
	}

	virtual void MemCpyTo(char* pDstcBuff, int nDstLen)
	{
		if (nDstLen > nLen)
		{
			memcpy(pDstcBuff, szBuff, nLen);
		}
		else
		{
			memcpy(pDstcBuff, szBuff, nDstLen);
		}
	}

	virtual char* GetMem()
	{
		return szBuff;
	}

public:
	char szBuff[2048];
};


// Fix size memory unit, 4KB
class MemUnit4KB : public MemUnitBase
{
public:
	virtual void MemCpy(char* pSrcBuff, int nSrcLen)
	{
		memcpy(szBuff, pSrcBuff, nSrcLen);
		nLen = nSrcLen;
	}

	virtual void MemCpyTo(char* pDstcBuff, int nDstLen)
	{
		if (nDstLen > nLen)
		{
			memcpy(pDstcBuff, szBuff, nLen);
		}
		else
		{
			memcpy(pDstcBuff, szBuff, nDstLen);
		}
	}

	virtual char* GetMem()
	{
		return szBuff;
	}

public:
	char szBuff[4096];
};


template <typename T>
class MemUnitArray
{
public:
	MemUnitArray();
	MemUnitArray(int mem_unit_size);
	~MemUnitArray();

	void Init();

	T* GetMemUnit();

	int GetMemUnitSize()
	{
		return m_mem_unit_size;
	}

private:
	const static int total_unit = 10240;		// memory unit sizes, can change at compile time

	int m_cur_avaliable_index;
	
	int m_mem_unit_size;

	T* m_arrUnit[total_unit];
};


class SimpleScopedLock
{
public:
	SimpleScopedLock();
	SimpleScopedLock(pthread_mutex_t& mutex)
	{
		m_pMutex = &mutex;
		pthread_mutex_lock(m_pMutex);
	}

	~SimpleScopedLock()
	{
		pthread_mutex_unlock(m_pMutex);
	}

private:
	pthread_mutex_t* m_pMutex;
	
};


class MemMgr
{
public:
	MemMgr(void);
	virtual ~MemMgr(void);

	void Init();

	MemUnitBase* GetMemUnit(int nBytes);

private:
	MemUnitBase* Get1KBMemUnit();
	MemUnitBase* Get2KBMemUnit();
	MemUnitBase* Get4KBMemUnit();
	MemUnitArray<MemUnit1KB>* AddNewMemUnit1KBArray();
	MemUnitArray<MemUnit2KB>* AddNewMemUnit2KBArray();
	MemUnitArray<MemUnit4KB>* AddNewMemUnit4KBArray();

private:
	const static int max_array_num = 100;
	vector<MemUnitArray<MemUnit1KB>*> m_vecMemUnit1KBArray;		// 动态存储1KB内存的数组
	vector<MemUnitArray<MemUnit2KB>*> m_vecMemUnit2KBArray;		// 动态存储2KB内存的数组
	vector<MemUnitArray<MemUnit4KB>*> m_vecMemUnit4KBArray;		// 动态存储4KB内存的数组

	pthread_mutex_t m_t1KBArrMutex;
	pthread_mutex_t m_t2KBArrMutex;
	pthread_mutex_t m_t4KBArrMutex;
};

#endif // _MEMMGR_H_
