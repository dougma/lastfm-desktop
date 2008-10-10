// comdate.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998-2000 Chris Sells
// All rights reserved.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// History:
//  9/27/01 - Dropped in operator SYSTEMTIME() and operator FILETIME() from
//            Scott Zionic [Scott.Zionic@petersweb.com]. Thanks, Scott.
//  4/9/01  - Implemented operator time_t() and operator struct tm() as per
//            Shawn Wildermuth's request.
//  2/8/01  - Fixed a rounding bug in CComSpan reported by Ralf Mohaupt
//            [r.mohaupt@lit.lineas.de].
//  11/4/00 - Moved everything to the header file for convenience.
//  4/13/00 - Sudhakar M [msudhakar@yahoo.com] contributed
//            CComDATE(const struct tm& tmSrc), from which I also extrapolated
//            CComDATE::operator=(const struct tm& tmSrc).
//  3/10/00 - Inspired by Eric White [EricW@DATA-TECH.COM] to add an
//            operator DBTIMESTAMP using his implementation. Also added
//            extra ctor, operator= and conversion operators for other date/time
//            formats, but haven't implemented all of them yet.
//            Inspired by Artyom Tyazhelov [artyom6@hotmail.com] to
//            leave current time alone when calling SetDate and leaving current
//            date alone when calling SetTime.
//  1/15/00 - Fixed a 2038 bug in Now. Thanks Michael Entin [entin@swusa.com].
//  7/14/99 - Fixed a linker problem w/ some inline functions release
//            builds. Thanks to David Maw [David.Maw@unisys.com] for
//            first pointing it out.
//   6/9/99 - Fixed a problem in Maketm where tm_isdst was not being set
//            (as is required by the CRT). Thanks to Jim Hoffman
//            [JHoffman@Enerx.com] for pointing it out.
//  4/18/99 - Fixed a problem in Format to provide a work-around in the
//            VC6 implementation of wcsftime. Thanks for Joe O'Leary for
//            pointing it out.
//            Even better, Joe also contributed FormatDate and FormatTime
//            to wrap the National Language Support functions GetTimeFormat()
//            and GetDateFormat(). Thanks, Joe!
//            Also, Juan Rivera (juanc@kodak.com) pointed out a missing operator=
//            declaration. Thanks, Juan.
// 12/24/98 - Based on a suggestion by Ronald Laeremans [RonaldL@mvps.org],
//            replaced all of the manual Maketm code w/ VariantToSystemTime.
//            Also updated MakeDATE to use SystemTimeToVariantTime. Thanks, Ron!
// 12/23/98 - Fixed a bug Ward pointed out in CComDATE::operator=(const time_t&).
// 12/21/98 - Ward Fuller <wfuller@tsisoft.com> pointed out a BSTR leak in
//            the Format implementation. It's plugged. Thanks, Ward!
// 12/21/98 - Initial release.
/////////////////////////////////////////////////////////////////////////////
// The file provides two class for use with dates and times under Windows.
//
// CComDATE: Wraps the COM DATE type, providing conversions from VARIANT,
//           DATE, ANSI string, UNICODE string, time_t, SYSTEMTIME, FILETIME,
//           DOS time and DBTIMESTAMP data types.
//
// CComSpan: Represents the result of subtracting one CComDATE from another.
//           Useful for time/date math.
//
// NOTE: This usage of these classes are based loosely on the MFC classes
//       COleDateTime and COleDateTimeSpan. However, the implementation is 99% mine.

#pragma once
#ifndef __COMDATE_H__
#define __COMDATE_H__

#include <time.h>
#include <limits.h>
#include <tchar.h>
#include <math.h>   // ceil, floor and modf
#include <winnls.h>
#include <OAIDL.H>
#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////
// CComDATE class

class CComSpan; // Forward declaration

class CComDATE
{
public:
    static CComDATE Now();
    static bool     IsValidDate(const DATE& dt);
    
