/******************************************************************************
* Filename: SoundMgr.cpp
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#include "..\Shared\stdafx.h"

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#else
#include "..\UAFWin\Dungeon.h"
#endif
#include "class.h"
#include "SoundMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UAFEDITOR
#include "..\UAFwinEd\resource.h"
#include "class.h"
#include "..\UAFwinEd\CrossReference.h"
#endif

#include "GlobalData.h"


//IMPLEMENT_SERIAL( BACKGROUND_SOUNDS, CObject, 1 )
//IMPLEMENT_SERIAL( BACKGROUND_SOUND_DATA, CObject, 1 )

#ifdef USE_BASS

#define BASSDEF(f) (WINAPI *f)	// define the functions as pointers
#include "Bass.h"
SYNCPROC BassSyncCallback;
SYNCPROC BassSyncBgSoundCallback;
DWORD ver=0;
#endif

BOOL MyDeleteFile(LPCSTR name);
BOOL MyCopyFile(LPCSTR src, LPCSTR dest, BOOL failfIfExists);


SoundMgr *pSndMgr=NULL;

// adventure sounds
CString DEF_STEP_SOUND = "Step.wav"; // non-combat move sound
CString DEF_BUMP_SOUND = "Bump.wav";

// game intro, looped
CString DEF_INTRO_SOUND = "Intro.wav";
// played when party dies, game over
CString DEF_DEATH_SOUND = "Death.wav";

// combat sounds
CString DEF_HIT_SOUND = "Hit.wav";       // attack successful - stored in item/moster data
CString DEF_MISS_SOUND = "Miss.wav";      // attack missed     - stored in item/monster data
CString DEF_LAUNCH_SOUND = "Launch.wav";  // played as attack is made, before hit or miss sounds
CString DEF_CAST_SOUND = "Cast.wav";      // spell cast        - stored in spell data
CString DEF_CHAR_DEATH_SOUND = "CharDeath.wav"; // character dies    - stored in global combat data
CString DEF_CHAR_MOVE_SOUND = "CharMove.wav";  // character moves   - stored in global combat data

HINSTANCE bassDLL = NULL;

///////////////////////////////////////////////////////////////////////////////
//
//  MIDIPlayer functions
//
///////////////////////////////////////////////////////////////////////////////

UINT MIDIPlayer::m_MIDI_Device=NULL;

//*****************************************************************************
//    NAME: MIDIPlayer::MIDIPlayer
//
// PURPOSE: 
//
//*****************************************************************************
MIDIPlayer::MIDIPlayer(HWND hWnd, const char *devname)
{
	m_hWnd = hWnd;
  m_Filename="";
  m_UseNotify=FALSE;
  //if (devname!=NULL) m_DevName=devname;
  //else m_DevName="UAFMIDI";
}

//*****************************************************************************
//    NAME: MIDIPlayer::~MIDIPlayer
//
// PURPOSE: 
//
//*****************************************************************************
MIDIPlayer::~MIDIPlayer()
{
	Stop();
  Close();
}

//*****************************************************************************
//    NAME: MIDIPlayer::Load
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MIDIPlayer::Load(const char *filename)
{
  //WriteDebugString("MIDI Load()\n");
  if (IsLoaded()) 
  {
    if (CanControlDevice())
    {
      if (Stop()) 
      {
        Close();
      }
      else
      {
        WriteDebugString("MIDI %s already loaded in MIDI::Load()\n", m_Filename);
        return FALSE;
      }
    }
    else
    {
      WriteDebugString("MIDI %s already loaded in MIDI::Load()\n", m_Filename);
      return FALSE;
    }
  }

  if (!FileExists(filename))
  {
    WriteDebugString("File %s missing in MIDI::Load()\n",filename);
    return FALSE;
  }
  m_Filename = filename;
  m_UseNotify = FALSE;
  return TRUE;
}

//*****************************************************************************
//    NAME: MIDIPlayer::Play
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MIDIPlayer::Play()
{
	if (!IsLoaded()) 
  {
    WriteDebugString("File not loaded in MIDI::Play()\n");
    return FALSE;
  }

  if (!Open()) return FALSE;    

	MCI_PLAY_PARMS mciPlayParms;
  memset(&mciPlayParms, 0, sizeof(mciPlayParms));
  DWORD flags=0;
  if (m_UseNotify)
  {
    mciPlayParms.dwCallback = (DWORD)m_hWnd;
    flags=MCI_NOTIFY;
  }
  DWORD rc=0;
  if ((rc=mciSendCommand(m_MIDI_Device, MCI_PLAY, flags, (DWORD)&mciPlayParms))!=0)
  {
    char buffer[_MAX_PATH+1];
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Play Err: %s\n", buffer);
      WriteDebugString(temp);
    }
    else
      WriteDebugString("Unkn error after play MIDI\n");
    if (Stop()) Close();
		return FALSE;
  }

  /*
  DWORD rc;
  char buffer[_MAX_PATH+1];
  CString playcommand;

  playcommand.Format("play %s from 0", m_DevName);
  if (m_UseNotify) playcommand += " notify";
  //WriteDebugString("MIDI Play: %s\n", playcommand);
	if ((rc = mciSendString(playcommand, NULL, 0, m_hWnd)) != 0)
	{
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Play Err: %s\n", buffer);
      WriteDebugString(temp);
    }
    else
      WriteDebugString("Unkn error after play MIDI string\n");
    if (Stop()) Close();
		return FALSE;
	}
  */
  
	return TRUE;
}

BOOL MIDIPlayer::Open()
{
  MCI_OPEN_PARMS mciOpenParms;
  memset(&mciOpenParms, 0, sizeof(mciOpenParms));
  //mciOpenParms.lpstrDeviceType = "sequencer";
  mciOpenParms.lpstrElementName = m_Filename;

  DWORD rc=0;
  if ((rc=mciSendCommand(NULL, MCI_OPEN, /*MCI_OPEN_TYPE|*/MCI_OPEN_ELEMENT|MCI_WAIT, (DWORD)&mciOpenParms))!=0)
  {
    int pathLength;
    char buffer[_MAX_PATH+1];
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Open Err: %s\n", buffer);
      WriteDebugString(temp);
      pathLength = m_Filename.GetLength();
      if (pathLength > 127)
      {
        WriteDebugString("Perhaps the pathname of your MIDI file is too long (%d)\n",pathLength);
      };
    }
    else
      WriteDebugString("Unkn error after open MIDI\n");
		return FALSE;
  }
  m_MIDI_Device = mciOpenParms.wDeviceID;

  /*
  DWORD rc;
  const char format[] = "open \"%s\" type sequencer alias %s";
  const WORD len = sizeof(format);

  char buffer[_MAX_PATH+len+1];
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, format, m_Filename, m_DevName);
  
  //WriteDebugString("MIDI Open: %s\n", buffer);
	if ((rc = mciSendString(buffer, NULL, 0, NULL)) != 0)
	{
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Open Err: %s\n", buffer);
      WriteDebugString(temp);
    }
    else
      WriteDebugString("Unkn error after open MIDI string\n");
		return FALSE;
	}
  //WriteDebugString("MIDI Dev Id: %u\n",mciGetDeviceID(m_DevName));
  */
  return TRUE;
}

BOOL MIDIPlayer::Close()
{
  if (m_MIDI_Device) 
  {
		MCI_GENERIC_PARMS mciGenericParams;
    memset(&mciGenericParams, 0, sizeof(mciGenericParams));
    mciGenericParams.dwCallback=NULL;
    DWORD rc=0;
    if ((rc=mciSendCommand(m_MIDI_Device, MCI_CLOSE, MCI_WAIT, (DWORD)&mciGenericParams))!=0)
    {
      char buffer[_MAX_PATH+1];
      if (mciGetErrorString(rc, buffer, sizeof(buffer)))
      {
        CString temp;
        temp.Format("MCI Close Err: %s\n", buffer);
        WriteDebugString(temp);
      }
      else
        WriteDebugString("Unkn error after close MIDI\n");
		  return FALSE;
    }
  }
  m_MIDI_Device = NULL;
  m_Filename="";

  /*
  char buffer[_MAX_PATH+1];
  CString stopcommand;
  DWORD rc;

  m_Filename="";
  stopcommand.Format("close all");
  //WriteDebugString("MIDI Close: %s\n", stopcommand);
	if ( (rc=mciSendString(stopcommand, NULL, 0, NULL)) != 0)
  {
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Close Err: %s\n", buffer);
      WriteDebugString(temp);
    }
    else
      WriteDebugString("Unkn error after close MIDI string\n");
    return FALSE;
  }
  */
	return TRUE;
}

BOOL MIDIPlayer::CanControlDevice()
{
  /*
  HMIDIOUT hmidi;
  UINT err = midiOutOpen(&hmidi, MIDI_MAPPER, 0,0,0);
  if (err==MMSYSERR_NOERROR)
  {
    midiOutClose(hmidi);
    return TRUE;
  }
  return FALSE;
  */
  return TRUE;
}

//*****************************************************************************
//    NAME: MIDIPlayer::Stop
//
// PURPOSE: 
//
//*****************************************************************************
BOOL MIDIPlayer::Stop()
{  
	if (m_MIDI_Device) // Means we loaded a MIDI already.
	{
		MCI_GENERIC_PARMS mciGenericParams;
    memset(&mciGenericParams, 0, sizeof(mciGenericParams));
    mciGenericParams.dwCallback=NULL;
    DWORD rc=0;
		if ((rc=mciSendCommand(m_MIDI_Device, MCI_STOP, MCI_WAIT, (DWORD)&mciGenericParams))!=0)
    {
      char buffer[_MAX_PATH+1];
      if (mciGetErrorString(rc, buffer, sizeof(buffer)))
      {
        CString temp;
        temp.Format("MCI Stop Err: %s\n", buffer);
        WriteDebugString(temp);
      }
      else
        WriteDebugString("Unkn error after stop MIDI\n");
		  return FALSE;
    }
	}

  /*
  char buffer[_MAX_PATH+1];
  CString stopcommand;
  DWORD rc;

  stopcommand.Format("stop all");
  //WriteDebugString("MIDI Stop: %s\n", stopcommand);
	if ( (rc=mciSendString(stopcommand, NULL, 0, NULL)) != 0)
  {
    if (mciGetErrorString(rc, buffer, sizeof(buffer)))
    {
      CString temp;
      temp.Format("MCI Stop Err: %s\n", buffer);
      WriteDebugString(temp);
    }
    else
      WriteDebugString("Unkn error after stop MIDI string\n");
		return FALSE;
  }
  */
  return TRUE;
}

BOOL MIDIPlayer::Mute()
{
	if (m_MIDI_Device) // Means we loaded a MIDI already.
	{
		MCI_GENERIC_PARMS mciGenericParams;
    memset(&mciGenericParams, 0, sizeof(mciGenericParams));
    mciGenericParams.dwCallback=NULL;
    DWORD rc=0;
		if ((rc=mciSendCommand(m_MIDI_Device, MCI_PAUSE, MCI_WAIT, (DWORD)&mciGenericParams))!=0)
    {
      char buffer[_MAX_PATH+1];
      if (mciGetErrorString(rc, buffer, sizeof(buffer)))
      {
        CString temp;
        temp.Format("MCI Pause Err: %s\n", buffer);
        WriteDebugString(temp);
      }
      else
        WriteDebugString("Unkn error after pause MIDI\n");
		  return FALSE;
    }
	}
  return TRUE;
}

