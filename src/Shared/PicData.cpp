/******************************************************************************
* Filename: PicData.cpp
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
#include "..\shared\stdafx.h"
//#include "..\Shared\Version.h"

#include "externs.h"

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#endif
#include "sharedqueue.h"
#include "class.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "PicSlot.h"
#include "PicData.h"
#include "Viewport.h"

#ifdef UAFEDITOR
#include "..\UAFWinEd\resource.h"
#include "..\UAFWinEd\CrossReference.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char *SurfaceTypeText(int mask);

//IMPLEMENT_SERIAL( PIC_DATA, CObject, 1 )

// used by the game to globally hold info for currently
// displayed pic. Can be a small pic, or a big pic.
//
PIC_DATA currPic;
PIC_DATA CursorArt;


// Used to hold list of pics to be added to design data.
// These pics are provided to players for selection 
// while creating characters in the game.
//
mCList<PIC_DATA, PIC_DATA&> global_IconPicImport;
mCList<PIC_DATA, PIC_DATA&> global_SmallPicImport;

void PIC_DATA::PreSerialize(BOOL IsStoring)
{
  if (IsStoring)
  {
    StripFilenamePath(filename);
  }
}

void PIC_DATA::PostSerialize(BOOL IsStoring, double ver)
{
  if (!IsStoring && (ver < 0.954))
  {
    StripFilenamePath(filename);
  };
  // PicArtDir migh not be correct!!!  AddFolderToPath(filename, rte.PicArtDir());
}

void PIC_DATA::Serialize(CArchive &ar, double version, const CString& path)
{
  PreSerialize(ar.IsStoring());
//  CObject::Serialize(ar);  
  int i;

  if (ar.IsStoring())
  {
    i = picType;
    ar << i;

    StripFilenamePath(filename);
    AS(ar, filename);
    ar << timeDelay;
    ar << NumFrames;
    ar << FrameWidth;
    ar << FrameHeight;   
    ar << flags;
    ar << MaxLoops;
    // no need to save LoopCounter
 
    // if you use a non-default style flag, it should be stored
    // in the attribute map for the object that is using it.
    //ar << style;

    ar << UseAlpha;
    ar << AlphaValue;
    ar << RestartFrame;

  }
  else
  {
    ar >> i;
    picType = (SurfaceType)i;

    DAS(ar,filename);
    //if (filename != "") filename = path + filename;
    //  No.No. Why Icon?  Why not Cursor?  AddFolderToPath(filename, rte.IconArtDir());
    if (globalData.version <0.930269)
    {
      StripFilenamePath(filename);
    };
    if (filename.IsEmpty())
    {
      picType = BogusDib;
    };
    ar >> timeDelay;
    ar >> NumFrames;
    ar >> FrameWidth;
    ar >> FrameHeight;
    if (version >= _VERSION_0790_)
      ar >> flags;
    if (version >= _VERSION_0810_)
      ar >> MaxLoops;

    // if you use a non-default style flag, it should be stored
    // in the attribute map for the object that is using it.
    //ar << style;

    if (version >= _VERSION_0906_)
    {
      ar >> UseAlpha;
      ar >> AlphaValue;
    }    

    if (version >= _VERSION_524)
    {
        ar >> RestartFrame;
    }
  }

  PostSerialize(ar.IsStoring(), version);
}

void PIC_DATA::Serialize(CAR &ar, double version, const CString& path)
{
  PreSerialize(ar.IsStoring());
  //CObject::Serialize(ar);
  int i;
  if (ar.IsStoring())
  {
    i = picType;
    ar << i;
  
    StripFilenamePath(filename);
    AS(ar,filename);
    ar << timeDelay;
    ar << NumFrames;
    ar << FrameWidth;
    ar << FrameHeight;   
    ar << flags;
    ar << MaxLoops;
    ar <<  style;
    // no need to save LoopCounter
    ar << UseAlpha;
    ar << AlphaValue;    
    ar << RestartFrame;
  }
  else
  {
    ar >> i;
    picType = (SurfaceType)i;

    DAS(ar,filename);
    //if (filename != "") filename = path + filename;
    if (globalData.version <0.930269)
    {
      StripFilenamePath(filename);
    };
    if (filename.IsEmpty())
    {
      picType = BogusDib;
    };
    ar >> timeDelay;
    ar >> NumFrames;
    ar >> FrameWidth;
    ar >> FrameHeight;
    if (version >= _VERSION_0790_)
      ar >> flags;
    if (version >= _VERSION_0810_)
      ar >> MaxLoops;
    if (version >= _VERSION_0900_)
      ar >> style;

    if (version >= _VERSION_0906_)
    {
      ar >> UseAlpha;
      ar >> AlphaValue;
    }
    if (version >= _VERSION_524)
    {
      ar >> RestartFrame;
    }
  }

  PostSerialize(ar.IsStoring(), version);
}


#ifdef UAFEDITOR

const char *JKEY_PICTYPE = "picType";
const char *JKEY_FILENAME = "filename";
const char *JKEY_TIMEDELAY = "timeDelay";
const char *JKEY_NUMFRAME = "numFrame";
extern const char *JKEY_WIDTH;
extern const char *JKEY_HEIGHT;
extern const char *JKEY_FLAGS;
const char *JKEY_MAXLOOPS = "maxLoops";
const char *JKEY_STYLE = "style";
const char *JKEY_ALLOWCENTERING = "allowCentering";
const char *JKEY_USEALPHA = "useAlpha";
const char *JKEY_ALPHA = "alpha";
const char* JKEY_RESTART_FRAME = "restartFrame";

void PIC_DATA::Export(JWriter& jw, const char *name)
{
  if (picType == BogusDib) return;
  jw.StartList(name);
  jw.NameAndEnum(JKEY_PICTYPE, picType);
  jw.NameAndValue(JKEY_FILENAME, filename);
  jw.NameAndValue(JKEY_TIMEDELAY, timeDelay);
  jw.NameAndValue(JKEY_NUMFRAME, NumFrames);
  jw.NameAndValue(JKEY_WIDTH, FrameWidth);
  jw.NameAndValue(JKEY_HEIGHT, FrameHeight);   
  jw.NameAndValue(JKEY_FLAGS, flags);
  jw.NameAndValue(JKEY_MAXLOOPS, MaxLoops);
  jw.NameAndValue(JKEY_STYLE, style);
  jw.NameAndBool(JKEY_ALLOWCENTERING, AllowCentering);
    // no need to save LoopCounter
  jw.NameAndBool(JKEY_USEALPHA, UseAlpha);
  jw.NameAndValue(JKEY_ALPHA, AlphaValue);    
  jw.NameAndValue(JKEY_RESTART_FRAME, RestartFrame);
  jw.EndList();
};
void PIC_DATA::Import(JReader& jr, const char *name)
{
  if (jr.Optional(), !jr.StartList(name)) return;
  jr.NameAndEnum(JKEY_PICTYPE, picType);
  jr.NameAndValue(JKEY_FILENAME, filename);
  jr.NameAndValue(JKEY_TIMEDELAY, timeDelay);
  jr.NameAndValue(JKEY_NUMFRAME, NumFrames);
  jr.NameAndValue(JKEY_WIDTH, FrameWidth);
  jr.NameAndValue(JKEY_HEIGHT, FrameHeight);   
  jr.NameAndValue(JKEY_FLAGS, flags);
  jr.NameAndValue(JKEY_MAXLOOPS, MaxLoops);
  jr.NameAndValue(JKEY_STYLE, style);
  jr.NameAndBool(JKEY_ALLOWCENTERING, AllowCentering);
    // no need to save LoopCounter
  jr.NameAndBool(JKEY_USEALPHA, UseAlpha);
  jr.NameAndValue(JKEY_ALPHA, AlphaValue);    
  jr.NameAndValue(JKEY_RESTART_FRAME, RestartFrame);
  jr.EndList();
};
#endif

#ifdef UAFEDITOR
void PIC_DATA::CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference) const
{
  pCRList->CR_AddPicReference(this->filename, pCRReference);
}
#endif

//*****************************************************************************
// NAME:    getPic
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
long int PIC_DATA::getPic()
{
  return key;
}

//*****************************************************************************
// NAME:    FreePic
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PIC_DATA::FreePic()
{
  lastTime=0;
  if (CTimer::Active())
    StopTimer();

  if (key >= 0)
    GraphicsMgr.ReleaseSprite(key);
}

//*****************************************************************************
// NAME:    IsPicLoaded
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PIC_DATA::IsPicLoaded()
{
   return (GraphicsMgr.ValidSprite(key));
}

//*****************************************************************************
// NAME:    LoadPicSurfaces
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PIC_DATA::LoadPicSurfaces(const CString& path)
{
   FreePic();  

   // default destination on screen, most go into viewport
   sx = ViewportX;
   sy = ViewportY;

   if (filename.IsEmpty())
     return FALSE;

   if (picType == BigPicDib) //DLD 12/20/00
   {                         //
     //SetDefaults();          // // Only overwrite if really empty because we now allow animation frames in BigPics
   }                         //end add

   //
   // multi-frame sprites will have a larger width than that
   // of the viewport.
   //

   if ((FrameWidth <= 0) || (FrameHeight <= 0) || (NumFrames <= 0))
     SetDefaults(); // probably wrong guess, but try it anyway

   if (FrameWidth <= 0) FrameWidth = ViewportWidth;
   if (FrameHeight <= 0) FrameHeight = ViewportHeight;
   if (NumFrames <= 0) NumFrames = 1;

  if (path.IsEmpty())
  {
    //WriteDebugString("110 Calling AddSprite(%s)\n", filename);
    key = GraphicsMgr.AddSprite(filename, FrameWidth, FrameHeight, NumFrames, RestartFrame, picType);
  }
  else
  {
    //WriteDebugString("111 Calling AddSprite(%s)\n", path + filename);
    key = GraphicsMgr.AddSprite(filename, FrameWidth, FrameHeight, NumFrames, RestartFrame, picType);
  };

//   if (key < 0)
//   {
//     if (filename.Find(prefix,0) < 0)
//     {
//       CString name(filename);
//       CString path("");
//       GetFilenamePath(filename, path);
//       StripFilenamePath(name);
//       path = prefix + name;
//
//       key = GraphicsMgr.AddSprite(path, FrameWidth, FrameHeight, NumFrames, picType);
//       if (key >= 0)
//       {
//         WriteDebugString("Adjusted pic file %s to %s in LoadPicSurfaces\n", filename, path);
//         filename = path;
//       }
//     }
//     
     if (key < 0)
     {
       if (!debugStrings.AlreadyNoted(CString("FTLPF01") + filename))
       {
         debugSeverity = 7;
         WriteDebugString("Failed to load pic file %s in LoadPicSurfaces\n", filename);
       };
       return FALSE;
     }
//   }

   if (NumFrames <= 1)
     timeDelay = 0;

   if (timeDelay > 0)
   {
     if ((timeDelay % 30) != 0)
       timeDelay -= (timeDelay%30);
     if (timeDelay < 30) timeDelay=30;
   }

   // default destination on screen, most go into viewport
   if (picType == BigPicDib)
   {
     sx = BigPic_x; //DLD 12/20/00 sx = ViewportScreenX;
     sy = BigPic_y; //DLD 12/20/00 sy = ViewportScreenY;
   }
   else 
   {
     sx = ViewportX;
     sy = ViewportY;
   }

   // force area view off if viewing overland map
   if (globalData.levelInfo.stats[globalData.currLevel].overland)
      globalData.useAreaView = FALSE;

   BOOL valid = GraphicsMgr.ValidSprite(key);
   SetFirstFrame();
   return valid;
}

BOOL PIC_DATA::SetFirstFrame()
{
#ifdef UAFEDITOR
   SetPicTimeDelay(timeDelay);
#endif

#ifdef UAFEngine
   if (timeDelay > 0)
     lastTime = virtualGameTime;//timeGetTime();
#endif

  return GraphicsMgr.SetFirstFrame(key);
}

/* not used
BOOL PIC_DATA::AnimateNextFrame()
{
  return AnimateNextFrame(timeGetTime());
}
*/

