//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlog.h,v $
// $Author: cocoaspud $
//
// $Log: cdxlog.h,v $
// Revision 1.5  2001/11/12 16:35:03  cocoaspud
// copy/paste events, cdx compile error under VC7
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:29  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.3  2000/02/22 00:52:44  Karasu
// Timestamp preceeds log entries. Output filename changed to cdxYYYYMMDDHHMMSS.log
//
// Revision 1.2  1999/09/01 13:05:41  bsimser
// Small catches to handle Borland compatilbility with ASSERT and CDXLOG macros
//
// Revision 1.1  1999/08/30 05:20:55  MICHAELR
// Added to repository
//
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXLOG_H
#define CDXLOG_H

#include "cdxscreen.h"

// CDX Logging Types.  May be combined together to log to different
// areas simultaneously
#define CDXLOG_TYPE_FILE			0x00000001L
#define CDXLOG_TYPE_STDOUT			0x00000002L
#define CDXLOG_TYPE_STDERR			0x00000004L
#define CDXLOG_TYPE_COM1			0x00000008L
#define CDXLOG_TYPE_COM2			0x00000010L
#define CDXLOG_TYPE_LPT1			0x00000020L
#define CDXLOG_TYPE_LPT2			0x00000040L
#define CDXLOG_TYPE_GUIWIN			0x00000080L
#define CDXLOG_TYPE_WINSOCK			0x00000100L
#define CDXLOG_TYPE_DEBUGWIN		0x00000200L

// Logging levels.  More detail as you increase
#define CDXLOG_LEVEL_1				0x00000001L
#define CDXLOG_LEVEL_2				0x00000002L

class CDXLog
{
public:
	CDXLog();
	~CDXLog();

public:
	// write output to the current log for level 0
	void Write(const char *, ...);
	// write output to the current log for level <= 1
	void Write2(const char *, ...);
	// clears the current log
	void Clear();

	void Screen(CDXScreen *);
	CDXScreen *Screen();

	void LogType(const int);
	int LogType();

	void Level(const int);
	int Level();

protected:
	void WriteInternal(const char *);
	void ClearFile();
	void FileWrite(const char *buff);
	void COMWrite(const int, const char *);
	void LPTWrite(const int, const char *);
	void GUIWrite(const char *);

private:
	CDXScreen *m_Screen;
	int m_LogType;
	int m_Level;
	DWORD m_dwStartTime;
	char m_FileName[MAX_PATH];
};

#ifdef CDX_LOG

extern CDXLog g_cdxlog;

// debug logging functions
#define CDXLOG g_cdxlog.Write
#define CDXLOG2 g_cdxlog.Write2
#define CDXLOGSCREEN g_cdxlog.Screen
#define CDXLOGLEVEL g_cdxlog.Level
#define CDXLOGTYPE g_cdxlog.LogType
#define CDXLOGCLEAR g_cdxlog.Clear

#else

// logging is NOT enabled

// just define it and let it go for a non-logging build
//#ifdef _MSC_VER
//#define CDXLOG ((void)0)
//#define CDXLOG2 ((void)0)
//#define CDXLOGSCREEN ((void)0)
//#define CDXLOGLEVEL ((void)0)
//#define CDXLOGTYPE ((void)0)
//#define CDXLOGCLEAR ((void)0)
//#else
#define CDXLOG
#define CDXLOG2
#define CDXLOGSCREEN
#define CDXLOGLEVEL
#define CDXLOGTYPE
#define CDXLOGCLEAR
//#endif          // _MSC_VER

#endif		// #ifdef CDX_LOG


#endif		// #ifndef CDXLOG_H