BOOL MIDIPlayer::Unmute()
{
	if (m_MIDI_Device) // Means we loaded a MIDI already.
	{
		MCI_GENERIC_PARMS mciGenericParams;
    memset(&mciGenericParams, 0, sizeof(mciGenericParams));
    mciGenericParams.dwCallback=NULL;
    DWORD rc=0;
		if ((rc=mciSendCommand(m_MIDI_Device, MCI_RESUME, MCI_WAIT, (DWORD)&mciGenericParams))!=0)
    {
      char buffer[_MAX_PATH+1];
      if (mciGetErrorString(rc, buffer, sizeof(buffer)))
      {
        CString temp;
        temp.Format("MCI Resume Err: %s\n", buffer);
        WriteDebugString(temp);
      }
      else
        WriteDebugString("Unkn error after resume MIDI\n");
		  return FALSE;
    }
	}
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Background Sound list functions
//
///////////////////////////////////////////////////////////////////////////////

void BACKGROUND_SOUNDS::SaveSounds(int level) 
{ 
  POSITION pos = sounds.GetHeadPosition();
  while (pos != NULL)
    ::SaveSound(sounds.GetNext(pos), level, rte.SoundDir());
};

void BACKGROUND_SOUNDS::Serialize(CArchive &ar)
{
  int count;
  CString temp;
//  CObject::Serialize(ar);
  if (ar.IsStoring())
  {      
    count = sounds.GetCount();      
    ar << count;
    
    POSITION pos = sounds.GetHeadPosition();
    while (pos != NULL)
    {
      temp = sounds.GetAt(pos);
      StripFilenamePath(temp);
      AS(ar,temp);
      //AddFolderToPath(temp, rte.SoundDir());
      sounds.SetAt(pos, temp);
      sounds.GetNext(pos);
    }
  }
  else
  {
    ar >> count;
    for (int i=0;i<count;i++)
    {
      DAS(ar,temp);
      //AddFolderToPath(temp, rte.SoundDir());
      StripFilenamePath(temp);
      sounds.AddTail(temp);
    }
  }
}
void BACKGROUND_SOUNDS::Serialize(CAR &ar)
{
  int count;
  CString temp;

  if (ar.IsStoring())
  {      
    count = sounds.GetCount();      
    ar << count;
    
    POSITION pos = sounds.GetHeadPosition();
    while (pos != NULL)
    {
      temp = sounds.GetAt(pos);
      StripFilenamePath(temp);
      AS(ar,temp); 
      //AddFolderToPath(temp, rte.SoundDir());
      sounds.SetAt(pos, temp);
      sounds.GetNext(pos);
    }
  }
  else
  {
    ar >> count;
    for (int i=0;i<count;i++)
    {
      DAS(ar,temp);
      //AddFolderToPath(temp, rte.SoundDir());
      if (globalData.version < 0.930285)
      {
        StripFilenamePath(temp);
      };
      sounds.AddTail(temp);
    }
  }
}

#ifdef UAFEDITOR
const char *JKEY_SOUND = "sound";

void BACKGROUND_SOUNDS::Export(JWriter& jw, const char *name)
{
  POSITION pos;
  jw.StartList(name);
  pos = sounds.GetHeadPosition();
  while (pos != NULL)
  {
    CString *pSound;
    pSound = &sounds.GetNext(pos);
    jw.NameAndValue(JKEY_SOUND, *pSound);
  };
  jw.EndList();
}
bool BACKGROUND_SOUNDS::Import(JReader& jr, const char *name)
{
  CString sound;
  if(jr.Optional(), !jr.StartList(name)) return false;
  while (jr.Optional(), jr.NameAndValue(JKEY_SOUND, sound))
  {
    sounds.AddTail(sound);
  };
  jr.EndList();
  return true;
}
#endif

#ifdef UAFEDITOR
void BACKGROUND_SOUNDS::CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference)
{
  POSITION pos;
  for (pos=sounds.GetHeadPosition(); pos!=NULL; sounds.GetNext(pos))
  {
    CString name;
    name = sounds.GetAt(pos);
    pCRList->CR_AddSoundReference(name, pCRReference);
  };
}
#endif

BACKGROUND_SOUNDS& BACKGROUND_SOUNDS::operator =(const BACKGROUND_SOUNDS& src)
{
  if (&src == this) return *this;

  sounds.RemoveAll();
  POSITION pos = src.sounds.GetHeadPosition();

  CString temp;
  while (pos != NULL)
  {
    temp = src.sounds.GetNext(pos);
    sounds.AddTail(temp);
  }
  return *this;
}

BOOL BACKGROUND_SOUNDS::operator ==(const BACKGROUND_SOUNDS& src) const
{
  if (sounds.GetCount()!=src.sounds.GetCount()) return FALSE;
  if (sounds.GetCount()==0) return TRUE;
  POSITION pos1,pos2;
  pos1 = src.sounds.GetHeadPosition();
  if (pos1==NULL) return FALSE;
  pos2 = sounds.GetHeadPosition();
  if (pos2==NULL) return FALSE;

  bool found=false;
  while (pos1 != NULL)
  {
    CString tmp = src.sounds.GetAt(pos1);
    pos2 = sounds.GetHeadPosition();
    while ((pos2!=NULL)&&(!found))
    {
      if (sounds.GetAt(pos2)==tmp) found=true;
      sounds.GetNext(pos2);
    }
    if (!found) return FALSE;
    src.sounds.GetNext(pos1);
  }
  return TRUE;
}

void BACKGROUND_SOUND_DATA::Clear()
{
  backgroundSounds.Clear();
  nightSounds.Clear();
  UseNightMusic=FALSE;
  EndTime=600;
  StartTime=1800;
}

BACKGROUND_SOUND_DATA& BACKGROUND_SOUND_DATA::operator =(const BACKGROUND_SOUND_DATA& src)
{
  if (&src==this) return *this;
  backgroundSounds = src.backgroundSounds;
  nightSounds=src.nightSounds;
  UseNightMusic=src.UseNightMusic;
  EndTime=src.EndTime;
  StartTime=src.StartTime;
  return *this;
}
#ifdef UAFEDITOR
BOOL BACKGROUND_SOUND_DATA::operator ==(const BACKGROUND_SOUND_DATA& src) const
{
  if (!(backgroundSounds==src.backgroundSounds)) return FALSE;
  if (!(nightSounds==src.nightSounds)) return FALSE;
  if (UseNightMusic!=src.UseNightMusic) return FALSE;
  if (EndTime!=src.EndTime) return FALSE;
  if (StartTime!=src.StartTime) return FALSE;
  return TRUE;
}
#endif
void BACKGROUND_SOUND_DATA::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    backgroundSounds.Serialize(ar);
    nightSounds.Serialize(ar);
    ar << UseNightMusic;
    ar << EndTime;
    ar << StartTime;
  }
  else
  {
    backgroundSounds.Serialize(ar);
    nightSounds.Serialize(ar);
    ar >> UseNightMusic;
    ar >> EndTime;
    ar >> StartTime;
  }
}

void BACKGROUND_SOUND_DATA::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    backgroundSounds.Serialize(ar);
    nightSounds.Serialize(ar);
    ar << UseNightMusic;
    ar << EndTime;
    ar << StartTime;
  }
  else
  {
    backgroundSounds.Serialize(ar);
    nightSounds.Serialize(ar);
    ar >> UseNightMusic;
    ar >> EndTime;
    ar >> StartTime;
  }
}

#ifdef UAFEDITOR
const char *JKEY_BACKGROUNDSOUNDS = "backgroundSounds";
const char *JKEY_BACKGROUNDSOUND = "backgroundSound";
const char *JKEY_NIGHTSOUNDS = "nightSounds";
const char *JKEY_USENIGHTMUSIC = "useNightMusic";
const char *JKEY_STARTTIME = "startTime";
const char *JKEY_ENDTIME = "endTime";
const char *JKEY_SOUNDS = "sounds";

void BACKGROUND_SOUND_DATA::Export(JWriter& jw)
{
  jw.StartList(JKEY_SOUNDS);
  backgroundSounds.Export(jw, JKEY_BACKGROUNDSOUNDS);
  nightSounds.Export(jw, JKEY_NIGHTSOUNDS);
  jw.NameAndBool(JKEY_USENIGHTMUSIC, UseNightMusic);
  jw.NameAndValue(JKEY_STARTTIME, StartTime);
  jw.NameAndValue(JKEY_ENDTIME, EndTime);
  jw.EndList();
}
bool BACKGROUND_SOUND_DATA::Import(JReader& jr)
{
  jr.StartList(JKEY_SOUNDS);
  backgroundSounds.Import(jr, JKEY_BACKGROUNDSOUNDS);
  nightSounds.Import(jr, JKEY_NIGHTSOUNDS);
  jr.NameAndBool(JKEY_USENIGHTMUSIC, UseNightMusic);
  jr.NameAndValue(JKEY_STARTTIME, StartTime);
  jr.NameAndValue(JKEY_ENDTIME, EndTime);
  jr.EndList();
  return true;
}
#endif


