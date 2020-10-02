//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/Cdxsound.h,v $
// $Author: cocoaspud $
//
// $Log: Cdxsound.h,v $
// Revision 1.5  2003/07/12 13:29:38  cocoaspud
// updated cdxsound.h
//
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.4  2000/08/21 21:21:02  mindcry
// Removed all fstreams and exchanged them with CDXFile. I had to do this
// cause fstream wasn't working right with MFC apps/games and also in some
// circumstances it crashed or not opened the file. Sorry for breaking
// backwards compatibility in some ways. But it was necessary.
//
// Revision 1.3  2000/05/10 23:29:26  hebertjo
// Cleaned up all the old code that was commented out.  Version 3.0 is getting
// very close to release.
//
// Revision 1.2  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.1.1.1  2000/04/22 16:11:35  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.17  2000/03/04 11:48:01  Mindcry
// small bugfix to compile correct with dx6
//
// Revision 2.16  2000/03/01 09:16:42  Mindcry
// Many changes:
// - Rewritten totally the loading routines
// - Streamed playback of wavefiles
// - ACM Decompressing of wave files (define CDX_ACM_SUPPORT)
// - 3d sound support
// - exclusive mode
// - primary buffer continuos play
// - and much much more
//
// Revision 2.13  2000/02/01 12:58:46  Mindcry
// - changed the last few functions to return a HRESULT
// - fixed the strange memory (loading) bug
//
// Revision 2.12  2000/02/01 11:27:20  Mindcry
// - SetFormat and ContinuousPlay are working now correct
// - Cleaned up the code a bit more
//
// Revision 2.11  2000/01/29 00:50:57  Mindcry
// Initial Release Checkin for 3.0:
// - Playing the Primary Buffer Continuously
// - Allow the User to change the Sound Output Format
// - Exclusive Mode
// - Privatizing of the Variables and some internal functions
// - Extended Error Returning (CDXSound only)
//
// $Revision: 1.5 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXSOUND_H
#define CDXSOUND_H

// Fixes the compile problem with DirectX7 SDK
#ifndef DSBCAPS_CTRLDEFAULT
	#define DSBCAPS_CTRLDEFAULT DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
#endif

// Fixes problem with DX6 SDK
#ifndef DSERR_ACCESSDENIED
	#define DSERR_ACCESSDENIED	E_ACCESSDENIED
#endif

//////////////////////////////////////////////////////////////////////////////////
// CDXSound is a simple wrapper for a DirectSound object. It is necessary to 
// create a CDXSound object before you can load and play sound using the 
// CDXSoundBuffer object.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXSound
{
public:
	CDXSound(void);
	virtual ~CDXSound(void);

	HRESULT Create( void *hWnd, BOOL Exclusive=FALSE );
	HRESULT Enable3d( void );
	HRESULT GetCaps( void );
	HRESULT SetFormat( long Frequency, BOOL Bit16=TRUE, BOOL stereo=TRUE );
	HRESULT ContinuousPlay( BOOL Enable=TRUE );
	LPDIRECTSOUND GetDS( void ) { return m_lpDS; }
#if DIRECTDRAW_VERSION >= CDX_DDVER
	LPDIRECTSOUND3DLISTENER Get3dListener( void ) { return m_lp3DListener; }
#endif

	HRESULT Set3dFactors( float DopplerFactor=1.0, float RolloffFactor=1.0, float DistanceFactor=1.0 );
	HRESULT Place3dListener( float posX, float posY, float posZ, float frontX=0.0, float frontY=0.0, float frontZ=1.0, float topX=0.0, float topY=1.0, float topZ=0.0, BOOL setOrientation=FALSE );

	void SetSoundOn( BOOL sw ) { if( m_lpDS!=NULL ) m_SoundOff=sw; }
	BOOL GetSoundOn(void) { return m_SoundOff; };
	HRESULT SetMIDIVolume( BYTE rvol, BYTE lvol );
	HRESULT SetDIGIVolume( BYTE rvol, BYTE lvol );
	HRESULT SetCDVolume( BYTE rvol, BYTE lvol );
	DWORD GetMIDIVolume(void);
	DWORD GetDIGIVolume(void);
	DWORD GetCDVolume(void);

private:
	BOOL m_SoundOff;		// Stores if Sound Playback is on or off
	BOOL m_3dSound;			// Stores if 3d Sound is enabled
	LPDIRECTSOUND m_lpDS;	// The DirectSound Object
	DSCAPS m_DSCaps;		// Direct sound capabilities

#if DIRECTDRAW_VERSION >= CDX_DDVER
	LPDIRECTSOUND3DLISTENER m_lp3DListener;	// The DIRECTSOUND3D Listener
#endif
};

