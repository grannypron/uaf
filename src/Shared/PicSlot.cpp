/******************************************************************************
* Filename: PicSlot.cpp
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

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#include "..\UAFWinEd\resource.h"
#include "class.h"
#include "..\UAFWinEd\CrossReference.h"
#else
#include "externs.h"
#include "..\UAFWin\Dungeon.h"
#endif

#include "class.h"
#include "SurfaceMgr.h"
#include "globalData.h"
//#include "level.h"
#include "SoundMgr.h"
#include "PicData.h"
#include "PicSlot.h"
#include "Graphics.h"
#include "Viewport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Deleted 20121208 PRS
//IMPLEMENT_SERIAL( WallSetSlotMemType, CObject, 1 )
//IMPLEMENT_SERIAL( BackgroundSlotMemType, CObject, 1 )
//IMPLEMENT_SERIAL( PicDataType, CObject, 1 )

#define IMAGE_CACHE_MAP_SIZE (0xFFFFFFFD)

#ifdef UAFEDITOR
mCList<ResourceSaveType, ResourceSaveType&> LevelPicData[MAX_LEVELS+1];
#endif


extern const double VersionSpellNames;
extern const double VersionSpellIDs;
extern const double PRODUCT_VER;



BackgroundSlotMemType BackgroundSets[MAX_BACKGROUNDS];
WallSetSlotMemType WallSets[MAX_WALLSETS];
BOOL MyDeleteFile(LPCSTR name);
BOOL MyCopyFile(LPCSTR src, LPCSTR dest, BOOL failfIfExists);

#ifdef _DEBUG
// low size forces frequent image reloading, good
// for performance checking.
int WALL_CACHE_SIZE = 4; 
int BACKGROUND_CACHE_SIZE = 4;
#else
// allow up to 16 slots to be loaded
// and in memory at once.
int WALL_CACHE_SIZE = 16*3;       // max 3 images per slot
int BACKGROUND_CACHE_SIZE = 16*2; // max 2 images per slot
#endif

ImageCacheMgr *pWallSlotMgr=NULL;
ImageCacheMgr *pBgSlotMgr=NULL;

PicDataType HBarVPArt;
PicDataType VBarVPArt;
PicDataType FrameVPArt;
PicDataType VBarCombArt;
PicDataType HBarCombArt;
PicDataType CombatWallArt;
//PicDataType CombatCommonArt;
PicDataType CombatCursorArt;
PicDataType CombatDeathIconArt;
PicDataType CombatPetrifiedIconArt;
PicDataType CombatDeathArt;
PicDataType CharViewFrameVPArt;

CString DEFAULT_TITLE_BG = "Title.png";
CString DEFAULT_CREDITS_BG = "Credits.png";

// combat cursor
CString DEFAULT_CC = "cu_DefCC.png";
// combat death icon
CString DEFAULT_CDI = "DefCDI.png";
// combat petrified icon
CString DEFAULT_CPI = "DefCPI.png";
// combat death
CString DEFAULT_CD = "DefCD.png";
// combat wall set (Dungeon)
CString DEFAULT_CWD = "cw_DefCW.png";
// combat wall set (Wilderness)
CString DEFAULT_CWW = "cww_DefCWW.png";
// frame
CString DEFAULT_FR = "bo_DefFR.png";
// horz bar
CString DEFAULT_HB = "bo_DefHB.png"; 
// vert bar
CString DEFAULT_VB = "bo_DefVB.png"; 
// icon bg, when player picks icon
CString DEFAULT_IB = "DefIB.png";
// camp pic
CString DEFAULT_CA = "sp_DefCA.png";
// treasure pic 
CString DEFAULT_TR = "sp_DefTR.png";
// icon used when monster has none specified
CString DEFAULT_MONSTER_ICON = "cm_DefMI.png";

// item in-route-to-target throw sprite (item type = HandThrow, Throw)
CString DEFAULT_IIRT = "item_Dagger.png"; // manikus
// item in-route-to-target Sling sprite (item type = SlingNoAmmo)
CString DEFAULT_IIRS = "item_Stone.png"; // manikus
// item in-route-to-target Ammo sprite (item type = Ammo, such as arrows)
CString DEFAULT_IIRA = "item_Arrow.png";  // manikus

// spell in-route-to-target sprite
CString DEFAULT_SIR = "spell_missile_MU.png"; // manikus
// spell target coverage sprite
CString DEFAULT_STC = "spl_DefSTC.png";
// spell target hit sprite
CString DEFAULT_STH = "spl_DefSTH.png";
// spell target linger sprite
CString DEFAULT_STL = "spl_DefSTL.png";
// NPC default combat icon
CString DEFAULT_NPC_ICON = "icon_NPC_Guard.png"; // manikus
// NPC default small pic
CString DEFAULT_NPC_PIC = "portrait_NPC_Guard.png"; // manikus
// default cursor art
CString DEFAULT_CURSOR = "cu_Cursor.png";
// default overland background image
CString DEFAULT_OVERLAND = "bigpic_Map.png"; // manikus

CString DEFAULT_ZONEAVART = "AreaViewZoneArt.png";

#ifdef UAFEDITOR


extern const char *SurfaceTypeTexts[];
const char *SurfaceTypeText(int mask)
{
  int i;
  for (i=0; i<NUM_SURFACE_TYPES; i++, mask>>=1)
  {
    if (mask & 1) return SurfaceTypeTexts[i];
  }
  return "";
};
#endif

ImageCacheMgr::ImageCacheMgr(DWORD maxsize, CString id)
{
  m_maxsize=maxsize;
  m_sortsize = max(m_maxsize+50, 100);
  m_sortarray=new sortdata[m_sortsize];
  m_id = id;
  WriteDebugString("Init %s image cache with size %u\n",m_id, maxsize);
  int tablesize = m_maxsize+(m_maxsize*0.2);
  m_SurfaceMap.InitHashTable(tablesize);
}

ImageCacheMgr::~ImageCacheMgr() 
{ 
  Clear(); 
  if (m_sortarray!=NULL) 
    delete [] m_sortarray;
}

void ImageCacheMgr::Clear()
{
//  CSingleLock sl(&m_cs, TRUE);
  m_SurfaceMap.RemoveAll();
  int tablesize = m_maxsize+(m_maxsize*0.2);
  m_SurfaceMap.InitHashTable(tablesize);
}

BOOL ImageCacheMgr::LockSurface(DWORD hashkey)
{
//  CSingleLock sl(&m_cs, TRUE);
  ImageCacheData tmp;
  if (!m_SurfaceMap.Lookup(hashkey, tmp))
    return FALSE;
  tmp.locked=TRUE;
  return TRUE;
}

BOOL ImageCacheMgr::UnlockSurface(DWORD hashkey)
{
//  CSingleLock sl(&m_cs, TRUE);
  ImageCacheData tmp;
  if (!m_SurfaceMap.Lookup(hashkey, tmp))
    return FALSE;
  tmp.locked=FALSE;
  return TRUE;
}

BOOL ImageCacheMgr::GetSurface(DWORD hashkey, ImageCacheData &surf)
{
//  CSingleLock sl(&m_cs, TRUE);
  if (!m_SurfaceMap.Lookup(hashkey, surf))
  {
    //surf.surface=-1;
    return FALSE;
  }
  return TRUE;
}

BOOL ImageCacheMgr::AddSurface(DWORD hashkey, ImageCacheData &surf)
{
//  CSingleLock sl(&m_cs, TRUE);
  /*
  if ((DWORD)m_SurfaceMap.GetCount() >= m_maxsize)
  {
    RemoveOldest();
    //WriteDebugString("ImageCacheMgr(%s): Removed surface to make room for %s (%i)\n",
    //                 m_id, surf.name, surf.surface);
  }
  */

#ifdef _DEBUG
  ImageCacheData tmp;
  if (GetSurface(hashkey, tmp))
  {
    ASSERT (tmp.name == surf.name);
    if (tmp.name!=surf.name)
    {
      WriteDebugString("ImageCacheMgr(%s): Hash collision - %s with %s, key %u\n",
                       m_id, tmp.name, surf.name, hashkey);
    }
  }

  //WriteDebugString("ImageCacheMgr(%s): Adding surf %s (%i) to image cache, %u total\n", 
  //                  m_id, surf.name, surf.surface, m_SurfaceMap.GetCount()+1);
#endif

  surf.insertTime=time(NULL); // in seconds
  m_SurfaceMap.SetAt(hashkey, surf);
  //WriteDebugString("Cacheing surface %s hash=%08x ID=%d\n", surf.name, hashkey, surf.surface);
  return TRUE;
}

BOOL ImageCacheMgr::NeedsFlushing()
{
  return ((DWORD)m_SurfaceMap.GetCount() > m_maxsize);
}

int ImageCacheMgr::compareimagecache( const void *arg1, const void *arg2 )
{
  /*
    must return
  < 0 elem1 less than elem2 
  = 0 elem1 equivalent to elem2 
  > 0 elem1 greater than elem2 
  */
  ASSERT( (arg1!=NULL) && (arg2!=NULL) );
  if ((arg1==NULL)||(arg2==NULL)) return 0;
  sortdata *pElem1 = (sortdata*)arg1;
  sortdata *pElem2 = (sortdata*)arg2;

  if (pElem1->timestamp < pElem2->timestamp) return -1;
  if (pElem1->timestamp > pElem2->timestamp) return  1;
  return 0;
}

