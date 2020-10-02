//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmidi.h,v $
// $Author: cocoaspud $
//
// $Log: cdxmidi.h,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.4  2000/05/29 18:44:52  hebertjo
// Added the new CDXMidi stuff that uses DirectMusic.
//
// Revision 1.3  2000/05/01 17:06:26  hebertjo
// Many small changes.  Fixed a few problems identified by BoundsChecker.
// Removed some old code that was commented out.  Cleaned up the look of
// some of the code.
//
// Revision 1.2  2000/04/26 18:45:43  hebertjo
// Made many small changes to fix numerous bugs and some enhancements.
//
// Revision 1.1.1.1  2000/04/22 16:10:41  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 1.1  2000/03/20 22:51:24  jhebert
// Thanks to Mark Kruse CDX now has MIDI support.  These files will work under
// NT and '9X.  He has also done a MIDI implementation using Direct Music but
// we need to upgrade the library to DX7 first.
//
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////

#ifndef CDXMIDI_H
#define CDXMIDI_H

#include "cdx.h"

#ifdef NTDX3

typedef struct CDX_MIDI_TYP
{
	char*	fileName;
	int		id;	              // ID of where the file is in the array
	int		state;            // current state of the midi file
} CDX_MIDI, *CDX_MIDI_PTR;

#else

#include <wchar.h>
#include <direct.h>

#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#define MULTI_TO_WIDE( x,y )  MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, y,-1,x,_MAX_PATH);

typedef struct CDX_MIDI_TYP
{
	IDirectMusicSegment			*m_segment;
	IDirectMusicSegmentState	*m_segstate;
	int							id;				// ID of where the file is in the array
	int							state;			// current state of the midi file
} CDX_MIDI, *CDX_MIDI_PTR;

#endif

#include "cdxsound.h"

#define CDX_NUM_SEGMENTS 64	  // maximum number of midi files that can be loaded
#define MIDI_NULL        0	  // midi has not been loaded
#define MIDI_LOADED      1	  // midi has been loaded, but is not active
#define MIDI_PLAYING     2	  // midi is currently playing
#define MIDI_STOPPED     3	  // midi is current midi, but not playing

class CDXMidi
{
public:
    CDXMidi();
	virtual ~CDXMidi();

	HRESULT Init(HWND hWnd, CDXSound* pCDXDS = NULL);
	HRESULT Shutdown();                                // called in ~CDXMidi

	int     LoadMidi(const char * name);
	HRESULT Play(int MidiID);
	HRESULT Stop(int MidiID);
	HRESULT Remove(int MidiID);
	HRESULT RemoveAll();
	BOOL    IsPlaying(int MidiID);
	HRESULT SetVolume(long Vol);
	long    GetVolume();

	CDX_MIDI                 m_Midi[CDX_NUM_SEGMENTS]; // array of midi's and data
	int                      m_ActiveMidi;             // current id of active midi
	HWND                     m_hWnd;
#ifndef NTDX3
    IDirectMusicPerformance* m_Performance;
	IDirectMusicLoader*      m_Loader;
#endif
};

#endif CDXMIDI_H