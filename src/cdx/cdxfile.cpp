//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxfile.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxfile.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.2  2000/08/21 21:16:06  mindcry
// Changed the return type. Now it follows the rules of the other CDX classes.
//
// Revision 1.1.1.1  2000/04/22 16:09:18  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.5  2000/01/13 23:59:16  jhebert
// First checkin for v3.0
// Made the destructor virtual
//
// 
//
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxfile.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "cdxfile.h"

/////////////////////////////////////////////////////////////////////
// Func: CDXFile
//
// Desc: Constructor
//
// No return value
/////////////////////////////////////////////////////////////////////
CDXFile::CDXFile()
{
	m_fh = INVALID_HANDLE_VALUE;
}


/////////////////////////////////////////////////////////////////////
// Func: ~CDXFile
//
// Desc: Destructor
//
// No return value
/////////////////////////////////////////////////////////////////////
CDXFile::~CDXFile()
{
	Close();
}

/////////////////////////////////////////////////////////////////////
// Func: Position
//
// Desc: Returns the location in the currrent file
//
// Returns the pointer location, or zero if there is no location
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::Position()
{
	if (m_fh == INVALID_HANDLE_VALUE) return 0;
	return SetFilePointer(m_fh, 0, NULL, FILE_CURRENT);
}

/////////////////////////////////////////////////////////////////////
// Func: Position
//
// Desc: Changes the location in the current file
//		 MoveMethod: FILE_BEGIN, FILE_CURRENT, or FILE_END
//
// Returns the pointer location, or zero if there is no location
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::Position(LONG lDistanceToMove, DWORD dwMoveMethod)
{
	if (m_fh == INVALID_HANDLE_VALUE) return 0;
	return SetFilePointer(m_fh, lDistanceToMove, NULL, dwMoveMethod);
}

/////////////////////////////////////////////////////////////////////
// Func: FileSize
//
// Desc: Returns the size of the currently opened file
//
// Returns the file size of the current file, or zero otherwise
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::FileSize()
{
	if (m_fh == INVALID_HANDLE_VALUE) return 0;
	return GetFileSize(m_fh, NULL);
}

/////////////////////////////////////////////////////////////////////
// Func: Close
//
// Desc: Closes any file we might have open
//
// No return value
/////////////////////////////////////////////////////////////////////
void CDXFile::Close()
{
	if (m_fh!=INVALID_HANDLE_VALUE) CloseHandle(m_fh);
	m_fh = INVALID_HANDLE_VALUE;
}

/////////////////////////////////////////////////////////////////////
// Func: SeekToStart
//
// Desc: Moves the file pointer to the beginning of the file
//
// No return value
/////////////////////////////////////////////////////////////////////
void CDXFile::SeekToStart()
{
	if (m_fh == INVALID_HANDLE_VALUE) return;
	Position(0, FILE_BEGIN);
}

/////////////////////////////////////////////////////////////////////
// Func: SeekToEnd
//
// Desc: Moves the file pointer to the end of the file
//
// No return value
/////////////////////////////////////////////////////////////////////
void CDXFile::SeekToEnd()
{
	if (m_fh == INVALID_HANDLE_VALUE) return;
	Position(0, FILE_END);
}