void ImageCacheMgr::FlushOldest()
{
//  CSingleLock sl(&m_cs, TRUE);
  DWORD count = m_SurfaceMap.GetCount();

  if (count <= m_maxsize)
    return;

  if (count > m_sortsize)
  {
    m_sortsize=count+50;
    if (m_sortarray!=NULL) delete [] m_sortarray;
    m_sortarray=new sortdata[m_sortsize];  
  }

  ImageCacheData tmp;
  DWORD key;
  POSITION pos = m_SurfaceMap.GetStartPosition();
  int index=0;
  while (pos!=NULL)
  {
    m_SurfaceMap.GetNextAssoc(pos, key, tmp);
    m_sortarray[index].key=key;
    m_sortarray[index].timestamp=tmp.insertTime;
    m_sortarray[index].surf=tmp.surface;
    index++;
  }
  qsort( m_sortarray, count, sizeof(sortdata), compareimagecache);
  int deleteqty = count-m_maxsize;
  for (int i=0;i<deleteqty;i++)
  {
    m_SurfaceMap.RemoveKey(m_sortarray[i].key);
    GraphicsMgr.ReleaseSurface(m_sortarray[i].surf);
  }
}

void ImageCacheMgr::RemoveOldest()
{
  POSITION pos = m_SurfaceMap.GetStartPosition();
  DWORD oldestTime=0xFFFFFFFF;
  DWORD oldestKey=0;
  long oldestSurf=-1;
#ifdef _DEBUG
  CString oldestName="";
#endif
  ImageCacheData tmp;
  DWORD key;

  while (pos!=NULL)
  {
    m_SurfaceMap.GetNextAssoc(pos, key, tmp);
    if ((!tmp.locked) && (tmp.insertTime < oldestTime))
    {
      oldestTime = tmp.insertTime;
      oldestKey = key;
      oldestSurf=tmp.surface;
#ifdef _DEBUG
      oldestName=tmp.name;
#endif
    }
  }

  if ((oldestKey > 0) && (oldestTime!=0xFFFFFFFF))
  {
#ifdef _DEBUG
    //WriteDebugString("ImageCacheMgr(%s): Releasing surf %s (%i) from image cache\n", 
    //                 m_id, oldestName, oldestSurf);
#endif
    m_SurfaceMap.RemoveKey(oldestKey);
    GraphicsMgr.ReleaseSurface(oldestSurf);
  }
}

void WallSetSlotMemType::SaveArt(int level)
{
  if (!wallFile.IsEmpty())
    ::SaveArt(wallFile, WallDib, level, TRUE, rte.WallArtDir());

  if (!doorFile.IsEmpty())
    ::SaveArt(doorFile, DoorDib, level, TRUE, rte.DoorArtDir());

  if (!overlayFile.IsEmpty())
    ::SaveArt(overlayFile, OverlayDib, level, TRUE, rte.OverlayArtDir());

  if (!AVArtFile.IsEmpty())
    ::SaveArt(AVArtFile, TransBufferDib, level, TRUE, rte.AreaViewArtDir());
}
void WallSetSlotMemType::SaveSound(int level)
{
  if (!soundFile.IsEmpty())
    ::SaveSound(soundFile, level, rte.SoundDir());
}

void WallSetSlotMemType::PlaySlotSound()
{
  if (!PlaySoundEffects) return;
  if (soundFile == "") return;
  if (!ValidSound(hsound))
    hsound = LoadSound(soundFile);
  if (!PlaySound(hsound,FALSE))
    hsound=-1;
}

void BackgroundSlotMemType::SaveArt(int level)
{
  if (!backgroundFile.IsEmpty())
    ::SaveArt(backgroundFile, BackGndDib, level, TRUE, rte.BackgroundArtDir());
  if (!backgroundFileAlt.IsEmpty())
    ::SaveArt(backgroundFileAlt, BackGndDib, level, TRUE, rte.BackgroundArtDir());
}

void BackgroundSlotMemType::SaveSound(int level)
{
  if (!soundFile.IsEmpty())
    ::SaveSound(soundFile, level, rte.SoundDir());
}

#ifdef UAFEDITOR
void BackgroundSlotMemType::CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference)
{
  CR_REFERENCE crRef;
  crRef.m_referenceName = pCRReference->m_referenceName + "[Background]";
  crRef.m_referenceType = pCRReference->m_referenceType;
  pCRList->CR_AddPicReference(backgroundFile, &crRef);
}

#endif

void BackgroundSlotMemType::PlaySlotSound()
{
  if (!PlaySoundEffects) return;
  if (soundFile == "") return;
  if (!ValidSound(hsound))
    hsound = LoadSound(soundFile);
  if (!PlaySound(hsound,FALSE))
    hsound=-1;
}

void WallSetSlotMemType::Serialize(CArchive &ar, double version)
{
//  CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     //StripFilenamePath(wallFile);
     AS(ar,wallFile);

     //StripFilenamePath(doorFile);
     AS(ar,doorFile);

     //StripFilenamePath(overlayFile);
     AS(ar,overlayFile);

     //StripFilenamePath(soundFile);
     AS(ar,soundFile);

     //StripFilenamePath(AVArtFile);
     AS(ar,AVArtFile);

     ar << used;
     ar << doorFirst;
     ar << drawAV;

     //AddFolderToPath(wallFile,    rte.WallArtDir());
     //AddFolderToPath(doorFile,    rte.DoorArtDir());
     //AddFolderToPath(overlayFile, rte.OverlayArtDir());
     //AddFolderToPath(soundFile,   rte.SoundDir());
     //AddFolderToPath(AVArtFile,   rte.AreaViewArtDir());

     //UnlockSpell.Serialize(ar, version); //ar << UnlockSpell;
     ar << UnlockSpellID;
     //ar << UnlockSpellClass;
     //ar << UnlockSpellLevel;
     ar << BlendOverlay;
     ar << BlendAmount;
  }
  else
  {
    DAS(ar,wallFile);
    DAS(ar,doorFile);
    DAS(ar,overlayFile);

     if (version >= _VERSION_05771_)
       DAS(ar,soundFile);

     if (version >= _VERSION_0698_)
       DAS(ar,AVArtFile);

     ar >> used;
     if (version >= _VERSION_0694_)
       ar >> doorFirst;
     if (version >= _VERSION_0698_)
       ar >> drawAV;

     //AddFolderToPath(wallFile,    rte.WallArtDir());
     //AddFolderToPath(doorFile,    rte.DoorArtDir());
     //AddFolderToPath(overlayFile, rte.OverlayArtDir());
     //AddFolderToPath(soundFile,   rte.SoundDir());
     //AddFolderToPath(AVArtFile,   rte.AreaViewArtDir());

     wallHash = HashString(wallFile, IMAGE_CACHE_MAP_SIZE);
     doorHash = HashString(doorFile, IMAGE_CACHE_MAP_SIZE);
     overlayHash = HashString(overlayFile, IMAGE_CACHE_MAP_SIZE);
     AVArtHash = HashString(AVArtFile, IMAGE_CACHE_MAP_SIZE);

     //UnlockSpell.Serialize(ar, version); //ar >> UnlockSpell;
     ar >> UnlockSpellID;
     //ar >> UnlockSpellClass;
     //ar >> UnlockSpellLevel;

     if (version >= _VERSION_0620_)
     {
       ar >> BlendOverlay;
       ar >> BlendAmount;
     }
  }
}

void WallSetSlotMemType::Serialize(CAR &ar, double version)
{
  // Deleted 20121208 PRS
  //CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     //StripFilenamePath(wallFile);
     //StripFilenamePath(doorFile);
     //StripFilenamePath(overlayFile);
     //StripFilenamePath(soundFile);
     //StripFilenamePath(AVArtFile);

     AS(ar,wallFile);
     AS(ar,doorFile);
     AS(ar,overlayFile);
     AS(ar,soundFile);
     AS(ar,AVArtFile);
     ar << used;
     ar << doorFirst;
     ar << drawAV;

     //AddFolderToPath(wallFile,    rte.WallArtDir());
     //AddFolderToPath(doorFile,    rte.DoorArtDir());
     //AddFolderToPath(overlayFile, rte.OverlayArtDir());
     //AddFolderToPath(soundFile,   rte.SoundDir());
     //AddFolderToPath(AVArtFile,   rte.AreaViewArtDir());

     wallHash = HashString(wallFile, IMAGE_CACHE_MAP_SIZE);
     doorHash = HashString(doorFile, IMAGE_CACHE_MAP_SIZE);
     overlayHash = HashString(overlayFile, IMAGE_CACHE_MAP_SIZE);
     AVArtHash = HashString(AVArtFile, IMAGE_CACHE_MAP_SIZE);

     //UnlockSpell.Serialize(ar, version); //ar << UnlockSpell;
     ar << UnlockSpellID;
     //ar << UnlockSpellClass;
     //ar << UnlockSpellLevel;
     ar << BlendOverlay;
     ar << BlendAmount;
  }
  else
  {
    DAS(ar,wallFile);
    DAS(ar,doorFile);
    DAS(ar,overlayFile);

    StripFilenamePath(wallFile);
    StripFilenamePath(doorFile);
    StripFilenamePath(overlayFile);

     if (version >= _VERSION_05771_)
       DAS(ar,soundFile);

     if (version >= _VERSION_0698_)
       DAS(ar,AVArtFile);

     ar >> used;
     if (version >= _VERSION_0694_)
       ar >> doorFirst;
     if (version >= _VERSION_0698_)
       ar >> drawAV;

     //AddFolderToPath(wallFile,    rte.WallArtDir());
     //AddFolderToPath(doorFile,    rte.DoorArtDir());
     //AddFolderToPath(overlayFile, rte.OverlayArtDir());
     //AddFolderToPath(soundFile,   rte.SoundDir());
     //AddFolderToPath(AVArtFile,   rte.AreaViewArtDir());

     //UnlockSpell.Serialize(ar, version); //ar >> UnlockSpell;
#ifdef UAFEDITOR
     if (version < VersionSpellNames)
     {
       int preSpellNameKey;
       ar >> preSpellNameKey;
       if (preSpellNameKey < 0)
       {
         UnlockSpellID.Empty();
       }
       else
       {
         UnlockSpellID = spellData.FindPreVersionSpellNamesSpellID(preSpellNameKey);
       };
     }
     else
#endif
     {
      ar >> UnlockSpellID;
     };

#ifdef UAFEDITOR
     {
       if (version < VersionSpellNames)
       {
          long UnlockSpellClass;
          long UnlockSpellLevel;
          ar >> UnlockSpellClass;
          ar >> UnlockSpellLevel;
          //ar >> UnlockSpellClass;
          //ar >> UnlockSpellLevel;
       };
     };
#endif
     if (version >= _VERSION_0620_)
     {
       ar >> BlendOverlay;
       ar >> BlendAmount;
     }
  }
}