    CComDATE();
    CComDATE(const CComDATE& dtSrc);
    CComDATE(const VARIANT& varSrc);
    CComDATE(DATE dtSrc);
    CComDATE(const char* pszSrc);
    CComDATE(const wchar_t* pszSrc);
    CComDATE(time_t timeSrc);
    CComDATE(const struct tm& tmSrc);
    CComDATE(const SYSTEMTIME& systimeSrc);
    CComDATE(const FILETIME& filetimeSrc);
    CComDATE(long nYear, long nMonth, long nDay, long nHour = 0, long nMin = 0, long nSec = 0);
    CComDATE(WORD wDosDate, WORD wDosTime);
    
#ifdef __oledb_h__
    CComDATE(const DBTIMESTAMP& dbts);
#endif
    
public:
    bool  IsValid() const;
    long  Year() const;
    long  Month() const;     // month of year (1 = Jan)
    long  Day() const;       // day of month (1-31)
    long  Hour() const;      // hour in day (0-23)
    long  Minute() const;    // minute in hour (0-59)
    long  Second() const;    // second in minute (0-59)
    long  DayOfWeek() const; // 1=Sun, 2=Mon, ..., 7=Sat
    long  DayOfYear() const; // days since start of year, Jan 1 = 1
    
    bool  IsLeapYear() const;   // Whether it's a leap year or not
    bool  IsNoon() const;       // Whether it's 12:00:00pm or not
    bool  IsMidnight() const;   // Whether it's 12:00:00am or not
    
    // Operations
public:
    const CComDATE& operator=(const CComDATE& dtSrc);
    const CComDATE& operator=(const VARIANT& varSrc);
    const CComDATE& operator=(DATE dtSrc);
    const CComDATE& operator=(const char* pszSrc);
    const CComDATE& operator=(const wchar_t* pszSrc);
    
    const CComDATE& operator=(const time_t& timeSrc);
    const CComDATE& operator=(const struct tm& tmSrc);
    const CComDATE& operator=(const SYSTEMTIME& systimeSrc);
    const CComDATE& operator=(const FILETIME& filetimeSrc);

#ifdef __oledb_h__
    const CComDATE& CComDATE::operator=(const DBTIMESTAMP& dbts);
#endif

    bool operator==(const CComDATE& date) const;
    bool operator!=(const CComDATE& date) const;
    bool operator<(const CComDATE& date) const;
    bool operator>(const CComDATE& date) const;
    bool operator<=(const CComDATE& date) const;
    bool operator>=(const CComDATE& date) const;
    
    // Date math
    const CComDATE& operator+=(const CComSpan& span);
    const CComDATE& operator-=(const CComSpan& span);
    
    friend CComDATE operator+(const CComDATE& date, const CComSpan& span);
    friend CComDATE operator-(const CComDATE& date, const CComSpan& span);
    friend CComDATE operator+(const CComSpan& span, const CComDATE& date);
    friend CComSpan operator-(const CComDATE& date1, const CComDATE& date2);
    
    operator DATE() const;
    DATE* operator&();

    operator VARIANT() const;
    operator time_t() const;
    operator struct tm() const;
    operator SYSTEMTIME() const;
    operator FILETIME() const;
#ifdef __oledb_h__
    operator DBTIMESTAMP() const;
#endif
    
    bool  SetDateTime(long nYear, long nMonth, long nDay, long nHour, long nMin, long nSec);
    bool  SetDate(long nYear, long nMonth, long nDay);
    bool  SetTime(long nHour, long nMin, long nSec);
    bool  ParseDateTime(const char* lpszDate, DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT);
    bool  ParseDateTime(const wchar_t* lpszDate, DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT);
    
    // Formatting
    LPTSTR Format(LPTSTR pszOut, DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT) const;
    LPTSTR Format(LPTSTR pszOut, LPCTSTR lpszFormat) const;
#ifdef __ATLCOM_H__
    LPTSTR Format(LPTSTR pszOut, UINT nFormatID, HINSTANCE hinst = _Module.GetResourceInstance()) const;
#else
    LPTSTR Format(LPTSTR pszOut, UINT nFormatID, HINSTANCE hinst = GetModuleHandle(0)) const;
#endif

	// Formatting using National Language Support functions GetTimeFormat() and GetDateFormat()
	LPTSTR	FormatTime(LPTSTR pszOut, LPCTSTR szFmt=NULL, DWORD dwFlags=0, LCID lcid=LOCALE_USER_DEFAULT) const;
	LPTSTR	FormatDate(LPTSTR pszOut, LPCTSTR szFmt=NULL, DWORD dwFlags=0, LCID lcid=LOCALE_USER_DEFAULT) const;
    
protected:
    enum
    {
        COMDATE_ERROR = INT_MIN,
        MIN_DATE      = -657434L,       // About year 100
        MAX_DATE      = 2958465L,       // About year 9999
        INVALID_DATE  = (MIN_DATE-1),   // Used to indicate invalid date
    };

protected:
    DATE  m_date;
    
protected:
    void Invalidate();
};

