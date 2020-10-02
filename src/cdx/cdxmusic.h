//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmusic.h,v $
// $Author: cocoaspud $
//
// $Log: cdxmusic.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:44  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.3  2000/01/21 19:05:08  jhebert
// Destructor is now virtual.
//
// Revision 2.2  1999/08/29 22:29:04  MICHAELR
// added CDXDLL support to classes/etc
//
// Revision 2.1  1999/05/27 05:33:41  janni
// added destructor which stops the music when the object is destroyed
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifndef CDXMUSIC_H
#define CDXMUSIC_H

//////////////////////////////////////////////////////////////////////////////////
// CDXMusic allows you to load and play MIDI files for the music in your game. 
// CDXMusic currently uses the Windows MCI functions and in terms of performance 
// is quite demanding.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXMusic
{
public:
	CDXMusic(void *hWnd);
	virtual ~CDXMusic();

	BOOL Play(const char *Filename);
	BOOL Stop(void);
	BOOL Pause(void);
	BOOL Resume(void);
	BOOL Restart(void);

public:
	HWND m_hWnd;				// Window handle
};

//////////////////////////////////////////////////////////////////////////////////
// CDXMusicCd allows you to load and play audio CD files for the music in your game. 
// CDXMusicCd currently uses the Windows MCI functions and in terms of performance 
// is quite demanding.
//////////////////////////////////////////////////////////////////////////////////
class CDXAPI CDXMusicCd
{
public:
	CDXMusicCd(void);
	~CDXMusicCd(void);

	short	Read(void);
	short	GetNumberOfTracks(void)	{ return m_nNumberOfTracks; }
	void  SetNumberOfTracks(short nTracks) { m_nNumberOfTracks = nTracks; }
	short	GetTrackLength(short nTrack);
	void  SetTrackLength(short nTrack, short nNewLength);
	short	GetTotalLength(void);
	void  Play(short nTrack);
	void  Stop(void);

private:
	short	m_nNumberOfTracks;
	short	m_nTrackLength[100];
	MCI_STATUS_PARMS m_MCIStatus;
	MCI_OPEN_PARMS m_MCIOpen;
};

#endif
