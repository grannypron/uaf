//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlog.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxlog.cpp,v $
// Revision 1.8  2014/09/26 13:42:12  cocoaspud
// Added solution files for VS2012
//
// Revision 1.7  2013/04/01 14:55:01  stevens-madison
// Working towards Version 1.0.....
// First 'commit' since sourceforge changed URLs.
// Fingers crossed.
//
// Revision 1.6  2011/06/21 14:14:29  stevens-madison
// Bugfixes working towards version 1.0
//
// Revision 1.5  2006/04/16 16:08:10  cocoaspud
// range check race/class definitions in config files
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
// Revision 1.2  2000/01/20 02:23:32  jhebert
// Minor correction in order to comply with v3.0
//
// Revision 1.1  1999/08/30 05:20:55  MICHAELR
// Added to repository
//
//
// $Revision: 1.8 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef CDX_LOG

#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxlog.cpp,v 1.8 2014/09/26 13:42:12 cocoaspud Exp $";
#endif

#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "cdx.h"
#include "cdxscreen.h"
#include "cdxsurface.h"
#include "cdxlog.h"

// our instance of the log
CDXLog g_cdxlog;


CDXLog::CDXLog()
{
	m_LogType = CDXLOG_TYPE_FILE,
	m_Screen = NULL;
	m_Level = CDXLOG_LEVEL_2;
	m_dwStartTime = timeGetTime();
	SYSTEMTIME st;
    GetLocalTime( &st );
							// YYYYMMDDHHMMSS
    sprintf(m_FileName, "cdx%.4d%.2d%.2d%.2d%.2d%.2d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

CDXLog::~CDXLog()
{

}

// changes the logging level
void CDXLog::Level(const int NewLevel)
{
	// save the verfied type
	m_Level = NewLevel;
}

// return the current logging level
int CDXLog::Level()
{
	return m_Level;
}

// changes the logging type
void CDXLog::LogType(const int NewType)
{
	// save the verfied type
	m_LogType = NewType;
}

// return the current logging type
int CDXLog::LogType()
{
	return m_LogType;
}

// changes the logging type
void CDXLog::Screen(CDXScreen *s)
{
	// save the screen
	m_Screen = s;
}

// return the current logging type
CDXScreen *CDXLog::Screen()
{
	return m_Screen;
}

// clears our logging file
void CDXLog::ClearFile()
{
	DeleteFile(m_FileName);
	//MyDeleteFile(m_FileName);
}

// clears the current log file
void CDXLog::Clear()
{
	switch(m_LogType)
	{
	case CDXLOG_TYPE_FILE:
		ClearFile();
		break;
	case CDXLOG_TYPE_STDOUT:
	case CDXLOG_TYPE_STDERR:
		// don't think these can be cleared
		break;
	};
}

// writes to our logging file
void CDXLog::FileWrite(const char *buff)
{
	FILE *fp = NULL;

	// open our logging file
    fp = fopen(m_FileName, "a");
    if (fp==NULL) return;

	fprintf(fp, buff);

	fclose(fp);
}

// writes to the com port
void CDXLog::COMWrite(const int portnum, const char *buff)
{
	FILE *fp = NULL;
	char port[10];

	// open our logging file
	switch(portnum)
	{
	case CDXLOG_TYPE_COM1:
		lstrcpy(port, "COM1:");
		break;
	case CDXLOG_TYPE_COM2:
		lstrcpy(port, "COM2:");
		break;
	};

	fp = fopen(port, "w");
	if (fp==NULL) return;

	fprintf(fp, buff);

	fclose(fp);
}

// writes to the LPT port
void CDXLog::LPTWrite(const int portnum, const char *buff)
{
	FILE *fp = NULL;
	char port[10];

	// open our logging file
	switch(portnum)
	{
	case CDXLOG_TYPE_LPT1:
		lstrcpy(port, "LPT1:");
		break;
	case CDXLOG_TYPE_LPT2:
		lstrcpy(port, "LPT2:");
		break;
	};

	fp = fopen(port, "w");
	if (fp==NULL) return;

	fprintf(fp, buff);

	fclose(fp);
}

// writes to a GUI window
void CDXLog::GUIWrite(const char *buff)
{
	// allocate a static array of 5 lines of text
	// holding 255 chars each
	static char lines[5][256];
	static int curline = 0;
	SIZE sz;
	int i;
	CDXSurface *s;
	int x, y, h, w;

	lstrcpy(lines[curline], buff);
	curline++;

	// if we've ran over our EOL then start at the beginning
	if (curline==5) curline = 0;

	// sanity check
	if (m_Screen==NULL)
	{
		FileWrite("No CDXScreen object supplied for GUIWIN output\n");
		return;
	}

	s = m_Screen->GetFront();

	s->GetDC();
    s->SetFont();

    // get the height and width current font (use M since its widest)
    ::GetTextExtentPoint32(s->GetDC(), "M", 1, &sz);

	s->ReleaseDC();

	w = sz.cx * 26;
	h = sz.cy * 6;

	x = s->GetWidth() - w;
	y = s->GetHeight() - h;

	// fill the rect with black
	s->FillRect(x, y, x+w, y+h, RGB(255,255,255));
	s->FillRect(x+1, y+1, x+w-1, y+h-1, 0);

	// start the text slightly indented
	x+=(sz.cx/2); y+=(sz.cy/2);

	s->GetDC();
    s->SetFont();

	// render the window
	for(i=0;i<5;i++)
	{
		s->TextXY(x, y, RGB(255,255,255), lines[i]);
		y+=sz.cy;
	}

	s->ReleaseDC();

	s = NULL;
}

// our internal writing function
void CDXLog::WriteInternal(const char *buff)
{
	if ((m_LogType & CDXLOG_TYPE_FILE)==CDXLOG_TYPE_FILE)
		FileWrite(buff);
	if ((m_LogType & CDXLOG_TYPE_STDOUT)==CDXLOG_TYPE_STDOUT)
		fprintf(stdout, buff);
	if ((m_LogType & CDXLOG_TYPE_STDERR)==CDXLOG_TYPE_STDERR)
		fprintf(stderr, buff);
	if ((m_LogType & CDXLOG_TYPE_COM1)==CDXLOG_TYPE_COM1)
		COMWrite(CDXLOG_TYPE_COM1, buff);
	if ((m_LogType & CDXLOG_TYPE_COM2)==CDXLOG_TYPE_COM2)
		COMWrite(CDXLOG_TYPE_COM2, buff);
	if ((m_LogType & CDXLOG_TYPE_LPT1)==CDXLOG_TYPE_LPT1)
		LPTWrite(CDXLOG_TYPE_LPT1, buff);
	if ((m_LogType & CDXLOG_TYPE_LPT2)==CDXLOG_TYPE_LPT2)
		LPTWrite(CDXLOG_TYPE_LPT2, buff);
	if ((m_LogType & CDXLOG_TYPE_GUIWIN)==CDXLOG_TYPE_GUIWIN)
		GUIWrite(buff);

#if _MSC_VER
	// output to the debug window also
	if ((m_LogType & CDXLOG_TYPE_DEBUGWIN)==CDXLOG_TYPE_DEBUGWIN)
		ODS(buff);
#endif
}


// writes an entry to the debug log
void CDXLog::Write(const char *fmt, ...)
{
    char    buff[256];
    va_list	va;

    va_start(va, fmt);

    //put the time in milliseconds at beginning of line.
    wsprintf(buff, "%u: ", timeGetTime() - m_dwStartTime);
	wvsprintf(&buff[lstrlen(buff)], fmt, va);
	lstrcat(buff, "\n");

	WriteInternal(buff);
}

// writes an entry to the debug log
void CDXLog::Write2(const char *fmt, ...)
{
    char    buff[256];
    va_list	va;

	if (!(m_Level == CDXLOG_LEVEL_2)) return;

    va_start(va, fmt);

    //put the time in milliseconds at beginning of line.
    wsprintf(buff, "%d: ", timeGetTime() - m_dwStartTime);
	wvsprintf(&buff[lstrlen(buff)], fmt, va);
	lstrcat(buff, "\n");

	WriteInternal(buff);
}


#endif			// #ifdef CDX_LOG