#ifdef UAFEDITOR

const char *JKEY_WALLFILE = "wallFile";
const char *JKEY_DOORFILE = "doorFile";
const char *JKEY_OVERLAYFILE = "overlayFile";
const char *JKEY_SOUNDFILE = "soundFile";
const char *JKEY_AVARTFILE = "avArtFile";
const char *JKEY_USED = "used";
const char *JKEY_DOORFIRST = "doorFirst";
const char *JKEY_DRAWAV = "drawAV";
const char *JKEY_UNLOCKSPELLID = "unlockSpellID";
const char *JKEY_BLENDOVERLAY = "blendOverlay";
const char *JKEY_BLENDAMOUNT = "blendAmount";

void WallSetSlotMemType::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_WALLFILE, wallFile);
  jw.NameAndValue(JKEY_DOORFILE, doorFile);
  jw.NameAndValue(JKEY_OVERLAYFILE, overlayFile);
  jw.NameAndValue(JKEY_SOUNDFILE, soundFile);
  jw.NameAndValue(JKEY_AVARTFILE, AVArtFile);
  jw.NameAndBool(JKEY_USED, used);
  jw.NameAndBool(JKEY_DOORFIRST, doorFirst);
  jw.NameAndBool(JKEY_DRAWAV, drawAV);
  //wallHash = HashString(wallFile, IMAGE_CACHE_MAP_SIZE);
  //doorHash = HashString(doorFile, IMAGE_CACHE_MAP_SIZE);
  //overlayHash = HashString(overlayFile, IMAGE_CACHE_MAP_SIZE);
  //AVArtHash = HashString(AVArtFile, IMAGE_CACHE_MAP_SIZE);
  jw.NameAndValue(JKEY_UNLOCKSPELLID, UnlockSpellID);
  jw.NameAndValue(JKEY_BLENDOVERLAY, BlendOverlay);
  jw.NameAndValue(JKEY_BLENDAMOUNT, BlendAmount);
  jw.EndList();
}
void WallSetSlotMemType::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_WALLFILE, wallFile);
    wallHash = HashString(wallFile, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndValue(JKEY_DOORFILE, doorFile);
    doorHash = HashString(doorFile, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndValue(JKEY_OVERLAYFILE, overlayFile);
    overlayHash = HashString(overlayFile, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndValue(JKEY_SOUNDFILE, soundFile);
  jr.NameAndValue(JKEY_AVARTFILE, AVArtFile);
    AVArtHash = HashString(AVArtFile, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndBool(JKEY_USED, used);
  jr.NameAndBool(JKEY_DOORFIRST, doorFirst);
  jr.NameAndBool(JKEY_DRAWAV, drawAV);
  jr.NameAndValue(JKEY_UNLOCKSPELLID, UnlockSpellID);
  jr.NameAndValue(JKEY_BLENDOVERLAY, BlendOverlay);
  jr.NameAndValue(JKEY_BLENDAMOUNT, BlendAmount);
  jr.EndList();
}

void WallSetSlotMemType::CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference)
{
  CR_REFERENCE crRef;
  crRef.m_referenceName = pCRReference->m_referenceName + "[Wall]";
  crRef.m_referenceType = pCRReference->m_referenceType;
  pCRList->CR_AddPicReference(wallFile, &crRef);
  pCRList->CR_AddPicReference(doorFile, &crRef);
  pCRList->CR_AddPicReference(overlayFile, &crRef);
  pCRList->CR_AddPicReference(soundFile, &crRef);
  pCRList->CR_AddPicReference(AVArtFile, &crRef);
}
#endif

void BackgroundSlotMemType::Serialize(CArchive &ar, double version)
{
  // Deleted 20121208 PRS
  //CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     StripFilenamePath(backgroundFile);
     StripFilenamePath(backgroundFileAlt);
     StripFilenamePath(soundFile);

     AS(ar,backgroundFile);
     AS(ar,backgroundFileAlt);
     AS(ar,soundFile);

     
     if (globalData.version < 0.930276)
     {
       StripFilenamePath(backgroundFile);
       StripFilenamePath(backgroundFileAlt);
       StripFilenamePath(soundFile);
     };
     
     
     ar << suppressStepSound;
     ar << used;

     //AddFolderToPath(backgroundFile,    rte.BackgroundArtDir());
     //AddFolderToPath(backgroundFileAlt, rte.BackgroundArtDir());
     //AddFolderToPath(soundFile,         rte.SoundDir());

     ar << StartTime;
     ar << EndTime;
     ar << useAltBackground;
     ar << useAlphaBlend;
     ar << AlphaBlendPcnt;
     ar << useTransparency;
  }
  else
  {
    DAS(ar,backgroundFile);
    DAS(ar,backgroundFileAlt);
     if (version >= _VERSION_0640_)
     {
       DAS(ar,soundFile);
       ar >> suppressStepSound;
     }
     ar >> used;

     //AddFolderToPath(backgroundFile,    rte.BackgroundArtDir());
     //AddFolderToPath(backgroundFileAlt, rte.BackgroundArtDir());
     //AddFolderToPath(soundFile,         rte.SoundDir());
     backgroundFileHash = HashString(backgroundFile, IMAGE_CACHE_MAP_SIZE);
     backgroundFileAltHash = HashString(backgroundFileAlt, IMAGE_CACHE_MAP_SIZE);

     ar >> StartTime;
     ar >> EndTime;
     ar >> useAltBackground;
     if (version >= _VERSION_0620_)
     {
       ar >> useAlphaBlend;
       ar >> AlphaBlendPcnt;
     }

     if (version >= _VERSION_0630_)
      ar >> useTransparency;
  }
}

void BackgroundSlotMemType::Serialize(CAR &ar, double version)
{
  //CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     StripFilenamePath(backgroundFile);
     StripFilenamePath(backgroundFileAlt);
     StripFilenamePath(soundFile);

     AS(ar,backgroundFile);
     AS(ar,backgroundFileAlt);
     AS(ar,soundFile);
     ar << suppressStepSound;
     ar << used;

     //AddFolderToPath(backgroundFile,    rte.BackgroundArtDir());
     //AddFolderToPath(backgroundFileAlt, rte.BackgroundArtDir());
     //AddFolderToPath(soundFile,         rte.SoundDir());

     ar << StartTime;
     ar << EndTime;
     ar << useAltBackground;
     ar << useAlphaBlend;
     ar << AlphaBlendPcnt;
     ar << useTransparency;
  }
  else
  {
    DAS(ar,backgroundFile);
    DAS(ar,backgroundFileAlt);
     if (version >= _VERSION_0640_)
     {
       DAS(ar,soundFile);
       ar >> suppressStepSound;
     }
     ar >> used;

     if (globalData.version < 0.930276)
     {
       StripFilenamePath(backgroundFile);
       StripFilenamePath(backgroundFileAlt);
       StripFilenamePath(soundFile);
     };

     //AddFolderToPath(backgroundFile,    rte.BackgroundArtDir());
     //AddFolderToPath(backgroundFileAlt, rte.BackgroundArtDir());
     //AddFolderToPath(soundFile,         rte.SoundDir());
     backgroundFileHash = HashString(backgroundFile, IMAGE_CACHE_MAP_SIZE);
     backgroundFileAltHash = HashString(backgroundFileAlt, IMAGE_CACHE_MAP_SIZE);

     ar >> StartTime;
     ar >> EndTime;
     ar >> useAltBackground;
     if (version >= _VERSION_0620_)
     {
       ar >> useAlphaBlend;
       ar >> AlphaBlendPcnt;
     }
     if (version >= _VERSION_0630_)
      ar >> useTransparency;
  }
}

#ifdef UAFEDITOR

