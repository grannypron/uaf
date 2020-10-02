//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxsound.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxsound.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:11:30  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.5  2000/03/01 09:17:16  Mindcry
// Many changes:
// - Rewritten totally the loading routines
// - Streamed playback of wavefiles
// - ACM Decompressing of wave files (define CDX_ACM_SUPPORT)
// - 3d sound support
// - exclusive mode
// - primary buffer continuos play
// - and much much more
//
// Revision 2.4  2000/02/01 11:27:20  Mindcry
// - SetFormat and ContinuousPlay are working now correct
// - Cleaned up the code a bit more
//
// Revision 2.3  2000/01/29 00:50:57  Mindcry
// Initial Release Checkin for 3.0:
// - Playing the Primary Buffer Continuously
// - Allow the User to change the Sound Output Format
// - Exclusive Mode
// - Privatizing of the Variables and some internal functions
// - Extended Error Returning (CDXSound only)
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxsound.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxsound.h"

//Makes the mixed LONG value out of the Left and Right channels...
#define MAKEVOL(right,left) MAKELONG(((right*65535L)/100), ((left*65535L)/100))

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXSound::CDXSound()
{
	m_SoundOff=TRUE;
	m_3dSound=FALSE;
	m_lpDS = NULL;
#if DIRECTDRAW_VERSION >= CDX_DDVER
	m_lp3DListener=NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Default destructor.
//////////////////////////////////////////////////////////////////////////////////
CDXSound::~CDXSound()
{
	RELEASE( m_lpDS );
}

//////////////////////////////////////////////////////////////////////////////////
// Initializes the DirectSound object. The return value contains either the 
// return value of the failed SetCooperativeLevel call, or DS_OK if the 
// function succeeds.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::Create( void *hWnd, BOOL Exclusive )
{
	HRESULT rval;

	rval = DirectSoundCreate(NULL, &m_lpDS, NULL);
	if(rval != DS_OK) return rval;

	rval = m_lpDS->SetCooperativeLevel((HWND)hWnd, (Exclusive) ? DSSCL_EXCLUSIVE : DSSCL_NORMAL );
	if(rval != DS_OK) return rval;

	m_SoundOff=FALSE;
	if( Exclusive )
		return SetFormat( 22050, TRUE, TRUE );

	return DS_OK;
}

//////////////////////////////////////////////////////////////////////////////////
// Gets the current sound capabilities. The return value contains the result 
// of the GetCaps call if the call fails. DS_OK is returned is call succeeds.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::GetCaps(void)
{
	if(m_lpDS==NULL) return DSERR_UNINITIALIZED;
	return m_lpDS->GetCaps(&m_DSCaps);
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the Format of the Primary SoundBuffer (you need to be in exclusive mode)
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::SetFormat( long Frequency, BOOL Bit16, BOOL stereo )
{
	if(m_lpDS==NULL) return DSERR_UNINITIALIZED;
	
	LPDIRECTSOUNDBUFFER lpDSB;
    DSBUFFERDESC dsbdesc;
	WAVEFORMATEX wfx;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	rval = m_lpDS->CreateSoundBuffer(&dsbdesc, &lpDSB, NULL); 
	if( FAILED(rval) ) return rval;

	// Set the desired Format
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec=Frequency;
	wfx.wBitsPerSample=(Bit16) ? 16 : 8;
	wfx.nChannels=(stereo) ? 2 : 1;
	wfx.nBlockAlign=wfx.wBitsPerSample/8*wfx.nChannels;
	wfx.nAvgBytesPerSec=Frequency*wfx.nBlockAlign;
	rval=lpDSB->SetFormat( &wfx );

	RELEASE( lpDSB );
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Lets the mixer play continuously, in order to eliminate problems associated
// with frequent short periods of silence
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::ContinuousPlay( BOOL Enable )
{
	if(m_lpDS==NULL) return DSERR_UNINITIALIZED;

	LPDIRECTSOUNDBUFFER lpDSB;
    DSBUFFERDESC dsbdesc;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	rval = m_lpDS->CreateSoundBuffer(&dsbdesc, &lpDSB, NULL);
	if( FAILED(rval) ) return rval;

	if( Enable ) rval=lpDSB->Play( 0, 0, DSBPLAY_LOOPING );
	else rval=lpDSB->Stop( );
	RELEASE( lpDSB );

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Enables 3d Sound. Creates the nessesary interfaces...
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::Enable3d( void )
{
	if(m_lpDS==NULL) return DSERR_UNINITIALIZED;

#if DIRECTDRAW_VERSION >= CDX_DDVER
	LPDIRECTSOUNDBUFFER lpDSB;
    DSBUFFERDESC dsbdesc;
	HRESULT rval;

	// Get primary SoundBuffer
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    dsbdesc.dwFlags = DSBCAPS_CTRL3D|DSBCAPS_PRIMARYBUFFER; 
	rval = m_lpDS->CreateSoundBuffer(&dsbdesc, &lpDSB, NULL);
	if( FAILED(rval) ) return rval;

	rval=lpDSB->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&m_lp3DListener);
	if( FAILED(rval) ) return rval;
	RELEASE( lpDSB );

	m_3dSound = TRUE;
	return DS_OK;
#else
	return DSERR_UNSUPPORTED;
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Allows you to change the Factors of the 3d Listener
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::Set3dFactors( float DopplerFactor, float RolloffFactor, float DistanceFactor )
{
#if DIRECTDRAW_VERSION >= CDX_DDVER
	HRESULT rval;
	if(m_lp3DListener==NULL) return DSERR_UNINITIALIZED;

	rval=m_lp3DListener->SetDopplerFactor( DopplerFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	rval=m_lp3DListener->SetRolloffFactor( RolloffFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	rval=m_lp3DListener->SetDistanceFactor( DistanceFactor, DS3D_DEFERRED );
	if(FAILED(rval)) return rval;

	return m_lp3DListener->CommitDeferredSettings();
#else
	return DSERR_UNSUPPORTED;
#endif
}

HRESULT CDXSound::Place3dListener( float posX, float posY, float posZ, float frontX, float frontY, float frontZ, float topX, float topY, float topZ, BOOL setOrientation )
{
#if DIRECTDRAW_VERSION >= CDX_DDVER
	HRESULT rval;
	if(m_lp3DListener==NULL) return DSERR_UNINITIALIZED;

	rval=m_lp3DListener->SetPosition( posX, posY, posZ, DS3D_DEFERRED ); //DS3D_IMMEDIATE
	if(FAILED(rval)) return rval;

	if( setOrientation )
	{
		rval=m_lp3DListener->SetOrientation( frontX, frontY, frontZ, topX, topY, topZ, DS3D_DEFERRED );
		if(FAILED(rval)) return rval;
	}

	return m_lp3DListener->CommitDeferredSettings();
#else
	return DSERR_UNSUPPORTED;
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the Midi Output Volume to rvol, lvol( they must betwen 0 and 100 )
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::SetMIDIVolume( BYTE rvol, BYTE lvol )
{
	MIDIOUTCAPS midiCaps;
	if( FAILED( midiOutGetDevCaps(0, &midiCaps, sizeof(midiCaps)) ) ) 
		return DSERR_NODRIVER;

	if( midiCaps.dwSupport&MIDICAPS_VOLUME )
		return midiOutSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the Digital Output Volume to rvol, lvol( they must betwen 0 and 100 )
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::SetDIGIVolume( BYTE rvol, BYTE lvol )
{
	WAVEOUTCAPS waveCaps;
	if( FAILED( waveOutGetDevCaps(0, &waveCaps, sizeof(waveCaps)) ) )
		return DSERR_NODRIVER;

	if( waveCaps.dwSupport&WAVECAPS_VOLUME )
		return waveOutSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the CD Output Volume to rvol, lvol( they must betwen 0 and 100 )
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXSound::SetCDVolume( BYTE rvol, BYTE lvol )
{
	AUXCAPS auxCaps;
	if( FAILED( auxGetDevCaps(0, &auxCaps, sizeof(auxCaps)) ) )
		return DSERR_NODRIVER;

	if( auxCaps.dwSupport&AUXCAPS_VOLUME )
		return auxSetVolume( 0, MAKEVOL(rvol, lvol) );

	return DSERR_UNSUPPORTED;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current Midi Output Volume
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSound::GetMIDIVolume( void )
{
	DWORD vol;
	midiOutGetVolume(0, &vol);
	return vol;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current Digital Output Volume
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSound::GetDIGIVolume( void )
{
	DWORD vol;
	waveOutGetVolume(0, &vol);
	return vol;
}

//////////////////////////////////////////////////////////////////////////////////
// Returns the current CD Output Volume
//////////////////////////////////////////////////////////////////////////////////
DWORD CDXSound::GetCDVolume( void )
{
	DWORD vol;
	auxGetVolume(0, &vol);
	return vol;
}