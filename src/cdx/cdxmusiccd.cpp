//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmusiccd.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxmusiccd.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:47  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.3  1999/08/04 18:32:02  janni
// fixed stop bug, stop did not work (thanks to Perry Robertson)
//
// Revision 2.2  1999/05/27 05:34:14  janni
// changed destructor which stops the music when the object is destroyed
//
// Revision 2.1  1999/05/20 15:29:03  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxmusiccd.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxmusic.h"

#define MAX_TRACKS 100

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXMusicCd::CDXMusicCd(void)
{
	m_MCIOpen.wDeviceID = 0;
	m_nNumberOfTracks = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
CDXMusicCd::~CDXMusicCd(void)
{
	Stop();
}

//////////////////////////////////////////////////////////////////////////////////
// Reads an audio CD.
//////////////////////////////////////////////////////////////////////////////////
short CDXMusicCd::Read(void)
{
	int i;
	short nTrackLength;

	m_nNumberOfTracks = 0;
	m_MCIOpen.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;

	if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID, (DWORD)(LPVOID)&m_MCIOpen))
	{
		return 0;
	}	
		
	m_MCIStatus.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if(mciSendCommand(m_MCIOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (DWORD)(LPVOID)&m_MCIStatus))
	{
		mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, NULL, NULL);
		return 0;
	}

	m_nNumberOfTracks = (short)m_MCIStatus.dwReturn;
	if(m_nNumberOfTracks > MAX_TRACKS) m_nNumberOfTracks = MAX_TRACKS;
	m_MCIStatus.dwItem = MCI_STATUS_LENGTH;

	for(i = 0; i < m_nNumberOfTracks; i++)
	{
		m_MCIStatus.dwTrack = i + 1;
		mciSendCommand(m_MCIOpen.wDeviceID, MCI_STATUS, MCI_TRACK|MCI_STATUS_ITEM|MCI_WAIT,	(DWORD)(LPVOID)&m_MCIStatus);
		nTrackLength = (short)(MCI_MSF_MINUTE(m_MCIStatus.dwReturn)*60 + MCI_MSF_SECOND(m_MCIStatus.dwReturn));
		m_nTrackLength[i] = nTrackLength;
	}

	mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, NULL, NULL);

	return m_nNumberOfTracks;
}

//////////////////////////////////////////////////////////////////////////////////
// Gets the current track length for a given track.
//////////////////////////////////////////////////////////////////////////////////
short	CDXMusicCd::GetTrackLength(short nTrack)
{
	if(nTrack > 0 && nTrack <= m_nNumberOfTracks) return m_nTrackLength[nTrack-1];
	else return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the track length for each track on a CD.
//////////////////////////////////////////////////////////////////////////////////
void CDXMusicCd::SetTrackLength(short nTrack, short nNewLength)
{
	if(nTrack > 0 && nTrack <= m_nNumberOfTracks)
		m_nTrackLength[nTrack-1] = nNewLength;
}

//////////////////////////////////////////////////////////////////////////////////
// Gets the total length of the CD contents.
//////////////////////////////////////////////////////////////////////////////////
short CDXMusicCd::GetTotalLength(void)
{
	short nTotalLength = 0;
	short nTrack;

	for(nTrack = 0; nTrack < m_nNumberOfTracks; nTrack++)
		nTotalLength = (short)(nTotalLength + m_nTrackLength[nTrack]);

	return nTotalLength;
}

//////////////////////////////////////////////////////////////////////////////////
// Plays a track from a CD.
//////////////////////////////////////////////////////////////////////////////////
void CDXMusicCd::Play(short nTrack)
{
	MCI_SET_PARMS	mciSet;
	MCI_PLAY_PARMS mciPlay;

	m_MCIOpen.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
	if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID, (DWORD)&m_MCIOpen))
	{
		return;
	}

	// Set the time format to track/minute/second/frame (TMSF)
	mciSet.dwTimeFormat = MCI_FORMAT_TMSF;
	if(mciSendCommand(m_MCIOpen.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)&mciSet))
	{
		mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, 0, NULL);
		return;
	}

	mciPlay.dwCallback = 0;
	mciPlay.dwFrom = MCI_MAKE_TMSF(nTrack, 0, 0, 0);
	if(mciSendCommand(m_MCIOpen.wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)&mciPlay))
	{
		return;
	}

	mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, 0, NULL);
}

//////////////////////////////////////////////////////////////////////////////////
// Stops playing music.
//////////////////////////////////////////////////////////////////////////////////
void CDXMusicCd::Stop(void)
{
  if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID,
                   (DWORD)(LPVOID)&m_MCIOpen))
  {
      return;
  }

  if (m_MCIOpen.wDeviceID != -1)
  {
    MCIERROR err = mciSendCommand( m_MCIOpen.wDeviceID, MCI_STOP, NULL, NULL );
  }

  mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, 0, NULL);
}