/////////////////////////////////////////////////////////////////////////////
// CComSpan class

class CComSpan
{
public:
    CComSpan();
    CComSpan(double dblSpanSrc);
    CComSpan(const CComSpan& dateSpanSrc);
    CComSpan(long nDays, long nHours = 0, long nMins = 0, long nSecs = 0);
    
    bool IsValid() const;
    
    double TotalDays() const;    // span in days (about -3.65e6 to 3.65e6)
    double TotalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
    double TotalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
    double TotalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)
    
    long Days() const;       // component days in span
    long Hours() const;      // component hours in span (-23 to 23)
    long Minutes() const;    // component minutes in span (-59 to 59)
    long Seconds() const;    // component seconds in span (-59 to 59)
    
public:
    const CComSpan& operator=(double dblSpanSrc);
    const CComSpan& operator=(const CComSpan& dateSpanSrc);
    
    bool operator==(const CComSpan& dateSpan) const;
    bool operator!=(const CComSpan& dateSpan) const;
    bool operator<(const CComSpan& dateSpan) const;
    bool operator>(const CComSpan& dateSpan) const;
    bool operator<=(const CComSpan& dateSpan) const;
    bool operator>=(const CComSpan& dateSpan) const;
    
    // Math
    const CComSpan& operator+=(const CComSpan& rhs);
    const CComSpan& operator-=(const CComSpan& rhs);
    CComSpan operator-() const;
    
    friend CComSpan operator+(const CComSpan& span1, const CComSpan& span2);
    friend CComSpan operator-(const CComSpan& span1, const CComSpan& span2);
    
    operator double() const;
    
    void SetSpan(long nDays, long nHours, long nMins, long nSecs);
    
protected:
    enum
    {
        MAX_SPAN      = 3615897L,
        MIN_SPAN      = -MAX_SPAN,
        INVALID_SPAN  = (MIN_SPAN-1),   // Used to indicate invalid span
    };

protected:
    double  m_span;

protected:
    void Invalidate();
};

/////////////////////////////////////////////////////////////////////////////
// CComDATE inline implementations

inline bool CComDATE::IsValidDate(const DATE& dt)
{
    // About year 100 to about 9999
    return (dt >= MIN_DATE && dt <= MAX_DATE);
}

inline CComDATE CComDATE::Now()
{
    SYSTEMTIME  sysTime;
    GetLocalTime(&sysTime);
    return CComDATE(sysTime);
}

inline CComDATE::CComDATE()
{ Invalidate(); }

inline CComDATE::CComDATE(const CComDATE& dtSrc)
{ m_date = dtSrc.m_date; }

inline CComDATE::CComDATE(const VARIANT& varSrc)
{ *this = varSrc; }

inline CComDATE::CComDATE(DATE dtSrc)
{ m_date = dtSrc; }

inline CComDATE::CComDATE(const char* pszSrc)
{ ParseDateTime(pszSrc); }

inline CComDATE::CComDATE(const wchar_t* pszSrc)
{ ParseDateTime(pszSrc); }

inline CComDATE::CComDATE(time_t timeSrc)
{ *this = timeSrc; }

inline CComDATE::CComDATE(const struct tm& tmSrc)
{ 
    SetDateTime(tmSrc.tm_year + 1900,
                tmSrc.tm_mon + 1,
                tmSrc.tm_mday,
                tmSrc.tm_hour,
                tmSrc.tm_min,
                tmSrc.tm_sec); 
}

inline CComDATE::CComDATE(const SYSTEMTIME& systimeSrc)
{ *this = systimeSrc; }

inline CComDATE::CComDATE(const FILETIME& filetimeSrc)
{ *this = filetimeSrc; }

inline CComDATE::CComDATE(long nYear, long nMonth, long nDay, long nHour, long nMin, long nSec)
{ SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec); }

inline CComDATE::CComDATE(WORD wDosDate, WORD wDosTime)
{ if( !DosDateTimeToVariantTime(wDosDate, wDosTime, &m_date) ) Invalidate(); }

