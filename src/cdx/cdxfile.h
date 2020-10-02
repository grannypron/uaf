//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxfile.h,v $
// $Author: cocoaspud $
//
// $Log: cdxfile.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/08/21 21:16:41  mindcry
// Changed the return type. Now it follows the rules of the other CDX classes.
//
// Revision 1.1.1.1  2000/04/22 16:09:18  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.4  2000/01/13 23:59:16  jhebert
// First checkin for v3.0
// Made the destructor virtual
//
// 
//
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXFILE_H
#define CDXFILE_H

#ifndef CDX_H
class CDXFile;
#include "cdx.h"
#endif

#ifndef QWORD
#define QWORD unsigned __int64
#endif

#ifndef INT64
#define INT64 __int64
#endif

//////////////////////////////////////////////////////////////////////////////////
// File handling wrapper
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXFile
{
public:
	CDXFile();
	virtual ~CDXFile();

	int		Open(const char* FileName);
	int 	OpenRead(const char* FileName);
	int		OpenWrite(const char* FileName);

	void	Close();

	// reading functions
	DWORD	Read(void *whence, UINT howmuch);
	
	UCHAR	ReadUCHAR();
	BYTE	ReadBYTE();

	USHORT	ReadUSHORT();
	WORD	ReadWORD();

	UINT	ReadUINT();
	DWORD	ReadDWORD();

	INT64	ReadINT64();
	QWORD	ReadQWORD();

	// writing functions
	DWORD	Write(const void *whence, UINT howmuch);

	DWORD	WriteUCHAR(const UCHAR data);
	DWORD	WriteBYTE(const BYTE data);

	DWORD	WriteUSHORT(const USHORT data);
	DWORD	WriteWORD(const WORD data);

	DWORD	WriteUINT(const UINT data);
	DWORD	WriteDWORD(const DWORD data);

	DWORD	WriteINT64(const INT64 data);
	DWORD	WriteQWORD(const QWORD data);

	// File pointer functions
	DWORD	Position();
	DWORD	Position(LONG lDistanceToMove, DWORD dwMoveMethod);
	
	void	SeekToStart();
	void	SeekToEnd();

	// information functions
	DWORD	FileSize();

protected:
	int		OpenCommon(const char *FileName, DWORD Access, DWORD Sharing, DWORD CreateDisposition);

private:
	HANDLE	m_fh;
};

#endif CDXFILE_H
