//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxresource.cpp,v $
// $Author: stevens-madison $
//
// $Log: cdxresource.cpp,v $
// Revision 1.6  2011/06/21 14:14:29  stevens-madison
// Bugfixes working towards version 1.0
//
// Revision 1.5  2010/02/28 21:36:39  cocoaspud
// mods for compat with Visual Studio 2005
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:57  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.9  1999/09/01 13:05:41  bsimser
// Small catches to handle Borland compatilbility with ASSERT and CDXLOG macros
//
// Revision 2.8  1999/05/20 15:29:03  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.7  1999/05/20 13:33:53  pietro
// fixed the rcsid[] bug
//
// Revision 2.6  1999/05/19 04:34:14  pietro
// comments
//
// Revision 2.5  1999/05/19 04:07:20  pietro
// removed LoadBMP from CDX Resource. Now every object handles his own loading
// via his own interfaces. in cdx surface, use create.
//
// Revision 2.4  1999/05/18 18:27:11  pietro
// seek returns now FILE* handle of RF for subsequent read operations
//
// Revision 2.3  1999/05/17 21:24:42  pietro
// Added comments
//
// Revision 2.2  1999/05/17 21:10:31  pietro
// Added CDXResWorldObject handling
//
// Revision 2.1  1999/05/17 00:14:10  pietro
// new CDXResource, backward compatibility is maintained.
// For CDXResourceCompiler see Tools.
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.6 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxresource.cpp,v 1.6 2011/06/21 14:14:29 stevens-madison Exp $";
#endif

#include "CDX.h"
#include "cdxresource.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif  // _MSC_VER

BOOL MyDeleteFile(LPCSTR name);