inline const CComDATE& CComDATE::operator=(const CComDATE& dtSrc)
{ m_date = dtSrc.m_date; return *this; }

inline const CComDATE& CComDATE::operator=(const char* pszSrc)
{ ParseDateTime(pszSrc); return *this; }

inline const CComDATE& CComDATE::operator=(const wchar_t* pszSrc)
{ ParseDateTime(pszSrc); return *this; }

inline bool CComDATE::operator==(const CComDATE& date) const
{ return (m_date == date.m_date); }

inline bool CComDATE::operator!=(const CComDATE& date) const
{ return (m_date != date.m_date); }

inline CComDATE::operator DATE() const
{ return m_date; }

inline DATE* CComDATE::operator&()
{ return &m_date; }

inline bool CComDATE::SetDate(long nYear, long nMonth, long nDay)
{ return SetDateTime(nYear, nMonth, nDay, Hour(), Minute(), Second()); }

inline bool CComDATE::SetTime(long nHour, long nMin, long nSec)
{ return SetDateTime(Year(), Month(), Day(), nHour, nMin, nSec); }

inline const CComDATE& CComDATE::operator+=(const CComSpan& span)
{ *this = *this + span; return *this; }

inline const CComDATE& CComDATE::operator-=(const CComSpan& span)
{ *this = *this - span; return *this; }

inline bool CComDATE::IsValid() const
{ return IsValidDate(m_date); }

inline void CComDATE::Invalidate()
{ m_date = INVALID_DATE; }

#ifdef __oledb_h__
inline CComDATE::CComDATE(const DBTIMESTAMP& dbts)
{ SetDateTime(dbts.year, dbts.month, dbts.day, dbts.hour, dbts.minute, dbts.second); }

inline const CComDATE& CComDATE::operator=(const DBTIMESTAMP& dbts)
{ SetDateTime(dbts.year, dbts.month, dbts.day, dbts.hour, dbts.minute, dbts.second); return *this; }

#endif  // __oledb_h__

/////////////////////////////////////////////////////////////////////////////
// CComSpan inline implementations

inline CComSpan::CComSpan()
{ Invalidate(); }

inline CComSpan::CComSpan(double db) : m_span(db) {}

inline CComSpan::CComSpan(const CComSpan& rhs)
{ m_span = rhs.m_span; }

inline CComSpan::CComSpan(long nDays, long nHours, long nMins, long nSecs)
{ SetSpan(nDays, nHours, nMins, nSecs); }

inline const CComSpan& CComSpan::operator=(double db)
{ m_span = db; return *this; }

inline const CComSpan& CComSpan::operator=(const CComSpan& span)
{ m_span = span.m_span; return *this; }

inline double CComSpan::TotalDays() const
{ _ASSERTE(IsValid()); return m_span; }

inline double CComSpan::TotalHours() const
{ _ASSERTE(IsValid()); return m_span * 24; }

inline double CComSpan::TotalMinutes() const
{ _ASSERTE(IsValid()); return m_span * 24 * 60; }

inline double CComSpan::TotalSeconds() const
{ _ASSERTE(IsValid()); return m_span * 24 * 60 * 60; }

inline long CComSpan::Days() const
{ _ASSERTE(IsValid()); return (long)m_span; }

inline bool CComSpan::operator==(const CComSpan& rhs) const
{ return m_span == rhs.m_span; }

inline bool CComSpan::operator!=(const CComSpan& rhs) const
{ return m_span != rhs.m_span; }

inline bool CComSpan::operator<(const CComSpan& rhs) const
{ return m_span < rhs.m_span; }

inline bool CComSpan::operator>(const CComSpan& rhs) const
{ return m_span > rhs.m_span; }

inline bool CComSpan::operator<=(const CComSpan& rhs) const
{ return m_span <= rhs.m_span; }

inline bool CComSpan::operator>=(const CComSpan& rhs) const
{ return m_span >= rhs.m_span; }

inline const CComSpan& CComSpan::operator+=(const CComSpan& rhs)
{ return (*this = *this + rhs); }

inline const CComSpan& CComSpan::operator-=(const CComSpan& rhs)
{ return (*this = *this - rhs); }

inline CComSpan CComSpan::operator-() const
{ return CComSpan(-m_span); }

inline CComSpan::operator double() const
{ return m_span; }

inline bool CComSpan::IsValid() const
{ return (m_span >= MIN_SPAN && m_span <= MAX_SPAN); }

