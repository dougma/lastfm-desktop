
#include "StdAfx.h"
#include "StdStringArray.h"

#ifdef _DEBUG
#if !defined(ASSERT_VALID)
#define ASSERT_VALID(x)									\
	ASSERT(!IsBadReadPtr(x, sizeof(CStdStringArray)));	\
	x->AssertValid();
#endif
#else
#define ASSERT_VALID(x)
#endif

template<class TYPE>
void ConstructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 ||
		!IsBadReadPtr(pElements, nCount * sizeof(TYPE)));

	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}



template<class TYPE>
void DestructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 ||
		!IsBadReadPtr(pElements, nCount * sizeof(TYPE)));

	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}



template<class TYPE>
void CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	ASSERT(nCount == 0 ||
		!IsBadReadPtr(pDest, nCount * sizeof(TYPE)));
	ASSERT(nCount == 0 ||
		!IsBadReadPtr(pSrc, nCount * sizeof(TYPE)));

	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}


/////////////////////////////////////////////////////////////////////////////

CStdStringArray::CStdStringArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CStdStringArray::~CStdStringArray()
{
	ASSERT_VALID(this);

	DestructElements(m_pData, m_nSize);
	delete[] (BYTE*)m_pData;
}

void CStdStringArray::SetSize(int nNewSize, int nGrowBy)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing

		DestructElements(m_pData, m_nSize);
		delete[] (BYTE*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(CStdString));    // no overflow
#endif
		m_pData = (CStdString*) new BYTE[nNewSize * sizeof(CStdString)];

		ConstructElements(m_pData, nNewSize);

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			ConstructElements(&m_pData[m_nSize], nNewSize-m_nSize);

		}

		else if (m_nSize > nNewSize)  // destroy the old elements
			DestructElements(&m_pData[nNewSize], m_nSize-nNewSize);

		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = min(1024, max(4, m_nSize / 8));
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(CStdString)); // no overflow
#endif
		CStdString* pNewData = (CStdString*) new BYTE[nNewMax * sizeof(CStdString)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(CStdString));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);

		ConstructElements(&pNewData[m_nSize], nNewSize-m_nSize);


		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

int CStdStringArray::Append(const CStdStringArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);

	CopyElements(m_pData + nOldSize, src.m_pData, src.m_nSize);

	return nOldSize;
}

void CStdStringArray::Copy(const CStdStringArray& src)
{
	ASSERT_VALID(this);
	ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);

	CopyElements(m_pData, src.m_pData, src.m_nSize);

}

void CStdStringArray::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(CStdString)); // no overflow
#endif
		CStdString* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (CStdString*) new BYTE[m_nSize * sizeof(CStdString)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(CStdString));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CStdStringArray::SetAtGrow(int nIndex, LPCTSTR newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}


void CStdStringArray::SetAtGrow(int nIndex, const CStdString& newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);
	m_pData[nIndex] = newElement;
}



void CStdStringArray::InsertEmpty(int nIndex, int nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(CStdString));

		// re-init slots we copied from

		ConstructElements(&m_pData[nIndex], nCount);

	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
}


void CStdStringArray::InsertAt(int nIndex, LPCTSTR newElement, int nCount)
{

	// make room for new elements
	InsertEmpty(nIndex, nCount);



	// copy elements into the empty space
	CStdString temp = newElement;
	while (nCount--)
		m_pData[nIndex++] = temp;

}


void CStdStringArray::InsertAt(int nIndex, const CStdString& newElement, int nCount)
{
	// make room for new elements
	InsertEmpty(nIndex, nCount);

	// copy elements into the empty space
	while (nCount--)
		m_pData[nIndex++] = newElement;
}


void CStdStringArray::RemoveAt(int nIndex, int nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	DestructElements(&m_pData[nIndex], nCount);

	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(CStdString));
	m_nSize -= nCount;
}

void CStdStringArray::InsertAt(int nStartIndex, CStdStringArray* pNewArray)
{
	ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}


/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CStdStringArray::AssertValid() const
{
	if (m_pData == NULL)
	{
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(!IsBadReadPtr(m_pData, m_nMaxSize * sizeof(CStdString)));
	}
}
#endif //_DEBUG