/////////////////////////////////////////////////////////////////////
// Func: OpenCommon
//
// Desc: Common function to open a file to service
//
// Returns 0 if file opened ok, <0 otherwise
/////////////////////////////////////////////////////////////////////
int CDXFile::OpenCommon(const char* FileName, DWORD Access, DWORD Sharing, DWORD CreateDisposition)
{
	Close();

	if ((m_fh = CreateFile(FileName, Access, Sharing, NULL, CreateDisposition,
		FILE_ATTRIBUTE_NORMAL, 0))==INVALID_HANDLE_VALUE) return -1;

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Func: Open
//
// Desc: Opens the given file for read and write access
//
// Returns 0 if file opened ok, <0 otherwise
/////////////////////////////////////////////////////////////////////
int CDXFile::Open(const char *FileName)
{
	return OpenCommon(FileName, GENERIC_READ | GENERIC_WRITE, 0, OPEN_ALWAYS);
}

/////////////////////////////////////////////////////////////////////
// Func: OpenRead
//
// Desc: Opens the given file for read access only
//
// Returns 0 if file opened ok, <0 otherwise
/////////////////////////////////////////////////////////////////////
int CDXFile::OpenRead(const char *FileName)
{
	return OpenCommon(FileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
}

/////////////////////////////////////////////////////////////////////
// Func: OpenWrite
//
// Desc: Opens the given file for write access only
//
// Returns 0 if file opened ok, <0 otherwise
/////////////////////////////////////////////////////////////////////
int CDXFile::OpenWrite(const char *FileName)
{
	return OpenCommon(FileName, GENERIC_WRITE, 0, OPEN_ALWAYS);
}

/////////////////////////////////////////////////////////////////////
// Func: Read
//
// Desc: Reads the specified number of bytes from the file
//
// Returns the number of bytes actually read from the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::Read(void *whence, UINT howmuch)
{
	DWORD dwBytesRead;

	// read the file into memory
	if (ReadFile(m_fh, whence, howmuch, &dwBytesRead, NULL))
		return dwBytesRead;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadUCHAR
//
// Desc: Reads an UCHAR from the file
//
// Return the UCHAR read from the file
/////////////////////////////////////////////////////////////////////
UCHAR CDXFile::ReadUCHAR()
{
	UCHAR whence;
	Read(&whence, sizeof(UCHAR));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadBYTE
//
// Desc: Reads an BYTE from the file
//
// Return the BYTE read from the file
/////////////////////////////////////////////////////////////////////
BYTE CDXFile::ReadBYTE()
{
	BYTE whence;
	Read(&whence, sizeof(BYTE));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadUSHORT
//
// Desc: Reads an USHORT from the file
//
// Return the USHORT read from the file
/////////////////////////////////////////////////////////////////////
USHORT CDXFile::ReadUSHORT()
{
	USHORT whence;
	Read(&whence, sizeof(USHORT));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadWORD
//
// Desc: Reads an WORD from the file
//
// Return the WORD read from the file
/////////////////////////////////////////////////////////////////////
WORD CDXFile::ReadWORD()
{
	WORD whence;
	Read(&whence, sizeof(WORD));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadUINT
//
// Desc: Reads an UINT from the file
//
// Return the UINT read from the file
/////////////////////////////////////////////////////////////////////
UINT CDXFile::ReadUINT()
{
	UINT whence;
	Read(&whence, sizeof(UINT));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadDWORD
//
// Desc: Reads an DWORD from the file
//
// Return the DWORD read from the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::ReadDWORD()
{
	DWORD whence;
	Read(&whence, sizeof(DWORD));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadINT64
//
// Desc: Reads an INT64 from the file
//
// Return the INT64 read from the file
/////////////////////////////////////////////////////////////////////
INT64 CDXFile::ReadINT64()
{
	INT64 whence;
	Read(&whence, sizeof(INT64));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: ReadQWORD
//
// Desc: Reads an QWORD from the file
//
// Return the QWORD read from the file
/////////////////////////////////////////////////////////////////////
QWORD CDXFile::ReadQWORD()
{
	QWORD whence;
	Read(&whence, sizeof(QWORD));
	return whence;
}

/////////////////////////////////////////////////////////////////////
// Func: Write
//
// Desc: Writes the specified number of bytes to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::Write(const void *whence, UINT howmuch)
{
	DWORD dwBytesWrote;

	// read the file into memory
	if (WriteFile(m_fh, whence, howmuch, &dwBytesWrote, NULL))
		return dwBytesWrote;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////
// Func: WriteUCHAR
//
// Desc: Writes an UCHAR to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteUCHAR(const UCHAR data)
{
	return Write(&data, sizeof(UCHAR));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteBYTE
//
// Desc: Writes a BYTE to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteBYTE(const BYTE data)
{
	return Write(&data, sizeof(BYTE));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteUSHORT
//
// Desc: Writes an USHORT to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteUSHORT(const USHORT data)
{
	return Write(&data, sizeof(USHORT));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteWORD
//
// Desc: Writes a WORD to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteWORD(const WORD data)
{
	return Write(&data, sizeof(WORD));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteUINT
//
// Desc: Writes an UINT to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteUINT(const UINT data)
{
	return Write(&data, sizeof(UINT));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteDWORD
//
// Desc: Writes a DWORD to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteDWORD(const DWORD data)
{
	return Write(&data, sizeof(DWORD));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteINT64
//
// Desc: Writes a INT64 to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteINT64(const INT64 data)
{
	return Write(&data, sizeof(INT64));
}

/////////////////////////////////////////////////////////////////////
// Func: WriteQWORD
//
// Desc: Writes a QWORD to the file
//
// Returns the number of bytes actually written to the file
/////////////////////////////////////////////////////////////////////
DWORD CDXFile::WriteQWORD(const QWORD data)
{
	return Write(&data, sizeof(QWORD));
}