BOOL PIC_DATA::AnimateNextFrame(LONGLONG timestamp)
{
  if (timeDelay < 30) return FALSE;
  if (!IsPicLoaded()) return FALSE;
  
  //ASSERT(picType == SmallPicDib);
  OutputDebugString(CString("PIC_DATA::AnimateNextFrame"));
  OutputDebugString(CString(filename));
  OutputDebugString("\n");
  // combat art is animated in COMBAT_SPRITE_DATA::Update()
  if ((picType == SmallPicDib || picType == BigPicDib) && (style == AS_None))
  {
    if ((timestamp - lastTime) >= (DWORD)timeDelay)
    {
#ifdef UAFEngine
      lastTime = timestamp;
      BOOL islastframe = IsLastFrame();

      // if last frame and not looping, then we're done
      if ((islastframe) && ((flags&AF_Loop)==0))
        return FALSE;

      // want keypress before looping back to first frame?
      if (flags & AF_KeypressBeforeLoop)
      {
        if (islastframe)
          return TRUE;
      }
      // loop count throttled?
      if (flags & AF_MaxLoopCounter)
      {
        if (islastframe)
        {
          LoopCounter++;
          if (LoopCounter >= MaxLoops)
            return FALSE;
        }
      }
#endif
      return GraphicsMgr.SetNextFrame(key);
    }
  }
  return FALSE;
}