inline void CComSpan::Invalidate()
{ m_span = INVALID_SPAN; }

/////////////////////////////////////////////////////////////////////////////
// CComDATE class helper definitions

#define HALF_SECOND 1.0/172800.0    // Half a second, expressed in days

bool   MakeDATE(DATE* pdtDest, WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond);
bool   Maketm(struct tm* ptmDest, DATE dtSrc);
double DATEAsDouble(DATE dtSrc);
DATE   MakeDATE(double dbSrc);

/////////////////////////////////////////////////////////////////////////////
// CComDATE class

inline
long CComDATE::Year() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_year + 1900;
    return COMDATE_ERROR;
}

inline
long CComDATE::Month() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_mon + 1;
    return COMDATE_ERROR;
}

inline
long CComDATE::Day() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_mday;
    return COMDATE_ERROR;
}

inline
long CComDATE::Hour() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_hour;
    return COMDATE_ERROR;
}

inline
long CComDATE::Minute() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_min;
    return COMDATE_ERROR;
}

inline
long CComDATE::Second() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_sec;
    return COMDATE_ERROR;
}

inline
long CComDATE::DayOfWeek() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_wday + 1;
    return COMDATE_ERROR;
}

inline
long CComDATE::DayOfYear() const
{
    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) ) return tmDest.tm_yday + 1;
    return COMDATE_ERROR;
}

inline
bool CComDATE::IsLeapYear() const
{
    long year = Year();
    return ((year != COMDATE_ERROR) && ((year & 3) == 0) && ((year % 100) != 0 || (year % 400) == 0));
}

inline
bool CComDATE::IsNoon() const
{
    return Hour() == 12 && Minute() == 0 && Second() == 0;
}

inline
bool CComDATE::IsMidnight() const
{
    return Hour() == 0 && Minute() == 0 && Second() == 0;
}

inline
const CComDATE& CComDATE::operator=(const VARIANT& varSrc)
{
    if( varSrc.vt == VT_DATE )
    {
        m_date = varSrc.date;
    }
    else
    {
        VARIANT varDest = { 0 };
        if( SUCCEEDED(VariantChangeType(&varDest, const_cast<VARIANT*>(&varSrc), 0, VT_DATE)) )
        {
            m_date = varDest.date;
            VariantClear(&varDest);
        }
        else
        {
            Invalidate();
        }
    }
    
    return *this;
}

inline
const CComDATE& CComDATE::operator=(DATE dtSrc)
{
    m_date = dtSrc;
    return *this;
}

inline
const CComDATE& CComDATE::operator=(const time_t& tmSrc)
{
    // Convert time_t to struct tm
    tm *ptm = localtime(&tmSrc);
    
    if( !ptm ||
        !MakeDATE(&m_date,
                  (WORD)(ptm->tm_year + 1900),
                  (WORD)(ptm->tm_mon + 1),
                  (WORD)ptm->tm_mday,
                  (WORD)ptm->tm_hour,
                  (WORD)ptm->tm_min,
                  (WORD)ptm->tm_sec) )
    {
        // Local time must have failed (tmSrc before 1/1/70 12am)
        Invalidate();
    }
    
    return *this;
}

inline
const CComDATE& CComDATE::operator=(const struct tm& tmSrc)
{
    SetDateTime(tmSrc.tm_year + 1900,
                tmSrc.tm_mon + 1,
                tmSrc.tm_mday,
                tmSrc.tm_hour,
                tmSrc.tm_min,
                tmSrc.tm_sec); 

    return *this;
}

inline
const CComDATE& CComDATE::operator=(const SYSTEMTIME& systmSrc)
{
    if( !MakeDATE(&m_date,
                  systmSrc.wYear,
                  systmSrc.wMonth,
                  systmSrc.wDay,
                  systmSrc.wHour,
                  systmSrc.wMinute,
                  systmSrc.wSecond) )
    {
        Invalidate();
    }
    
    return *this;
}