#define BLOCK_SIZE 16384

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXResource::CDXResource(void)
{
	m_pFE = NULL;
	m_Infile = 0;
	m_Outfile = 0;
	m_nFiles = 0;
	version = 0;
	Directory = NULL;
	fName = NULL;
	fHandle = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXResource::~CDXResource(void)
{
	if (version == 1)
	{
		MyDeleteFile("CDX.TMP");
		if(m_Infile) _close(m_Infile);
		if(m_Outfile) _close(m_Outfile);
	}
	else
	{
		SAFEDELETE(Directory);
		SAFEDELETE(fName);
		if (fHandle!=NULL)
		{
			fclose(fHandle);
			fHandle=NULL;
		}
	}
}


BOOL CDXResource::Open(const char *Filename)
{
	CDXRESOURCEFILEHEADER	rh;

	if(NULL==(fHandle = fopen(Filename, "rb")))
		return false;
	if (1 != (fread(&rh, sizeof(rh),1,fHandle)))
		return false;
	if(rh.Signature != 'CDXR')
	{
		//compatibility mode. old cdxresource haven't used signature or versioning at all...
		version = 1;
		fclose(fHandle);
		return (OpenV1(Filename));
	}

	//resource file > version 1. (1=old style)

	version = rh.Version;
	fName = _strdup(Filename);
	NumOfEntries = rh.NumOfEntries;
	
	Directory = new CDXResDirEntry[NumOfEntries];
	for(UINT i = 0; i < NumOfEntries; i++)
	{
		fread(&Directory[i].pos, sizeof(Directory[i].pos), 1, fHandle);
		fread(&Directory[i].size, sizeof(Directory[i].size), 1, fHandle);
		fread(&Directory[i].type, sizeof(Directory[i].type), 1, fHandle);
		fread(&Directory[i].id, sizeof(Directory[i].id), 1, fHandle);
	}
	//fread(Directory, NumOfEntries * sizeof(CDXResDirEntry), 1, fHandle);

	fseek(fHandle, 0, SEEK_END);
	fSize = ftell(fHandle);
	rewind(fHandle);

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
// Opens a resource file to be used with CDXResource.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXResource::OpenV1(const char *Filename)
{
	int bytes;

	// Open the resource file
	m_Infile = _open(Filename, O_RDONLY | O_BINARY);
	if(m_Infile < 1) return FALSE;

	// Read the number of files in the resource
	bytes = _read(m_Infile, &m_nFiles, sizeof(int));
	if(bytes != sizeof(int))
	{
		Close();
		return FALSE;
	}

	// Make space for the file header
	m_pFE = new FILEENTRY[m_nFiles];
	if(m_pFE == NULL)
	{
		Close();
		return FALSE;
	}

	// Read the header file
	bytes = _read(m_Infile, m_pFE, m_nFiles * sizeof(FILEENTRY));
	if(bytes != (int)(m_nFiles * sizeof(FILEENTRY)))
	{
		Close();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Closes the resource file. Also deletes the temporary file.
//////////////////////////////////////////////////////////////////////////////////
void CDXResource::Close(void)
{
	if (version == 1)
	{
		MyDeleteFile("CDX.TMP");
		if(m_Infile) _close(m_Infile);
		if(m_Outfile) _close(m_Outfile);
	}
	else
	{
		SAFEDELETE(Directory);
		SAFEDELETE(fName);
		if (fHandle!=NULL)
		{
			fclose(fHandle);
			fHandle=NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Gets a file from a resource file. The file is stored as a temporary file
// called CDX.TMP.
// Only to be used in compatibility mode! (resource files of version 1)
//////////////////////////////////////////////////////////////////////////////////
char* CDXResource::GetFile(const char *Filename)
{
	char *buffer;
	int i;

	MyDeleteFile("CDX.TMP");

	for(i = 0; i < m_nFiles; i++)
	{
		if(_strnicmp(Filename, m_pFE[i].name, 13) == 0) break;
	}

	long offset = m_pFE[i].offset;
	_lseek(m_Infile, offset, SEEK_SET);

	m_Outfile = _open("CDX.TMP", O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);
	if(m_Outfile < 1) return NULL;

	int size = m_pFE[i+1].offset - offset;

	// Get space for the buffer
	buffer = new char[size];
	if(buffer == NULL) return NULL;

	_read(m_Infile, buffer, size);
	_write(m_Outfile, buffer, size);

	_close(m_Outfile);

	return (char*)"CDX.TMP";
}

//////////////////////////////////////////////////////////////////////////////////
// Gets the file handle of resource file being used
//////////////////////////////////////////////////////////////////////////////////
FILE*	CDXResource::GetfHandle()
{
	return fHandle;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns RESHANDLE (which in fact is only index to internal CDXResource tables)
// ID is generated by ResourceCompiler (and stored in the Directory of ResourceFile).
// All subsequent operation on files stored in Resource File are based
// only on RESHANDLE.
// If the ID is invalid (not found in directory) -1 (which is 0xffffffff for UINT)
// is returned.
//////////////////////////////////////////////////////////////////////////////////
RESHANDLE CDXResource::GetHandle(UINT id)
{
	for(UINT i = 0; i < NumOfEntries; i++)
	{
		if (Directory[i].id == id)
			return i;
	}
#ifdef _MSC_VER
	_ASSERT (FALSE);
#endif
	return -1;		//no such id
}

//////////////////////////////////////////////////////////////////////////////////
// Set the position in resource file to the start of particular file.
//////////////////////////////////////////////////////////////////////////////////
FILE*	CDXResource::Seek(RESHANDLE i)
{
	fseek(fHandle, Directory[i].pos, SEEK_SET);
	return fHandle;
}


//////////////////////////////////////////////////////////////////////////////////
// Returns position of particular file in Resource File
//////////////////////////////////////////////////////////////////////////////////
UINT CDXResource::GetPos(RESHANDLE i)
{
	return Directory[i].pos;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns size of particular file in Resource File
//////////////////////////////////////////////////////////////////////////////////
UINT CDXResource::GetSize(RESHANDLE i)
{
	return Directory[i].size;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns type of particular file in Resource File
//////////////////////////////////////////////////////////////////////////////////
UCHAR CDXResource::GetType(RESHANDLE i)
{
	return Directory[i].type;
}