const char *JKEY_BACKGROUNDFILE = "backgroundFile";
const char *JKEY_BACKGROUNDFILEALT = "backgroundFileAlt";
//const char *JKEY_SOUNDFILE = "soundFile";
const char *JKEY_SUPPRESSSTEPSOUND = "suppressStepSound";
//const char *JKEY_USED = "used";
extern const char *JKEY_STARTTIME;
extern const char *JKEY_ENDTIME;
const char *JKEY_USEALTBACKGROUND = "useAltBackground";
const char *JKEY_USEALPHABLEND = "useAlphaBlend";
const char *JKEY_ALPHABLENDPERCENT = "alphaBlendPercent";
const char *JKEY_USETRANSPARENCY = "useTransparency";

void BackgroundSlotMemType::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_BACKGROUNDFILE, backgroundFile);
  jw.NameAndValue(JKEY_BACKGROUNDFILEALT, backgroundFileAlt);
  jw.NameAndValue(JKEY_SOUNDFILE, soundFile);
  jw.NameAndBool(JKEY_SUPPRESSSTEPSOUND, suppressStepSound);
  jw.NameAndBool(JKEY_USED, used);
  jw.NameAndValue(JKEY_STARTTIME, StartTime);
  jw.NameAndValue(JKEY_ENDTIME, EndTime);
  jw.NameAndBool(JKEY_USEALTBACKGROUND, useAltBackground);
  jw.NameAndBool(JKEY_USEALPHABLEND, useAlphaBlend);
  jw.NameAndValue(JKEY_ALPHABLENDPERCENT, AlphaBlendPcnt);
  jw.NameAndBool(JKEY_USETRANSPARENCY, useTransparency);
  jw.EndList();
}
void BackgroundSlotMemType::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_BACKGROUNDFILE, backgroundFile);
      backgroundFileHash = HashString(backgroundFile, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndValue(JKEY_BACKGROUNDFILEALT, backgroundFileAlt);
      backgroundFileAltHash = HashString(backgroundFileAlt, IMAGE_CACHE_MAP_SIZE);
  jr.NameAndValue(JKEY_SOUNDFILE, soundFile);
  jr.NameAndBool(JKEY_SUPPRESSSTEPSOUND, suppressStepSound);
  jr.NameAndBool(JKEY_USED, used);
  jr.NameAndValue(JKEY_STARTTIME, StartTime);
  jr.NameAndValue(JKEY_ENDTIME, EndTime);
  jr.NameAndBool(JKEY_USEALTBACKGROUND, useAltBackground);
  jr.NameAndBool(JKEY_USEALPHABLEND, useAlphaBlend);
  jr.NameAndValue(JKEY_ALPHABLENDPERCENT, AlphaBlendPcnt);
  jr.NameAndBool(JKEY_USETRANSPARENCY, useTransparency);
  jr.EndList();
}
#endif

void PicDataType::PreSerialize(BOOL IsStoring)
{
  if (IsStoring)
  {
    StripFilenamePath(name);
  }
}

void PicDataType::PostSerialize(BOOL IsStoring)
{
  //AddFolderToPath(name, rte.PicArtDir());
}

void PicDataType::Serialize(CArchive &ar)
{
  PreSerialize(ar.IsStoring());

  // Deleted 20121208 PRS
  //CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     ar << (int)picType;

     AS(ar,name);
  }
  else
  {
     int temp;
     ar >> temp;
     picType = (SurfaceType)temp;
     DAS(ar,name);
  }
  PostSerialize(ar.IsStoring());
}

void PicDataType::Serialize(CAR &ar)
{
  PreSerialize(ar.IsStoring());

  //Deleted 20121208 PRS
  //CObject::Serialize(ar);

  if (ar.IsStoring())
  {
     ar << (int)picType;

     AS(ar,name);
  }
  else
  {
     int temp;
     ar >> temp;
     picType = (SurfaceType)temp;
     DAS(ar,name);
  }
  PostSerialize(ar.IsStoring());
}

#ifdef UAFEDITOR

extern const char *JKEY_PICTYPE;
extern const char *JKEY_NAME;
//extern const char *JKEY_SURFACE = "surface";
void PicDataType::Export(JWriter& jw, const char *pName)
{
  jw.StartList(pName);
  jw.Linefeed(false);
  jw.NameAndEnum(JKEY_PICTYPE, picType);
  jw.NameAndValue(JKEY_NAME, name);
//  jw.NameAndValue(JKEY_SURFACE, surface);
  jw.EndList();
  jw.Linefeed(true);
}
void PicDataType::Import(JReader& jr, const char *pName)
{
  jr.StartList(pName);
  jr.Linefeed(false);
  jr.NameAndEnum(JKEY_PICTYPE, picType);
  jr.NameAndValue(JKEY_NAME, name);
//  jr.NameAndValue(JKEY_SURFACE, surface);
  jr.EndList();
  jr.Linefeed(true);
}
#endif


void SaveArt(CString &filename, SurfaceType type, int level, BOOL alterPath, const CString& destDir)
{
#ifdef UAFEDITOR
  if (filename.IsEmpty())
    return;


  CString saveName;
  int index = filename.ReverseFind('\\');

  if (index > 1)
    saveName = filename.Right(filename.GetLength()-(index+1));
  else
    return;  // No folder name in source.

  POSITION pos = LevelPicData[level].GetHeadPosition();
  while (pos != NULL)
  {
    if (LevelPicData[level].GetAt(pos)->saveName == saveName)
    {
      if (alterPath)
      {
        // make this path point to saved pic in design folder
        filename = LevelPicData[level].GetAt(pos)->name;
      }
      return;
    }
    LevelPicData[level].GetNext(pos);
  }

  CString temp;
  temp = filename;
  if (WriteArtToFile(temp, destDir))
  {
    if (alterPath)
      filename = temp;
    ResourceSaveType data;
    data.name = temp;
    data.saveName = saveName;
    LevelPicData[level].AddTail(data);
  }
#endif
}


void deleteAllLevelArt(int level)
{
#ifdef UAFEDITOR
  LevelPicData[level].RemoveAll();
#endif
}