BOOL PIC_DATA::IsLastFrame()
{
  return GraphicsMgr.IsLastFrame(key);
}

BOOL PIC_DATA::SetNextFrame()
{
  // this will loop back to the first frame
  // after the last frame is displayed
  return GraphicsMgr.SetNextFrame(key);
}

BOOL PIC_DATA::SetFrame(int frame)
{
  return GraphicsMgr.SetFrame(key, frame);
}

int PIC_DATA::GetFrame(void)
{
  return GraphicsMgr.GetFrame(key);
}

void PIC_DATA::SetPicTimeDelay(int ms)
{
  if (CTimer::Active())
    StopTimer();

  timeDelay = ms;

  if (style != AS_None) // is sequenced?
    return;
  
  if (key < 0)
    return;

  LoopCounter=0;
  if (MaxLoops <= 0)
    MaxLoops=1;

  if (timeDelay >= 30)
  {
    if (!StartPeriodicTimer(timeDelay))
      WriteDebugString("Failed to start timer for PIC_DATA(%s)\n", filename);
  }
  else
    GraphicsMgr.SetFirstFrame(key);
}

//
// This function sets common defaults for pic types, but only
// when the data has not already been initialized with valid values.
//
void PIC_DATA::SetDefaults()
{
  ASSERT (picType != BogusDib);

  if (   (NumFrames > 0) 
      && (FrameWidth > 0) 
      && (FrameHeight > 0) 
      && (picType != BigPicDib) 
      && (picType != SmallPicDib) 
      && (picType != IconDib) 
      && (picType != SpriteDib)) //DLD 12/20/00
    return;

  switch (picType)
  {
  case SmallPicDib:
    sx = ViewportX;
    sy = ViewportY;
    FrameWidth = ViewportWidth;
    FrameHeight = ViewportHeight;
    NumFrames = 1;
    RestartFrame = 1;
    SetPre0840LoopForeverDefaults();
    break;
  case IconDib:
    sx = -1; // never used by icons
    sy = -1;
    FrameWidth = COMBAT_TILE_WIDTH;
    FrameHeight = COMBAT_TILE_HEIGHT;
    NumFrames = 2; // 1 ready, 1 attack pose
    SetPre0840NonLoopDefaults();
    break;
  case SpriteDib:
    sx = ViewportX;
    sy = ViewportY;
    FrameWidth = ViewportWidth;
    FrameHeight = ViewportHeight;
    NumFrames = 3; // 1 for each distance from party: UpClose, Near, Distant
    SetPre0840NonLoopDefaults();
    break;
  case BigPicDib:
    sx = BigPic_x;         //DLD 12/20/00 //sx = ViewportScreenX;
    sy = BigPic_y;         // sy = ViewportScreenY;
    FrameWidth = BigPic_w; // FrameWidth = 608;
    FrameHeight = BigPic_h;//FrameHeight = 288; //end add change 
    NumFrames = 1;
    SetPre0840NonLoopDefaults();
    break;

  default:
    ASSERT( FALSE );
    break;
  }
}

