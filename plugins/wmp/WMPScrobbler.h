// WMPScrobbler.h: interface for the CWMPScrobbler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMPSCROBBLER_H__9E9C7CB2_8718_4C0C_8BA3_E5F7D55D5118__INCLUDED_)
#define AFX_WMPSCROBBLER_H__9E9C7CB2_8718_4C0C_8BA3_E5F7D55D5118__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Scrobbler.h"

class CWMPScrobbler : public CScrobbler  
{
public:
	CWMPScrobbler();
	virtual ~CWMPScrobbler();

};

#endif // !defined(AFX_WMPSCROBBLER_H__9E9C7CB2_8718_4C0C_8BA3_E5F7D55D5118__INCLUDED_)