#ifdef REQART
void WriteRequiredArtList()
{
#ifdef UAFEDITOR
  static char dsnPath[_MAX_PATH+1];
  strcpy(dsnPath, rte.DataDir());
  CString filename;
  filename.Format("%sReqArt.txt", dsnPath);
  FILE *fp = fopen(filename, "w");
  CString temp;
  
  if (fp != NULL)
  {
    for (int i=0;i<=MAX_LEVELS;i++)
    {
      POSITION pos = LevelPicData[i].GetHeadPosition();
      while (pos != NULL)
      {
        if (i < MAX_LEVELS)
        {
          temp.Format("Level %u: %s\n", i+1, LevelPicData[i].GetNext(pos)->name);
          fprintf(fp, temp);
        }
        else
        {
          temp.Format("Global: %s\n", LevelPicData[i].GetNext(pos)->name);
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

BOOL WriteArtToFile(CString &sourceFile, const CString& destinationDir)
{
  // Given a path and filename in 'sourceFile'.
  // Copy it to the directory 'destinationDir' if it is not already there.
  // Set 'sourceFile' to the path and filename of the copy.
#ifdef UAFEDITOR
 CString dsnPath;

 if (sourceFile.IsEmpty())
    return TRUE;
  dsnPath = destinationDir;

  // determine path and name of destination file
  CString saveName;
  int index = sourceFile.ReverseFind('\\');

  if (index > 1)
    saveName.Format("%s%s", dsnPath, sourceFile.Right(sourceFile.GetLength()-(index+1)));
  else
    saveName.Format("%s%s", dsnPath, sourceFile);

  // source file missing?
  if (!FileExists(sourceFile))
  {
    CString tmp;
    tmp = sourceFile;
    //if (!SearchForArtFile(tmp)) // returns full path file
    {
      WriteDebugString("Can't find file \'%s\' in WriteArtToFile()\n", tmp);
      return FALSE;
    }
  }

  // if same file is already in design folder,
  // skip the copy
  // 20110508 PRS   This gets done after the copy   SetFileAttributes(saveName, FILE_ATTRIBUTE_NORMAL);

  //******* 20110508 PRS
  if (sourceFile == saveName) return TRUE;  //Not much sense in attempting this!
  //******* 20110508 PRS

  // copy src to design folder
  if (MyCopyFile(sourceFile, saveName, FALSE))
  {
    SetFileAttributes(saveName, FILE_ATTRIBUTE_NORMAL);
    sourceFile = saveName;
    return TRUE;
  }  
  else
  {
    WriteDebugString("CopyFile failed (%u): \'%s\' to \'%s\'\n", GetLastError(), sourceFile, saveName);
    return FALSE;
  }
#else
  return FALSE;
#endif
}

void WallSetSlotMemType::ClearResources()
{
  GraphicsMgr.ReleaseSurface(wallSurface);
  GraphicsMgr.ReleaseSurface(doorSurface);
  GraphicsMgr.ReleaseSurface(overlaySurface);
  GraphicsMgr.ReleaseSurface(AVArtSurface);
  if (pSndMgr != NULL) pSndMgr->RemoveSound(&hsound);
}

void BackgroundSlotMemType::ClearResources()
{
  GraphicsMgr.ReleaseSurface(backgroundSurface);
  GraphicsMgr.ReleaseSurface(backgroundSurfaceAlt);
  if (pSndMgr != NULL) pSndMgr->RemoveSound(&hsound);
}

void clearArtSlots(void)
{
  int i;
  
   HBarVPArt.Clear();
   VBarVPArt.Clear();
   FrameVPArt.Clear();
   VBarCombArt.Clear();
   HBarCombArt.Clear();
   CombatCursorArt.Clear();
   CombatDeathIconArt.Clear();
   CombatPetrifiedIconArt.Clear();
   CombatDeathArt.Clear();
   CharViewFrameVPArt.Clear();
   CursorArt.Clear();

  for (i=0; i<MAX_WALLSETS;i++)
    WallSets[i].Clear();
  for (i=0; i<MAX_BACKGROUNDS;i++)
    BackgroundSets[i].Clear();

  if (pWallSlotMgr!=NULL)
    pWallSlotMgr->Clear();
  if (pBgSlotMgr!=NULL)
    pBgSlotMgr->Clear();
}

BOOL GetWallSlotSurfaces(WallSetSlotMemType &data)
{
  if (pWallSlotMgr==NULL)
  {
    die(0xab52b);
    return FALSE;
  }

  BOOL success=TRUE;
  ImageCacheData tmp;  
  RECT rect;
  int tmpFormat = -1;
  data.SlotType = DEFAULT_FORMAT_TYPE;

  if (!data.wallFile.IsEmpty())
  {             
    if (data.wallHash==0) data.wallHash = HashString(data.wallFile, IMAGE_CACHE_MAP_SIZE);
    if (pWallSlotMgr->GetSurface(data.wallHash, tmp))
    {
      data.wallSurface = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.wallSurface);
#ifdef UAFEngine
      if ((data.wallSurface = GraphicsMgr.AddFileSurface(data.wallFile, 
                                                         WallDib, 
                                                         rte.WallArtDir())) < 0)
                                                         //""))<0)
#else
      if ((data.wallSurface = GraphicsMgr.AddFileSurface(data.wallFile, 
                                                         WallDib, 
                                                         rte.WallArtDir(), 
                                                         ede.TemplateWallArtDir())) < 0)
#endif
      {
        if (data.wallFile.Find("wa_",0) < 0)
        {
          CString name(data.wallFile);
          CString path("");
          GetFilenamePath(data.wallFile, path);
          StripFilenamePath(name);
          path = "wa_" + name;

#ifdef UAFEngine
          if ((data.wallSurface = GraphicsMgr.AddFileSurface(path, WallDib, rte.WallArtDir())) >= 0)
#else
          if ((data.wallSurface = GraphicsMgr.AddFileSurface(path, WallDib, rte.WallArtDir(), ede.TemplateWallArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted wall file %s to %s in GetWallSlotSurfaces\n", data.wallFile, path);
            data.wallFile = path;
          }
        }

        if (data.wallSurface < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLWS01")+data.wallFile))
          {
            WriteDebugString("Failed to load wall surface \'%s\' in GetWallSlotSurfaces\n", data.wallFile);
          };
          success = FALSE;
        }
      }
      else
      {
        GraphicsMgr.GetSurfaceRect(data.wallSurface ,&rect);
        tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        tmp.name = data.wallFile;
        tmp.surface = data.wallSurface;
        tmp.type = tmpFormat;
        pWallSlotMgr->AddSurface(data.wallHash, tmp);
      }
    }
  }
  
  if (!data.doorFile.IsEmpty())
  {        
    if (data.doorHash==0) data.doorHash = HashString(data.doorFile, IMAGE_CACHE_MAP_SIZE);
    if (pWallSlotMgr->GetSurface(data.doorHash, tmp))
    {
      data.doorSurface = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.doorSurface);      
#ifdef UAFEngine
      if ((data.doorSurface = GraphicsMgr.AddFileSurface(data.doorFile, DoorDib, rte.DoorArtDir())) < 0)
#else
      if ((data.doorSurface = GraphicsMgr.AddFileSurface(data.doorFile, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir())) < 0)
#endif
      {
        if (data.doorFile.Find("dr_",0) < 0)
        {
          CString name(data.doorFile);
          CString path("");
          GetFilenamePath(data.doorFile, path);
          StripFilenamePath(name);
          path = "dr_" + name;

#ifdef UAFEngine
          if ((data.doorSurface = GraphicsMgr.AddFileSurface(path, DoorDib, rte.DoorArtDir())) >= 0)
#else
          if ((data.doorSurface = GraphicsMgr.AddFileSurface(path, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted door file %s to %s in GetWallSlotSurfaces\n", data.doorFile, path);
            data.doorFile = path;
          }
        }

        if (data.doorSurface < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLDS01") + data.doorFile))
          {
            debugSeverity = 7;
            WriteDebugString("Failed to load door surface \'%s\' in GetWallSlotSurfaces\n", data.doorFile);
          };
          success = FALSE;
        }
      }
      else
      {
        if (tmpFormat == -1)
        {
          GraphicsMgr.GetSurfaceRect(data.doorSurface ,&rect);
          tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        }
        tmp.name = data.doorFile;
        tmp.surface = data.doorSurface;
        tmp.type = tmpFormat;
        pWallSlotMgr->AddSurface(data.doorHash, tmp);
      }
    }
  }
  
  if (!data.overlayFile.IsEmpty())
  {            
    if (data.overlayHash==0) data.overlayHash = HashString(data.overlayFile, IMAGE_CACHE_MAP_SIZE);
    if (pWallSlotMgr->GetSurface(data.overlayHash, tmp))
    {
      data.overlaySurface = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.overlaySurface);
#ifdef UAFEngine
      if ((data.overlaySurface = GraphicsMgr.AddFileSurface(data.overlayFile, OverlayDib, rte.OverlayArtDir())) < 0)
#else
      if ((data.overlaySurface = GraphicsMgr.AddFileSurface(data.overlayFile, OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlayArtDir())) < 0)
#endif
      {
        if (data.overlayFile.Find("ov_",0) < 0)
        {
          CString name(data.overlayFile);
          CString path("");
          GetFilenamePath(data.overlayFile, path);
          StripFilenamePath(name);
          path = "ov_" + name;

#ifdef UAFEngine
          if ((data.overlaySurface = GraphicsMgr.AddFileSurface(path, OverlayDib, rte.OverlayArtDir())) >= 0)
#else
          if ((data.overlaySurface = GraphicsMgr.AddFileSurface(path, OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlayArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted overlay file %s to %s in GetWallSlotSurfaces\n", data.overlayFile, path);
            data.overlayFile = path;
          }
        }

        if (data.overlaySurface < 0)
        {
          debugSeverity = 7;
          WriteDebugString("Failed to load overlay surface \'%s\' in GetWallSlotSurfaces\n", data.overlayFile);
          success = FALSE;
        }
      }
      else
      {
        if (tmpFormat == -1)
        {
          GraphicsMgr.GetSurfaceRect(data.overlaySurface ,&rect);
          tmpFormat = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        }
        tmp.name = data.overlayFile;
        tmp.surface = data.overlaySurface;
        tmp.type = tmpFormat;
        pWallSlotMgr->AddSurface(data.overlayHash, tmp);
      }
    }
  }

  if (tmpFormat == -1) tmpFormat = DEFAULT_FORMAT_TYPE;
  data.SlotType = tmpFormat;

  if ((!data.AVArtTried) && (!data.AVArtFile.IsEmpty()))
  {            
    if (data.AVArtHash==0) data.AVArtHash = HashString(data.AVArtFile, IMAGE_CACHE_MAP_SIZE);
    if (pWallSlotMgr->GetSurface(data.AVArtHash, tmp))
    {
      data.AVArtSurface = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.AVArtSurface);
      if (!LoadAreaViewArt(data.AVArtFile, data.AVArtSurface, data.AVZoneStyle))
      {
        data.AVArtTried=true;
        debugSeverity = 7;
        WriteDebugString("Failed to load area view art \'%s\' in GetWallSlotSurfaces\n", data.AVArtFile);
        // will default to globalData.mapart
      }
      else
      {
        tmp.name = data.AVArtFile;
        tmp.surface = data.AVArtSurface;
        tmp.type = DEFAULT_FORMAT_TYPE;
        pWallSlotMgr->AddSurface(data.AVArtHash, tmp);
      }
    }
  }

  return success;
}

BOOL GetBackgroudSlotSurfaces(BackgroundSlotMemType &data)
{
  if (pBgSlotMgr==NULL)
  {
    die(0xab52c);
    return FALSE;
  }

  BOOL success=TRUE;
  ImageCacheData tmp;  
  data.SlotType = DEFAULT_FORMAT_TYPE;
  
  if (!data.backgroundFile.IsEmpty())
  {  
    if (data.backgroundFileHash==0) data.backgroundFileHash = HashString(data.backgroundFile, IMAGE_CACHE_MAP_SIZE);
    if (pBgSlotMgr->GetSurface(data.backgroundFileHash, tmp))
    {
      data.backgroundSurface = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.backgroundSurface);  
#ifdef UAFEngine
      if ((data.backgroundSurface = GraphicsMgr.AddFileSurface(data.backgroundFile, BackGndDib, rte.BackgroundArtDir())) < 0)
#else
      if ((data.backgroundSurface = GraphicsMgr.AddFileSurface(data.backgroundFile, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) < 0)
#endif
      {
        if (data.backgroundFile.Find("bd_",0) < 0)
        {
          CString name(data.backgroundFile);
          CString path("");
          GetFilenamePath(data.backgroundFile, path);
          StripFilenamePath(name);
          path = "bd_" + name;

#ifdef UAFEngine
          if ((data.backgroundSurface = GraphicsMgr.AddFileSurface(path, BackGndDib, rte.BackgroundArtDir())) >= 0)
#else
          if ((data.backgroundSurface = GraphicsMgr.AddFileSurface(path, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted background file %s to %s in GetBackgroudSlotSurfaces\n", data.backgroundFile, path);
            data.backgroundFile = path;
          }
        }

        if (data.backgroundSurface < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLBS01")+data.backgroundFile))
          {
            debugSeverity = 7;
            WriteDebugString("Failed to load background surface \'%s\' in GetBackgroudSlotSurfaces\n", data.backgroundFile);
          };
          success = FALSE;
        }
      }
      else
      {
        tmp.name = data.backgroundFile;
        tmp.surface = data.backgroundSurface;
        pBgSlotMgr->AddSurface(data.backgroundFileHash, tmp);
        GraphicsMgr.GetSurfaceRect(data.backgroundSurface, &data.surfRect);
      }
    }
  }
  
  if (!data.backgroundFileAlt.IsEmpty())
  { 
    if (data.backgroundFileAltHash==0) data.backgroundFileAltHash = HashString(data.backgroundFileAlt, IMAGE_CACHE_MAP_SIZE);
    if (pBgSlotMgr->GetSurface(data.backgroundFileAltHash, tmp))
    {
      data.backgroundSurfaceAlt = tmp.surface;
    }
    else
    {
      GraphicsMgr.ReleaseSurface(data.backgroundSurfaceAlt); 
      SurfaceType type = (data.useTransparency?TransBufferDib:BackGndDib);
#ifdef UAFEngine
      if ((data.backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(data.backgroundFileAlt, type, rte.BackgroundArtDir())) < 0)
#else
      if ((data.backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(data.backgroundFileAlt, type, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) < 0)
#endif
      {
        if (data.backgroundFileAlt.Find("bd_",0) < 0)
        {
          CString name(data.backgroundFileAlt);
          CString path("");
          GetFilenamePath(data.backgroundFileAlt, path);
          StripFilenamePath(name);
          path = "bd_" + name;

#ifdef UAFEngine
          if ((data.backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(path, type, rte.BackgroundArtDir())) >= 0)
#else
          if ((data.backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(path, type, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted background alt file %s to %s in GetBackgroudSlotSurfaces\n", data.backgroundFileAlt, path);
            data.backgroundFileAlt = path;
          }
        }

        if (data.backgroundSurfaceAlt < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLBA01")+data.backgroundFileAlt))
          {
            debugSeverity = 7;
            WriteDebugString("Failed to load background alt surface \'%s\' in GetBackgroudSlotSurfaces\n", data.backgroundFileAlt);
            success = FALSE;
            data.useAltBackground = FALSE;
            data.useAlphaBlend = FALSE;
            data.useTransparency = FALSE;
          };
        }
      }
      else
      {
        tmp.name = data.backgroundFileAlt;
        tmp.surface = data.backgroundSurfaceAlt;
        pBgSlotMgr->AddSurface(data.backgroundFileAltHash, tmp);
      }
    }
  }

  if (data.StartTime < 0)        data.StartTime = 0;
  if (data.StartTime > 2359)     data.StartTime = 2359;
  if (data.EndTime < 0)          data.EndTime = 0;
  if (data.EndTime > 2359)       data.EndTime = 2359;
  if (data.AlphaBlendPcnt < 0)   data.AlphaBlendPcnt = 0;
  if (data.AlphaBlendPcnt > 256) data.AlphaBlendPcnt = 256;

  return success;
}

// loads first 16 wall slots and background slots
BOOL loadWallSetSurfaces()
{
  BOOL success = TRUE;
  int i;
  int count=0;
  
  //for (i=0; i<MAX_WALLSETS; i++)
  for (i=0; i<16; i++)
  {
    if (!GetWallSlotSurfaces(WallSets[i]))
    {
#ifdef _DEBUG
      // substitute default art for the missing art, just to
      // help out when debugging designs from other people
      // that doesn't include the artwork
      if ((!WallSets[i].wallFile.IsEmpty()) && (WallSets[i].wallSurface == -1))
        WallSets[i].wallFile = "wa_wall1.png";
      if ((!WallSets[i].doorFile.IsEmpty()) && (WallSets[i].doorSurface == -1))
        WallSets[i].doorFile = "dr_door5.png";
      if ((!WallSets[i].overlayFile.IsEmpty()) && (WallSets[i].overlaySurface == -1))
        WallSets[i].overlayFile = "";

      if (!GetWallSlotSurfaces(WallSets[i]))
      {
        if (!debugStrings.AlreadyNoted(CString("FGWSS01")))
        {
          WriteDebugString("Failed GetWallSlotSurfaces() after substituting default art\n");
        };
      };
#endif
    }
    else
      count++;
  }
  
  //for (i=0; i<MAX_BACKGROUNDS;i++)
  for (i=0; i<16;i++)
  {
    if (!GetBackgroudSlotSurfaces(BackgroundSets[i]))
    {
#ifdef _DEBUG
      // substitute default art for the missing art
      if ((!BackgroundSets[i].backgroundFile.IsEmpty()) && (BackgroundSets[i].backgroundSurface == -1))
      {
        BackgroundSets[i].backgroundFile = "bd_background1.png";
        BackgroundSets[i].backgroundFileAlt="";
      }
      if (!GetBackgroudSlotSurfaces(BackgroundSets[i]))
      {
        if (!debugStrings.AlreadyNoted(CString("FGBS01")))
        {
          WriteDebugString("Failed GetBackgroudSlotSurfaces() after substituting default art\n");
        };
      };
#endif
    }
    else
      count++;
  }
  
  if (count <= 0)
    WriteDebugString("No Wallsets or Backdrops were loaded in loadWallSetSurfaces()\n");
  return success;
}

void flushOldImages()
{
  static bool flushswitch=true;
  if (flushswitch)
  {
    if (pWallSlotMgr!=NULL) pWallSlotMgr->FlushOldest();
  }
  else
  {
    if (pBgSlotMgr!=NULL) pBgSlotMgr->FlushOldest();
  }
  flushswitch=!flushswitch;
}

BYTE WallSetSlotMemType::GetFormat()
{
  // Format is asked for before images are displayed.
  // To ensure correct format is used, load images
  // first if needed.
  if (wallSurface<=0)    GetWallSurface();
  if (doorSurface<=0)    GetDoorSurface();
  if (overlaySurface<=0) GetOverlaySurface();
  return SlotType;
}

long WallSetSlotMemType::GetWallSurface()
{
  if (wallFile.IsEmpty()) return -1;
  //WriteDebugString("WallSlot-GetWallSurface() entry %s (%i)\n", wallFile, wallSurface);
  //if (wallSurface <= 0)
  {
    static ImageCacheData tmp;
    if (wallHash==0)
    {
      wallHash = HashString(wallFile, IMAGE_CACHE_MAP_SIZE);
    };
    if ((pWallSlotMgr!=NULL) && (pWallSlotMgr->GetSurface(wallHash, tmp)))
    {
      //WriteDebugString("WallSlot-GetWallSurface() hash lookup success %s (%i)\n", wallFile, tmp.surface);
      wallSurface = tmp.surface;
      SlotType = tmp.type;
      if (!GraphicsMgr.ValidSurface(wallSurface))
      {
        wallSurface=-1;
        //WriteDebugString("WallSlot-GetWallSurface() hash lookup surface invalid %s\n", wallFile);
      }
    }
    else
    {
      wallSurface=-1;
      //WriteDebugString("WallSlot-GetWallSurface() hash lookup failed %s\n", wallFile);
    }
    if (wallSurface < 0)
    {
#ifdef UAFEngine
      if ((wallSurface = GraphicsMgr.AddFileSurface(wallFile, WallDib, rte.WallArtDir())) < 0)
#else
      if ((wallSurface = GraphicsMgr.AddFileSurface(wallFile, WallDib, rte.WallArtDir(), ede.TemplateWallArtDir())) < 0)
#endif
      {
        if (wallFile.Find("wa_",0) < 0)
        {
          CString name(wallFile);
          CString path("");
          GetFilenamePath(wallFile, path);
          StripFilenamePath(name);
          path = "wa_" + name;

#ifdef UAFEngine
          if ((wallSurface = GraphicsMgr.AddFileSurface(path, WallDib, rte.WallArtDir())) >= 0)
#else
          if ((wallSurface = GraphicsMgr.AddFileSurface(path, WallDib, rte.WallArtDir(), ede.TemplateWallArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted wall file %s to %s in GetWallSurface\n", wallFile, path);
            wallFile = path;
          }
        }

        if (wallSurface < 0)
          WriteDebugString("Failed to load wall surface \'%s\' in GetWallSurface\n", wallFile);
      }
      else
      {
        RECT rect;
        GraphicsMgr.GetSurfaceRect(wallSurface ,&rect);
        SlotType = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        tmp.name = wallFile;
        tmp.surface = wallSurface;
        tmp.type=SlotType;
        pWallSlotMgr->AddSurface(wallHash, tmp);
      }
    }
  }
  //WriteDebugString("WallSlot-GetWallSurface() returns %s (%i)\n", wallFile, wallSurface);
  return wallSurface;
}

long WallSetSlotMemType::GetDoorSurface()
{
  if (doorFile.IsEmpty()) return -1;
  //if (doorSurface <= 0)
  //WriteDebugString("WallSlot-GetDoorSurface() entry %s (%i)\n", doorFile, doorSurface);
  {
    static ImageCacheData tmp;
    if (doorHash==0) doorHash = HashString(doorFile, IMAGE_CACHE_MAP_SIZE);
    if ((pWallSlotMgr!=NULL) && (pWallSlotMgr->GetSurface(doorHash, tmp)))
    {
      //WriteDebugString("WallSlot-GetDoorSurface() hash lookup success %s (%i)\n", doorFile, tmp.surface);
      doorSurface = tmp.surface;
      if (wallSurface <= 0) SlotType=tmp.type;
      if (!GraphicsMgr.ValidSurface(doorSurface))
      {
        doorSurface=-1;
        //WriteDebugString("WallSlot-GetDoorSurface() hash lookup surface invalid %s\n", doorFile);
      }
    }
    else
    {
      doorSurface=-1;
      //WriteDebugString("WallSlot-GetDoorSurface() hash lookup failed %s\n", doorFile);
    }

    if (doorSurface < 0)
    {
#ifdef UAFEngine
      if ((doorSurface = GraphicsMgr.AddFileSurface(doorFile, DoorDib, rte.DoorArtDir())) < 0)
#else
      if ((doorSurface = GraphicsMgr.AddFileSurface(doorFile, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir())) < 0)
#endif
      {
        if (doorFile.Find("dr_",0) < 0)
        {
//          CString name(doorFile);
//          CString path("");
//          GetFilenamePath(doorFile, path);
//          StripFilenamePath(name);
//          path = "dr_" + name;

#ifdef UAFEngine
          doorSurface = GraphicsMgr.AddFileSurface(doorFile, DoorDib, rte.DoorArtDir());
#else
          doorSurface = GraphicsMgr.AddFileSurface(doorFile, DoorDib, rte.DoorArtDir(), ede.TemplateDoorArtDir());
#endif
//          {
//            WriteDebugString("Adjusted door file %s to %s in GetDoorSurface\n", doorFile, path);
//            doorFile = path;
//          }
        }

        if (doorSurface < 0)
          WriteDebugString("Failed to load door surface \'%s\' in GetDoorSurface\n", doorFile);
      }
      else
      {
        if (wallSurface <= 0) // only reset format if wall surface hasn't already
        {
          RECT rect;
          GraphicsMgr.GetSurfaceRect(doorSurface ,&rect);        
          SlotType = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        }
        tmp.name = doorFile;
        tmp.surface = doorSurface;        
        tmp.type=SlotType;       
        pWallSlotMgr->AddSurface(doorHash, tmp);
      }
    }
  }
  //WriteDebugString("WallSlot-GetDoorSurface() returns %s (%i)\n", doorFile, doorSurface);
  return doorSurface;
}

long WallSetSlotMemType::GetOverlaySurface()
{
  if (overlayFile.IsEmpty()) return -1;
  //if (overlaySurface <= 0)
  {
    static ImageCacheData tmp;
    if (overlayHash==0) overlayHash = HashString(overlayFile, IMAGE_CACHE_MAP_SIZE);
    if ((pWallSlotMgr!=NULL) && (pWallSlotMgr->GetSurface(overlayHash, tmp)))
    {
      overlaySurface = tmp.surface;
      if ((wallSurface <= 0)&&(doorSurface <= 0)) SlotType = tmp.type;
      if (!GraphicsMgr.ValidSurface(overlaySurface))
        overlaySurface=-1;
    }
    else
    {
      overlaySurface=-1;
      //WriteDebugString("WallSlot-GetOverlaySurface() hash lookup failed %s\n", overlayFile);
    }

    if (overlaySurface < 0)
    {
#ifdef UAFEngine
      if ((overlaySurface = GraphicsMgr.AddFileSurface(overlayFile, OverlayDib, rte.OverlayArtDir())) < 0)
#else
      if ((overlaySurface = GraphicsMgr.AddFileSurface(overlayFile, OverlayDib, rte.OverlayArtDir(), ede.TemplateOverlayArtDir())) < 0)
#endif
      {
          WriteDebugString("Failed to load overlay surface \'%s\' in GetOverlaySurface\n", overlayFile);
      }
      else
      {
        if ((wallSurface <= 0)&&(doorSurface <= 0)) // only reset format if wall and door surfaces haven't already
        {
          RECT rect;
          GraphicsMgr.GetSurfaceRect(overlaySurface ,&rect);        
          SlotType = WallFormatMgr.GetFormat(rect.right-rect.left, rect.bottom-rect.top);
        }
        tmp.name = overlayFile;
        tmp.surface = overlaySurface;
        tmp.type = SlotType;
        pWallSlotMgr->AddSurface(overlayHash, tmp);
      }
    }
  }
  //WriteDebugString("WallSlot-GetOverlaySurface() returns %s (%i)\n", overlayFile, overlaySurface);
  return overlaySurface;
}

long WallSetSlotMemType::GetAVArtSurface()
{
  if ((AVArtTried) || (AVArtFile.IsEmpty()))
  {
    AVZoneStyle=FALSE;
    return -1;
  }

  {
    static ImageCacheData tmp;
    if (AVArtHash==0) AVArtHash = HashString(AVArtFile, IMAGE_CACHE_MAP_SIZE);
    if ((pWallSlotMgr!=NULL) && (pWallSlotMgr->GetSurface(AVArtHash, tmp)))
    {
      AVArtSurface = tmp.surface;
      if (!GraphicsMgr.ValidSurface(AVArtSurface)) 
      {
        AVArtSurface=-1;
      }
      else
      {
        RECT tmpRect;
        GraphicsMgr.GetSurfaceRect(AVArtSurface, &tmpRect);
        // Make sure area view art is big enough to contain
        // the zone squares before enabling their usage.
        // The old style file was 92x80 pixels
        if (   ((tmpRect.right-tmpRect.left) >= 92)
            && ((tmpRect.bottom-tmpRect.top) > 80))
          AVZoneStyle = TRUE;
        else
          AVZoneStyle = FALSE;
      }
    }
    else
      AVArtSurface=-1;

    if (AVArtSurface < 0)
    {
      if (!LoadAreaViewArt(AVArtFile, AVArtSurface, AVZoneStyle))
      {
        AVArtTried=true;
        WriteDebugString("Failed to load area view art \'%s\' in WallSetSlotMemType::GetAVArtSurface\n", AVArtFile);
      }
      else
      {
        tmp.name = AVArtFile;
        tmp.surface = AVArtSurface;
        tmp.type=DEFAULT_FORMAT_TYPE;
        pWallSlotMgr->AddSurface(AVArtHash, tmp);
      }
    }
  }
  return AVArtSurface;
}

long BackgroundSlotMemType::GetBackgroundSurface()
{
  if (backgroundFile.IsEmpty()) return -1;
  //if (backgroundSurface <= 0)
  {    
    ImageCacheData tmp;
    if (backgroundFileHash==0) backgroundFileHash = HashString(backgroundFile, IMAGE_CACHE_MAP_SIZE);
    if ((pBgSlotMgr!=NULL) && (pBgSlotMgr->GetSurface(backgroundFileHash, tmp)))
    {
      backgroundSurface = tmp.surface;
      if (!GraphicsMgr.ValidSurface(backgroundSurface))
        backgroundSurface=-1;
    }
    else
    {
      backgroundSurface=-1;
      //WriteDebugString("WallSlot-GetBackgroundSurface() hash lookup failed %s\n", backgroundFile);
    }

    if (backgroundSurface < 0)
    {
      SlotType = DEFAULT_FORMAT_TYPE;
#ifdef UAFEngine
      if ((backgroundSurface = GraphicsMgr.AddFileSurface(backgroundFile, BackGndDib, rte.BackgroundArtDir())) < 0)
#else
      if ((backgroundSurface = GraphicsMgr.AddFileSurface(backgroundFile, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) < 0)
#endif
      {
        if (backgroundFile.Find("bd_",0) < 0)
        {
          CString name(backgroundFile);
          CString path("");
          GetFilenamePath(backgroundFile, path);
          StripFilenamePath(name);
          path = "bd_" + name;

#ifdef UAFEngine
          if ((backgroundSurface = GraphicsMgr.AddFileSurface(path, BackGndDib, rte.BackgroundArtDir())) >= 0)
#else
          if ((backgroundSurface = GraphicsMgr.AddFileSurface(path, BackGndDib, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted background file %s to %s in GetBackgroundSurface\n", backgroundFile, path);
            backgroundFile = path;
          }
        }

        if (backgroundSurface < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLBS03")+backgroundFile))
          {
            debugSeverity = 7;
            WriteDebugString("Failed to load background surface \'%s\' in GetBackgroundSurface\n", backgroundFile);
          };
        }
      }
      else
      {
        tmp.name = backgroundFile;
        tmp.surface = backgroundSurface;
        pBgSlotMgr->AddSurface(backgroundFileHash, tmp);
        GraphicsMgr.GetSurfaceRect(backgroundSurface,&surfRect);        
      }
    }
  }
  return backgroundSurface;
}

long BackgroundSlotMemType::GetBackgroundAltSurface()
{
  if (backgroundFileAlt.IsEmpty()) return -1;
  //if (backgroundSurfaceAlt <= 0)
  {    
    ImageCacheData tmp;
    if (backgroundFileAltHash==0) backgroundFileAltHash = HashString(backgroundFileAlt, IMAGE_CACHE_MAP_SIZE);
    if ((pBgSlotMgr!=NULL) && (pBgSlotMgr->GetSurface(backgroundFileAltHash, tmp)))
    {
      backgroundSurfaceAlt = tmp.surface;
      if (!GraphicsMgr.ValidSurface(backgroundSurfaceAlt))
        backgroundSurfaceAlt=-1;
    }
    else
    {
      backgroundSurfaceAlt=-1;
      //WriteDebugString("WallSlot-GetBackgroundAltSurface() hash lookup failed %s\n", backgroundFileAlt);
    }
    if (backgroundSurfaceAlt < 0)
    {
      SlotType = DEFAULT_FORMAT_TYPE;
      SurfaceType type = (useTransparency?TransBufferDib:BackGndDib);
#ifdef UAFEngine
      if ((backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(backgroundFileAlt, type, rte.BackgroundArtDir())) < 0)
#else
      if ((backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(backgroundFileAlt, type, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) < 0)
#endif
      {
        if (backgroundFileAlt.Find("bd_",0) < 0)
        {
          CString name(backgroundFileAlt);
          CString path("");
          GetFilenamePath(backgroundFileAlt, path);
          StripFilenamePath(name);
          path = "bd_" + name;

#ifdef UAFEngine
          if ((backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(path, type, rte.BackgroundArtDir())) >= 0)
#else
          if ((backgroundSurfaceAlt = GraphicsMgr.AddFileSurface(path, type, rte.BackgroundArtDir(), ede.TemplateBackgroundArtDir())) >= 0)
#endif
          {
            WriteDebugString("Adjusted background alt file %s to %s in GetBackgroundAltSurface\n", backgroundFileAlt, path);
            backgroundFileAlt = path;
          }
        }

        if (backgroundSurfaceAlt < 0)
        {
          if (!debugStrings.AlreadyNoted(CString("FTLBA02")+backgroundFileAlt))
          {
            debugSeverity = 7;
            WriteDebugString("Failed to load background alt surface \'%s\' in GetBackgroundAltSurface\n", backgroundFileAlt);
          };
        };
      }
      else
      {
        tmp.name = backgroundFileAlt;
        tmp.surface = backgroundSurfaceAlt;
        pBgSlotMgr->AddSurface(backgroundFileAltHash, tmp);
        GraphicsMgr.GetSurfaceRect(backgroundSurfaceAlt,&surfRect);        
      }
    }
  }
  return backgroundSurfaceAlt;
}

void WallSetSlotMemType::Clear()
{ 
  SlotType = DEFAULT_FORMAT_TYPE;
  ClearResources();  
  wallHash=0;
  doorHash=0;
  overlayHash=0;
  AVArtHash=0;
  wallSurface=doorSurface=overlaySurface=-1;hsound=-1;
  wallFile="";doorFile="";overlayFile="";soundFile="";
  used=FALSE;
  doorFirst=FALSE;
  drawAV=TRUE;
  //UnlockSpell.Clear();UnlockSpellClass=0;UnlockSpellLevel=0;
  UnlockSpellID.Empty();
  BlendOverlay=FALSE;
  BlendAmount=0;
  AVArtTried=false;
  AVZoneStyle=FALSE;
  AVArtFile="";
  AVArtSurface=-1;
}

WallSetSlotMemType& WallSetSlotMemType::operator =(const WallSetSlotMemType& src)
{
  if (&src == this)
    return *this;
  Clear();
  used=src.used;
  doorFirst=src.doorFirst;
  drawAV=src.drawAV;
  wallSurface=src.wallSurface;
  doorSurface=src.doorSurface;
  overlaySurface=src.overlaySurface;
  AVArtSurface=src.AVArtSurface;
  hsound=src.hsound;
  BlendOverlay=src.BlendOverlay;
  BlendAmount=src.BlendAmount;
  UnlockSpellID=src.UnlockSpellID;
  //UnlockSpellClass=src.UnlockSpellClass;
  //UnlockSpellLevel=src.UnlockSpellLevel;
  wallHash=src.wallHash;
  doorHash=src.doorHash;
  overlayHash=src.overlayHash;
  AVArtHash=src.AVArtHash;
  if (!src.wallFile.IsEmpty()) wallFile=src.wallFile;
  if (!src.doorFile.IsEmpty()) doorFile=src.doorFile;
  if (!src.overlayFile.IsEmpty()) overlayFile=src.overlayFile;
  if (!src.soundFile.IsEmpty()) soundFile=src.soundFile;
  if (!src.AVArtFile.IsEmpty()) AVArtFile=src.AVArtFile;
  return *this;
}

#ifdef UAFEDITOR
bool WallSetSlotMemType::operator ==(const WallSetSlotMemType& src)
{
  if (&src == this) return true;
  if (used != src.used) return false;
  if (doorFirst != src.doorFirst) return false;
  if (drawAV != src.drawAV) return false;
  //if (wallSurface != src.wallSurface) return false;
  //if (doorSurface != src.doorSurface) return false;
  //if (overlaySurface != src.overlaySurface) return false;
  //if (AVArtSurface != src.AVArtSurface) return false;
  if (hsound != src.hsound) return false;
  if (BlendOverlay != src.BlendOverlay) return false;
  if (BlendAmount != src.BlendAmount) return false;
  if (UnlockSpellID != src.UnlockSpellID) return false;
  //if (wallHash != src.wallHash) return false;
  //if (doorHash != src.doorHash) return false;
  //if (overlayHash != src.overlayHash) return false;
  //if (AVArtHash != src.AVArtHash) return false;
  if (wallFile != src.wallFile) return false;
  if (doorFile != src.doorFile) return false;
  if (overlayFile != src.overlayFile) return false;
  if (soundFile != src.soundFile) return false;
  if (AVArtFile != src.AVArtFile) return false;
  return true;
}
#endif


void BackgroundSlotMemType::Clear()
{ 
  SlotType = DEFAULT_FORMAT_TYPE;
  ClearResources();
  suppressStepSound=FALSE;
  useAltBackground=FALSE; 
  backgroundSurface=backgroundSurfaceAlt=hsound-1; 
  backgroundFileAlt="";backgroundFile="";soundFile="";
  used=FALSE; 
  AlphaBlendPcnt=0;
  useAlphaBlend=FALSE;
  useTransparency = FALSE;
  backgroundFileHash=0;
  backgroundFileAltHash=0;
  memset(&surfRect, 0, sizeof(surfRect));
}

BackgroundSlotMemType& BackgroundSlotMemType::operator =(const BackgroundSlotMemType& src)
{
   if (&src == this)
     return *this;
   Clear();
   suppressStepSound=src.suppressStepSound;
   used=src.used; 
   hsound=src.hsound;
   useAltBackground=src.useAltBackground;
   useAlphaBlend=src.useAlphaBlend;
   useTransparency=src.useTransparency;
   AlphaBlendPcnt=src.AlphaBlendPcnt;
   backgroundSurface=src.backgroundSurface;
   backgroundSurfaceAlt=src.backgroundSurfaceAlt;
   StartTime=src.StartTime;
   EndTime=src.EndTime;
   backgroundFileHash=src.backgroundFileHash;
   backgroundFileAltHash=src.backgroundFileAltHash;
   if (!src.backgroundFile.IsEmpty()) backgroundFile = src.backgroundFile;
   if (!src.backgroundFileAlt.IsEmpty()) backgroundFileAlt = src.backgroundFileAlt;
   if (!src.soundFile.IsEmpty()) soundFile=src.soundFile;
   return *this;
}
#ifdef UAFEDITOR
bool BackgroundSlotMemType::operator ==(const BackgroundSlotMemType& src)
{
   if (&src == this) return true;
   if (suppressStepSound != src.suppressStepSound) return false;
   if (used != src.used) return false; 
   if (hsound != src.hsound) return false;
   if (useAltBackground != src.useAltBackground) return false;
   if (useAlphaBlend != src.useAlphaBlend) return false;
   if (useTransparency != src.useTransparency) return false;
   if (AlphaBlendPcnt != src.AlphaBlendPcnt) return false;
   //if (backgroundSurface != src.backgroundSurface) return false;
   //if (backgroundSurfaceAlt != src.backgroundSurfaceAlt) return false;
   if (StartTime != src.StartTime) return false;
   if (EndTime != src.EndTime) return false;
   //if (backgroundFileHash != src.backgroundFileHash) return false;
   //if (backgroundFileAltHash != src.backgroundFileAltHash) return false;
   if (backgroundFile != src.backgroundFile) return false;
   if (backgroundFileAlt != src.backgroundFileAlt) return false;
   if (soundFile != src.soundFile) return false;
   return true;
}
#endif
void PicDataType::Clear() 
{ 
  picType=BogusDib;name="";surface=-1;
}

PicDataType& PicDataType::operator =(const PicDataType& src) 
{
 if (&src == this)
   return *this;
 Clear();
 picType=src.picType;
 surface=src.surface;
 if (!src.name.IsEmpty()) name=src.name;
 return *this;
}

PicDataType::PicDataType(const PicDataType& src) 
{ 
  *this=src; 
}