////////////////////////////////////////////////////////////////////////////


#include "StdString.h"

class CStdStringArray 
{
public:

// Construction
	CStdStringArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	CStdString GetAt(int nIndex) const;
	void SetAt(int nIndex, LPCTSTR newElement);

	void SetAt(int nIndex, const CStdString& newElement);

	CStdString& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const CStdString* GetData() const;
	CStdString* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, LPCTSTR newElement);

	void SetAtGrow(int nIndex, const CStdString& newElement);

	int Add(LPCTSTR newElement);

	int Add(const CStdString& newElement);

	int Append(const CStdStringArray& src);
	void Copy(const CStdStringArray& src);

	// overloaded operator helpers
	CStdString operator[](int nIndex) const;
	CStdString& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, LPCTSTR newElement, int nCount = 1);

	void InsertAt(int nIndex, const CStdString& newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CStdStringArray* pNewArray);

// Implementation
protected:
	CStdString* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

	void InsertEmpty(int nIndex, int nCount);


public:
	~CStdStringArray();

#ifdef _DEBUG
	void AssertValid() const;
#endif

protected:
	// local typedefs for class templates
	typedef CStdString BASE_TYPE;
	typedef LPCTSTR BASE_ARG_TYPE;
};




inline int CStdStringArray::GetSize() const
	{ return m_nSize; }
inline int CStdStringArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CStdStringArray::RemoveAll()
	{ SetSize(0); }
inline CStdString CStdStringArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CStdStringArray::SetAt(int nIndex, LPCTSTR newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline void CStdStringArray::SetAt(int nIndex, const CStdString& newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }

inline CStdString& CStdStringArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const CStdString* CStdStringArray::GetData() const
	{ return (const CStdString*)m_pData; }
inline CStdString* CStdStringArray::GetData()
	{ return (CStdString*)m_pData; }
inline int CStdStringArray::Add(LPCTSTR newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline int CStdStringArray::Add(const CStdString& newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }

inline CStdString CStdStringArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline CStdString& CStdStringArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