inline
const CComDATE& CComDATE::operator=(const FILETIME& ftSrc)
{
    // Assume UTC FILETIME, so convert to LOCALTIME
    FILETIME ftLocal;
    if( !FileTimeToLocalFileTime( &ftSrc, &ftLocal) )
    {
#ifdef ATLTRACE
        ATLTRACE("\nFileTimeToLocalFileTime failed. Error = %lu.\n\t", GetLastError());
#endif
        Invalidate();
    }
    else
    {
        // Take advantage of SYSTEMTIME -> FILETIME conversion
        SYSTEMTIME  systime;
        
        // At this polong systime should always be valid, but...
        if( !FileTimeToSystemTime(&ftLocal, &systime) ||
            !MakeDATE(&m_date,
                      systime.wYear,
                      systime.wMonth,
                      systime.wDay,
                      systime.wHour,
                      systime.wMinute,
                      systime.wSecond) )
        {
            Invalidate();
        }
    }
    
    return *this;
}

inline
CComDATE::operator VARIANT() const
{
    VARIANT var = { 0 };
    var.vt = VT_DATE;
    var.date = m_date;
    return var;
}

inline
CComDATE::operator time_t() const
{
    struct  tm  tmDest;
    if( Maketm(&tmDest, m_date) )
    {
        return mktime(&tmDest);
    }

    return -1;
}

inline
CComDATE::operator struct tm() const
{
    struct  tm  tmDest = { 0 };
    Maketm(&tmDest, m_date);
    return tmDest;
}

inline
CComDATE::operator SYSTEMTIME() const
{
    SYSTEMTIME  st = { 0 };

    VariantTimeToSystemTime(m_date, &st);

    return st;
}

inline
CComDATE::operator FILETIME() const
{
    FILETIME    ft = { 0 };
	
    SYSTEMTIME st;
    VariantTimeToSystemTime(m_date, &st);
    SystemTimeToFileTime(&st, &ft);

    return ft;
}

#ifdef __oledb_h__
inline
CComDATE::operator DBTIMESTAMP() const
{
    DBTIMESTAMP dbts = { 0 };

    struct tm tmDest;
    if( IsValid() && Maketm(&tmDest, m_date) )
    {
        dbts.day    = tmDest.tm_mday;
        dbts.month  = tmDest.tm_mon + 1;
        dbts.year   = tmDest.tm_year + 1900;
        dbts.hour   = tmDest.tm_hour;
        dbts.minute = tmDest.tm_min;
        dbts.second = tmDest.tm_sec;
    }

    return dbts;
}
#endif

inline
bool CComDATE::operator<(const CComDATE& date) const
{
    _ASSERTE(IsValid());
    _ASSERTE(date.IsValid());
    
    // Handle negative dates
    return DATEAsDouble(m_date) < DATEAsDouble(date.m_date);
}

inline
bool CComDATE::operator>(const CComDATE& date) const
{
    _ASSERTE(IsValid());
    _ASSERTE(date.IsValid());
    
    // Handle negative dates
    return DATEAsDouble(m_date) > DATEAsDouble(date.m_date);
}

inline
bool CComDATE::operator<=(const CComDATE& date) const
{
    _ASSERTE(IsValid());
    _ASSERTE(date.IsValid());
    
    // Handle negative dates
    return DATEAsDouble(m_date) <= DATEAsDouble(date.m_date);
}

inline
bool CComDATE::operator>=(const CComDATE& date) const
{
    _ASSERTE(IsValid());
    _ASSERTE(date.IsValid());
    
    // Handle negative dates
    return DATEAsDouble(m_date) >= DATEAsDouble(date.m_date);
}

inline
bool CComDATE::SetDateTime(
    long nYear,
    long nMonth,
    long nDay,
    long nHour,
    long nMin,
    long nSec)
{
    if( !MakeDATE(&m_date, (WORD)nYear, (WORD)nMonth, (WORD)nDay, (WORD)nHour, (WORD)nMin, (WORD)nSec) )
    {
        Invalidate();
    }
    
    return IsValid();
}

/*inline
bool CComDATE::ParseDateTime(const char* lpszDate, DWORD dwFlags, LCID lcid)
{
    USES_CONVERSION;
    return ParseDateTime(A2OLE(lpszDate), dwFlags, lcid);
}*/

inline
bool CComDATE::ParseDateTime(const wchar_t* lpszDate, DWORD dwFlags, LCID lcid)
{
    if( FAILED(VarDateFromStr(const_cast<wchar_t*>(lpszDate), lcid, dwFlags, &m_date)) )
    {
        Invalidate();
    }
    
    return IsValid();
}

