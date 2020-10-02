//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxsoundbuffer.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxsoundbuffer.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.6  2000/08/21 21:21:28  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
//
// Revision 1.5  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.4  2000/05/08 20:05:19  hebertjo
// Fixed the #include statements.
//
// Revision 1.3  2000/05/02 21:49:40  hebertjo
// Fixed a bug that MindCry uncovered, I was calling the function with incorrect
// parameters.
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:11:45  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.19  2000/03/04 11:48:01  Mindcry
// small bugfix to compile correct with dx6
//
// Revision 2.18  2000/03/01 09:17:48  Mindcry
// Many changes:
// - Rewritten totally the loading routines
// - Streamed playback of wavefiles
// - ACM Decompressing of wave files (define CDX_ACM_SUPPORT)
// - 3d sound support
// - exclusive mode
// - primary buffer continuos play
// - and much much more
//
// Revision 2.15  2000/02/01 12:58:46  Mindcry
// - changed the last few functions to return a HRESULT
// - fixed the strange memory (loading) bug
//
// Revision 2.14  2000/02/01 11:28:10  Mindcry
// - cleaned up the code
// - most functions return now HRESULT
//
// Revision 2.13  2000/01/29 00:51:19  Mindcry
// Initial Release Checkin for 3.0
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxsoundbuffer.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "cdx.h"
#include "cdxsound.h"

