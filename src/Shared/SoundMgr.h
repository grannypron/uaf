/******************************************************************************
* Filename: SoundMgr.h
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
#ifndef __SOUNDMGR_H_
#define __SOUNDMGR_H_

//#include "externs.h"
#include "Thread.h"
class CAR;

// Sound lib to use
// Only one of the following may be defined
//#define USE_DIRECTSOUND 1
#define USE_BASS 1


#ifdef USE_DIRECTSOUND

//#ifdef _WIN32_WINNT
//#define NTDX3
//#endif

#define CDXINCLUDEALL
#include "CDX.h"
#include "CDXSound.h"
//#include "CDXMusic.h"

#define SOUNDLIB  CDXSound
#define SAMPLEBUF CDXSoundBuffer
//#define STREAMBUF CDXMusic

#endif // USE_DIRECTX

#ifdef USE_BASS


struct BassSurfaceType
{
  BassSurfaceType() { id=0;filename=""; }
  CString GetFilename() { return filename; }
  DWORD id;
  CString filename;
};

//#define SOUNDLIB  no sound object to point to using BASS
#define SAMPLEBUF BassSurfaceType
//#define STREAMBUF DWORD

#endif

#include "SurfaceMgr.h"

// adventure sounds
extern CString DEF_STEP_SOUND;
extern CString DEF_BUMP_SOUND;

// game intro, looped
extern CString DEF_INTRO_SOUND;
// played when party dies, game over
extern CString DEF_DEATH_SOUND;

// combat sounds
extern CString DEF_HIT_SOUND;
extern CString DEF_MISS_SOUND;
extern CString DEF_LAUNCH_SOUND;
extern CString DEF_CAST_SOUND;
extern CString DEF_CHAR_DEATH_SOUND;
extern CString DEF_CHAR_MOVE_SOUND;
extern DWORD SoundPlaybackRate;

// ST_SAMPLE: wav
// ST_STREAM: midi, mp3
//
// Using DirectSound, only wav and midi are supported
// Using BASS, all types are supported
 
enum SoundType { ST_NONE=0, ST_SAMPLE=0x01 };
#define ALL_SOUND_TYPES  (ST_SAMPLE)

#ifdef USE_BASS
const char szAllSndFilter[] = 
  "All Supported|*.wav;*.mid;*.mp3;*.mp2;*.mp1;*.mo3;*.mod;*.mtm;*.xm;*.s3m;*.it;*.umx|"
  "WAV(*.wav)|*.wav|"
  "MIDI(*.mid)|*.mid|"
  "MP3(*.mp3)|*.mp3|"
  "MP2(*.mp2)|*.mp2|"
  "MP1(*.mp1)|*.mp1|"
  "MO3(*.mo3)|*.mo3|"
  "MOD(*.mod)|*.mod|"
  "MTM(*.mtm)|*.mtm|"
  "XM(*.xm)|*.xm|"
  "S3M(*.s3m)|*.s3m|"
  "IT(*.it)|*.it|"
  "UMX(*.umx)|*.umx||";

//const char szStreamSndFilter[] = "Stream File Types(*.mp3;*.mid)|*.mp3;*.mid|MP3 Files (*.mp3)|*.mp3|MIDI Files (*.mid)|*.mid||";
//const char szSampleSndFilter[] = "Sampled File Types(*.wav)|*.wav|WAV Files (*.wav)|*.wav||";
//const char szStreamSndFilter[] = "Supported File Types(*.wav;*.mid;*.mp3)|*.wav;*.mid;*.mp3|WAV Files(*.wav)|*.wav|MIDI Files (*.mid)|*.mid|MP3 Files (*.mp3)|*.mp3||";
//const char szSampleSndFilter[] = "Supported File Types(*.wav;*.mid;*.mp3)|*.wav;*.mid;*.mp3|WAV Files(*.wav)|*.wav|MIDI Files (*.mid)|*.mid|MP3 Files (*.mp3)|*.mp3||";
#endif


#ifdef USE_DIRECTSOUND
const char szStreamSndFilter[] = "Stream File Types(*.mid)|*.mid|MIDI Files (*.mid)|*.mid||";
const char szSampleSndFilter[] = "Sampled File Types(*.wav)|*.wav|WAV Files (*.wav)|*.wav||";
const char szAllSndFilter[] = "Supported File Types(*.wav;*.mid)|*.wav;*.mid|WAV Files(*.wav)|*.wav|MIDI Files (*.mid)|*.mid||";
#endif

//const DWORD STREAM_KEY = 0;
const DWORD SOUND_MIDI_STREAM_ID = 1;
const DWORD QUEUE_MIDI_STREAM_ID = 2;
const DWORD BGQUEUE_MIDI_STREAM_ID = 3;
const DWORD INVALID_STREAM_KEY = UINT_MAX;

inline BOOL IsMidiKey(long key)
{
  return ( (key==SOUND_MIDI_STREAM_ID)||(key==QUEUE_MIDI_STREAM_ID)||(key==BGQUEUE_MIDI_STREAM_ID));
}

class MIDIPlayer
{
public:
  MIDIPlayer(HWND hwnd, const char *devname);
  ~MIDIPlayer();

  BOOL Open();
  BOOL Close();
  BOOL Load(const char *filename);
  BOOL Play();
  BOOL Stop();
  static BOOL Mute();
  static BOOL Unmute();
  BOOL IsLoaded() { return (!m_Filename.IsEmpty()); }
  BOOL CanControlDevice();

  CString m_Filename;
  BOOL m_UseNotify;
  //DWORD m_type;

private:
	HWND m_hWnd;
  //CString m_DevName;
  static UINT m_MIDI_Device;

};

class CR_LIST;
class CR_REFERENCE;


class BACKGROUND_SOUNDS //: public CObject
{
public:
//  DECLARE_SERIAL( BACKGROUND_SOUNDS )
  BACKGROUND_SOUNDS() { Clear(); }
  ~BACKGROUND_SOUNDS() { Clear(); }
  void Clear() { sounds.RemoveAll(); }
  void SaveSounds(int level); 
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference);
#ifdef UAFEDITOR
  void Export(JWriter& jw, const char *name);
  bool Import(JReader& jr, const char *name);
#endif

  BACKGROUND_SOUNDS& operator =(const BACKGROUND_SOUNDS& src);
  BOOL operator ==(const BACKGROUND_SOUNDS& src) const;

  CList<CString, CString&> sounds;
};

class CR_LIST;
class CR_REFERENCE;

class BACKGROUND_SOUND_DATA //: public CObject
{
public:
//  DECLARE_SERIAL( BACKGROUND_SOUND_DATA )
  BACKGROUND_SOUND_DATA() { Clear(); }

  void Clear();
  BACKGROUND_SOUND_DATA& operator =(const BACKGROUND_SOUND_DATA& src);
#ifdef UAFEDITOR
  BOOL operator ==(const BACKGROUND_SOUND_DATA& src) const;
#endif

  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference);

  BACKGROUND_SOUNDS backgroundSounds;
  BACKGROUND_SOUNDS nightSounds;

  int EndTime;  // defines night-time hours
  int StartTime;
  BOOL UseNightMusic; // use nightSounds queue?
};

class SoundQueue : public CThread
{
public:
  SoundQueue();
  ~SoundQueue();

  DWORD Thread();
  void Clear(); // remove all queued filenames
  void Add(CString &sound); // put filename in queue
  void PlayQueue(BOOL Loop); // start playing queued filenames
  void PlayQueueAndWait(DWORD timeout); // timeout in millisecs, or INFINITE
  void StopQueue(); // stop playing
  BOOL IsPlaying() { return m_IsPlaying; }
  DWORD GetCount() { return list.GetCount(); }
  HANDLE SoundFinishedEvent;

private:
  BOOL m_Loop;
  HANDLE ExitEvent;
  long CurrSound;
  BOOL m_IsPlaying;
  BOOL Play(const char *filename);
  CCriticalSection m_cs;
  
  CList<CString, CString&> list;
};

class MIDIAsyncPlayer : public CThread
{
public:
  MIDIAsyncPlayer(HWND hwnd, const char *DevId);
  ~MIDIAsyncPlayer();

  DWORD Thread();
  BOOL Load(const char *filename, DWORD type);
  BOOL Play();
  void Stop();
  void Finished() { if (SoundFinishedEvent!=NULL) SetEvent(SoundFinishedEvent); }
  BOOL IsPlaying() { return CThread::ThreadActive(); }
  BOOL IsLoaded() const { return (!m_Filename.IsEmpty()); }
  DWORD GetType() const { return m_type; }
  HANDLE SoundFinishedEvent;
  
private:
  DWORD m_key;
  DWORD m_type;
  CString m_Filename;
  HWND m_hwnd;
  CString m_DevId;
  HANDLE ExitEvent;
  CCriticalSection m_cs;  
};

class BackgroundSoundQueue : public CThread
{
public:
  BackgroundSoundQueue();
  ~BackgroundSoundQueue();

  DWORD Thread();
  void Clear(); // remove all queued filenames
  void Add(CString &sound); // put filename in queue
  void PlayQueue(BOOL Loop); // start playing queued filenames
  void StopQueue(); // stop playing
  BOOL IsPlaying() { return m_IsPlaying; }
  DWORD GetCount() { return list.GetCount(); }
  HANDLE SoundFinishedEvent;
  BOOL IsLevelMusic() const { return m_IsLevel; }
  BOOL m_IsLevel; // else zone music

private:

  BOOL m_Loop;
  HANDLE ExitEvent;
  long CurrSound;
  BOOL m_IsPlaying;
  BOOL Play(const char *filename);
  CCriticalSection m_cs;  
  CList<CString, CString&> list;
};

class SoundMgr
{
public:
  friend SoundQueue;
  friend BackgroundSoundQueue;

  SoundMgr(HWND hWnd);
  ~SoundMgr();

  void Clear();
  long AddSound(const char *filename);
  BOOL RemoveSound(long *key);

  BOOL Play(long key, BOOL loop=FALSE); // play any
  void StopAll();
  void StopSample(long key);

  BOOL ValidHandle(long key);
  void QueueSound(CString filename); 
  void PlayQueue(); 
  void PlayQueueAndWait(DWORD timeout=INFINITE); 
  void StopQueue(); 

  void QueueBgndSound(CString filename, BOOL IsLevel); 
  void PlayBgndQueue(); 
  void StopBgndQueue(); 
  BOOL IsBgndQueueActive() const;
  BOOL IsBgndQueueLevelMusic() const { if (IsBgndQueueActive()) return m_BgndSndQueue->IsLevelMusic(); else return FALSE; } 

  void EnableSound(WORD types, BOOL enable);
  void MidiStopNotify(DWORD status, long lParam);
  void QueuedStopNotify();
  void BgndQueuedStopNotify();
  void SoundEffectStopNotify();

  void SetMasterVolume(int vol);
  int  GetMasterVolume();
  void SetMusicEnable(BOOL enable);
  BOOL GetMusicEnable();
  void MuteVolume(void);
  void UnMuteVolume(void);
  
#ifdef USE_DIRECTSOUND
  LPDIRECTSOUNDBUFFER GetDSB(long key);
#endif

  //MIDIPlayer m_MIDIMgr;
  MIDIAsyncPlayer *m_MIDIMgr;
  DWORD  m_MidiType;

private:

  WORD SoundEnabled;

#ifdef USE_DIRECTSOUND
  CDXSound *pSound;
  LPDIRECTSOUNDNOTIFY m_lpDsNotify;
#endif

#ifdef USE_BASS
  DWORD m_StreamId;
  BOOL LoadBASS();
  void FreeBASS();
#endif

  HWND m_hWnd;
  CCriticalSection m_cs;
  SoundQueue *m_SndQueue;
  BackgroundSoundQueue *m_BgndSndQueue;
  SurfaceMgr<SAMPLEBUF*> m_WaveMgr;

  DWORD m_MusicVol;
  DWORD m_SampleVol;
  DWORD m_StreamVol;
  DWORD m_PrevMusicVol; // to restore volume at exit
  DWORD m_PrevSampleVol;
  DWORD m_PrevStreamVol;
  BOOL  m_MusicOn;

  long AddStream(const char *filename);
  long AddSample(const char *filename);

  BOOL PlaySample(long key, BOOL loop=FALSE); // for ST_SAMPLE  
  void StopAllSamples();

  BOOL PlayStream(long key, BOOL loop=FALSE); // ST_STREAM
  void StopStream(long key);

  BOOL SetupNotifyStop(DWORD key, HANDLE hEvent=NULL);
  BOOL SetupBgndNotifyStop(DWORD key, HANDLE hEvent=NULL);
  void CleanupNotifyStop();
};

extern SoundMgr *pSndMgr;

const int GLOBAL_SOUND = MAX_LEVELS;

void MidiStopNotify(WPARAM wParam, LPARAM lParam);

BOOL IsWaveFile(const char *file);
BOOL IsMIDIFile(const char *file);
BOOL IsMP3File(const char *file);
BOOL IsStreamFile(const char *file);
BOOL IsSampleFile(const char *file);
BOOL IsMusicFile(const char *file);

void StopAllSounds();
void ReleaseAllSounds();
BOOL PlaySound(long sound, BOOL loop=FALSE);
long PlaySound(const char *filename, BOOL loop=FALSE);
BOOL ValidSound(long sound);
void ClearSound(long &sound);
void ClearSound(int *sound );
void StopSound(long sound);
long LoadSound(const char *file);

void SaveSound(const CString &filename, int level, /*BOOL alterPath,*/ const CString& dir);
void deleteAllLevelSound(int level);
BOOL writeSoundToFile(CString &filename);
#ifdef REQSNDS
void WriteRequiredSoundList();
#endif

#ifdef USE_DIRECTSOUND
char* DSErrorToString( HRESULT hErr );
#endif

#ifdef USE_BASS
char* BASSErrorToString( int hErr );
#endif

char* SndErrorToString( int hErr );

#endif // __SOUNDMGR_H_