//*****************************************************************************
// NAME:    SavePicData
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PIC_DATA::SavePicData(int level)
{
  if (strlen(filename))
    SaveArt(filename, picType, level, TRUE, rte.SmallPicDir());
}

VOID PIC_DATA::OnTimerEvent(UINT TimerId) // TimerId is a PIC_DATA* that started timer
{
  if (style != AS_None) // is sequenced?
    return;

  // cannot provide feedback to display dialog
  // for when animation stops
  /*
  if (flags&AF_MaxLoopCounter)
  {
    if (GraphicsMgr.IsLastFrame(key))
      LoopCounter++;

    if (LoopCounter<MaxLoops)
    {
      GraphicsMgr.SetNextFrame(key);
#ifdef UAFEDITOR
      Invalidate();
#endif
    }
    else
    {
      SetPicTimeDelay(0); // stop timer
      GraphicsMgr.SetFirstFrame(key);
    }
  }
  else
  */
  {
    GraphicsMgr.SetNextFrame(key);
#ifdef UAFEDITOR
    Invalidate();
#endif
  }
}

/*
// helper functions that store/retrieve values in attribute maps
void StorePicDataAsASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix, unsigned char flags)
{
  CString val;
  CString key;

  key.Format("%s_FName", prefix);
  asl.Insert(key, data.filename, flags);

  key.Format("%s_Type", prefix);
  val.Format("%i", data.picType);
  asl.Insert(key, val, flags);

  key.Format("%s_Delay", prefix);
  val.Format("%i", data.timeDelay);
  asl.Insert(key, val, flags);

  key.Format("%s_NFrames", prefix);
  val.Format("%i", data.NumFrames);
  asl.Insert(key, val, flags);

  key.Format("%s_FWidth", prefix);
  val.Format("%i", data.FrameWidth);
  asl.Insert(key, val, flags);

  key.Format("%s_FHeight", prefix);
  val.Format("%i", data.FrameHeight);
  asl.Insert(key, val, flags);

  key.Format("%s_Style", prefix);
  val.Format("%i", data.style);
  asl.Insert(key, val, flags);
}
*/

