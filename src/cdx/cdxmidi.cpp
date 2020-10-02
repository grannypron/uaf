//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxmidi.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxmidi.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.5  2000/09/10 02:41:42  istan
// Replaced a _getcwd call with a Win32 GetCurrentDirectory so it would compile on BCB
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
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxmidi.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "cdxmidi.h"

//////////////////////////////////////////////////////////////////////////////////
// Default Constructor
//////////////////////////////////////////////////////////////////////////////////
CDXMidi::CDXMidi(void)
{
    m_hWnd       = NULL;
    m_ActiveMidi = -1;

	for(int i=0; i < CDX_NUM_SEGMENTS; i++)
	{
#ifdef NTDX3
        m_Midi[i].fileName = NULL;
#else
        m_Midi[i].m_segment   = NULL;  
	    m_Midi[i].m_segstate  = NULL;  
#endif
		m_Midi[i].id       = i;
		m_Midi[i].state    = MIDI_NULL;
	}	
}

//////////////////////////////////////////////////////////////////////////////////
// Default Destructor
// -=Note=-
// You cannot do Shutdown error checking with this method
//////////////////////////////////////////////////////////////////////////////////
CDXMidi::~CDXMidi(void)
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////
// Init
// Initalizes CDXMidi, CDXSound is not used in the DX3 version
// returns FALSE if any errors occur
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::Init(HWND hWnd, CDXSound *pCDXDS)
{
	m_hWnd       = hWnd;

#ifndef NTDX3

	BOOL UseCDXSound;

	//check for CDXSound...
	if(pCDXDS != NULL)
	{
		if(pCDXDS->GetDS() != NULL)
		{
			UseCDXSound = TRUE;
		}
		else
			return -1;
	}
	else
		UseCDXSound = FALSE;

	// set up directmusic
	// initialize COM
	if (FAILED(CoInitialize(NULL)))
	{
		return -2;
	}

	// create the performance
	if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance,
                            NULL,
                            CLSCTX_INPROC,
                            IID_IDirectMusicPerformance,
                            (void**)&m_Performance)))    
	{       
	    return -3;
	}

	if(!UseCDXSound)
	{
		if(FAILED(m_Performance->Init(NULL, NULL, NULL)))
		{
			Shutdown();
			return -4;
		}
	}
	else
	{
		if(FAILED(m_Performance->Init(NULL, pCDXDS->GetDS(), NULL)))
		{
			Shutdown();
			return -5;
		}
	}


	// add the port to the performance
	if (FAILED(m_Performance->AddPort(NULL)))
	{    
	    return -6;
	}

	// create the loader to load midi files
	if (FAILED(CoCreateInstance(
			CLSID_DirectMusicLoader,
			NULL,
			CLSCTX_INPROC, 
			IID_IDirectMusicLoader,
			(void**)&m_Loader)))
	{
	    return -7;
	}
#endif

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Shutdown
// Stops playback of the current Midi, and removes all "tracks"
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::Shutdown()
{
    HRESULT rval;

#ifdef NTDX3

	if( (rval = mciSendString("close all", NULL, 0, NULL)) != 0)
		return -1;
	
	rval = RemoveAll();

#else

    // If there is any music playing, stop it.
	if (m_Performance)
		m_Performance->Stop(NULL, NULL, 0, 0 ); 

	// delete all the midis if they already haven't been
	rval = RemoveAll();

	// CloseDown and Release the performance object.    
	if(m_Performance)
	{
		m_Performance->CloseDown();
		m_Performance->Release();  
		m_Performance = NULL;
	}

	// Release the loader object.
	if(m_Loader)
	{
		m_Loader->Release();
		m_Loader = NULL;
	}

	// Release COM
	CoUninitialize(); 

#endif

    return rval;

}

