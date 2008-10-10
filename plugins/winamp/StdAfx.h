// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>
#include <atlcom.h>
#include <atlhost.h>
#pragma warning( disable : 4192 ) 

#include "stdstring.h"
#include "dbg.h"
#include "ComDATE.h"