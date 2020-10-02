/******************************************************************************
* Filename: PicData.h
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
#ifndef __PIC_DATA_H__
#define __PIC_DATA_H__

#include "Timer.h"
#include "SurfaceMgr.h"

class CAR;
class JWriter;
class CR_LIST;
enum CR_TYPE;
class CR_REFERENCE;

class PIC_DATA : public CTimer//: public CObject, public CTimer
{
public:
//  DECLARE_SERIAL( PIC_DATA )
  PIC_DATA() : key(-1) { Clear(); }
  ~PIC_DATA() { Clear(); }
  void Serialize(CArchive &ar, double version, const CString& path);
  void Serialize(CAR &ar, double version, const CString& path);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double ver);
#ifdef UAFEDITOR
  void Export(JWriter& jw, const char *name);
  void Import(JReader& jr, const char *name);
#endif
  void CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference) const;
  
  VOID OnTimerEvent(UINT TimerId);

  long int getPic();
  void FreePic();
  BOOL IsPicLoaded();
  BOOL LoadPicSurfaces(const CString& path);
  void SavePicData(int level);
  void SetPicTimeDelay(int ms);  
  void SetDefaults();
  void SetIconDefaults(const char *file, int frames);
  void SetPre0840LoopForeverDefaults();
  void SetPre0840LoopOnceDefaults();
  void SetPre0840NonLoopDefaults();

  BOOL SetFirstFrame();
  BOOL SetNextFrame();     // force next frame
  BOOL IsLastFrame();
  BOOL AnimateNextFrame(); // next frame only if timeDelay exceeded
  BOOL AnimateNextFrame(LONGLONG timestamp); // provide your current timestamp
  int  GetFrame(void);
  BOOL SetFrame(int frame);

  void Clear();
  PIC_DATA& operator =(const PIC_DATA& src); 
  BOOL operator ==(const PIC_DATA& src) const;

  PIC_DATA(const PIC_DATA& src) { *this = src; }

  // special behavior flags applicable to item and spell
  // animations.
  //
  // by default this pic will sequence through frames if
  // style is AS_None.
  enum 
  { 
    AS_None=0,        // all frames are sequenced
    AS_Directional=1, // 1 frame per direction: N,E,S,W,NW,NE,SW,SE
    AS_Radius=2,      // single frame covers target area radius
    AS_EachTarget=3   // single frame covers each target character only
  };

  // flags that alter behavior of the animation
  enum
  {
    AF_None=0,
    AF_KeypressBeforeLoop=1,
    AF_MaxLoopCounter=2,
    AF_Loop=4,
    AF_LoopForever=8
  };

  SurfaceType picType;
  CString filename;
  int timeDelay;
  int NumFrames;
  int FrameWidth;
  int FrameHeight;
  long key;
  int style;
  LONGLONG lastTime;
  int sx, sy;
  DWORD flags;
  DWORD MaxLoops;
  DWORD LoopCounter;
  BOOL  AllowCentering;
  BOOL  UseAlpha;
  WORD  AlphaValue;

#ifdef UAFEDITOR
   CWnd *pWnd;
   void Invalidate() { if (pWnd != NULL) pWnd->Invalidate(FALSE); }
#endif
};


// used by the game to globally hold info for currently
// displayed pic. Can be a small pic, big pic, or sprite pic.
//
extern PIC_DATA currPic;

extern PIC_DATA CursorArt;

// Used to hold list of pics to be added to design data.
// These pics are provided to players for selection 
// while creating characters in the game.
//
extern mCList<PIC_DATA, PIC_DATA&> global_IconPicImport;
extern mCList<PIC_DATA, PIC_DATA&> global_SmallPicImport;

#endif