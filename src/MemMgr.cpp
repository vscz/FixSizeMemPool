// 
// @desc: a simple fix size mempool
// @author: songchongzhan@qq.com
// @date: 2012-11-14
// 
#include "MemMgr.h"

MemUnitBase::MemUnitBase() : nLen(0), bUsed(false)
{

}

MemUnitBase::~MemUnitBase()
{

}

template <typename T>
MemUnitArray<T>::MemUnitArray() 
: m_cur_avaliable_index(0)
{

}

template <typename T>
MemUnitArray<T>::MemUnitArray(int mem_unit_size) 
: m_mem_unit_size(mem_unit_size), m_cur_avaliable_index(0)
{

}


template <typename T>
void MemUnitArray<T>::Init()
{
	for (int i = 0; i < total_unit; ++i)
	{
		m_arrUnit[i] = new T;
	}
}

template <typename T>
T* MemUnitArray<T>::GetMemUnit()
{
	// no need lock, lock at MemMgr
	for (int i = m_cur_avaliable_index; i < total_unit; ++i)
	{
		if (m_arrUnit[i]->Acquire())
		{
			if (++m_cur_avaliable_index > total_unit - 1)
			{
				m_cur_avaliable_index = 0;
			}

			//printf("1.GetMemUnit from index:%d\n", i);
			return m_arrUnit[i];
		}
	}
	
	m_cur_avaliable_index = 0;
	for (int i = m_cur_avaliable_index; i < total_unit; ++i)
	{
		if (++m_cur_avaliable_index > total_unit - 1)
		{
			m_cur_avaliable_index = 0;
		}

		//printf("2.GetMemUnit from index:%d\n", i);
		return m_arrUnit[i];
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////
MemMgr::MemMgr(void)
{
	// recurisive lock
	pthread_mutexattr_t _attr;
	pthread_mutexattr_init(&_attr);
	pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&m_t1KBArrMutex, &_attr);
	pthread_mutex_init(&m_t2KBArrMutex, &_attr);
	pthread_mutex_init(&m_t4KBArrMutex, &_attr);

	pthread_mutexattr_destroy(&_attr);
}

MemMgr::~MemMgr(void)
{
	pthread_mutex_destroy(&m_t1KBArrMutex);
	pthread_mutex_destroy(&m_t2KBArrMutex);
	pthread_mutex_destroy(&m_t4KBArrMutex);
}

void MemMgr::Init()
{
	// Fist init a 1k mem unit array
	MemUnitArray<MemUnit1KB>* pMemUnit1KBArray = new MemUnitArray<MemUnit1KB>;
	pMemUnit1KBArray->Init();
	m_vecMemUnit1KBArray.push_back(pMemUnit1KBArray);

	// init a 2k£¬4k mem unit array
	//MemUnitArray<MemUnit2KB>* pMemUnit2KBArray = new MemUnitArray<MemUnit2KB>;
	//pMemUnit2KBArray->Init();
	//m_vecMemUnit2KBArray.push_back(pMemUnit2KBArray);

	//MemUnitArray<MemUnit4KB>* pMemUnit4KBArray = new MemUnitArray<MemUnit4KB>;
	//pMemUnit4KBArray->Init();
	//m_vecMemUnit4KBArray.push_back(pMemUnit4KBArray);
}


MemUnitBase* MemMgr::GetMemUnit(int nBytes)
{
	if (nBytes < 4096)
	{
		if (nBytes < 1024)
		{
			return Get1KBMemUnit();
		}
		else if (nBytes < 2048)
		{
			return Get2KBMemUnit();
		}
		else
		{
			return Get4KBMemUnit();
		}
	}

	return NULL;
}


MemUnitBase* MemMgr::Get1KBMemUnit()
{
	// locks
	SimpleScopedLock scopeLock(m_t1KBArrMutex);
	vector<MemUnitArray<MemUnit1KB>*>::iterator it = m_vecMemUnit1KBArray.begin();
	for (; it < m_vecMemUnit1KBArray.end(); ++it)
	{
		MemUnitBase* pMemUnit = (*it)->GetMemUnit();
		if (pMemUnit != NULL)
		{
			return pMemUnit;
		}
	}

	return AddNewMemUnit1KBArray()->GetMemUnit();
}

MemUnitBase* MemMgr::Get2KBMemUnit()
{
	// locks
	SimpleScopedLock scopeLock(m_t2KBArrMutex);
	vector<MemUnitArray<MemUnit2KB>*>::iterator it = m_vecMemUnit2KBArray.begin();
	for (; it < m_vecMemUnit2KBArray.end(); ++it)
	{
		MemUnitBase* pMemUnit = (*it)->GetMemUnit();
		if (pMemUnit != NULL)
		{
			return pMemUnit;
		}
	}

	return AddNewMemUnit2KBArray()->GetMemUnit();
}

MemUnitBase* MemMgr::Get4KBMemUnit()
{
	// locks
	SimpleScopedLock scopeLock(m_t4KBArrMutex);
	vector<MemUnitArray<MemUnit4KB>*>::iterator it = m_vecMemUnit4KBArray.begin();
	for (; it < m_vecMemUnit4KBArray.end(); ++it)
	{
		MemUnitBase* pMemUnit = (*it)->GetMemUnit();
		if (pMemUnit != NULL)
		{
			return pMemUnit;
		}
	}

	return AddNewMemUnit4KBArray()->GetMemUnit();
}


MemUnitArray<MemUnit1KB>* MemMgr::AddNewMemUnit1KBArray()
{
	printf("AddNewMemUnit1KBArray\n");
	MemUnitArray<MemUnit1KB>* pMemUnitArray = new MemUnitArray<MemUnit1KB>;
	pMemUnitArray->Init();
	m_vecMemUnit1KBArray.push_back(pMemUnitArray);

	return pMemUnitArray;
}

MemUnitArray<MemUnit2KB>* MemMgr::AddNewMemUnit2KBArray()
{
	printf("AddNewMemUnit2KBArray\n");
	MemUnitArray<MemUnit2KB>* pMemUnitArray = new MemUnitArray<MemUnit2KB>;
	pMemUnitArray->Init();
	m_vecMemUnit2KBArray.push_back(pMemUnitArray);

	return pMemUnitArray;
}


MemUnitArray<MemUnit4KB>* MemMgr::AddNewMemUnit4KBArray()
{
	printf("AddNewMemUnit4KBArray\n");
	MemUnitArray<MemUnit4KB>* pMemUnitArray = new MemUnitArray<MemUnit4KB>;
	pMemUnitArray->Init();
	m_vecMemUnit4KBArray.push_back(pMemUnitArray);

	return pMemUnitArray;
}