#ifdef UAFEDITOR
void RetrievePicDataFromASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix)
{
  CString key;
  CString val;

  data.Clear();

  key.Format("%s_FName", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  //AddFolderToPath(val, rte.IconArtDir());
  data.filename = val;

  key.Format("%s_Type", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.picType = (SurfaceType)atoi(val);
  if (data.filename.IsEmpty())
  {
    data.picType = BogusDib;
  };

  key.Format("%s_Delay", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.timeDelay = atoi(val);

  key.Format("%s_NFrames", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.NumFrames = atoi(val);

  key.Format("%s_FWidth", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.FrameWidth = atoi(val);

  key.Format("%s_FHeight", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.FrameHeight = atoi(val);

  key.Format("%s_Style", prefix);
  val = asl.Lookup(key);
  asl.Delete(key);
  data.style = atoi(val);
}
#endif

void PIC_DATA::Clear()
{ 
   FreePic();
   filename="";
   timeDelay=0;
   key=-1;
   picType=BogusDib; 
   NumFrames=0;
   FrameWidth=0;
   FrameHeight=0;
   style=AS_None;
   lastTime=0;
   sx = ViewportX;
   sy = ViewportY;
   flags=AF_Loop;
   MaxLoops=1;
   LoopCounter=0;
   AllowCentering=TRUE;
   UseAlpha=FALSE;
   AlphaValue=0;   
   RestartFrame=0;
}

PIC_DATA& PIC_DATA::operator =(const PIC_DATA& src) 
{ 
  if (this==&src) return *this;
  Clear();
  picType = src.picType;
  if (!src.filename.IsEmpty()) filename=src.filename;
  timeDelay=src.timeDelay;
  key=src.key;
  NumFrames=src.NumFrames;
  FrameWidth=src.FrameWidth;
  FrameHeight=src.FrameHeight;
  style=src.style;
  sx=src.sx;
  sy=src.sy;
  flags=src.flags;
  MaxLoops=src.MaxLoops;
  LoopCounter=src.LoopCounter;
  AllowCentering=src.AllowCentering;
  UseAlpha=src.UseAlpha;
  AlphaValue=src.AlphaValue;
  RestartFrame=src.RestartFrame;
  return *this;
}

BOOL PIC_DATA::operator ==(const PIC_DATA& src) const
{ 
  if (picType != src.picType) return FALSE;
  if (filename != src.filename) return FALSE;
  if (timeDelay != src.timeDelay) return FALSE;
  if (key != src.key) return FALSE;
  if (NumFrames != src.NumFrames) return FALSE;
  if (FrameWidth != src.FrameWidth) return FALSE;
  if (FrameHeight != src.FrameHeight) return FALSE;
  if (style != src.style) return FALSE;
  if (sx!=src.sx) return FALSE;
  if (sy!=src.sy) return FALSE;
  if (flags!=src.flags) return FALSE;
  if (MaxLoops!=src.MaxLoops) return FALSE;
  if (AllowCentering!=src.AllowCentering) return FALSE;
  if (UseAlpha!=src.UseAlpha) return FALSE;
  if (AlphaValue!=src.AlphaValue) return FALSE;
  if (RestartFrame!=src.RestartFrame) return FALSE;

  // ignore transient LoopCounter
  return TRUE; 
}

void PIC_DATA::SetPre0840LoopForeverDefaults()
{
  if (NumFrames <= 1) return;
  flags |= AF_Loop;
  flags &= ~AF_MaxLoopCounter;
}
void PIC_DATA::SetPre0840LoopOnceDefaults()
{
  if (NumFrames <= 1) return;
  flags |= AF_Loop;
  flags |= AF_MaxLoopCounter;
  MaxLoops=1;
}

void PIC_DATA::SetPre0840NonLoopDefaults()
{
  flags &= ~AF_Loop;
}


