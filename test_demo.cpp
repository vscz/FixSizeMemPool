// 
// @desc: a demo to display how to use fix size mempool
// @author: songchongzhan@qq.com
// @date: 2012-11-14
// 

#include "src/MemMgr.h"

#include <iostream>
#include <list>

#include <unistd.h>

using namespace std;

MemMgr* g_pMemMgr = NULL;

int apply(list<MemUnitBase*>& listMemUnit, const int nApplyNums);
int display_returnback(list<MemUnitBase*>& listMemUnit);

int g_nTotalApplyCnt = 0;

int main()
{
	g_pMemMgr = new MemMgr;
	
	const int nApplyNums = 3000;

	list<MemUnitBase*> listMemUnit;

	int i = 0;
	for (; i < 1000; i++)
	{
		apply(listMemUnit, nApplyNums);
		display_returnback(listMemUnit);
		
		listMemUnit.clear();
		//sleep(1);
	}
	
	cout << "Finish fix size mempool test, total apply: " << g_nTotalApplyCnt << endl;

	return 0;
}


// apply mem unit and set content
int apply(list<MemUnitBase*>& listMemUnit, const int nApplyNums)
{
	int i = 0;
	
	for (i = 0; i < nApplyNums; ++i)
	{
		char szBuf[4096] = {'\0'};
		snprintf(szBuf, sizeof(szBuf), "apply count:%d", i);
		int nLen = strlen(szBuf);

		MemUnitBase* pMemUnit = g_pMemMgr->GetMemUnit(nLen);

		if (pMemUnit != NULL)
		{
			++g_nTotalApplyCnt;
			pMemUnit->MemCpy(szBuf, nLen);
			//cout << "get mem unit, size:" << pMemUnit->nLen << endl;
			listMemUnit.push_back(pMemUnit);
		}
	}
}


// display mem content and return back mem unit to mempool
int display_returnback(list<MemUnitBase*>& listMemUnit)
{
	
	list<MemUnitBase*>::iterator it = listMemUnit.begin();
	for (; it != listMemUnit.end(); ++it)
	{
		//cout << "size: " << (*it)->nLen << " content: " << (*it)->GetMem() << endl;

		(*it)->ReturnBack();
	}
}