inline
LPTSTR CComDATE::Format(LPTSTR pszOut, DWORD dwFlags, LCID lcid) const
{
    *pszOut = 0;
    
    // If invalild, return empty string
    if (!IsValid()) return pszOut;
    
    BSTR  bstr = 0;
    if( SUCCEEDED(VarBstrFromDate(m_date, lcid, dwFlags, &bstr)) )
    {
#ifdef _UNICODE
        wcscpy(pszOut, bstr);
#else
        wcstombs(pszOut, bstr, wcslen(bstr) + 1);
#endif
        
        SysFreeString(bstr);
    }
    
    return pszOut;
}

inline
LPTSTR CComDATE::Format(LPTSTR pszOut, LPCTSTR pFormat) const
{
    *pszOut = 0;
    
    // If invalild, return empty string
    struct tm tmDest;
    if (!IsValid() || !Maketm(&tmDest, m_date) ) return pszOut;
    
    // Fill in the buffer, disregard return value as it's not necessary
    // NOTE: 4096 is an arbitrary value picked lower than INT_MAX
    // as the VC6 implementation of wcsftime allocates memory based
    // on the 2nd parameter for some reason.
    _tcsftime(pszOut, 4096, pFormat, &tmDest);
    return pszOut;
}

inline
LPTSTR CComDATE::Format(LPTSTR pszOut, UINT nFormatID, HINSTANCE hinst) const
{
    *pszOut = 0;
    
    TCHAR   sz[256];
    if( LoadString(hinst, nFormatID, sz, sizeof(sz)/sizeof(*sz)) )
    {
        return Format(pszOut, sz);
    }
    
    return pszOut;
}

// The FormatDate and FormatTime functions were provided by Joe O'Leary (joleary@artisoft.com) to
// wrap he Win32 National Language Support functions ::GetDateFormat() and ::GetTimeFormat().
// The format strings used here are specified in the on-line help for those functions.
// The default format is the current user locale.

inline
LPTSTR CComDATE::FormatDate(LPTSTR pszOut, LPCTSTR szFmt, DWORD dwFlags, LCID lcid) const
{
	_ASSERTE((szFmt==NULL) || (dwFlags==0));// if format is non-NULL, 'dwFlags' MUST be zero

	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	*pszOut = NULL;

	if ( !VariantTimeToSystemTime(m_date, &st) )
	{
#ifdef ATLTRACE
        ATLTRACE("\nVariantTimeToSystemTime failed. Error = %lu.\n\t", GetLastError());
#endif
	}
	else if ( GetDateFormat(lcid, dwFlags, &st, szFmt, pszOut, 255) == 0 )
	{
#ifdef ATLTRACE
        ATLTRACE("\nGetDateFormat failed. Error = %lu.\n\t", GetLastError());
#endif
	}

	return pszOut;
}

inline
LPTSTR CComDATE::FormatTime(LPTSTR pszOut, LPCTSTR szFmt, DWORD dwFlags, LCID lcid) const
{
	_ASSERTE((szFmt==NULL) || (dwFlags==0));// if format is non-NULL, 'dwFlags' MUST be zero

	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	*pszOut = NULL;

	if ( !VariantTimeToSystemTime(m_date, &st) )
	{
#ifdef ATLTRACE
        ATLTRACE("\nVariantTimeToSystemTime failed. Error = %lu.\n\t", GetLastError());
#endif
	}
	else if ( GetTimeFormat(lcid, dwFlags, &st, szFmt, pszOut, 255) == 0 )
	{
#ifdef ATLTRACE
        ATLTRACE("\nGetTimeFormat failed. Error = %lu.\n\t", GetLastError());
#endif
	}

	return pszOut;
}

inline
bool Maketm(struct tm* ptmDest, DATE dtSrc)
{
    SYSTEMTIME  st;
    if( !VariantTimeToSystemTime(dtSrc, &st) ) return false;
    
    struct tm& tmDest = *ptmDest; // Convenience
    tmDest.tm_sec = st.wSecond;
    tmDest.tm_min = st.wMinute;
    tmDest.tm_hour = st.wHour;
    tmDest.tm_mday = st.wDay;
    tmDest.tm_mon = st.wMonth - 1;
    tmDest.tm_year = st.wYear - 1900;
    tmDest.tm_wday = st.wDayOfWeek;
    tmDest.tm_isdst = -1;   // Force DST checking
    mktime(&tmDest);    // Normalize
    
    return true;
}