//////////////////////////////////////////////////////////////////////////////////
// LoadMidi
// Takes a file name as param
// Returns the midi ID of that file, -1 if error
//////////////////////////////////////////////////////////////////////////////////
int CDXMidi::LoadMidi(const char *name)
{
    int id = -1;
	int index;

#ifdef NTDX3

	for(index=0; index<CDX_NUM_SEGMENTS; index++)
	{
		if(m_Midi[index].state == MIDI_NULL)
		{
			id = index;
			break;
		}
	}

	if(id == -1)
		return -1;


    m_Midi[id].fileName = new char[256];
	strncpy(m_Midi[id].fileName, name, 256);
	m_Midi[id].state = MIDI_LOADED;

#else

    DMUS_OBJECTDESC ObjDesc; 
	HRESULT hr;
	IDirectMusicSegment* pSegment = NULL;

	for (index = 0; index < CDX_NUM_SEGMENTS; index++)
	{
	    // is this one open
	    if (m_Midi[index].state == MIDI_NULL)
        {
			// validate id, but don't validate object until loaded
			id = index;
			break;
		}
	}

	// found good id?
	if (id == -1)
		return -1;

	// get current working directory
	char szDir[_MAX_PATH];
	WCHAR wszDir[_MAX_PATH]; 

	if(GetCurrentDirectory(_MAX_PATH, szDir) == 0)
	{
		return -1;
	}

	MULTI_TO_WIDE(wszDir, szDir);

	// tell the loader were to look for files
	hr = m_Loader->SetSearchDirectory(GUID_DirectMusicAllTypes,wszDir, FALSE);

	if (FAILED(hr)) 
	{
		return -1;
	}

	// convert filename to wide string
	WCHAR wfilename[_MAX_PATH]; 
	MULTI_TO_WIDE(wfilename, name);
 
	// setup object description
	ObjDesc.dwSize = sizeof(ObjDesc);
	ObjDesc.guidClass = CLSID_DirectMusicSegment;
	wcscpy(ObjDesc.wszFileName, wfilename );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
 
	// load the object and query it for the IDirectMusicSegment interface
	hr = m_Loader->GetObject(&ObjDesc,IID_IDirectMusicSegment, (void**) &pSegment);

	if (FAILED(hr))
		return -1;
 
	// ensure that the segment plays as a standard MIDI file
	hr = pSegment->SetParam(GUID_StandardMIDIFile,-1, 0, 0, (void*)m_Performance);

	if (FAILED(hr))
		return -1;
  
	// The next step is to download the instruments. 
	hr = pSegment->SetParam(GUID_Download, -1, 0, 0, (void*)m_Performance);

	if (FAILED(hr))
		return -1;

	// at this point we have MIDI loaded and a valid object

	m_Midi[id].m_segment  = pSegment;
	m_Midi[id].m_segstate = NULL;
	m_Midi[id].state      = MIDI_LOADED;

#endif

	return id;
}