#ifdef CDX_ACM_SUPPORT
#include <mmreg.h>
#include <msacm.h>
#endif

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXSoundBuffer::CDXSoundBuffer()
{
	m_pDS = NULL;
	m_lpDSB = NULL;
	m_Filename = NULL;
	m_Current = 0;
	m_nBuffers = 1;

	m_3d = FALSE;
#if DIRECTDRAW_VERSION >= CDX_DDVER
	m_lp3dBuffer = NULL;
#endif

	m_Streamed = FALSE;
	m_sFile=NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Default destructor.
//////////////////////////////////////////////////////////////////////////////////
CDXSoundBuffer::~CDXSoundBuffer()
{
	Release( );
}

//////////////////////////////////////////////////////////////////////////////////
// Releases all the Memory that was needed for the SoundBuffer
//////////////////////////////////////////////////////////////////////////////////
void CDXSoundBuffer::Release( )
{
	if( m_lpDSB )
	{
		for(int i = 0; i < m_nBuffers; i++) RELEASE(m_lpDSB[i]);
		delete [] m_lpDSB;
		m_lpDSB = NULL;
	}
	
#if DIRECTDRAW_VERSION >= CDX_DDVER
	if( m_lp3dBuffer )
	{
		for(int i = 0; i < m_nBuffers; i++) RELEASE(m_lp3dBuffer[i]);
		delete [] m_lp3dBuffer;
		m_lp3dBuffer=NULL;
	}
#endif

	if( m_Streamed && m_sStop )
	{
		if( m_sFile )
		{
			fclose(m_sFile);
			m_sFile=NULL;
		}
	}

	m_pDS = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Activates streaming for this soundbuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::EnableStreaming( int size, int blocks )
{
	if( m_3d ) return DSERR_ACCESSDENIED;
	m_Streamed=TRUE;
	m_sSize=size;
	m_sBlocks=blocks;
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Activates 3d for this soundbuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Enable3d( void )
{
	if( m_Streamed ) return DSERR_ACCESSDENIED;
	m_3d=TRUE;
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Called internally by the LoadFromFile function. It is not necessary to 
// call this directly.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::CreateSoundBuffer(DWORD dwBufSize, PCMWAVEFORMAT *pcmwf, int Num)
{
	HRESULT rval;
	// Create Space for Soundbuffer Pointers
	m_lpDSB = new LPDIRECTSOUNDBUFFER[Num];
	if( m_lpDSB == NULL ) return DSERR_OUTOFMEMORY;
	ZeroMemory( m_lpDSB, sizeof( LPDIRECTSOUNDBUFFER )*Num );
	
#if DIRECTDRAW_VERSION >= CDX_DDVER
	if( m_3d )
	{
		m_lp3dBuffer = new LPDIRECTSOUND3DBUFFER[Num];
		if( m_lp3dBuffer == NULL ) return DSERR_OUTOFMEMORY;
		ZeroMemory( m_lp3dBuffer, sizeof( LPDIRECTSOUNDBUFFER )*Num );
	}
#endif
	
	DSBUFFERDESC dsbdesc;
	// Set up DSBUFFERDESC structure.
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
	dsbdesc.dwBufferBytes = dwBufSize; 
	dsbdesc.lpwfxFormat   = (LPWAVEFORMATEX)pcmwf;
	if( m_Streamed ) dsbdesc.dwFlags=DSBCAPS_CTRLDEFAULT|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLPOSITIONNOTIFY;
	else if( m_3d )  dsbdesc.dwFlags=DSBCAPS_CTRL3D|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLVOLUME;
	else dsbdesc.dwFlags=DSBCAPS_CTRLDEFAULT|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_STATIC;
	
	rval=m_pDS->GetDS()->CreateSoundBuffer(&dsbdesc, &m_lpDSB[0], NULL);
	if( FAILED(rval) ) return rval;
	
#if DIRECTDRAW_VERSION >= CDX_DDVER
	if( m_3d )
	{
		rval=m_lpDSB[0]->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&m_lp3dBuffer[0]);
		if(FAILED(rval)) return rval;
	}
#endif
	
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Called internally by the LoadFromMemory function. It is not necessary to 
// call this directly.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::ReadDataFromMemory(char *ptr, DWORD dwSize)
{
	// Lock data in buffer for writing
	LPVOID pData1, pData2;
	DWORD  dwData1Size, dwData2Size;
	HRESULT rval;
	
#if DIRECTDRAW_VERSION >= CDX_DDVER
	rval = m_lpDSB[0]->Lock(0, 0, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_ENTIREBUFFER);
#else
	rval = m_lpDSB[0]->Lock(0, dwSize, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_FROMWRITECURSOR);
#endif
	if(FAILED(rval)) return rval;
	
	// Read in data(on dx3 read in first chunk)
	if(dwData1Size>0) 
	{
		memcpy(pData1, ptr, dwData1Size);
		ptr+=dwData1Size;
	}
	
	// Read in second chunk if necessary
	if(dwData2Size>0) memcpy(pData2, ptr, dwData2Size);
	
	// Unlock data in buffer
	rval = m_lpDSB[0]->Unlock(pData1, dwData1Size, pData2, dwData2Size);
	if(FAILED(rval)) return rval;
	
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Called internally. It is not necessary to call this directly.
//////////////////////////////////////////////////////////////////////////////////
LPDIRECTSOUNDBUFFER CDXSoundBuffer::GetFreeBuffer(void)
{
	DWORD Status;
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer;
	
	if(m_lpDSB==NULL) return NULL;
	
	if(Buffer=m_lpDSB[m_Current])
	{
		rval = Buffer->GetStatus(&Status);
		if(FAILED(rval)) Status = 0;
		
		if((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
		{
			if(m_nBuffers > 1)
			{
				if (++m_Current >= m_nBuffers) m_Current = 0;
				
				Buffer = m_lpDSB[m_Current];
				rval = Buffer->GetStatus(&Status);
				
				if(SUCCEEDED(rval) && (Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
				{
					Buffer->Stop();
					Buffer->SetCurrentPosition(0);
				}
			}
			else Buffer=NULL;
		}
		
		if(Buffer && (Status & DSBSTATUS_BUFFERLOST))
			if(FAILED(Buffer->Restore()))
				Buffer = NULL;
	}
	
	return Buffer;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a WAVE file from Filename. Num is the number of times this sound 
// is likely to be played simultaneously, multiple buffers may be created 
// if necessary so try to keep this number small.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Load(CDXSound* pDS, const char *Filename, int Num)
{
	// Check the Parameters
	if( Filename == NULL ) return DSERR_INVALIDPARAM;
	
	HRESULT rval=DSERR_GENERIC;
	FILE *fptr;
	fptr = fopen( Filename, "rb" );
	if( fptr )
	{
		if(m_Streamed)
		{
			m_sStop=FALSE;
			m_sFile=fptr;
		}
		rval = Load( pDS, 0, fptr, Num );
		if(!m_Streamed) fclose( fptr );
		m_Filename = Filename;
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a WAVE file from Filepointer. Num is the number of times this sound 
// is likely to be played simultaneously, multiple buffers may be created 
// if necessary so try to keep this number small.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Load(CDXSound* pDS, LONG lSize, FILE *fptr, int Num)
{
    HRESULT rval;
    CHAR* lpCache;
    int length, save;

	// Release all allocated Memory, befor continue
	Release( );

    length = lSize;

    // If lSize equals zero get the size of the file.
    if(length == 0)
    {
        // Save the pointer location
        save = ftell(fptr);
        if(ferror(fptr))
            return NULL;

        // Seek to end of file
        fseek(fptr, 0, SEEK_END);
        if(ferror(fptr))
            return NULL;

        // Get the size of the file
        length = ftell(fptr);
        if(ferror(fptr))
            return NULL;

        // Seek back to save position
        fseek(fptr, save, SEEK_SET);
        if(ferror(fptr))
            return NULL;

    }

    // Cache the whole file in memory
    // Allocate memory to hold the data
    lpCache = new CHAR[length];
    if(lpCache == NULL)
        return NULL;

    // Read in the data
    fread(lpCache, 1, length, fptr);
    if(ferror(fptr))
    {
        delete [] lpCache;
        return NULL;
    }

    // Load it from memory.
    rval = Extract(pDS, length, lpCache, Num);

    // Delete the cache
    delete [] lpCache;

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a WAVE file from a fstream pointer. Num is the number of times this sound 
// is likely to be played simultaneously, multiple buffers may be created 
// if necessary so try to keep this number small.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Load(CDXSound* pDS, LONG lSize, CDXFile* fsptr, int Num)
{
    HRESULT rval;
    LONG length, save;
    char* lpCache;

    // If size is zero get the length of file.
    length = lSize;
    if(lSize == 0)
    {
        // Get pointer location
        save = fsptr->Position();

        // Get file length
        length = fsptr->FileSize();

        // Seek back to save location
        fsptr->Position( save, FILE_BEGIN );
    }
   
    // Allocate space for the data
    lpCache = new char[length];
    if( lpCache == NULL )
        return -1;

    // Load the cache
    if( fsptr->Read(lpCache, length)==0 )
    {
        // Delete the cache
        delete [] lpCache;

        return -1;
    }

    // Load it from memory.
    rval = Extract(pDS, length, lpCache, Num);

    // Delete the cache
    delete [] lpCache;

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a WAVE file from a fstream pointer. Num is the number of times this sound 
// is likely to be played simultaneously, multiple buffers may be created 
// if necessary so try to keep this number small.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Load(CDXSound* pDS, LONG lSize, CHAR* lpCache, int Num)
{
    HRESULT rval;
    
    rval = Extract(pDS, lSize, lpCache, Num);

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Loads a WAVE file from [WAV] Resource. Num is the number of times this sound 
// is likely to be played simultaneously, multiple buffers may be created 
// if necessary so try to keep this number small.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::LoadFromRes( CDXSound* pDS, const char *Filename, int Num )
{
	if( Filename == NULL ) return DSERR_INVALIDPARAM;
	
	HRSRC hResInfo;
	HGLOBAL hResData;
	void *pvRes;
	
	if((( hResInfo = FindResource(NULL, Filename, "WAV")) == NULL ) ) return DSERR_GENERIC;
	if(( hResData = LoadResource(NULL, hResInfo)) == NULL) return DSERR_GENERIC;
	if(( pvRes = LockResource(hResData)) == NULL) return DSERR_GENERIC;
	
	return CDXSoundBuffer::Extract( pDS, 0, (char*)pvRes, Num);
}

/////////////////////////////////////////////////////////////////////
// This function creates a sound buffer for a WAV file already loaded
//in memory. It's the same as Load.
/////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Extract(CDXSound* pDS, LONG lSize, char *ptr, int Num)
{
    // NOTE: lSize is not used at this time but is included to be consistent
    //       with the rest of the library and also because it might be necessary
    //       if additional sound formats are to be supported.
	HRESULT rval;

	// Release all memory, befor continue
	Release();
	
	// Check the Parameters
	if( ptr == NULL ) return DSERR_INVALIDPARAM;
	if( pDS == NULL ) return DSERR_INVALIDPARAM;
	if( pDS->GetDS() == NULL ) return DSERR_UNINITIALIZED;
	if( m_Streamed ) return DSERR_INVALIDPARAM;

	if( Num<1 ) Num = 1;
	
	m_pDS      = pDS;
	m_nBuffers = Num;
	m_Filename = NULL;
	
	if( strncmp( ptr, "RIFF", 4 ) || strncmp( ptr+8, "WAVE", 4 ) )
		return DSERR_BADFORMAT;

	DWORD hSize = *(DWORD*)(ptr+16);
	LPWAVEFORMATEX lpWav = (LPWAVEFORMATEX)(ptr+20);
	
	// skip all "fact" and search for the data
	// the fact stores the uncompressed filesize
	DWORD dSize = 0;
	ptr += 20 + hSize;
	do
	{
		if( dSize ) ptr += dSize + 8;
		    dSize = *(DWORD*)(ptr+4);
	} while( !strncmp( ptr, "fact", 4 ) );
	ptr += 8;

	// Is it compressed or a normal PCM Wave File
	if( lpWav->wFormatTag == WAVE_FORMAT_PCM )
	{
		// Create the sound buffer for the wave file
		rval = CreateSoundBuffer( dSize, (PCMWAVEFORMAT*)lpWav, Num );
		if(FAILED(rval)) return rval;
			
		// Read the data for the wave file into the sound buffer
		rval = ReadDataFromMemory( ptr, dSize );
		if(FAILED(rval)) return rval;

	}
	else
	{
#ifdef CDX_ACM_SUPPORT
		rval = ACMDecode( lpWav, (BYTE*)ptr, dSize );
#else
		rval = DSERR_UNSUPPORTED;
#endif
		if(FAILED(rval)) return rval;
	}
		
	// Duplicate Buffers
	for( int i = 1; i < m_nBuffers; i++ )
	{
		rval = m_pDS->GetDS()->DuplicateSoundBuffer(m_lpDSB[0], &m_lpDSB[i]);
		if(FAILED(rval)) return rval;

		if( m_3d )
		{
#if DIRECTDRAW_VERSION >= CDX_DDVER
			rval = m_lpDSB[0]->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&m_lp3dBuffer[i]);
			if(FAILED(rval)) return rval;
#endif
		}
	}
	
	return DS_OK;
	//return DSERR_UNSUPPORTED;
}

//////////////////////////////////////////////////////////////////////////////////
// Plays the loaded WAV file. Without adjusting the Pan, Volume and frequency.
// If the dwFlags parameter is set to DSBPLAY_LOOPING then the sound will play
// continually until Stop is called.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Play( DWORD dwFlags )
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;
	
	if( m_pDS==NULL ) return DSERR_UNINITIALIZED;
	if( m_pDS->GetSoundOn( ) ) return DS_OK;
	
	Buffer = GetFreeBuffer();
	if( Buffer==NULL ) return DSERR_ALLOCATED;
	
	if(m_Streamed)
	{
		m_sLoop=dwFlags;
		dwFlags=DSBPLAY_LOOPING;
	}
	rval=Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;
	
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Plays the loaded WAV file. On a stereo sound card you can adjust the Pan 
// parameter to play sounds from the left or right speaker, the range of values 
// is from 10,000 to -10,000. If the dwFlags parameter is set to DSBPLAY_LOOPING 
// then the sound will play continually until Stop is called.
// As additional function you can set the frequency in which the sound is played
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::PlayExtended( DWORD dwFlags, int Pan, int Volume, DWORD freq )
{
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;
	
	if( m_pDS==NULL ) return DSERR_UNINITIALIZED;
	if( m_pDS->GetSoundOn( ) ) return DS_OK;
	
	Buffer=GetFreeBuffer();
	if(Buffer==NULL) return DSERR_ALLOCATED;
	
	rval=Buffer->SetPan(Pan);
	if(FAILED(rval)) return rval;
	
	rval=Buffer->SetVolume(Volume);
	if(FAILED(rval)) return rval;
	
	rval=Buffer->SetFrequency(freq);
	if(FAILED(rval)) return rval;
	
	if(m_Streamed)
	{
		m_sLoop=dwFlags;
		dwFlags=DSBPLAY_LOOPING;
	}
	rval=Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;
	
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Plays the loaded WAV file. You need to set the x, y and z for the place from
// where the sound will come. If the dwFlags parameter is set to DSBPLAY_LOOPING 
// then the sound will play continually until Stop is called.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::PlayExtended( float x, float y, float z, DWORD dwFlags )
{
#if DIRECTDRAW_VERSION >= CDX_DDVER
	HRESULT rval;
	LPDIRECTSOUNDBUFFER Buffer = NULL;
	
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->GetSoundOn()) return DS_OK;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	
	Buffer=GetFreeBuffer();
	if(Buffer==NULL) return DSERR_ALLOCATED;
	if(m_lp3dBuffer[m_Current]==NULL) return DSERR_UNINITIALIZED;
	
	rval=m_lp3dBuffer[m_Current]->SetPosition( x, y, z, DS3D_IMMEDIATE );
	if(FAILED(rval)) return rval;
	
	rval = Buffer->Play(0, 0, dwFlags);
	if(FAILED(rval)) return rval;
	
	return DS_OK;
#else
	return DSERR_UNSUPPORTED;
#endif
}
//////////////////////////////////////////////////////////////////////////////////
// Stops the current WAV file.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Stop()
{
	HRESULT rval;
	
	if( m_pDS==NULL || m_lpDSB==NULL ) return DSERR_UNINITIALIZED;
	
	for(int i=0; i<m_nBuffers; i++)
	{
		if( m_lpDSB[i] )
		{
			rval=m_lpDSB[i]->Stop();
			if( rval!=DS_OK ) return rval;
	
			rval=m_lpDSB[i]->SetCurrentPosition(0);
			if( rval!=DS_OK ) return rval;
		}
	}
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Changes the volume of the sound buffer. Values range from 0 to -10,000 
// (sounds cannot be amplified).
//////////////////////////////////////////////////////////////////////////////////
void CDXSoundBuffer::SetVolume(LONG Volume)
{
	if(m_lpDSB!=NULL)
	{
		for(int i=0; i<m_nBuffers; i++)
			if( m_lpDSB[i] )
				m_lpDSB[i]->SetVolume(Volume);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Changes the pan of the sound buffer. Values range from 10,000 to -10,000
//////////////////////////////////////////////////////////////////////////////////
void CDXSoundBuffer::SetPan(LONG Pan)
{	
	if(m_lpDSB!=NULL)
	{
		for(int i=0; i<m_nBuffers; i++)
			if(m_lpDSB[i])
				m_lpDSB[i]->SetPan(Pan);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Changes the pan of the sound buffer. Values range from 10,000 to -10,000
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSoundBuffer::Playing( void )
{
	DWORD ctr=0, Status;
	if(m_lpDSB)
		for( int i=0; i<m_nBuffers; i++ )
			if(m_lpDSB[i])
				if( SUCCEEDED( m_lpDSB[i]->GetStatus(&Status) ) )
					if( Status & DSBSTATUS_PLAYING )
						ctr++;
					
	return ctr;
}

//////////////////////////////////////////////////////////////////////////////////
// Changes the parameters of the Soundcone of a 3d Buffer.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::SetCone( float x, float y, float z, DWORD InsideAngle, DWORD OutsideAngle, long OutsideVolume )
{
#if DIRECTDRAW_VERSION >= CDX_DDVER
	HRESULT rval;
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->Get3dListener()==NULL) return DSERR_UNINITIALIZED;
	
	for( int i=0; i<m_nBuffers; i++ )
	{
		if(m_lp3dBuffer[i])
		{
			rval=m_lp3dBuffer[i]->SetConeOrientation( x, y, z, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetConeAngles( InsideAngle, OutsideAngle, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetConeOutsideVolume( OutsideVolume, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
		}
	}
	
	rval=m_pDS->Get3dListener()->CommitDeferredSettings();
	return rval;
#else
	return DSERR_UNSUPPORTED;
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Allows you to change all the other Properties of a 3d SoundBuffer
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::Set3dProperties( DWORD Mode, float MinDistance, float MaxDistance, float VelX, float VelY, float VelZ )
{
#if DIRECTDRAW_VERSION >= CDX_DDVER
	HRESULT rval;
	if(m_pDS==NULL) return DSERR_UNINITIALIZED;
	if(m_lp3dBuffer==NULL) return DSERR_UNINITIALIZED;
	if(m_pDS->Get3dListener()==NULL) return DSERR_UNINITIALIZED;
	
	for( int i=0; i<m_nBuffers; i++ )
	{
		if(m_lp3dBuffer[i])
		{
			rval=m_lp3dBuffer[i]->SetMode( Mode, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetMinDistance( MinDistance, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetMaxDistance( MaxDistance, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
			
			rval=m_lp3dBuffer[i]->SetVelocity( VelX, VelY, VelZ, DS3D_DEFERRED );
			if(FAILED(rval)) return rval;
		}
	}
	
	rval=m_pDS->Get3dListener()->CommitDeferredSettings();
	return rval;
#else
	return DSERR_UNSUPPORTED;
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Fills the streaming Buffer (internal)
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::FillStreamBuffer(int nr)
{
	LPVOID pData;
	DWORD  dwDataSize;
	HRESULT rval;
	DWORD numread;

	if(!m_Streamed) return DSERR_UNINITIALIZED;
	if(m_lpDSB==NULL) return DSERR_UNINITIALIZED;
	if(m_sFile==NULL) return DSERR_UNINITIALIZED;

	rval = m_lpDSB[0]->Lock(nr*m_sSize, m_sSize, &pData, &dwDataSize, 0, 0, 0);
	if(FAILED(rval)) return rval;

	numread=fread( pData, 1, dwDataSize, m_sFile );

	if( numread<dwDataSize )
	{
		if(m_sLoop)
		{
			fseek( m_sFile, -(long)m_sWaveLength, SEEK_CUR );
			fread( (char*)pData+numread, 1, dwDataSize-numread, m_sFile );
		}
		else
		{
			BYTE fill=128;
			if( numread>0 )	fill=*((BYTE*)pData+numread-1);
			FillMemory( (BYTE*)pData+numread, dwDataSize-numread, fill );

			if( m_sDone==255 )
			{
				m_sDone=nr;
				m_sStop=FALSE;
			}
		}
	}
	m_lpDSB[0]->Unlock(pData,dwDataSize,0,0);
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Need to be called with every gametick to update the streamed sound buffer.
// It will fill allready played parts with new data.
// The FillAll variable allows you to choose if you want to fill all played parts
// or only the last.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::UpdateStream( BOOL FillAll )
{
	HRESULT rval;
	if(!m_Streamed) return DSERR_UNINITIALIZED;
	if(m_lpDSB==NULL) return DSERR_UNINITIALIZED;
	
	DWORD pos;
	rval=m_lpDSB[0]->GetCurrentPosition( NULL, &pos );
	if(FAILED(rval)) return rval;
	pos/=m_sSize;

	if( m_sDone<255 )
	{
		if( m_sDone!=pos )
		{
			if( m_sStop )
			{
				Stop();
				return 1;
			}
		}
		else m_sStop=TRUE;
	}

	do
	{
		if(pos!=m_sCurrent)
		{
			rval=FillStreamBuffer(m_sCurrent);
			if(FAILED(rval)) return rval;

			char str[255];
			sprintf( str, "PlayPos:%i\tFillPos:%i\tEndPos:%i\n", pos, m_sCurrent, m_sDone );
			ODS( str );

			m_sCurrent++;
			if( m_sCurrent>=m_sBlocks ) m_sCurrent=0;
		}
	}
	while( FillAll && pos!=m_sCurrent );
	
	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Uses the Audio Compression Manager to decompress a wave file (internal)
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSoundBuffer::ACMDecode( WAVEFORMATEX *wfm, BYTE *srcbuf, DWORD srclen )
{
#ifdef CDX_ACM_SUPPORT
	HRESULT rval;
	WAVEFORMATEX dest;
	CopyMemory( &dest, wfm, sizeof(WAVEFORMATEX) );
	dest.wFormatTag=WAVE_FORMAT_PCM;
	dest.wBitsPerSample=8;	// how would i find out what the user wants ? (8/16?)
	dest.nBlockAlign=dest.nChannels*dest.wBitsPerSample/8;
	dest.nAvgBytesPerSec=dest.nSamplesPerSec*dest.nBlockAlign;

	HACMSTREAM has=NULL;
	if( acmStreamOpen( &has , NULL, wfm, &dest, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME ) )
		return DSERR_BADFORMAT;

	DWORD destSize;
	if( acmStreamSize( has, srclen, &destSize, ACM_STREAMSIZEF_SOURCE ) )
		return DSERR_BADFORMAT;

	rval=CreateSoundBuffer( destSize, (PCMWAVEFORMAT*)&dest, m_nBuffers );
	if(FAILED(rval)) return rval;

	DWORD dwData1Size;
	LPVOID pData1;

	rval=m_lpDSB[0]->Lock(0, 0, &pData1, &dwData1Size, 0, 0, DSBLOCK_ENTIREBUFFER);
	if(FAILED(rval)) return rval;

	ACMSTREAMHEADER pash;
	ZeroMemory( &pash, sizeof( ACMSTREAMHEADER ) );
	pash.cbStruct=sizeof( ACMSTREAMHEADER );
	pash.pbSrc=srcbuf;
	pash.cbSrcLength=srclen;
	pash.pbDst=(BYTE*)pData1;
	pash.cbDstLength=dwData1Size;

	if( acmStreamPrepareHeader( has, &pash, NULL ))
		return DSERR_BADFORMAT;

	if( acmStreamConvert( has, &pash, ACM_STREAMCONVERTF_BLOCKALIGN ) )
		return DSERR_BADFORMAT;

	acmStreamUnprepareHeader( has, &pash, NULL );
	m_lpDSB[0]->Unlock( pData1, dwData1Size, 0, 0 );
	acmStreamClose( has, NULL );

	return DS_OK;
#else
	return DSERR_UNSUPPORTED;
#endif
}