inline
double DATEAsDouble(DATE dt)
{
    // No problem if positive
    if (dt >= 0) return dt;
    
    // If negative, must convert since negative dates not continuous
    // (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
    double temp = ceil(dt);
    return temp - (dt - temp);
}

inline
DATE MakeDATE(double dbl)
{
    // No problem if positive
    if (dbl >= 0) return dbl;
    
    // If negative, must convert since negative dates not continuous
    // (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
    double temp = floor(dbl); // dbl is now whole part
    return temp + (temp - dbl);
}

inline
bool MakeDATE(
    DATE* pdtDest,
    WORD  wYear,
    WORD  wMonth,
    WORD  wDay,
    WORD  wHour,
    WORD  wMinute,
    WORD  wSecond)
{
    SYSTEMTIME st = { 0 };
    st.wYear = wYear;
    st.wMonth = wMonth;
    st.wDay = wDay;
    st.wHour = wHour;
    st.wMinute = wMinute;
    st.wSecond = wSecond;
    
    return SystemTimeToVariantTime(&st, pdtDest) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
// CComSpan class

inline
long CComSpan::Hours() const
{
    _ASSERTE(IsValid());
    
    // Truncate days and scale up
    double dbTemp = modf(m_span, &dbTemp);
    return (long)(dbTemp * 24);
}

inline
long CComSpan::Minutes() const
{
    _ASSERTE(IsValid());
    
    // Truncate hours and scale up
    double dbTemp = modf(m_span * 24, &dbTemp);
    return (long)((dbTemp + HALF_SECOND) * 60);
}

inline
long CComSpan::Seconds() const
{
    _ASSERTE(IsValid());
    
    // Truncate minutes and scale up
    double dbTemp = modf(m_span * 24 * 60, &dbTemp);
    return (long)(dbTemp * 60);
}

inline
void CComSpan::SetSpan(
    long  nDays,
    long  nHours,
    long  nMins,
    long  nSecs)
{
    // Set date span by breaking longo fractional days (all input ranges valid)
    m_span = nDays +
             ((double)nHours)/24 +
             ((double)nMins)/(24*60) +
             ((double)nSecs)/(24*60*60);
}

/////////////////////////////////////////////////////////////////////////////
// CComDATE/CComSpan math friend functions

// date2 = date1 + span;
inline
CComDATE operator+(const CComDATE& date1, const CComSpan& span)
{
    CComDATE  date2;
    
    if( date1.IsValid() && span.IsValid() )
    {
        // Compute the actual date difference by adding underlying data
        date2 = MakeDATE(DATEAsDouble(static_cast<DATE>(date1)) + static_cast<double>(span));
    }
    
    return date2;
}

// date2 = span + date1;
inline
CComDATE operator+(const CComSpan& span, const CComDATE& date1)
{
    CComDATE  date2;
    
    if( date1.IsValid() && span.IsValid() )
    {
        // Compute the actual date difference by adding underlying data
        date2 = MakeDATE(DATEAsDouble(static_cast<DATE>(date1)) + static_cast<double>(span));
    }
    
    return date2;
}

// date2 = date1 - span;
inline
CComDATE operator-(const CComDATE& date1, const CComSpan& span)
{
    CComDATE  date2;
    
    if( date1.IsValid() && span.IsValid() )
    {
        // Compute the actual date difference by adding underlying data
        date2 = MakeDATE(DATEAsDouble(static_cast<DATE>(date1)) - static_cast<double>(span));
    }
    
    return date2;
}

// span = date1 - date2;
inline
CComSpan operator-(const CComDATE& date1, const CComDATE& date2)
{
    CComSpan span;
    
    if( date1.IsValid() && date2.IsValid() )
    {
        span = DATEAsDouble(static_cast<DATE>(date1)) - DATEAsDouble(static_cast<DATE>(date2));
    }
    
    return span;
}

// span3 = span1 + span2;
inline
CComSpan operator+(const CComSpan& span1, const CComSpan& span2)
{
    CComSpan span3;
    if( span1.IsValid() && span2.IsValid() ) span3 = span1.m_span + span2.m_span;
    return span3;
}

// span3 = span1 - span2;
inline
CComSpan operator-(const CComSpan& span1, const CComSpan& span2)
{
    return span1 + (-span2);
}

#undef HALF_SECOND
#endif  // __COMDATE_H__