//////////////////////////////////////////////////////////////////////////////////
// Play
// Plays the midi file specified by the MidiID
// Returns FALSE on error
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::Play(int MidiID)
{
    HRESULT rval;

#ifdef NTDX3

    char buffer[256];

	if((m_Midi[MidiID].fileName == NULL) && (m_Midi[MidiID].state != MIDI_LOADED))
		return -1;

	sprintf(buffer, "open %s type sequencer alias MUSIC", m_Midi[MidiID].fileName);

	if ( (rval = mciSendString("close all", NULL, 0, NULL) ) != 0)
		return -2;


	if ( (rval = mciSendString(buffer, NULL, 0, NULL) ) != 0)
		return -3;


	if ( (rval = mciSendString("play MUSIC from 0 notify", NULL, 0, m_hWnd) ) != 0)
		return -4;


	m_Midi[MidiID].state = MIDI_PLAYING;
	m_ActiveMidi         = MidiID;

#else

    if (m_Midi[MidiID].m_segment && m_Midi[MidiID].state!=MIDI_NULL)
	{
		// if there is an active midi then stop it
		if (m_ActiveMidi != -1)
			Stop(m_ActiveMidi);
	
		// play segment and force tracking of state variable
		rval = m_Performance->PlaySegment(m_Midi[MidiID].m_segment, 0, 0, &m_Midi[MidiID].m_segstate);
        if( FAILED(rval) )
            return rval;

		m_Midi[MidiID].state = MIDI_PLAYING;
	
		// set the active midi segment
		m_ActiveMidi = MidiID;

        // small bug, without this sometimes it won't run...
		Sleep(100);  
	} 
	else
		rval = -1;
#endif

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Stop
// Stops current midi specified by MidiID
// Returns FALSE on error
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::Stop(int MidiID)
{
    HRESULT rval;

#ifdef NTDX3

	if ( (rval = mciSendString("close all", NULL, 0, NULL)) != 0)
		return -1;
 
	m_Midi[MidiID].state = MIDI_STOPPED;

#else

    if (m_Midi[MidiID].m_segment && m_Midi[MidiID].state!=MIDI_NULL)
	{
		// play segment and force tracking of state variable
		rval = m_Performance->Stop(m_Midi[MidiID].m_segment, NULL, 0, 0);
        if( FAILED(rval) )
            return rval;

		m_Midi[MidiID].state = MIDI_STOPPED;
	}
	else
		rval = -1;
#endif

    // reset active id
    m_ActiveMidi = -1;

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// Remove
// Cleans memory and resets the status of the "track"
// Returns FALSE on error
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::Remove(int MidiID)
{
    HRESULT rval;

#ifdef NTDX3

	if( (rval = Stop(MidiID)) != 0)
		return rval;

	if(m_Midi[MidiID].fileName != NULL)
    {
        delete [] m_Midi[MidiID].fileName;
        m_Midi[MidiID].fileName = NULL;
    }

	m_Midi[MidiID].state = MIDI_NULL;

#else

    if (m_Midi[MidiID].m_segment)
	{
		m_Midi[MidiID].m_segment->SetParam(GUID_Unload, -1, 0, 0, (void*)m_Performance); 

		// Release the segment and set to null
		m_Midi[MidiID].m_segment->Release(); 
		m_Midi[MidiID].m_segment  = NULL;
		m_Midi[MidiID].m_segstate = NULL;
		m_Midi[MidiID].state      = MIDI_NULL;

		rval = 1;
	}
	else
		rval = -1;

#endif

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// RemoveAll
// Clears all midi "tracks"
// If one fails it will return immediatly
// Recommended to verify the return and manually remove "tracks" on FALSE
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::RemoveAll()
{
    HRESULT rval;

	for(int i=0; i<CDX_NUM_SEGMENTS; i++)
	{
		if( (rval = Remove(i)) != 0)
			return rval;
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// IsPlaying
// Returns TRUE if "track" at MidiID is playing
// FALSE if not
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXMidi::IsPlaying(int MidiID)
{
    BOOL rval;

#ifndef NTDX3

    if (m_Midi[MidiID].m_segment && m_Midi[MidiID].state !=MIDI_NULL )
	{
		// get the status and translate to our defines
		if (m_Performance->IsPlaying(m_Midi[MidiID].m_segment,NULL) == S_OK) 
			m_Midi[MidiID].state = MIDI_PLAYING;
		else
			m_Midi[MidiID].state = MIDI_STOPPED;

		if(m_Midi[MidiID].state == MIDI_PLAYING)
			rval = TRUE;
		else
			rval = FALSE;
	} // end if
	else
		rval = FALSE;

#else

	if(m_Midi[MidiID].state == MIDI_PLAYING)
		rval = TRUE;
	else
		rval = FALSE;

#endif

    return rval;
}

//////////////////////////////////////////////////////////////////////////////////
// SetVolume
// Unsupported in the DX3 version at this time
// Always returns TRUE
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXMidi::SetVolume(long Vol)
{
#ifndef NTDX3

    HRESULT rval;

    if(m_Performance)
	{
		rval = m_Performance->SetGlobalParam(GUID_PerfMasterVolume, &Vol, sizeof(Vol));
		if( FAILED(rval) )
			return rval;
	}

	return rval;

#endif

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// GetVolume
// Unsupported in the DX3 version at this time
// Always returns 0
//////////////////////////////////////////////////////////////////////////////////
LONG CDXMidi::GetVolume()
{
#ifndef NTDX3

    long ret;
    HRESULT rval;

	if(m_Performance)
	{
		rval = m_Performance->GetGlobalParam(GUID_PerfMasterVolume, &ret, sizeof(ret));
		if( FAILED(rval) )
			return 0;
	}

	return ret;

#endif

	return 0;
}