//////////////////////////////////////////////////////////////////////////////////
// A simple wrapper for a DirectSoundBuffer object. It is used to load and play 
// WAVE files for the sound effects in your game.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXSoundBuffer
{
public:
	CDXSoundBuffer( void );
	~CDXSoundBuffer( void );

	HRESULT EnableStreaming( int size=4096, int events=4 );
	HRESULT Enable3d( void );

	HRESULT Load( CDXSound* pDS, const char* Filename, int Num = 1 );
	HRESULT Load( CDXSound* pDS, LONG lSize, FILE* fptr, int Num = 1 );
    HRESULT Load( CDXSound* pDS, LONG lSize, CDXFile* fptr, int Num = 1 );
    HRESULT Load( CDXSound* pDS, LONG lSize, CHAR* lpCache, int Num = 1 );
	HRESULT LoadFromRes( CDXSound* pDS, const char* Filename, int Num = 1 );
	HRESULT Extract( CDXSound* pDS, LONG lSize, char *ptr, int Num = 1 );
	HRESULT Play( DWORD dwFlags = 0 );
	HRESULT PlayExtended( DWORD dwFlags=0, int pan=0, int Volume=0, DWORD freq=DSBFREQUENCY_ORIGINAL );
	HRESULT PlayExtended( float x, float y, float z, DWORD dwFlags=0 );
	HRESULT Stop( void );
	void SetVolume( LONG Volume );
	void SetPan( LONG Pan );
	DWORD Playing( void );
	HRESULT SetCone( float x=0, float y=1.0, float z=0, DWORD InsideAngle=DS3D_DEFAULTCONEANGLE, DWORD OutsideAngle=DS3D_DEFAULTCONEANGLE, long OutsideVolume=DS3D_DEFAULTCONEOUTSIDEVOLUME );
	HRESULT Set3dProperties( DWORD Mode=DS3DMODE_NORMAL, float MinDistance=DS3D_DEFAULTMINDISTANCE, float MaxDistance=DS3D_DEFAULTMAXDISTANCE, float VelX=1.0, float VelY=1.0, float VelZ=1.0 );
	HRESULT UpdateStream( BOOL FillAll=FALSE );
  LPDIRECTSOUNDBUFFER GetDSB() { return *m_lpDSB; }
  
protected:
	void Release( void );
	HRESULT CreateSoundBuffer( DWORD dwBufSize, PCMWAVEFORMAT *pcmwf, int Num);
	HRESULT ReadData( FILE* fp, DWORD dwSize );
	HRESULT ReadDataFromMemory( char *ptr, DWORD dwSize );
	LPDIRECTSOUNDBUFFER GetFreeBuffer( void );
	HRESULT FillStreamBuffer( int nr );
	HRESULT	ACMDecode( WAVEFORMATEX *wfm, BYTE *srcbuf, DWORD srclen );

private:
	int m_nBuffers;					// The number of sound buffers created
	int m_Current;					// The currently selected buffer
	const char* m_Filename;			// The name of the WAVE file
	CDXSound* m_pDS;				// Contains a pointer to the CDXSound object
	LPDIRECTSOUNDBUFFER *m_lpDSB;	// The IDirectSoundBuffer object
#if DIRECTDRAW_VERSION >= CDX_DDVER
	LPDIRECTSOUND3DBUFFER *m_lp3dBuffer;
#endif

	BOOL m_Streamed;				// The playback will be streamed	
	BOOL m_sLoop;
	BOOL m_sStop;
	BYTE m_sDone;
	WORD m_sSize;					// size of each stream block
	BYTE m_sBlocks, m_sCurrent;		// Stream Block Count and Current position
	DWORD m_sWaveLength;
	FILE *m_sFile;					// Filepointer for Streaming
	BOOL m_3d;						// Enables 3d functions for this Buffer
};

#endif