#ifdef UAFEDITOR
void BACKGROUND_SOUND_DATA::CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference)
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceType = pCRReference->m_referenceType;
  CRReference.m_referenceName = pCRReference->m_referenceName + "(background)";
  backgroundSounds.CrossReference(pCRList, &CRReference);
  CRReference.m_referenceName = pCRReference->m_referenceName + "(night)";
  nightSounds.CrossReference(pCRList, &CRReference);
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  SoundQueue functions
//
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: SoundQueue::SoundQueue
//
// PURPOSE: 
//
//*****************************************************************************
SoundQueue::SoundQueue() : CThread(CREATE_SUSPENDED),
                ExitEvent(NULL),
                SoundFinishedEvent(NULL),
                CurrSound(-1),
                m_IsPlaying(FALSE),
                m_Loop(FALSE)
{ 
  list.RemoveAll();  
  ExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  SoundFinishedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//*****************************************************************************
//    NAME: SoundQueue::~SoundQueue
//
// PURPOSE: 
//
//*****************************************************************************
SoundQueue::~SoundQueue() 
{ 
  if (ThreadActive())
  {
#ifndef _DEBUG
    // wait for thread to exit
    if (WaitForSingleObject(Handle(), 5000)!=WAIT_OBJECT_0)
      Terminate();
#else
    WaitForSingleObject(Handle(), INFINITE);
#endif
  }

  if (ExitEvent != NULL)
  {
    CloseHandle(ExitEvent);
    ExitEvent = NULL;
  }

  if (SoundFinishedEvent != NULL)
  {
    CloseHandle(SoundFinishedEvent);
    SoundFinishedEvent = NULL;
  }
  m_IsPlaying = FALSE; 
}

//*****************************************************************************
//    NAME: SoundQueue::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SoundQueue::Clear() 
{
  //WriteDebugString("SoundQueue::Clear()\n");
  list.RemoveAll();
  m_Loop=FALSE;
  CurrSound = -1;
}

//*****************************************************************************
//    NAME: SoundQueue::Add
//
// PURPOSE: 
//
//*****************************************************************************
void SoundQueue::Add(CString &sound)
{
  //WriteDebugString("SoundQueue::Add()\n");
//  CSingleLock sl(&m_cs, TRUE);
  list.AddTail(sound);
}

//*****************************************************************************
//    NAME: SoundQueue::PlayQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundQueue::PlayQueue(BOOL Loop) 
{ 
  //WriteDebugString("PlayQueue()\n");
//  CSingleLock sl(&m_cs, TRUE);
  m_Loop=Loop;
  Resume(); 
}

//*****************************************************************************
//    NAME: SoundQueue::PlayQueueAndWait
//
// PURPOSE: 
//
//*****************************************************************************
void SoundQueue::PlayQueueAndWait(DWORD timeout) 
{ 
//  CSingleLock sl(&m_cs, TRUE);
  if (!ThreadActive())
    return;
  
  Resume(); 
  
  WaitForSingleObject(Handle(), timeout);
}

//*****************************************************************************
//    NAME: SoundQueue::StopQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundQueue::StopQueue() 
{ 
  //WriteDebugString("StopQueue()\n");
  //CSingleLock sl(&m_cs, TRUE); ** deadlock ** 
  if (ExitEvent != NULL)
    SetEvent(ExitEvent);

  // just in case PlayQueue was never called
  Resume();
}

//*****************************************************************************
//    NAME: SoundQueue::Play
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundQueue::Play(const char *filename) // With full path
{
  if (pSndMgr == NULL) return FALSE;
  if (WaitForSingleObject(ExitEvent,0)==WAIT_OBJECT_0) return FALSE;

  //WriteDebugString("SoundQueue::Play()\n");
//  CSingleLock sl(&m_cs, TRUE);
  BOOL success = FALSE;  

  BOOL ismidi = IsMIDIFile(filename);
  if (ismidi) 
    pSndMgr->m_MidiType=QUEUE_MIDI_STREAM_ID;       
  //WriteDebugString("101 Calling AddSound(%s)\n", filename);
  CurrSound = pSndMgr->AddSound(filename);

  if (CurrSound >= 0)
  {
    HANDLE objects[3];
    objects[0] = ExitEvent;
    objects[1] = SoundFinishedEvent;

    if (pSndMgr->SetupNotifyStop(CurrSound, SoundFinishedEvent))
    {
      ResetEvent(SoundFinishedEvent);
      if (pSndMgr->Play(CurrSound))
      {
        switch (WaitForMultipleObjects(2, objects, FALSE, INFINITE))
        {
        case WAIT_OBJECT_0: // shutdown
          pSndMgr->StopSample(CurrSound);
          //WriteDebugString("Queued sound stopped due to thead shutdown signal\n");
          success = FALSE;
          break;
        case WAIT_OBJECT_0+1: // curr sound complete
          //WriteDebugString("Queued sound completed\n");
          success = TRUE;
          break;
        default:
          pSndMgr->StopSample(CurrSound);
          WriteDebugString("Error after WaitForMultipleObj() in SoundQueue\n");
          success = FALSE;
          break;
        }
      }
      else
      {
        WriteDebugString("Queued sound failed to play\n");
        if (list.GetCount() > 1)
          success = TRUE; // will allow playing next queued sound
      }
    }

    //pSndMgr->StopSample(CurrSound);
    pSndMgr->RemoveSound(&CurrSound);
  }
  else
  {
    if (list.GetCount() > 1)
      success = TRUE; // will allow playing next queued sound
  }
  pSndMgr->CleanupNotifyStop();
  return success;
}

//*****************************************************************************
//    NAME: SoundQueue::Thread
//
// PURPOSE: 
//
//*****************************************************************************
DWORD SoundQueue::Thread()
{
  m_IsPlaying = TRUE;

  if (list.GetCount() <= 0)
    return 0;

  POSITION pos = list.GetHeadPosition();
  if (pos == NULL) 
    return 0;

  //WriteDebugString("Sound queue starting with %i sounds\n", list.GetCount());

  BOOL KeepPlaying = TRUE;
  CString CurrFile;
  CurrFile = list.GetAt(pos);

  while (KeepPlaying)
  {
    KeepPlaying = Play(CurrFile);

    if (KeepPlaying)
    {
      list.GetNext(pos);
      if (pos != NULL)
      {
        CurrFile = list.GetAt(pos);
      }
      else
      {
        if (!m_Loop)
        {
          KeepPlaying = FALSE;
        }
        else
        {
          pos = list.GetHeadPosition();
          CurrFile = list.GetAt(pos);
        }
      }
    }
  }
  //WriteDebugString("Sound queue thread exiting\n");
  Clear();
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  BackgroundSoundQueue functions
//
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//    NAME: SoundQueue::BackgroundSoundQueue
//
// PURPOSE: 
//
//*****************************************************************************
BackgroundSoundQueue::BackgroundSoundQueue() : CThread(CREATE_SUSPENDED),
                ExitEvent(NULL),
                SoundFinishedEvent(NULL),
                CurrSound(-1),
                m_IsLevel(TRUE),
                m_IsPlaying(FALSE),
                m_Loop(FALSE)
{ 
  list.RemoveAll();  
  ExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  SoundFinishedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::~BackgroundSoundQueue
//
// PURPOSE: 
//
//*****************************************************************************
BackgroundSoundQueue::~BackgroundSoundQueue() 
{ 
   if (ThreadActive())
  {
#ifndef _DEBUG
    // wait for thread to exit
    if (WaitForSingleObject(Handle(), 5000)!=WAIT_OBJECT_0)
      Terminate();
#else
    WaitForSingleObject(Handle(), INFINITE);
#endif
  }

  if (ExitEvent != NULL)
  {
    CloseHandle(ExitEvent);
    ExitEvent = NULL;
  }

  if (SoundFinishedEvent != NULL)
  {
    CloseHandle(SoundFinishedEvent);
    SoundFinishedEvent = NULL;
  }
  m_IsPlaying = FALSE; 
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void BackgroundSoundQueue::Clear() 
{
  //WriteDebugString("BackgroundSoundQueue::Clear()\n");
  list.RemoveAll();
  m_Loop=FALSE;
  CurrSound = -1;
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::Add
//
// PURPOSE: 
//
//*****************************************************************************
void BackgroundSoundQueue::Add(CString &sound)
{
  //WriteDebugString("BackgroundSoundQueue::Add()\n");
//  CSingleLock sl(&m_cs, TRUE);
  list.AddTail(sound);
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::PlayQueue
//
// PURPOSE: 
//
//*****************************************************************************
void BackgroundSoundQueue::PlayQueue(BOOL Loop) 
{ 
  //WriteDebugString("PlayQueue()\n");
//  CSingleLock sl(&m_cs, TRUE);
  m_Loop=Loop;
  Resume(); 
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::StopQueue
//
// PURPOSE: 
//
//*****************************************************************************
void BackgroundSoundQueue::StopQueue() 
{ 
  //WriteDebugString("StopQueue()\n");
  //CSingleLock sl(&m_cs, TRUE); ** deadlock ** 
  if (ExitEvent != NULL)
    SetEvent(ExitEvent);

  // just in case PlayQueue was never called
  Resume();
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::Play
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BackgroundSoundQueue::Play(const char *filename)
{
  if (pSndMgr == NULL) return FALSE;
  if (WaitForSingleObject(ExitEvent,0)==WAIT_OBJECT_0) return FALSE;

  //WriteDebugString("BackgroundSoundQueue::Play()\n");
//  CSingleLock sl(&m_cs, TRUE);
  BOOL success = FALSE;  

  BOOL ismidi = IsMIDIFile(filename);
  if (ismidi)
    pSndMgr->m_MidiType=BGQUEUE_MIDI_STREAM_ID;

  //WriteDebugString("102 Calling AddSound(%s)\n", filename);
  CurrSound = pSndMgr->AddSound(filename);
  //WriteDebugString("BackgroundSoundQueue starting to play %s\n", filename);
  BOOL HwLoop = (list.GetCount()==1);

  if (CurrSound >= 0)
  {
    HANDLE objects[3];
    objects[0] = ExitEvent;
    objects[1] = SoundFinishedEvent;

    if (!HwLoop)
      success = pSndMgr->SetupBgndNotifyStop(CurrSound, SoundFinishedEvent);
    else
      success = TRUE;

    if (success)
    {
      ResetEvent(SoundFinishedEvent);
      if (pSndMgr->Play(CurrSound,HwLoop))
      {       
        //WriteDebugString("BackgroundSoundQueue playing %s\n", filename);

        switch (WaitForMultipleObjects(2, objects, FALSE, INFINITE))
        {
        case WAIT_OBJECT_0: // shutdown
          pSndMgr->StopSample(CurrSound);
          //WriteDebugString("Background Queued sound stopped due to thead shutdown signal\n");
          success = FALSE;
          break;
        case WAIT_OBJECT_0+1: // curr sound complete
          //WriteDebugString("Background Queued sound completed\n");
          success = TRUE;
          break;
        default:
          pSndMgr->StopSample(CurrSound);
          WriteDebugString("Error after WaitForMultipleObj() in BackgroundSoundQueue\n");
          success = FALSE;
          break;
        }
      }
      else
      {
        WriteDebugString("Background Queued sound failed to play\n");
        if (list.GetCount() > 1)
          success = TRUE; // will allow playing next queued sound
      }
    }

    //pSndMgr->Stop(CurrSound);
    pSndMgr->RemoveSound(&CurrSound);
  }
  else
  {
    if (list.GetCount() > 1)
      success = TRUE; // will allow playing next queued sound
  }

  //WriteDebugString("BackgroundSoundQueue Play() exiting\n");
  return success;
}

//*****************************************************************************
//    NAME: BackgroundSoundQueue::Thread
//
// PURPOSE: 
//
//*****************************************************************************
DWORD BackgroundSoundQueue::Thread()
{
  m_IsPlaying = TRUE;

  if (list.GetCount() <= 0)
    return 0;

  POSITION pos = list.GetHeadPosition();
  if (pos == NULL) 
    return 0;

  //WriteDebugString("Background Sound queue starting with %i sounds\n", list.GetCount());

  BOOL KeepPlaying = TRUE;
  CString CurrFile;
  CurrFile = list.GetAt(pos);

  while (KeepPlaying)
  {
    KeepPlaying = Play(CurrFile);

    if (KeepPlaying)
    {
      list.GetNext(pos);
      if (pos != NULL)
      {
        CurrFile = list.GetAt(pos);
      }
      else
      {
        if (!m_Loop)
        {
          KeepPlaying = FALSE;
        }
        else
        {
          pos = list.GetHeadPosition();
          CurrFile = list.GetAt(pos);
        }
      }
    }
  }
  //WriteDebugString("Background Sound queue thread exiting\n");
  Clear();
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  SoundMgr functions
//
///////////////////////////////////////////////////////////////////////////////


//*****************************************************************************
//    NAME: SoundMgr::SoundMgr
//
// PURPOSE: 
//
//*****************************************************************************
SoundMgr::SoundMgr(HWND hWnd) : 
   m_SndQueue(NULL)
  ,m_BgndSndQueue(NULL)  
#ifdef USE_DIRECTSOUND
  ,pSound(NULL)
#else
  ,m_StreamId(INVALID_STREAM_KEY)
#endif
{
  m_hWnd=hWnd;
  m_MIDIMgr=NULL;
  m_WaveMgr.SetName("WAVE");
  SoundEnabled = ALL_SOUND_TYPES;
  m_MidiType=SOUND_MIDI_STREAM_ID;
  m_MusicOn=FALSE;

#ifdef USE_DIRECTSOUND
  pSound = new CDXSound;

#if (defined UAFEDITOR)
  HRESULT rc = pSound->Create(hWnd, FALSE);
#else
  HRESULT rc = pSound->Create(hWnd, TRUE);
#endif

  if (FAILED(rc))
  {
    SoundEnabled = 0;
    WriteDebugString("Failed to create SoundMgr() object: %s\n",
                     SndErrorToString(rc));
    return;
  }

#if (defined UAFEDITOR)
  pSound->ContinuousPlay(FALSE);
#else
  pSound->ContinuousPlay(TRUE);
#endif

#endif // USE_DIRECTSOUND

#ifdef USE_BASS
  if (!PlaySoundEffects)
  {
    WriteDebugString("Not loading BASS DLL, sound disabled\n");
    SoundEnabled = 0;    
    PlayEventSounds = FALSE;
    PlayEffectsDuringEvents = FALSE;
    m_MusicOn=FALSE;
    return;
  }

  if (!LoadBASS()) // load DLL
  {
    WriteDebugString("Failed to locate/load BASS DLL\n");
    SoundEnabled = 0;
    return;
  }

  if (BASS_Init != NULL)
  {
    DWORD flags = 0;
    DWORD sample_rate=SoundPlaybackRate; // 44100 or 22050
    WriteDebugString("Sound playback rate = %u\n", SoundPlaybackRate);

    if (  ((LOWORD(ver) == 0)&&(HIWORD(ver)>=9)) // new option with Bass version 0.9
        || (LOWORD(ver) >= 1))
      flags = BASS_DEVICE_LEAVEVOL;

	  if (!BASS_Init(-1,sample_rate,flags,hWnd))
    {
      // couldn't initialize device, so use no sound
      SoundEnabled = 0;
      BASS_Init(-2,sample_rate,flags,hWnd);
      WriteDebugString("Failed to initialize BASS - sound disabled, error %s\n",
                       SndErrorToString(BASS_ErrorGetCode()));
    }
  }

  if (BASS_Start != NULL)
    BASS_Start();

  m_MusicOn=TRUE;
  int defaultvol = -1;

#ifdef UAFEngine
  CString filename;
  filename = rte.DataDir() + "sound.ini";

  FILE *pFile = fopen(filename, "rt");
  if (pFile != NULL)
  {
    char text[50];
    while (fgets(text, 49, pFile) != NULL)
    {
      char *pToken = strchr(text, '=');
      if ((pToken != NULL) && (*pToken != '\0'))
      {
        *pToken='\0';
        pToken++;
        if (stricmp(text,"volume")==0)
        {
          if ((pToken != NULL) && (*pToken != '\0'))
          {
            defaultvol = atoi(pToken);
            if (defaultvol > 100) defaultvol = 100;
          }
        }
        else if (stricmp(text,"music")==0)
        {
          if ((pToken != NULL) && (*pToken != '\0'))
            m_MusicOn = atoi(pToken);
        }
      }
    }
    fclose(pFile);
  }
#endif

  //m_PrevMusicVol=70;
  //m_PrevSampleVol=70;
  m_PrevStreamVol=70;

  //if (BASS_GetGlobalVolumes != NULL)
  //  BASS_GetGlobalVolumes(&m_PrevMusicVol,&m_PrevSampleVol,&m_PrevStreamVol);

  if (BASS_GetVolume != NULL)
    m_PrevStreamVol= BASS_GetVolume();

  if (defaultvol >= 0)
    SetMasterVolume(defaultvol);
  else
    SetMasterVolume(m_PrevStreamVol);

  if (BASS_SetBufferLength != NULL)
    BASS_SetBufferLength(2.0); // two secs worth of buffer (max for bass)

  if (BASS_GetInfo)
  {
    BASS_INFO info;
    memset(&info,0,sizeof(info));
    info.size=sizeof(BASS_INFO);
    BASS_GetInfo(&info);
    if (info.flags&DSCAPS_EMULDRIVER) 
    {
      // device does NOT have hardware support, rest of info not valid
      WriteDebugString("Installed sound device/driver does not provide hardware DirectSound support\n");
    }
    else
    {
      if ((LOWORD(ver)>=1)&&(HIWORD(ver)>=5))
      {
         // ver 1.5 changed this function call
        int count=0; // the device counter
        char *dummy; // description pointer
        dummy = BASS_GetDeviceDescription(count); 
        while (dummy!=NULL)
        {
          WriteDebugString("Sound card %i desc: %s\n", count+1, dummy);
          count++;
          dummy = BASS_GetDeviceDescription(count); 
        }
      }

      WriteDebugString("Sound Card Info: hwsize %u, hwfree %u, freesam %u, free3d %u, minrate %u, maxrate %u, eax %i, a3d %u, dsver %u\n",
                       info.hwsize,info.hwfree,info.freesam,info.free3d,info.minrate,info.maxrate,info.eax,info.a3d,info.dsver);
      if (info.flags&DSCAPS_CONTINUOUSRATE)
        WriteDebugString("Sound Card Info: Continuous sample rates supported\n");
      if (info.flags&DSCAPS_CERTIFIED)
        WriteDebugString("Sound Card Info: Certified driver\n");
      if (info.flags&DSCAPS_SECONDARYMONO)
        WriteDebugString("Sound Card Info: mono sample rates supported in hw\n");
      if (info.flags&DSCAPS_SECONDARYSTEREO)
        WriteDebugString("Sound Card Info: stereo sample rates supported in hw\n");
      if (info.flags&DSCAPS_SECONDARY8BIT)
        WriteDebugString("Sound Card Info: 8-bit sample rates supported in hw\n");
      if (info.flags&DSCAPS_SECONDARY16BIT)
        WriteDebugString("Sound Card Info: 16-bit sample rates supported in hw\n");
    }
  }
#endif // ifdef BASS

  Clear();
}

//*****************************************************************************
//    NAME: SoundMgr::~SoundMgr
//
// PURPOSE: 
//
//*****************************************************************************
SoundMgr::~SoundMgr()
{
  Clear(); 

  if (m_SndQueue != NULL)
    delete m_SndQueue;
  m_SndQueue = NULL;

  if (m_BgndSndQueue!=NULL)
    delete m_BgndSndQueue;
  m_BgndSndQueue=NULL;

  if (m_MIDIMgr!=NULL)
  {
    delete m_MIDIMgr;
    m_MIDIMgr=NULL;
  }

#ifdef USE_DIRECTSOUND
  if (pSound != NULL) 
    delete pSound; 
  pSound = NULL;
#endif

#ifdef USE_BASS
  //if (BASS_SetGlobalVolumes != NULL)
  //  BASS_SetGlobalVolumes(m_PrevMusicVol,m_PrevSampleVol,m_PrevStreamVol);

  if (BASS_SetVolume != NULL)
    BASS_SetVolume(m_PrevStreamVol);


  if (BASS_Stop!=NULL) BASS_Stop();

  if (BASS_Free != NULL) BASS_Free();
  FreeBASS(); // unload DLL
#endif

}

//*****************************************************************************
//    NAME: SoundMgr::EnableSound
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::EnableSound(WORD types, BOOL enable)
{
#ifdef USE_BASS 
  if (BASS_Init == NULL) return;
#endif

  if (enable)
    SoundEnabled |= types;
  else
    SoundEnabled &= ~types;
}

void SoundMgr::MuteVolume(void)
{
  //WriteDebugString("SndMgr: Mute volume\n");
#ifdef USE_BASS  
  if (BASS_SetGlobalVolumes != NULL)
    BASS_SetGlobalVolumes(0,0,0);
#endif

  // don't own device here, so has no effect
  //MIDIPlayer::Mute();

  // this mutes sound for entire computer, not
  // just this app.
  //if (BASS_SetVolume != NULL)
  //  BASS_SetVolume(0);
}

void SoundMgr::UnMuteVolume(void)
{
  //WriteDebugString("SndMgr: Unmute volume\n");
#ifdef USE_BASS    
  if (BASS_SetGlobalVolumes != NULL)
    BASS_SetGlobalVolumes(m_StreamVol,m_StreamVol,m_StreamVol);
#endif
  // don't own device here, so has no effect
  //MIDIPlayer::Unmute();

  //if (BASS_SetVolume != NULL)
  //  BASS_SetVolume(m_StreamVol);
}

//*****************************************************************************
//    NAME: SoundMgr::SetMasterVolume
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::SetMasterVolume(int vol)
{
#ifdef USE_BASS   
  if (BASS_Init == NULL) return;
#endif
  
  //if (SoundEnabled == 0) return;
  if (vol < 0) vol = 0;
  if (vol > 100) vol = 100;

  m_MusicVol=vol;
  m_SampleVol=vol;
  m_StreamVol=vol;

#ifdef USE_BASS  
  if (BASS_SetVolume != NULL)
    BASS_SetVolume(m_StreamVol);
#endif

  EnableSound(ALL_SOUND_TYPES, (vol>0));

#ifdef UAFEngine
  CString filename;
  filename = rte.DataDir() + "sound.ini";
  FILE *pFile = fopen(filename, "wt");
  if (pFile != NULL)
  {
    fprintf(pFile, "volume=%i\n", vol);
    fprintf(pFile, "music=%i\n", m_MusicOn);
    fclose(pFile);
  }
#endif
}

//*****************************************************************************
//    NAME: SoundMgr::GetMasterVolume
//
// PURPOSE: 
//
//*****************************************************************************
int SoundMgr::GetMasterVolume()
{
  return m_MusicVol;
}

void SoundMgr::SetMusicEnable(BOOL enable)
{
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  

  m_MusicOn=enable;
#ifdef UAFEngine
  CString filename;
  filename = rte.DataDir() + "sound.ini";
  FILE *pFile = fopen(filename, "wt");
  if (pFile != NULL)
  {
    fprintf(pFile, "volume=%i\n", m_MusicVol);
    fprintf(pFile, "music=%i\n", m_MusicOn);
    fclose(pFile);
  }
#endif
}

BOOL SoundMgr::GetMusicEnable()
{
  return m_MusicOn;
}

//*****************************************************************************
//    NAME: SoundMgr::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::Clear()
{
  /* will have deadlock if m_cs locked here */
  //CSingleLock sl(&m_cs, TRUE);
  StopAll();
}

//*****************************************************************************
//    NAME: SoundMgr::SetupNotifyStop
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundMgr::SetupNotifyStop(DWORD key, HANDLE hEvent)
{
  BOOL success = FALSE;

#ifdef USE_DIRECTSOUND
  if (IsMidiKey(key))
    return TRUE; // midi notify performed by Win32 msgs

  DSBPOSITIONNOTIFY PositionNotify;
  PositionNotify.dwOffset = DSBPN_OFFSETSTOP;
  PositionNotify.hEventNotify = hEvent; 
  m_lpDsNotify = NULL;

  LPDIRECTSOUNDBUFFER pBuf = pSndMgr->GetDSB(key);
  if (pBuf != NULL)
  {
    HRESULT res = pBuf->QueryInterface(IID_IDirectSoundNotify, 
                                       (LPVOID *)&m_lpDsNotify); 
    if (SUCCEEDED(res))
    {
      m_lpDsNotify->SetNotificationPositions(1, &PositionNotify);
      success = TRUE;
    }
    else
      WriteDebugString("Unable to query intfc for DSNotify\n");       
  }
#endif

#ifdef USE_BASS
  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);
  if (pBuf != NULL)
  {
    // midi notification is done using Win32 messages and is always enabled
    if (!IsMidiKey(pBuf->id))
    {
      if (((BASS_ChannelSetSync) && BASS_ChannelSetSync(pBuf->id,(BASS_SYNC_ONETIME | BASS_SYNC_END),0,BassSyncCallback, (DWORD)this) == NULL))
      {
        WriteDebugString("Failed to set channel sync, error %s\n",
                         SndErrorToString(BASS_ErrorGetCode()));
      }
      else
        success = TRUE;
    }
    else
      success = TRUE;
  }
#endif

  return success;
}

BOOL SoundMgr::SetupBgndNotifyStop(DWORD key, HANDLE hEvent)
{
  BOOL success = FALSE;

#ifdef USE_BASS

  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);
  if (pBuf != NULL)
  {
    if (!IsMidiKey(pBuf->id))
    {
      if (((BASS_ChannelSetSync) && BASS_ChannelSetSync(pBuf->id,(BASS_SYNC_ONETIME | BASS_SYNC_END),0,BassSyncBgSoundCallback, (DWORD)this) == NULL))
      {
        WriteDebugString("Failed to set channel sync, error %s\n",
                         SndErrorToString(BASS_ErrorGetCode()));
      }
      else
        success = TRUE;
    }
    else
    {
      //pSndMgr->m_MIDIMgr.m_UseNotify=TRUE;
      success = TRUE;
    }
  }

#endif

  return success;
}

//*****************************************************************************
//    NAME: SoundMgr::CleanupNotifyStop
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::CleanupNotifyStop()
{
#ifdef USE_DIRECTSOUND
  if (m_lpDsNotify != NULL)
    m_lpDsNotify->Release();
#endif
}

//*****************************************************************************
//    NAME: SoundMgr::AddSound
//
// PURPOSE: 
//
//*****************************************************************************
long SoundMgr::AddSound(const char *filename)
{
  // filename only....no path please.

#ifdef USE_BASS    
  if (BASS_Init == NULL) return -1; 
#endif
  
#ifdef USE_DIRECTSOUND
  if (pSound == NULL)
    return -1;
#endif

  if (filename == NULL) return -1;
  if (filename[0]=='\0') return -1;

//  CSingleLock sl(&m_cs, TRUE);

  long key = -1;
  static char rootName[_MAX_PATH+1];

  CString tmp;
  tmp = filename;
#ifdef UAFEngine
  if (!SearchForFile(tmp, rte.SoundDir())) // returns full path file
  // 20110527 PRS if (!SearchForFile(tmp, rte.SoundDir())) // returns full path file
#else
  if (!SearchForFile(tmp, rte.SoundDir(), ede.TemplateSoundDir())) // returns full path file
#endif
  {
    if (!debugStrings.AlreadyNoted(CString("CFFIAS01") + tmp))
    WriteDebugString("Can't find file \'%s\' in AddSound()\n", tmp);
    return -1;
  }

  strcpy(rootName, tmp);

  if (SoundEnabled & ST_SAMPLE)
  {
    key = AddSample(rootName);
    if (key < 0)
      WriteDebugString("Failed AddSound(%s) for sample\n", rootName);
  }

  return key;
}

//*****************************************************************************
//    NAME: SoundMgr::AddSample
//
// PURPOSE: 
//
//*****************************************************************************
long SoundMgr::AddSample(const char *filename)
{
#ifdef USE_BASS    
  if (BASS_Init == NULL) return -1;
#endif
  
  long key = -1;

#ifdef USE_BASS
  POSITION pos = m_WaveMgr.GetHeadPosition();
  BOOL found = FALSE;
  BOOL ismidi = IsMIDIFile(filename);
  
  if (!ismidi)
  {
    // Search for existing buffer with same filename.
    // This way, all of the items/monsters/spells that use
    // the default hit/miss/cast sounds will not each load
    // their own copy, they will each play the same buffer.
    while ((pos != NULL) && (!found))
    {
      SAMPLEBUF *pBuf = m_WaveMgr.GetAt(pos);
      if (   (pBuf != NULL) 
          && (strcmp(pBuf->GetFilename(), filename) == 0))
      {
        // make sure sample is still loaded
        DWORD handle = pBuf->id;
        if (BASS_ChannelGetFlags(handle) != 0xFFFFFFFF)
        {
          found = TRUE;
          key = m_WaveMgr.GetKeyAt(pos);
        }
        else
        {
          // sample is not valid anymore, remove entry
          m_WaveMgr.RemoveSurface(key); 
          break;
        }
      }
      else
        m_WaveMgr.GetNext(pos);
    }

    if (key > 0)
      return key;
  }
#endif

  SAMPLEBUF *pBuf = new SAMPLEBUF;
  if (pBuf==NULL) return key;

#ifdef USE_DIRECTSOUND
  HRESULT rc = pBuf->Load(pSound, filename, 1); 
  if (!FAILED(rc))
  {
    key = m_WaveMgr.AddSurface(pBuf, BufferDib);
  }
  else
  {
    if (pBuf != NULL) delete pBuf;
    key = -1;
    WriteDebugString("Failed to load sample %s, error %s\n",
                     filename, SndErrorToString(rc));
  }
#endif

#ifdef USE_BASS
  CString loaderror("");
  if (ismidi)
  {
    if (m_MIDIMgr!=NULL)
    {
      m_MIDIMgr->Stop();
/*
      int count = 20; // wait up to two seconds
      while ( (WaitForSingleObject(m_MIDIMgr->SoundFinishedEvent,0)!=WAIT_OBJECT_0) && (count > 0))
      {
#ifdef UAFEngine
        theApp.AppSleep(100); // this one lets Win32 msg pump process queued msgs
#endif
        count--;
      }
      //WriteDebugString("Rcvd pending midi notification: %s\n", ( (WaitForSingleObject(m_MIDIMgr->SoundFinishedEvent,0)==WAIT_OBJECT_0)?"Yes":"No" ));
*/
      delete m_MIDIMgr;
      m_MIDIMgr=NULL;      
    }

    m_MIDIMgr = new MIDIAsyncPlayer(m_hWnd, "UAFMIDI");
    if (m_MIDIMgr->Load(filename, m_MidiType))
      pBuf->id = m_MIDIMgr->GetType();
     else
      loaderror.Format("Failed to load MIDI \'%s\' in AddSample\n",filename);
  }
  else
  {
    if (IsMusicFile(filename))
    {
      if (BASS_MusicLoad)
      {
        DWORD flags = BASS_MUSIC_RAMPS | BASS_MUSIC_STOPBACK;
        pBuf->id = BASS_MusicLoad(FALSE, (PVOID)filename, 0,0,flags);
        if (pBuf->id == NULL)
          loaderror.Format("Failed to load music %s, error %s\n",
                           filename, SndErrorToString(BASS_ErrorGetCode()));
      }
    }
    else
    {
      if (BASS_StreamCreateFile)
      {
        pBuf->id = BASS_StreamCreateFile(FALSE, (PVOID)filename, 0,0,0);
        if (pBuf->id == NULL)
          loaderror.Format("Failed to load sample %s, error %s\n",
                           filename, SndErrorToString(BASS_ErrorGetCode()));
      }
    }

  }

  if (pBuf->id == NULL)
  {
    delete pBuf;
    key = -1;
    WriteDebugString(loaderror);
  }
  else
  {
    pBuf->filename=filename;
    key = m_WaveMgr.AddSurface(pBuf, BufferDib);
  }
#endif

  return key;
}


//*****************************************************************************
//    NAME: SoundMgr::Play
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundMgr::Play(long key, BOOL loop)
{
#ifdef USE_BASS    
  if (BASS_Init == NULL) return FALSE;
#endif
  
//  CSingleLock sl(&m_cs, TRUE);

  if (SoundEnabled & ST_SAMPLE)
    return PlaySample(key, loop);
  else return FALSE;
}

BOOL SoundMgr::ValidHandle(long key)
{
#ifdef USE_BASS   
  if (BASS_Init == NULL) return FALSE;
//  CSingleLock sl(&m_cs, TRUE);
  DWORD handle;

  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);
  if (pBuf != NULL) handle = pBuf->id;
  else return FALSE;

  DWORD result;
  if (!IsMidiKey(handle))
  {
    result = BASS_ChannelGetFlags(handle);
    return (result != 0xFFFFFFFF);
  }
  else
  {
    return FALSE; // force MIDI reload for now
  }
#else
   return TRUE;
#endif
}

//*****************************************************************************
//    NAME: SoundMgr::PlaySample
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundMgr::PlaySample(long key, BOOL loop)
{
#ifdef USE_BASS   
  if (BASS_Init == NULL) return FALSE;
#endif

  if (key<0)return FALSE;

//  CSingleLock sl(&m_cs, TRUE);
  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);

  if (pBuf != NULL) 
  {
#ifdef USE_DIRECTSOUND
    DWORD flags = (loop ? DSBPLAY_LOOPING : 0 );
    return (SUCCEEDED(pBuf->Play( flags )));
#endif

#ifdef USE_BASS
    if (IsMidiKey(pBuf->id))
    {
      if (m_MIDIMgr!=NULL)
      {
        if (m_MIDIMgr->IsLoaded())
        {
          return m_MIDIMgr->Play();
        }
        else
        {
          WriteDebugString("MIDI Mgr not loaded in PlaySample()\n");
          return FALSE;
        }
      }
      else
      {
        WriteDebugString("MIDI Mgr not created in PlaySample()\n");
        return FALSE;
      }
    }
    else
    {      
      if (IsMusicFile(pBuf->GetFilename()))
      {
        DWORD flags = (loop?BASS_MUSIC_LOOP:0);
        flags |= BASS_MUSIC_RAMPS;
        if (!loop)
          flags |= BASS_MUSIC_STOPBACK;

        if (!BASS_MusicPlayEx(pBuf->id, 0, flags, FALSE))
        {
          WriteDebugString("Error playing music: %s\n",
                           SndErrorToString(BASS_ErrorGetCode()));
        }
        else
          return TRUE;
      }
      else
      {
        DWORD flags = (loop?BASS_SAMPLE_LOOP:0);
        if (!BASS_StreamPlay(pBuf->id, TRUE, flags))
        {
          WriteDebugString("Error playing sample: %s\n",
                           SndErrorToString(BASS_ErrorGetCode()));
        }
        else
          return TRUE;
      }
    }
#endif
  }
  return FALSE;
}


//*****************************************************************************
//    NAME: SoundMgr::StopSample
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::StopSample(long key)
{
#ifdef USE_BASS   
  if (BASS_Init == NULL) return;
#endif

//  CSingleLock sl(&m_cs, FALSE);

  //sl.Lock();
  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);

#ifdef USE_DIRECTSOUND
  if (pBuf != NULL)
    pBuf->Stop();
#endif

#ifdef USE_BASS
  if (pBuf != NULL) 
  {
    if (IsMidiKey(pBuf->id))
    {
      // cannot stop midi of a different type, but playing
      // a different type will still stop it
      if (pBuf->id==m_MidiType)
      {
        //sl.Unlock();
        if (m_MIDIMgr!=NULL)
        {
          m_MIDIMgr->Stop();
/*
          int count = 20; // wait up to two seconds
          while ( (WaitForSingleObject(m_MIDIMgr->SoundFinishedEvent,0)!=WAIT_OBJECT_0) && (count > 0))
          {
#ifdef UAFEngine
            theApp.AppSleep(100); // this one lets Win32 msg pump process queued msgs
#endif
            count--;
          }
          //WriteDebugString("Rcvd pending midi notification: %s\n", ( (WaitForSingleObject(m_MIDIMgr->SoundFinishedEvent,0)==WAIT_OBJECT_0)?"Yes":"No" ));
*/
          delete m_MIDIMgr;
          m_MIDIMgr=NULL;
        }
      }
    }
    else
    {
      if (BASS_ChannelStop)
      {
        BASS_ChannelStop(pBuf->id);
        if (IsMusicFile(pBuf->GetFilename()))
          BASS_MusicFree(pBuf->id);
        else
          BASS_StreamFree(pBuf->id);        
        pBuf->id = 0;
      }
    }
  }
#endif
}


//*****************************************************************************
//    NAME: SurfMgrStopCallback
//
// PURPOSE: 
//
//*****************************************************************************
void CALLBACK SurfMgrStopCallback(SAMPLEBUF *pSurf)
{
  if (pSurf==NULL) return;

#ifdef USE_DIRECTSOUND
  pSurf->Stop();
#endif

#ifdef USE_BASS
  if (IsMidiKey(pSurf->id))
  {
    if (pSndMgr->m_MIDIMgr!=NULL) pSndMgr->m_MIDIMgr->Stop();
    else WriteDebugString("MIDI Mgr not loaded in SurfMgrStopCallback\n");
  }
  else
  {
    if (BASS_ChannelStop)
    {
      BASS_ChannelStop(pSurf->id);
      BASS_StreamFree(pSurf->id);
      pSurf->id = 0;
    }
  }
#endif
}

//*****************************************************************************
//    NAME: SoundMgr::StopAll
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::StopAll()
{
#ifdef USE_BASS   
  if (BASS_Init == NULL) return;
#endif

  /* will have deadlock if m_cs locked here */
  StopQueue();
  StopBgndQueue();
  StopAllSamples();
}

//*****************************************************************************
//    NAME: SoundMgr::StopAllSamples
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::StopAllSamples()
{
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif

//  CSingleLock sl(&m_cs, TRUE);
  m_WaveMgr.IterateAllSurfaces(SurfMgrStopCallback);
  m_WaveMgr.RemoveSurfaceTypes(AllSurfTypes);
  m_WaveMgr.Clear();
  
  if (m_MIDIMgr!=NULL)
  {
    delete m_MIDIMgr;
    m_MIDIMgr=NULL;    
  }
}

//*****************************************************************************
//    NAME: SoundMgr::RemoveSound
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundMgr::RemoveSound(long *key) 
{ 
#ifdef USE_BASS   
  if (BASS_Init == NULL) return FALSE;
#endif
//  CSingleLock sl(&m_cs, TRUE);
  StopSample(*key);
  //WriteDebugString("SndMgr: RemoveSound %u\n", key);
  return m_WaveMgr.RemoveSurface(*key); 
}

//*****************************************************************************
//    NAME: SoundMgr::MidiStopNotify
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::MidiStopNotify(DWORD status, long devid)
{ 
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  // Only the sound queue cares about when a sound stops playing. It waits
  // for this event to be signaled before playing the next queued sound.
  //
  // MIDI stop notification only happens for queued sounds, the sound
  // effect MIDIs don't use the 'notify' flag.
  //
  // We need to distinguish between sound queue and background queue
  // stop notices.
  //

/*
  status can be one of:
    MCI_NOTIFY_SUCCESSFUL           0x0001
    MCI_NOTIFY_SUPERSEDED           0x0002
    MCI_NOTIFY_ABORTED              0x0004
    MCI_NOTIFY_FAILURE              0x0008
*/
  // killed by app before midi finished playing, we
  // already know about it so no need to signal it
  // again
  if (status==MCI_NOTIFY_ABORTED) return;

  switch (m_MidiType)
  {
  case QUEUE_MIDI_STREAM_ID:
    //WriteDebugString("MIDI stopped playing: status %u, devid %i\n", status,devid);
    QueuedStopNotify();  
    break;
  case BGQUEUE_MIDI_STREAM_ID:
    //WriteDebugString("Background MIDI stopped playing: status %u, devid %i\n", status,devid);
    BgndQueuedStopNotify();
    break;
  default:
    //WriteDebugString("Sound effect MIDI stopped playing: status %u, devid %i\n", status,devid);
    SoundEffectStopNotify();
    break;
  }
}

//*****************************************************************************
//    NAME: SoundMgr::QueueSound
//
// PURPOSE: 
//
//***********************************************************                                                   ******************
void SoundMgr::QueueSound(CString filename) 
{ 
  //WriteDebugString("SoundMgr::QueueSound()\n");
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  
  if (SoundEnabled == 0) return;
//  CSingleLock sl(&m_cs, TRUE);

  if ((m_SndQueue != NULL) && (m_SndQueue->IsPlaying()))
  {
    m_SndQueue->StopQueue();
    delete m_SndQueue;
    m_SndQueue=NULL;
  }

  if (m_SndQueue == NULL)
    m_SndQueue = new SoundQueue();

  if (m_SndQueue != NULL) 
  {
    m_SndQueue->Add(filename); 

    if (m_SndQueue->GetCount() == 0)
    {
      delete m_SndQueue;
      m_SndQueue = NULL;
    }
  }
}

//*****************************************************************************
//    NAME: SoundMgr::PlayQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::PlayQueue() 
{ 
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  
  //WriteDebugString("SoundMgr::PlayQueue()\n");
  if (SoundEnabled == 0) return;
//  CSingleLock sl(&m_cs, TRUE);
  if (m_SndQueue != NULL) 
    m_SndQueue->PlayQueue(FALSE); // don't loop these sounds
}

//*****************************************************************************
//    NAME: SoundMgr::PlayQueueAndWait
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::PlayQueueAndWait(DWORD timeout) 
{ 
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  
//  CSingleLock sl(&m_cs, TRUE);
  if (m_SndQueue != NULL) 
    m_SndQueue->PlayQueueAndWait(timeout); 
  StopQueue(); 
}

//*****************************************************************************
//    NAME: SoundMgr::StopQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::StopQueue() 
{ 
  //WriteDebugString("SoundMgr::StopQueue()\n");
  //CSingleLock sl(&m_cs, TRUE);
  if (m_SndQueue != NULL) 
  { 
    m_SndQueue->StopQueue(); 
    delete m_SndQueue; 
  } 
  m_SndQueue=NULL; 
}

//*****************************************************************************
//    NAME: SoundMgr::QueueSound
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::QueueBgndSound(CString filename, BOOL IsLevelMusic) 
{ 
  //WriteDebugString("SoundMgr::QueueSound()\n");
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  
  if (SoundEnabled == 0) return;
//  CSingleLock sl(&m_cs, TRUE);

  if (m_BgndSndQueue == NULL)
    m_BgndSndQueue = new BackgroundSoundQueue();

  if (m_BgndSndQueue != NULL) 
  {
    m_BgndSndQueue->m_IsLevel=IsLevelMusic;
    m_BgndSndQueue->Add(filename); 

    if (m_BgndSndQueue->GetCount() == 0)
    {
      delete m_BgndSndQueue;
      m_BgndSndQueue = NULL;
    }
  }
}

//*****************************************************************************
//    NAME: SoundMgr::PlayQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::PlayBgndQueue() 
{ 
  //WriteDebugString("SoundMgr::PlayQueue()\n");
#ifdef USE_BASS    
  if (BASS_Init == NULL) return;
#endif
  
  if (SoundEnabled == 0) return;
//  CSingleLock sl(&m_cs, TRUE);
  if (m_BgndSndQueue != NULL) 
    m_BgndSndQueue->PlayQueue(TRUE); // loop bg sounds
}

BOOL SoundMgr::IsBgndQueueActive() const
{
#ifdef USE_BASS    
  if (BASS_Init == NULL) return FALSE;
#endif
  
  if (m_BgndSndQueue != NULL) 
    return (m_BgndSndQueue->ThreadActive());
  return FALSE;
}

//*****************************************************************************
//    NAME: SoundMgr::StopQueue
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::StopBgndQueue() 
{ 
  //WriteDebugString("SoundMgr::StopQueue()\n");
  //CSingleLock sl(&m_cs, TRUE);
  if (m_BgndSndQueue != NULL) 
  { 
    m_BgndSndQueue->StopQueue(); 
    delete m_BgndSndQueue; 
  } 
  m_BgndSndQueue=NULL; 
}

#ifdef USE_DIRECTSOUND
//*****************************************************************************
//    NAME: SoundMgr::GetDSB
//
// PURPOSE: 
//
//*****************************************************************************
LPDIRECTSOUNDBUFFER SoundMgr::GetDSB(long key)
{
  SAMPLEBUF *pBuf = m_WaveMgr.GetSurfacePtr(key);
  if (pBuf != NULL)
    return pBuf->GetDSB();
  else
    return NULL;
}
#endif

#ifdef USE_BASS

//*****************************************************************************
//    NAME: SoundMgr::LoadBASS
//
// PURPOSE: 
//
//*****************************************************************************
BOOL SoundMgr::LoadBASS()
{
  CString path;
#ifdef UAFEngine
  path.Format("%sbass.dll", rte.DataDir());
#else
  path.Format("%sbass.dll", ede.TemplateDataDir());
#endif

  if (!FileExists(path))
  {
		WriteDebugString("*** Error, sound lib file \'%s\' does not exist - sound disabled\n", path);
    return FALSE;
  }

  bassDLL = LoadLibrary(path);

	if (bassDLL == NULL) 
  {
		WriteDebugString("*** Error, cannot load sound lib file: \'%s\'\n", path);
    return FALSE;
	}

	/* "load" all the BASS functions that are to be used */
#ifdef __cplusplus
	#define LOADBASSFUNCTION(f) {\
		char *func=#f;\
		__asm {push func}\
		__asm {push bassDLL}\
		__asm {mov eax,GetProcAddress}\
		__asm {call eax}\
		__asm {mov f,eax}\
	}
#else
	#define LOADBASSFUNCTION(f) (FARPROC)f=GetProcAddress(bassDLL,#f)
#endif

  LOADBASSFUNCTION(BASS_GetVersion);

  if (BASS_GetVersion != NULL)
  {
	  ver = BASS_GetVersion();
    WORD MajorVer = LOWORD(ver);
    WORD MinorVer = HIWORD(ver);
    WriteDebugString("Loaded sound lib file Bass.dll, ver %u.%u\n", MajorVer, MinorVer);
    if ( (MajorVer == 0) || ((MajorVer==1)&&(MinorVer <= 4)) )
    {
      CString msg =  "Unloading Bass.dll sound lib file, incompatible version!\n"
                     "Download latest Bass.dll file from http://uaf.sourceforge.net";
#ifdef UAFEDITOR
      MsgBoxError(msg);
#else
      WriteDebugString(msg);
#endif
      FreeBASS(); // unload DLL
      return FALSE;
    }
  }

	LOADBASSFUNCTION(BASS_ErrorGetCode);
	LOADBASSFUNCTION(BASS_Init);
	LOADBASSFUNCTION(BASS_Free);
	LOADBASSFUNCTION(BASS_Start);
  LOADBASSFUNCTION(BASS_Stop);
	LOADBASSFUNCTION(BASS_StreamCreateFile);
	LOADBASSFUNCTION(BASS_StreamPlay);
	LOADBASSFUNCTION(BASS_ChannelStop);
	LOADBASSFUNCTION(BASS_ChannelSetSync);
  LOADBASSFUNCTION(BASS_SetVolume);
  LOADBASSFUNCTION(BASS_GetVolume);
  LOADBASSFUNCTION(BASS_GetGlobalVolumes);
  LOADBASSFUNCTION(BASS_SetGlobalVolumes);
  LOADBASSFUNCTION(BASS_SetBufferLength);
  LOADBASSFUNCTION(BASS_GetInfo);
  LOADBASSFUNCTION(BASS_GetDeviceDescription);
  LOADBASSFUNCTION(BASS_StreamFree);
  LOADBASSFUNCTION(BASS_ChannelGetFlags);
  LOADBASSFUNCTION(BASS_MusicLoad);
  LOADBASSFUNCTION(BASS_MusicFree);
  LOADBASSFUNCTION(BASS_MusicPlayEx);

  if (BASS_ErrorGetCode == NULL)
    WriteDebugString("BASS_ErrorGetCode is NULL\n");
  if (BASS_Init == NULL)
    WriteDebugString("BASS_Init is NULL\n");
  if (BASS_Free == NULL)
    WriteDebugString("BASS_Free is NULL\n");
  if (BASS_Start == NULL)
    WriteDebugString("BASS_Start is NULL\n");
  if (BASS_Stop == NULL)
    WriteDebugString("BASS_Stop is NULL\n");
  if (BASS_StreamCreateFile == NULL)
    WriteDebugString("BASS_StreamCreateFile is NULL\n");
  if (BASS_StreamPlay == NULL)
    WriteDebugString("BASS_StreamPlay is NULL\n");
  if (BASS_ChannelStop == NULL)
    WriteDebugString("BASS_ChannelStop is NULL\n");
  if (BASS_ChannelSetSync == NULL)
    WriteDebugString("BASS_ChannelSetSync is NULL\n");
  if (BASS_SetVolume == NULL)
    WriteDebugString("BASS_SetVolume is NULL\n");
  if (BASS_GetVolume == NULL)
    WriteDebugString("BASS_GetVolume is NULL\n");
  if (BASS_GetGlobalVolumes == NULL)
    WriteDebugString("BASS_GetGlobalVolumes is NULL\n");
  if (BASS_SetGlobalVolumes == NULL)
    WriteDebugString("BASS_SetGlobalVolumes is NULL\n");
  if (BASS_SetBufferLength == NULL)
    WriteDebugString("BASS_SetBufferLength is NULL\n");
  if (BASS_GetInfo == NULL)
    WriteDebugString("BASS_GetInfo is NULL\n");
  if (BASS_MusicLoad == NULL)
    WriteDebugString("BASS_MusicLoad is NULL\n");
  if (BASS_MusicFree == NULL)
    WriteDebugString("BASS_MusicFree is NULL\n");
  if (BASS_MusicPlayEx == NULL)
    WriteDebugString("BASS_MusicPlayEx is NULL\n");

  return TRUE;
}

/* free the BASS library from memory and delete the temporary file */
//*****************************************************************************
//    NAME: SoundMgr::FreeBASS
//
// PURPOSE: 
//
//*****************************************************************************
void SoundMgr::FreeBASS()
{
	if (bassDLL == NULL)   return;
	if (!FreeLibrary(bassDLL)) WriteDebugString("Failed to unload BASS DLL\n");
	bassDLL=NULL;
}

#endif
///////////////////////////////////////////////////////////////////////////////
//
//  Global sound helper functions
//
///////////////////////////////////////////////////////////////////////////////
#ifdef UAFEDITOR
mCList<ResourceSaveType, ResourceSaveType&> LevelSoundData[MAX_LEVELS+1];
#endif

//*****************************************************************************
//    NAME: saveSound
//
// PURPOSE: 
//
//*****************************************************************************
void SaveSound(const CString &filename, int level, const CString& destDir)
{
#ifdef UAFEDITOR
  if (filename.IsEmpty())
    return;

  CString saveName;
  int index = filename.ReverseFind('\\');

  if (index > 2)
    saveName = filename.Right(filename.GetLength()-(index+1));
  else
    saveName = filename;

//  POSITION pos = LevelSoundData[level].GetHeadPosition();
//  while (pos != NULL)
//  {
//    if (LevelSoundData[level].GetAt(pos).saveName == saveName)
//    {
//      if (alterPath)
//      {
//        // make this path point to saved pic in design folder
//        filename = LevelSoundData[level].GetAt(pos).name;
//      }
//      return;
//    }
//    LevelSoundData[level].GetNext(pos);
//  }

  CString temp = filename;
  if (writeSoundToFile(temp))
  {
//    if (alterPath)
//      filename = temp;
    ResourceSaveType data;
    data.name = temp;
    data.saveName = saveName;
    LevelSoundData[level].AddTail(data);
  }
#endif
}

//*****************************************************************************
//    NAME: deleteAllLevelSound
//
// PURPOSE: 
//
//*****************************************************************************
void deleteAllLevelSound(int level)
{
#ifdef UAFEDITOR
  LevelSoundData[level].RemoveAll();
#endif
}

//*****************************************************************************
//    NAME: writeSoundToFile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL writeSoundToFile(CString &filename)
{
#ifdef UAFEDITOR
  static char dsnPath[_MAX_PATH+1];

  if (filename.IsEmpty())
      return TRUE;

  strcpy(dsnPath, rte.SoundDir());

  // determine path to design folder
  CString saveName;
  int index = filename.ReverseFind('\\');

  if (index > 2)
    saveName.Format("%s%s", dsnPath, filename.Right(filename.GetLength()-(index+1)));
  else
    saveName.Format("%s%s", dsnPath, filename);

  if (!FileExists(filename))
  {
    CString tmp;
    tmp = filename;
    if (!SearchForFile(tmp, rte.SoundDir(), ede.TemplateSoundDir())) // returns full path file
    {
      WriteDebugString("Can't find file \'%s\' in writeSoundToFile()\n", tmp);
      return FALSE;
    }

    filename = tmp;
  }

  // if same file is already in design folder,
  // skip the copy
  if (   (FileExists(saveName)) 
      && (FileSize(saveName) == FileSize(filename)))
    return TRUE;

  SetFileAttributes(saveName, FILE_ATTRIBUTE_NORMAL);
  // copy src to design folder
  if (MyCopyFile(filename, saveName, FALSE))
  {
    SetFileAttributes(saveName, FILE_ATTRIBUTE_NORMAL);
    filename = saveName;
    return TRUE;
  }
  else
    WriteDebugString("CopyFile failed (%u): \'%s\' to \'%s\'\n", GetLastError(), filename, saveName);
#endif
  return FALSE;
}


#ifdef REQSNDS
//*****************************************************************************
//    NAME: WriteRequiredSoundList
//
// PURPOSE: 
//
//*****************************************************************************
void WriteRequiredSoundList()
{
#ifdef UAFEDITOR
  CString filename;
  filename = rte.DataDir() + "ReqSnds.txt";
  FILE *fp = fopen(filename, "w");
  CString temp;
  
  if (fp != NULL)
  {
    for (int i=0;i<=MAX_LEVELS;i++)
    {
      POSITION pos = LevelSoundData[i].GetHeadPosition();
      while (pos != NULL)
      {
        if (i < MAX_LEVELS)
        {
          temp.Format("Level %u: %s\n", i+1, LevelSoundData[i].GetNext(pos)->name);
          fprintf(fp, temp);
        }
        else
        {
          temp.Format("Global: %s\n", LevelSoundData[i].GetNext(pos)->name);
          fprintf(fp, temp);
        }
      }
    }
  }

  if (fp != NULL)
    fclose(fp);
#endif
}
#endif


#ifdef USE_DIRECTSOUND
//*****************************************************************************
//    NAME: DSErrorToString
//
// PURPOSE: 
//
//*****************************************************************************
char* DSErrorToString(HRESULT hErr)
{
  switch(hErr) 
  {
		case DSERR_ALLOCATED :          return "DSERR_ALLOCATED"; break;
		case DSERR_CONTROLUNAVAIL :     return "DSERR_CONTROLUNAVAIL"; break;
		case DSERR_INVALIDPARAM :       return "DSERR_INVALIDPARAM"; break;
		case DSERR_INVALIDCALL :        return "DSERR_INVALIDCALL"; break;
		case DSERR_GENERIC :            return "DSERR_GENERIC"; break;
		case DSERR_PRIOLEVELNEEDED :    return "DSERR_PRIOLEVELNEEDED"; break;
		case DSERR_OUTOFMEMORY :        return "DSERR_OUTOFMEMORY"; break;
		case DSERR_BADFORMAT :          return "DSERR_BADFORMAT"; break;
		case DSERR_UNSUPPORTED :        return "DSERR_UNSUPPORTED"; break;
		case DSERR_NODRIVER :           return "DSERR_NODRIVER"; break;
		case DSERR_ALREADYINITIALIZED : return "DSERR_ALREADYINITIALIZED"; break;
		case DSERR_NOAGGREGATION :      return "DSERR_NOAGGREGATION"; break;
		case DSERR_BUFFERLOST :         return "DSERR_BUFFERLOST"; break;
		case DSERR_OTHERAPPHASPRIO :    return "DSERR_OTHERAPPHASPRIO"; break;
		case DSERR_UNINITIALIZED :      return "DSERR_UNINITIALIZED"; break;
    default:                        return "Unrecognized sound error"; break;
  }
}
#endif

#ifdef USE_BASS
//*****************************************************************************
//    NAME: *BASSErrorToString
//
// PURPOSE: 
//
//*****************************************************************************
char *BASSErrorToString(int hErr)
{
  switch (hErr)
  {
  case BASS_OK: return "BASS Ok";
  case BASS_ERROR_MEM: return	"memory error";
  case BASS_ERROR_FILEOPEN: return "can't open the file";
  case BASS_ERROR_DRIVER: return	"can't find a free/valid driver";
  case BASS_ERROR_BUFLOST: return	"the sample buffer was lost - please report this!";
  case BASS_ERROR_HANDLE: return "invalid handle";
  case BASS_ERROR_FORMAT: return	"unsupported format";
  case BASS_ERROR_POSITION: return	"invalid playback position";
  case BASS_ERROR_INIT: return		"BASS_Init has not been successfully called";
  case BASS_ERROR_START: return	"BASS_Start has not been successfully called";
  case BASS_ERROR_INITCD: return	"can't initialize CD";
  case BASS_ERROR_CDINIT: return	"BASS_CDInit has not been successfully called";
  case BASS_ERROR_NOCD: return		"no CD in drive";
  case BASS_ERROR_CDTRACK: return	"can't play the selected CD track";
  case BASS_ERROR_ALREADY: return	"already initialized";
  case BASS_ERROR_CDVOL: return	"CD has no volume control";
  case BASS_ERROR_NOPAUSE: return	"not paused";
  case BASS_ERROR_NOTAUDIO: return	"not an audio track";
  case BASS_ERROR_NOCHAN: return	"can't get a free channel";
  case BASS_ERROR_ILLTYPE: return	"an illegal type was specified";
  case BASS_ERROR_ILLPARAM: return	"an illegal parameter was specified";
  case BASS_ERROR_NO3D: return		"no 3D support";
  case BASS_ERROR_NOEAX: return	"no EAX support";
  case BASS_ERROR_DEVICE: return	"illegal device number";
  case BASS_ERROR_NOPLAY: return	"not playing";
  case BASS_ERROR_FREQ: return		"illegal sample rate";
  case BASS_ERROR_NOA3D: return	"A3D.DLL is not installed";
  case BASS_ERROR_NOTFILE: return	"the stream is not a file stream (WAV/MP3)";
  case BASS_ERROR_NOHW: return		"no hardware voices available";
  case BASS_ERROR_UNKNOWN: return	"some other mystery error";
  default:      return "Unrecognized sound error";
  }
}
#endif

//*****************************************************************************
//    NAME: *SndErrorToString
//
// PURPOSE: 
//
//*****************************************************************************
char *SndErrorToString(int hErr)
{
#ifdef USE_DIRECTSOUND
  return DSErrorToString(hErr);
#endif
#ifdef USE_BASS
  return BASSErrorToString(hErr);
#endif
}

#ifdef USE_BASS
//*****************************************************************************
//    NAME: BassSyncCallback
//
// PURPOSE: 
//
//*****************************************************************************
void CALLBACK BassSyncCallback(HSYNC handle,DWORD channel,DWORD data,DWORD user)
{
  // only called for queued sounds (non-MIDI)
  //WriteDebugString("BassSyncCallback(%u)\n", channel);
  pSndMgr->QueuedStopNotify();
}

void CALLBACK BassSyncBgSoundCallback(HSYNC handle,DWORD channel,DWORD data,DWORD user)
{
  // only called for queued background sounds (non-MIDI)
  //WriteDebugString("BassSyncBgSoundCallback(%u)\n", channel);
  pSndMgr->BgndQueuedStopNotify();
}
#endif

void SoundMgr::QueuedStopNotify()
{
  if ((m_SndQueue != NULL) && (m_SndQueue->SoundFinishedEvent != NULL))
    SetEvent(m_SndQueue->SoundFinishedEvent);
}

void SoundMgr::BgndQueuedStopNotify()
{
  if ((m_BgndSndQueue != NULL) && (m_BgndSndQueue->SoundFinishedEvent != NULL))
    SetEvent(m_BgndSndQueue->SoundFinishedEvent);
}

void SoundMgr::SoundEffectStopNotify()
{
  // set SoundFinishedEvent
  if (m_MIDIMgr!=NULL) { m_MIDIMgr->Finished(); /*WriteDebugString("MIDI Finished\n");*/ }
  else WriteDebugString("MIDI Mgr not loaded in SoundEffectStopNotify\n");
}

//*****************************************************************************
//    NAME: MidiStopNotify
//
// PURPOSE: 
//
//*****************************************************************************
void MidiStopNotify(WPARAM wParam, LPARAM lParam)
{
  if (pSndMgr == NULL) return;
  pSndMgr->MidiStopNotify(wParam, lParam);
}

//*****************************************************************************
//    NAME: IsWaveFile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsWaveFile(const char *file)
{
  CString temp = file;
  temp.MakeLower();
  return (strstr(temp, ".wav") != NULL);
}

//*****************************************************************************
//    NAME: IsMIDIFile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsMIDIFile(const char *file)
{
  CString temp = file;
  temp.MakeLower();
  return (strstr(temp, ".mid") != NULL);
}

//*****************************************************************************
//    NAME: IsMP3File
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsMP3File(const char *file)
{
#ifdef USE_BASS
  CString temp = file;
  temp.MakeLower();
  if (strstr(temp, ".mp3") != NULL) return TRUE;
  if (strstr(temp, ".mp2") != NULL) return TRUE;
  if (strstr(temp, ".mp1") != NULL) return TRUE;
  return FALSE;
#else
  return FALSE;
#endif

}

//*****************************************************************************
//    NAME: IsStreamFile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsStreamFile(const char *file)
{
  return IsSampleFile(file);
}

//*****************************************************************************
//    NAME: IsSampleFile
//
// PURPOSE: 
//
//*****************************************************************************
BOOL IsSampleFile(const char *file)
{
  return ( (IsWaveFile(file)) || (IsMIDIFile(file)) || (IsMP3File(file)) );
}

BOOL IsMusicFile(const char *file)
{
  CString temp = file;
  temp.MakeLower();
  if (strstr(temp, ".mod")) return TRUE;
  if (strstr(temp, ".mo3")) return TRUE;
  if (strstr(temp, ".it")) return TRUE;
  if (strstr(temp, ".xm")) return TRUE;
  if (strstr(temp, ".s3m")) return TRUE;
  if (strstr(temp, ".mtm")) return TRUE;
  if (strstr(temp, ".umx")) return TRUE;
  return FALSE;
}

//*****************************************************************************
//    NAME: PlaySound
//
// PURPOSE: 
//
//*****************************************************************************
BOOL PlaySound(long sound, BOOL loop)
{
  if (pSndMgr == NULL) return FALSE;
  if (pSndMgr->Play(sound, loop)) return TRUE;
  pSndMgr->RemoveSound(&sound);
  return FALSE;
  //WriteDebugString("PlaySound %u\n", sound);
}

//*****************************************************************************
//    NAME: PlaySound
//
// PURPOSE: 
//
//*****************************************************************************
long PlaySound(const char *filename, BOOL loop)
{
  // filename only....no path.
  if (pSndMgr == NULL) return -1;
  BOOL ismidi = IsMIDIFile(filename);
  if (ismidi) pSndMgr->m_MidiType=SOUND_MIDI_STREAM_ID;
  //WriteDebugString("103 Calling AddSound(%s)\n", filename);
  long key = pSndMgr->AddSound(filename);
  if (key >= 0)
  {
    if (!pSndMgr->Play(key, loop))
      pSndMgr->RemoveSound(&key);
  }
  return key;
}

//*****************************************************************************
//    NAME: ClearSound
//
// PURPOSE: 
//
//*****************************************************************************
void ClearSound(long& sound)
{
  if (pSndMgr == NULL) return;
  StopSound(sound);
  pSndMgr->RemoveSound(&sound);
  //WriteDebugString("ClearSound %u\n", sound);
}

void ClearSound(int *sound) { long snd=*sound; ClearSound(snd); *sound=(int)snd;}

//*****************************************************************************
//    NAME: StopSound
//
// PURPOSE: 
//
//*****************************************************************************
void StopSound(long sound)
{
  if (pSndMgr == NULL) return;
  pSndMgr->StopSample(sound);
  //WriteDebugString("StopSound %u\n", sound);
}

//*****************************************************************************
//    NAME: LoadSound
//
// PURPOSE: 
//
//*****************************************************************************
long LoadSound(const char *file)
{
  if (pSndMgr == NULL) return -1;
  BOOL ismidi = IsMIDIFile(file);
  if (ismidi) 
    pSndMgr->m_MidiType=SOUND_MIDI_STREAM_ID;
  //WriteDebugString("104 Calling AddSound(%s)\n", file);
  long key = pSndMgr->AddSound(file);  
  //WriteDebugString("LoadSound %u (%s)\n", key, file);
  return key;
}

BOOL ValidSound(long sound)
{
  if (pSndMgr == NULL) return FALSE;
  return pSndMgr->ValidHandle(sound);
}

MIDIAsyncPlayer::MIDIAsyncPlayer(HWND hwnd, const char *DevId) : CThread(CREATE_SUSPENDED)
{
  //TRACE("MidiAsync ctor\n");
  m_hwnd=hwnd; // window handle for Win32 midi notification messages
  m_DevId = DevId;
  ExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  SoundFinishedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_Filename="";
  m_type = SOUND_MIDI_STREAM_ID;
  m_key=0;
}

MIDIAsyncPlayer::~MIDIAsyncPlayer()
{
//  CSingleLock sl(&m_cs, TRUE);
  //TRACE("MidiAsync dtor\n");
  if (ExitEvent!=NULL) SetEvent(ExitEvent);

  if (ThreadActive())
  {
#ifndef _DEBUG
    // wait for thread to exit
    if (WaitForSingleObject(Handle(), 5000)!=WAIT_OBJECT_0)
      Terminate();
#else
    WaitForSingleObject(Handle(), INFINITE);
#endif
  }
  
  CloseHandle(ExitEvent);
  ExitEvent=NULL;
  CloseHandle(SoundFinishedEvent);
  SoundFinishedEvent=NULL;
}

void MIDIAsyncPlayer::Stop()
{
//  CSingleLock sl(&m_cs, TRUE);
  //TRACE("MidiAsync stop\n");
  SetEvent(ExitEvent);
  Resume();
  if (ThreadActive())
  {
#ifndef _DEBUG
    // wait for thread to exit
    if (WaitForSingleObject(Handle(), 5000)!=WAIT_OBJECT_0)
      Terminate();
#else
    WaitForSingleObject(Handle(), INFINITE);
#endif
  }
}
BOOL MIDIAsyncPlayer::Load(const char *filename, DWORD Type)
{
//  CSingleLock sl(&m_cs, TRUE);
  //TRACE("MidiAsync load\n");
  if (!m_Filename.IsEmpty()) return FALSE;

  m_Filename=filename;
  m_type = Type;
  return TRUE;
}

BOOL MIDIAsyncPlayer::Play()
{
//  CSingleLock sl(&m_cs, TRUE);
  //TRACE("MidiAsync play\n");
  if (!IsPlaying()) return FALSE;
  Resume();
  return TRUE;
}

DWORD MIDIAsyncPlayer::Thread()
{
  if (WaitForSingleObject(ExitEvent,0)==WAIT_OBJECT_0) return 0;
  if (pSndMgr==NULL) return 0;
  if (!FileExists(m_Filename)) return 0;
  
  MIDIPlayer *pMIDIMgr = new MIDIPlayer(m_hwnd, m_DevId);
  if (pMIDIMgr==NULL)
  {
    WriteDebugString("Failed to create MIDIPlayer in MIDIAsyncPlayer\n");
    return 0;
  }

  if (!pMIDIMgr->Load(m_Filename))
  {
    WriteDebugString("Failed to load midi %s in MIDIAsyncPlayer::Thread()\n", m_Filename);
    delete pMIDIMgr;
    return 0;
  }

  pMIDIMgr->m_UseNotify=TRUE;
  if (!pMIDIMgr->Play())
  {
    WriteDebugString("Failed to play midi %s in MIDIAsyncPlayer::Thread()\n", m_Filename);
    delete pMIDIMgr;
    return 0;
  }

  if ((pSndMgr->m_MidiType < SOUND_MIDI_STREAM_ID) || (pSndMgr->m_MidiType > BGQUEUE_MIDI_STREAM_ID))
    pSndMgr->m_MidiType = SOUND_MIDI_STREAM_ID;
  
  //const char *TypeText[] = { "Bogus", "Snd Effect", "Queued", "BgQueued", "DblBogus" };
  //WriteDebugString("MIDIAsyncPlayer playing %s, type %s\n", m_Filename, TypeText[pSndMgr->m_MidiType]);

  if (SoundFinishedEvent!=NULL) ResetEvent(SoundFinishedEvent);

  HANDLE objects[3];
  objects[0] = ExitEvent;
  objects[1] = SoundFinishedEvent;   

  switch (WaitForMultipleObjects(2, objects, FALSE, INFINITE))
  {
  case WAIT_OBJECT_0: // shutdown
    pMIDIMgr->Stop();
    //WriteDebugString("MIDIAsyncPlayer stopped due to thead shutdown signal\n");
    break;
  case WAIT_OBJECT_0+1: // curr sound complete
    //WriteDebugString("MIDIAsyncPlayer completed\n");
    break;
  default:
    pMIDIMgr->Stop();
    WriteDebugString("Error after WaitForMultipleObj() in MIDIAsyncPlayer\n");
    break;
  }

  delete pMIDIMgr;
/*
  int count = 10; // wait up to one second
  while ( (WaitForSingleObject(SoundFinishedEvent,0)!=WAIT_OBJECT_0) && (count > 0))
  {
#ifdef UAFEngine
    theApp.AppSleep(100); // this one lets Win32 msg pump process queued msgs
#else
    Sleep(100); // this one doesn't, but isn't really needed by the editor anyway
#endif
    count--;
  }
  WriteDebugString("Rcvd pending midi notification: %s\n", ( (WaitForSingleObject(SoundFinishedEvent,0)==WAIT_OBJECT_0)?"Yes":"No" ));
*/
  return 0;
}

BOOL SOUND_BYTE::Play(void) const
{
  if (hSound == -2) return FALSE; // We already tried and failed!
  if (!ValidSound(hSound))
  {
    hSound = LoadSound(*this);
  };
  if (PlaySound(hSound,FALSE)) return true;  // It worked
  hSound=-2;
  return FALSE;
}

void SOUND_BYTE::Clear(void)
{
  ClearSound(&hSound);
}