/******************************************************************************
* Filename: GlobalData.cpp
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
#include "..\Shared\ProjectVersion.h"

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#else
//#include "externs.h"
#include "..\UAFWin\Dungeon.h"
#include "..\UAFWin\DgnGame.h"
#endif

#include "class.h"
#ifdef UAFEDITOR
#include "..\UAFWinEd\resource.h"
#include "..\UAFWinEd\CrossReference.h"
#include "ConfigFile.h"
#endif
#include "FileParse.h"
#include "Graphics.h"
#include "PicSlot.h"
#include "party.h"
#include "GlobalData.h"
#include "Level.h"
#include "SoundMgr.h"
#include "monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// store/retrieve data from asl maps
//void StorePicDataAsASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrievePicDataFromASL(A_ASLENTRY_L &asl, PIC_DATA& data, const char *prefix);
void StoreIntAsASL(A_ASLENTRY_L &asl, int data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrieveIntFromASL(A_ASLENTRY_L &asl, int &data, const char *prefix);
void StoreStringAsASL(A_ASLENTRY_L &asl, CString &data, const char *prefix, unsigned char flags=ASLF_EDITOR);
void RetrieveStringFromASL(A_ASLENTRY_L &asl, CString &data, const char *prefix);




//IMPLEMENT_SERIAL( GLOBAL_SOUND_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( SPECIAL_OBJECT_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( SPECIAL_OBJECT_LIST, CObject, 1 )
//IMPLEMENT_SERIAL( QUEST_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( QUEST_LIST, CObject, 1 )
//IMPLEMENT_SERIAL( GLOBAL_VAULT_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( BACKGROUND_SOUNDS, CObject, 1 )
//IMPLEMENT_SERIAL( LEVEL_STATS, CObject, 1 )
//IMPLEMENT_SERIAL( LEVEL_INFO, CObject, 1 )
//IMPLEMENT_SERIAL( GLOBAL_STATS, CObject, 1 )

GLOBAL_STATS globalData;
unsigned int gamedatSignature;
void die(int errorNum);


extern const CString Skill_PickPockets        = "PickPockets";
extern const CString Skill_OpenLocks          = "OpenLocks";
extern const CString Skill_FindTraps          = "FindTraps";
extern const CString Skill_MoveSilent         = "MoveSilent";
extern const CString Skill_HideInShadows      = "HideInShadows"; 
extern const CString Skill_HearNoise          = "HearNoise"; 
extern const CString Skill_ClimbWalls         = "ClimbWalls"; 
extern const CString Skill_ReadLanguages      = "ReadLanguages"; 
extern const CString Skill_Turn               = "Turn";
extern const CString Skill_BackstabMultiplier = "BackstabMultiplier";
extern const CString Skill_RangerBonusLevel   = "RangerBonusLevel";


extern const CString SA_Turn             = "Turn$SYS$";
extern const CString SA_XP_Value         = "XP_Value$SYS$";
extern const CString Skill_MaxLevel      = "MaxLevel$SYS$";


extern const CString Ability_Dexterity    = "Dexterity";
extern const CString Ability_Constitution = "Constitution";
extern const CString Ability_Strength     = "Strength";
extern const CString Ability_Intelligence = "Intelligence";
extern const CString Ability_Charisma     = "Charisma";
extern const CString Ability_Wisdom       = "Wisdom";


extern const CString Baseclass_magicUser = "magicUser";
extern const CString Baseclass_thief     = "thief";
extern const CString Baseclass_paladin   = "paladin";
extern const CString Baseclass_cleric    = "cleric";
extern const CString Baseclass_fighter   = "fighter";
extern const CString Baseclass_ranger    = "ranger";
extern const CString Baseclass_druid     = "druid";


extern const CString School_Magic_User   = "Magic User";
extern const CString School_Cleric       = "Cleric";

extern const CString Save_Vs_PPDM = "Save Vs PPDM";
extern const CString Save_Vs_PP   = "Save Vs PP";
extern const CString Save_Vs_RSW  = "Save Vs RSW";
extern const CString Save_Vs_BR   = "Save Vs BR";
extern const CString Save_Vs_SP   = "Save Vs SP";

//extern const int CHARACTER_VERSION;  // Not to be confused with index values
extern const double VersionSpellNames;
//extern const double VersionSpellIDs;
extern const double PRODUCT_VER;
//extern const double ENGINE_VER;

extern int useWallIndex;
extern int useDoorAndOverlayIndex;


/*
int GetCurrentLevel(void)
{
  return globalData.currLevel;
}
*/
#ifdef UAFEDITOR


int ExportGlobalDB(LPCSTR file,
                   GLOBAL_STATS *pGlobalPointer,
                   CONFIG_FILE *pConfigFile);


void ExportGlobal() 
{
   CString fullPath;
   fullPath.Format("%sGlobalData.txt", rte.DataDir());
     CONFIG_FILE ConfFile;
     AfxGetApp()->DoWaitCursor(1);
     BOOL success = (ExportGlobalDB(fullPath, &globalData, &ConfFile)==0);
     AfxGetApp()->DoWaitCursor(-1);

     if (!success)
        MsgBoxError("Operation Failed!", "Database Error");
     else
        MsgBoxInfo("Operation Successful", "Database Message");
}


void ImportGlobal(CString filename) 
{
  if (filename.IsEmpty())
  {
   filename.Format("%sGlobalData.txt",rte.DataDir());
    if (!GetFilename(filename, "txt", TRUE)) return;
  }
   {  
     CFile cf;
     int status = CONFIG_STAT_ok;
     cf.Open(filename, CFile::modeRead);
     JReader jr(&cf);
     AfxGetApp()->DoWaitCursor(1);
     try
     {
       if ((status = jr.Initialize()) != 0)
       {
         status = CONFIG_STAT_syntax;
         throw (1);
       }
     }
     catch (int)
     {
       CString msg;
       msg.Format("Unable to Import Global Data - \n%s", jr.GetErrorMessage());
       MsgBoxInfo(msg);
       AfxGetApp()->DoWaitCursor(-1);
       return;
     };
     AfxGetApp()->DoWaitCursor(-1);
     {
       GLOBAL_STATS tempGlobal;
       CONFIG_ITEM_STATUS itemStatus = CONFIG_STAT_ok;;
       try
       {
          jr.StartList();
          itemStatus = tempGlobal.Import(jr, FALSE);
          if (itemStatus == CONFIG_STAT_ok)
          {
            tempGlobal.levelInfo = globalData.levelInfo;
          };
          jr.EndList();
       }
       catch(int e)
       {
         e = 1;
         MsgBoxInfo(jr.GetErrorMessage());
         itemStatus = CONFIG_STAT_semantic;
       };
       if (itemStatus != CONFIG_STAT_ok)
       {
         MsgBoxInfo("We failed to import the Global Data");
         AfxGetApp()->DoWaitCursor(-1);
         return;
       };
       globalData = tempGlobal;
     };
     AfxGetApp()->DoWaitCursor(-1);

     SET_MODIFIED;
     CString temp;
    MsgBoxInfo("Global Data Import Successful", "Dat File Message");
   }
}

#endif

void FillDefaultFontData(LPCSTR font, BYTE size, LOGFONT *plf)
{
  int tmpCharset, charset;
  charset = ANSI_CHARSET;
  if (ConfigFile.FindToken("CHARSET", tmpCharset))
  {
      charset = tmpCharset;
  }

  memset(plf,0,sizeof(LOGFONT));
	CFont Font;
  Font.CreateFont(size,0,0,0,
		              FW_BOLD,
		              FALSE,
		              FALSE,
		              FALSE,
                      (byte) charset,
		              OUT_DEFAULT_PRECIS,
		              CLIP_DEFAULT_PRECIS,
		              NONANTIALIASED_QUALITY,
		              VARIABLE_PITCH,
		              font);
  Font.GetLogFont(plf);
}

//*****************************************************************************

void TITLE_SCREEN::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    //StripFilenamePath(TitleBgArt);
    AS(ar, TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    ar << UseTrans;
    ar << UseBlend;
    ar << DisplayBy;
  }
  else
  {
    DAS(ar, TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    StripFilenamePath(TitleBgArt);
    ar >> UseTrans;
    ar >> UseBlend;
    ar >> DisplayBy;
  }
}

void TITLE_SCREEN::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    //StripFilenamePath(TitleBgArt);
    AS(ar, TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    ar << UseTrans;
    ar << UseBlend;
    ar << DisplayBy;
  }
  else
  {
    DAS(ar, TitleBgArt);
    StripFilenamePath(TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    ar >> UseTrans;
    ar >> UseBlend;
    ar >> DisplayBy;
  }
}

#ifdef UAFEDITOR

const char *JKEY_TITLEBGART = "titleBgArt";
const char *JKEY_USETRANS = "useTrans";
const char *JKEY_USEBLEND = "useBlend";
const char *JKEY_DISPLAYBY = "displayBy";

void TITLE_SCREEN::Export(JWriter& jw)
{
    //StripFilenamePath(TitleBgArt);
  jw.StartList();
    jw.NameAndValue(JKEY_TITLEBGART, TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    jw.NameAndValue(JKEY_USETRANS, UseTrans);
    jw.NameAndValue(JKEY_USEBLEND, UseBlend);
    jw.NameAndValue(JKEY_DISPLAYBY, DisplayBy);
  jw.EndList();
}
void TITLE_SCREEN::Import(JReader& jr)
{
    //StripFilenamePath(TitleBgArt);
  jr.StartList();
  {
    jr.NameAndValue(JKEY_TITLEBGART, TitleBgArt);
    //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
    jr.NameAndValue(JKEY_USETRANS, UseTrans);
    jr.NameAndValue(JKEY_USEBLEND, UseBlend);
    jr.NameAndValue(JKEY_DISPLAYBY, DisplayBy);
  };
  jr.EndList();
}
#endif


TITLE_SCREEN& TITLE_SCREEN::operator =(const TITLE_SCREEN& src)
{
  if (this==&src) return *this;
  Clear();
  TitleBgArt=src.TitleBgArt;
  UseTrans=src.UseTrans;
  UseBlend=src.UseBlend;
  DisplayBy=src.DisplayBy;
  return *this;
}

//*****************************************************************************

void TITLE_SCREEN_DATA::Serialize(CArchive &ar)
{
  DWORD count;
  if (ar.IsStoring())
  {
    ar << Timeout;
    count = Titles.GetCount();
    ar << count;
    POSITION pos = Titles.GetHeadPosition();
    while (pos != NULL)
      Titles.GetNext(pos).Serialize(ar);
  }
  else
  {
    ar >> Timeout;
    TITLE_SCREEN data;
    ar >> count;
    for (DWORD i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar);
      Titles.AddTail(data);
    }
  }
}

void TITLE_SCREEN_DATA::Serialize(CAR &ar)
{
  DWORD count;
  if (ar.IsStoring())
  {
    ar << Timeout;
    count = Titles.GetCount();
    ar << count;
    POSITION pos = Titles.GetHeadPosition();
    while (pos != NULL)
      Titles.GetNext(pos).Serialize(ar);
  }
  else
  {
    ar >> Timeout;
    TITLE_SCREEN data;
    ar >> count;
    for (DWORD i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar);
      Titles.AddTail(data);
    }
  }
}

#ifdef UAFEDITOR

const char *JKEY_TITLESCREENDATA = "titleScreenData";
const char *JKEY_TIMEOUT = "timeout";
const char *JKEY_TITLES = "titles";

void TITLE_SCREEN_DATA::Export(JWriter& jw)
{
  {
    jw.StartList(JKEY_TITLESCREENDATA);
    jw.NameAndValue(JKEY_TIMEOUT, Timeout);
    jw.StartArray(JKEY_TITLES);
    POSITION pos = Titles.GetHeadPosition();
    while (pos != NULL)
    {
      jw.NextEntry();
      Titles.GetNext(pos).Export(jw);
    };
    jw.EndArray();
    jw.EndList();
  }
};

void TITLE_SCREEN_DATA::Import(JReader& jr)
{
  {
    jr.StartList(JKEY_TITLESCREENDATA);
    jr.NameAndValue(JKEY_TIMEOUT, Timeout);
    jr.StartArray(JKEY_TITLES);
    while (jr.Optional(), jr.NextEntry())
    {
      TITLE_SCREEN title;
      title.Import(jr);
      AddTitle(title);
    };
    jr.EndArray();
    jr.EndList();
  }
};

void TITLE_SCREEN_DATA::CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference)
{
  POSITION pos;
  TITLE_SCREEN *pTitleScreen;
  for (pos=Titles.GetHeadPosition(); pos!=NULL; Titles.GetNext(pos))
  {
    pTitleScreen = &Titles.GetAt(pos);
    pCRList->CR_AddPicReference(pTitleScreen->TitleBgArt, pCRReference);
  };
}

#endif

TITLE_SCREEN_DATA& TITLE_SCREEN_DATA::operator =(const TITLE_SCREEN_DATA& src)
{
  if (this==&src) return *this;
  Clear();
  Timeout=src.Timeout;
  POSITION pos = src.Titles.GetHeadPosition();
  while (pos != NULL)
  {
    TITLE_SCREEN data = src.Titles.GetNext(pos);
    AddTitle(data);
  }
  return *this;
}

POSITION TITLE_SCREEN_DATA::AddTitle(TITLE_SCREEN &data)
{
  return (Titles.AddTail(data));
}

void TITLE_SCREEN_DATA::SetDefault(CString deftitle)
{
  TITLE_SCREEN data;
  data.TitleBgArt = deftitle;
  data.UseTrans=FALSE;
  data.DisplayBy=TITLE_SCREEN::tsFadeIn;
  Timeout=0;
  AddTitle(data);
}

//*****************************************************************************

void SPELL_SPECIAL_DATA::Serialize(CArchive &ar, double version)
{
  POSITION pos;
  if (ar.IsStoring())
  {
    ar << m_DetectMagicSpells.GetCount();
    pos = m_DetectMagicSpells.GetHeadPosition();
    while (pos!=NULL)
      //m_DetectMagicSpells.GetNext(pos).Serialize(ar, version); //ar << m_DetectMagicSpells.GetNext(pos);
    ar << m_DetectMagicSpells.GetNext(pos);

    ar << m_IdentifySpells.GetCount();
    pos = m_IdentifySpells.GetHeadPosition();
    while (pos!=NULL)
      //ar << m_IdentifySpells.GetNext(pos);
      //m_IdentifySpells.GetNext(pos).Serialize(ar, version);
    ar << m_IdentifySpells.GetNext(pos);
  }
  else
  {
    Clear();

    DWORD count;
    //GLOBAL_SPELL_ID temp;
    CString tempItemName;
    DWORD i;
    ar >> count;
    for (i=0;i<count;i++)
    {
      //temp.Serialize(ar, version); //ar >> temp;
      //m_DetectMagicSpells.AddTail(temp);
      ar >> tempItemName;
      m_DetectMagicSpells.AddTail(tempItemName);
    }

    ar >> count;
    for (i=0;i<count;i++)
    {
      //temp.Serialize(ar, version); //ar >> temp;
      //m_IdentifySpells.AddTail(temp);
      ar >> tempItemName;
      m_IdentifySpells.AddTail(tempItemName);
    }
  }
}

void SPELL_SPECIAL_DATA::Serialize(CAR &ar, double version)
{
  POSITION pos;
  if (ar.IsStoring())
  {
    ar << m_DetectMagicSpells.GetCount();
    pos = m_DetectMagicSpells.GetHeadPosition();
    while (pos!=NULL)
      //ar << m_DetectMagicSpells.GetNext(pos);
      //m_DetectMagicSpells.GetNext(pos).Serialize(ar, version);
    ar << m_DetectMagicSpells.GetNext(pos);

    ar << m_IdentifySpells.GetCount();
    pos = m_IdentifySpells.GetHeadPosition();
    while (pos!=NULL)
      //ar << m_IdentifySpells.GetNext(pos);
      //m_IdentifySpells.GetNext(pos).Serialize(ar, version);
    ar << m_IdentifySpells.GetNext(pos);
  }
  else
  {
    Clear();

    DWORD count;
    //GLOBAL_SPELL_ID temp;
    CString tempItemName;
    DWORD i;
    ar >> count;
    for (i=0;i<count;i++)
    {
      //temp.Serialize(ar, version);//ar >> temp;
      //m_DetectMagicSpells.AddTail(temp);
      ar >> tempItemName;
      m_DetectMagicSpells.AddTail(tempItemName);
    }

    ar >> count;
    for (i=0;i<count;i++)
    {
      //temp.Serialize(ar, version);//ar >> temp;
      //m_IdentifySpells.AddTail(temp);
      ar >> tempItemName;
      m_IdentifySpells.AddTail(tempItemName);
    }
  }
}

SPELL_SPECIAL_DATA& SPELL_SPECIAL_DATA::operator =(const SPELL_SPECIAL_DATA& src)
{
  if (&src==this) return *this;
  Clear();
  POSITION pos;

  pos = src.m_DetectMagicSpells.GetHeadPosition();
  while (pos!=NULL)
  {
    //m_DetectMagicSpells.AddTail(src.m_DetectMagicSpells.GetNext(pos));
    CString tempSpellName;
    tempSpellName = src.m_DetectMagicSpells.GetNext(pos);
    m_DetectMagicSpells.AddTail(tempSpellName);
  };
  pos = src.m_IdentifySpells.GetHeadPosition();
  while (pos!=NULL)
  {
    //m_IdentifySpells.AddTail(src.m_IdentifySpells.GetNext(pos));
    CString tempSpellName;
    tempSpellName = src.m_IdentifySpells.GetNext(pos);
    m_IdentifySpells.AddTail(tempSpellName);
  };
  return *this;
}

//GLOBAL_SPELL_ID SPELL_SPECIAL_DATA::HasDetectMagicSpell(CHARACTER &data)
CString SPELL_SPECIAL_DATA::HasDetectMagicSpell(CHARACTER &data)
{
  //if (m_DetectMagicSpells.GetCount()==0) return GLOBAL_SPELL_ID();
  if (m_DetectMagicSpells.GetCount()==0) return CString();
  POSITION pos = m_DetectMagicSpells.GetHeadPosition();
  while (pos!=NULL)
  {
    //GLOBAL_SPELL_ID gsID;
    SPELL_ID spellID;
    //gsID = m_DetectMagicSpells.GetAt(pos);
    spellID = m_DetectMagicSpells.GetAt(pos);
    //if (data.IsMemorized(gsID))
    if (data.IsMemorized(spellID))
      return m_DetectMagicSpells.GetAt(pos);
    m_DetectMagicSpells.GetNext(pos);
  }
  //return GLOBAL_SPELL_ID();
  return CString();
}

//GLOBAL_SPELL_ID SPELL_SPECIAL_DATA::HasIdentifySpell(CHARACTER &data)
CString SPELL_SPECIAL_DATA::HasIdentifySpell(CHARACTER &data)
{
  //if (m_IdentifySpells.GetCount()==0) return GLOBAL_SPELL_ID();
  if (m_IdentifySpells.GetCount()==0) return CString();
  POSITION pos = m_IdentifySpells.GetHeadPosition();
  while (pos!=NULL)
  {
    //GLOBAL_SPELL_ID gsID;
    SPELL_ID spellID;
    //gsID = m_IdentifySpells.GetAt(pos);
    spellID = m_IdentifySpells.GetAt(pos);
    //if (data.IsMemorized(gsID))
    if (data.IsMemorized(spellID))
      return m_IdentifySpells.GetAt(pos);
    m_IdentifySpells.GetNext(pos);
  }
  //return GLOBAL_SPELL_ID();
  return CString();
}

//*****************************************************************************

void DIFFICULTY_DATA::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
//    if (m_Name.IsEmpty()) m_Name="*";
//    ar << m_Name;
    AS(ar,m_Name);
    ar << m_ModifyHD;
    ar << m_ModifyQty;
    ar << m_ModifyMonsterExp;
    ar << m_ModifyAllExp;
    ar << m_HDAmount;
    ar << m_QtyAmount;
    ar << m_MExpAmount;
    ar << m_AExpAmount;
//    if (m_Name=="*") m_Name="";
  }
  else
  {
//    ar >> m_Name;
    DAS(ar,m_Name);
    ar >> m_ModifyHD;
    ar >> m_ModifyQty;
    ar >> m_ModifyMonsterExp;
    ar >> m_ModifyAllExp;
    ar >> m_HDAmount;
    ar >> m_QtyAmount;
    ar >> m_MExpAmount;
    ar >> m_AExpAmount;
//    if (m_Name=="*") m_Name="";
  }
}

void DIFFICULTY_DATA::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
//    if (m_Name.IsEmpty()) m_Name="*";
    AS(ar,m_Name);
    //ar << m_Name;
    ar << m_ModifyHD;
    ar << m_ModifyQty;
    ar << m_ModifyMonsterExp;
    ar << m_ModifyAllExp;
    ar << m_HDAmount;
    ar << m_QtyAmount;
    ar << m_MExpAmount;
    ar << m_AExpAmount;
//    if (m_Name=="*") m_Name="";
  }
  else
  {
//    ar >> m_Name;
    DAS(ar,m_Name);
    ar >> m_ModifyHD;
    ar >> m_ModifyQty;
    ar >> m_ModifyMonsterExp;
    ar >> m_ModifyAllExp;
    ar >> m_HDAmount;
    ar >> m_QtyAmount;
    ar >> m_MExpAmount;
    ar >> m_AExpAmount;
//    if (m_Name=="*") m_Name="";
  }
}

#ifdef UAFEDITOR
extern const char *JKEY_NAME;
    //ar << m_Name;
const char *JKEY_MODIFYHD = "modifyHD";
const char *JKEY_MODIFYQTY = "modifyQty";
const char *JKEY_MODIFYMONSTEREXP = "modifyMonsterExp";
const char *JKEY_MODIFYALLEXP = "modifyAllExp";
const char *JKEY_HDAMOUNT = "hdAmount";
const char *JKEY_QTYAMOUNT = "qtyAmount";
const char *JKEY_MEXPAMOUNT = "mExpAmount";
const char *JKEY_QEXPAMOUNT = "aExpAmount";
const char *JKEY_ENTRIES = "entries";
const char *JKEY_JOURNAL = "journal";

void DIFFICULTY_DATA::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_NAME ,m_Name);
    //ar << m_Name;
  jw.NameAndValue(JKEY_MODIFYHD ,m_ModifyHD);
  jw.NameAndValue(JKEY_MODIFYQTY ,m_ModifyQty);
  jw.NameAndValue(JKEY_MODIFYMONSTEREXP ,m_ModifyMonsterExp);
  jw.NameAndValue(JKEY_MODIFYALLEXP ,m_ModifyAllExp);
  jw.NameAndValue(JKEY_HDAMOUNT ,m_HDAmount);
  jw.NameAndValue(JKEY_QTYAMOUNT ,m_QtyAmount);
  jw.NameAndValue(JKEY_MEXPAMOUNT ,m_MExpAmount);
  jw.NameAndValue(JKEY_QEXPAMOUNT ,m_AExpAmount);
  jw.EndList();
}
void DIFFICULTY_DATA::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_NAME ,m_Name);
    //ar << m_Name;
  jr.NameAndValue(JKEY_MODIFYHD ,m_ModifyHD);
  jr.NameAndValue(JKEY_MODIFYQTY ,m_ModifyQty);
  jr.NameAndValue(JKEY_MODIFYMONSTEREXP ,m_ModifyMonsterExp);
  jr.NameAndValue(JKEY_MODIFYALLEXP ,m_ModifyAllExp);
  jr.NameAndValue(JKEY_HDAMOUNT ,m_HDAmount);
  jr.NameAndValue(JKEY_QTYAMOUNT ,m_QtyAmount);
  jr.NameAndValue(JKEY_MEXPAMOUNT ,m_MExpAmount);
  jr.NameAndValue(JKEY_QEXPAMOUNT ,m_AExpAmount);
  jr.EndList();
}
#endif


DIFFICULTY_DATA& DIFFICULTY_DATA::operator=(const DIFFICULTY_DATA& src)
{
  if (&src==this) return *this;
  m_Name=src.m_Name;
  m_ModifyHD=src.m_ModifyHD;
  m_ModifyQty=src.m_ModifyQty;
  m_ModifyMonsterExp=src.m_ModifyMonsterExp;
  m_ModifyAllExp=src.m_ModifyAllExp;
  m_HDAmount=src.m_HDAmount;
  m_QtyAmount=src.m_QtyAmount;
  m_MExpAmount=src.m_MExpAmount;
  m_AExpAmount=src.m_AExpAmount;
  return *this;
}
/*                   qty   hd  mexp  aexp
   { "NOVICE", 0 },  -50  -20  -30   -10
   { "SQUIRE", 0 },  -25  -10  -15    -5
   { "VETERAN", 0 },   0    0    0     0
   { "ADEPT", 0 },   +25  +10  +15    +5
   { "CHAMPION", 0 } +50  +20  +30   +10
*/
void DIFFICULTY_LEVEL_DATA::SetUADefaults()
{
  Clear();
  m_defaultLvl=2;

  m_data[0].m_Name="NOVICE";
  m_data[0].m_ModifyQty=TRUE;
  m_data[0].m_ModifyHD=TRUE;
  m_data[0].m_ModifyMonsterExp=TRUE;
  m_data[0].m_ModifyAllExp=TRUE;
  m_data[0].m_QtyAmount=-50;
  m_data[0].m_HDAmount=-20;
  m_data[0].m_MExpAmount=-30;
  m_data[0].m_AExpAmount=-10;

  m_data[1].m_Name="SQUIRE";
  m_data[1].m_ModifyQty=TRUE;
  m_data[1].m_ModifyHD=TRUE;
  m_data[1].m_ModifyMonsterExp=TRUE;
  m_data[1].m_ModifyAllExp=TRUE;
  m_data[1].m_QtyAmount=-25;
  m_data[1].m_HDAmount=-10;
  m_data[1].m_MExpAmount=-15;
  m_data[1].m_AExpAmount=-5;

  m_data[2].m_Name="VETERAN";
  m_data[2].m_ModifyQty=FALSE;
  m_data[2].m_ModifyHD=FALSE;
  m_data[2].m_ModifyMonsterExp=FALSE;
  m_data[2].m_ModifyAllExp=FALSE;
  m_data[2].m_QtyAmount=0;
  m_data[2].m_HDAmount=0;
  m_data[2].m_MExpAmount=0;
  m_data[2].m_AExpAmount=0;

  m_data[3].m_Name="ADEPT";
  m_data[3].m_ModifyQty=TRUE;
  m_data[3].m_ModifyHD=TRUE;
  m_data[3].m_ModifyMonsterExp=TRUE;
  m_data[3].m_ModifyAllExp=TRUE;
  m_data[3].m_QtyAmount=25;
  m_data[3].m_HDAmount=10;
  m_data[3].m_MExpAmount=15;
  m_data[3].m_AExpAmount=5;

  m_data[4].m_Name="CHAMPION";
  m_data[4].m_ModifyQty=TRUE;
  m_data[4].m_ModifyHD=TRUE;
  m_data[4].m_ModifyMonsterExp=TRUE;
  m_data[4].m_ModifyAllExp=TRUE;
  m_data[4].m_QtyAmount=50;
  m_data[4].m_HDAmount=20;
  m_data[4].m_MExpAmount=30;
  m_data[4].m_AExpAmount=10;
}

void DIFFICULTY_LEVEL_DATA::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
  {
    ar << m_defaultLvl;
  }
  else
  {
    ar >> m_defaultLvl;
  }
  for (int i=0;i<NUM_DIFF_LVLS;i++)
    m_data[i].Serialize(ar);
}

void DIFFICULTY_LEVEL_DATA::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    ar << m_defaultLvl;
  }
  else
  {
    ar >> m_defaultLvl;
  }
  for (int i=0;i<NUM_DIFF_LVLS;i++)
    m_data[i].Serialize(ar);
}

#ifdef UAFEDITOR
const char *JKEY_DEFAULTLEVEL = "defaultLevel";
const char *JKEY_DATA = "data";
void DIFFICULTY_LEVEL_DATA::Export(JWriter& jw)
{
  jw.NameAndValue(JKEY_DEFAULTLEVEL, m_defaultLvl);
  jw.StartArray(JKEY_DATA);
  for (int i=0;i<NUM_DIFF_LVLS;i++)
  {
    jw.NextEntry();
    m_data[i].Export(jw);
  };
  jw.EndArray();
}
void DIFFICULTY_LEVEL_DATA::Import(JReader& jr)
{
  jr.NameAndValue(JKEY_DEFAULTLEVEL, m_defaultLvl);
  jr.StartArray(JKEY_DATA);
  for (int i=0;i<NUM_DIFF_LVLS;i++)
  {
    jr.NextEntry();
    m_data[i].Import(jr);
  };
  jr.EndArray();
}
#endif


DIFFICULTY_LEVEL_DATA& DIFFICULTY_LEVEL_DATA::operator=(const DIFFICULTY_LEVEL_DATA& src)
{
  if (&src==this) return *this;
  Clear();
  m_defaultLvl=src.m_defaultLvl;
  for (int i=0;i<NUM_DIFF_LVLS;i++)
    m_data[i]=src.m_data[i];
  return *this;
}

//*****************************************************************************

GLOBAL_SOUND_DATA::GLOBAL_SOUND_DATA() : hPartyBump(-1),hPartyStep(-1),hDeathMusic(-1),
                        /*hIntroMusic(-1),*/ hCharHit(-1), hCharMiss(-1)
{  
  ClearSounds();
  PartyBump="";PartyStep="";DeathMusic="";IntroMusic.Clear(); CreditsMusic.Clear();
  CharHit="";CharMiss="";
  CampMusic.Clear();
}

void GLOBAL_SOUND_DATA::Clear(BOOL ctor) 
{ 
  ClearSounds();
  PartyBump="";PartyStep="";DeathMusic="";IntroMusic.Clear(); CreditsMusic.Clear();
  CharHit="";CharMiss="";
  CampMusic.Clear();

#if (defined UAFEDITOR)
  if (!ctor)
  {
    CString tmp;
    CharHit    = /*ede.TemplateSoundDir() + */DEF_HIT_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_HIT_SOUND);
    CharMiss   = /*ede.TemplateSoundDir() + */DEF_MISS_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_MISS_SOUND);
    PartyBump  = /*ede.TemplateSoundDir() + */DEF_BUMP_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_BUMP_SOUND);
    PartyStep  = /*ede.TemplateSoundDir() + */DEF_STEP_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_STEP_SOUND);
    DeathMusic = /*ede.TemplateSoundDir() + */DEF_DEATH_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_DEATH_SOUND);
    //IntroMusic.Format("%s%s", global_editorResourceDir, DEF_INTRO_SOUND);
    tmp        = /*ede.TemplateSoundDir() + */DEF_INTRO_SOUND;//.Format("%s%s", global_editorResourceDir, DEF_INTRO_SOUND);
    IntroMusic.sounds.AddHead(tmp);
  }
#endif
}

void GLOBAL_SOUND_DATA::ClearSounds()
{
  if (hPartyBump >= 0)
    ClearSound(&hPartyBump);
  if (hPartyStep >= 0)
    ClearSound(&hPartyStep);
  if (hDeathMusic >= 0)
    ClearSound(&hDeathMusic);
  //if (hIntroMusic >= 0)
  //  ClearSound(hIntroMusic);
  if (hCharHit >= 0)
    ClearSound(&hCharHit);
  if (hCharMiss >= 0)
    ClearSound(&hCharMiss);

  hPartyBump=-1; hPartyStep=-1;hDeathMusic=-1;//hIntroMusic=-1;
  hCharHit=-1;hCharMiss=-1;
}

void GLOBAL_SOUND_DATA::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  if (ar.IsStoring())
  {
    //StripFilenamePath(CharHit);
    //StripFilenamePath(CharMiss);
    //StripFilenamePath(PartyBump);
    //StripFilenamePath(PartyStep);
    //StripFilenamePath(DeathMusic);
    //StripFilenamePath(IntroMusic);
    AS(ar, CharHit);
    AS(ar, CharMiss);
    AS(ar, PartyBump);
    AS(ar, PartyStep);
    AS(ar, DeathMusic);
    IntroMusic.Serialize(ar);
    CreditsMusic.Serialize(ar);
    CampMusic.Serialize(ar);

    //AddFolderToPath(CharHit,    rte.SoundDir());
    //AddFolderToPath(CharMiss,   rte.SoundDir());
    //AddFolderToPath(PartyBump,  rte.SoundDir());
    //AddFolderToPath(PartyStep,  rte.SoundDir());
    //AddFolderToPath(DeathMusic, rte.SoundDir());
    //AddDesignFolderToPath(IntroMusic);
  }
  else
  {
    DAS(ar,CharHit);
    DAS(ar,CharMiss);
    DAS(ar,PartyBump);
    DAS(ar,PartyStep);
    DAS(ar,DeathMusic);

    IntroMusic.Clear();
    if (globalData.version < _VERSION_0710_)
    {
      CString tmp;
      ar >> tmp;
      //AddFolderToPath(tmp, rte.SoundDir());
      IntroMusic.sounds.AddHead(tmp);
    }
    else
      IntroMusic.Serialize(ar);

    CreditsMusic.Clear();
    if (globalData.version < _VERSION_525)
    {
        CString tmp;
        ar >> tmp;
        CreditsMusic.sounds.AddHead(tmp);
    }
    else
        CreditsMusic.Serialize(ar);

    CampMusic.Clear();
    if (globalData.version >= _VERSION_0910_)
    { 
      CampMusic.Serialize(ar);
    }

    //AddFolderToPath(CharHit,    rte.SoundDir());
    //AddFolderToPath(CharMiss,   rte.SoundDir());
    //AddFolderToPath(PartyBump,  rte.SoundDir());
    //AddFolderToPath(PartyStep,  rte.SoundDir());
    //AddFolderToPath(DeathMusic, rte.SoundDir());
    //AddDesignFolderToPath(IntroMusic);
  }
}

void GLOBAL_SOUND_DATA::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
#ifdef UAFEDITOR
    StripFilenamePath(CharHit);
    StripFilenamePath(CharMiss);
    StripFilenamePath(PartyBump);
    StripFilenamePath(PartyStep);
    StripFilenamePath(DeathMusic);
    //StripFilenamePath(IntroMusic);
#endif
    AS(ar, CharHit);
    AS(ar, CharMiss);
    AS(ar, PartyBump);
    AS(ar, PartyStep);
    AS(ar, DeathMusic);


    IntroMusic.Serialize(ar);
    CreditsMusic.Serialize(ar);
    CampMusic.Serialize(ar);

    //AddFolderToPath(CharHit,    rte.SoundDir());
    //AddFolderToPath(CharMiss,   rte.SoundDir());
    //AddFolderToPath(PartyBump,  rte.SoundDir());
    //AddFolderToPath(PartyStep,  rte.SoundDir());
    //AddFolderToPath(DeathMusic, rte.SoundDir());
    //AddDesignFolderToPath(IntroMusic);
  }
  else
  {
    DAS(ar,CharHit);
    DAS(ar,CharMiss);
    DAS(ar,PartyBump);
    DAS(ar,PartyStep);
    DAS(ar,DeathMusic);

    //AddFolderToPath(CharHit, rte.SoundDir());
    //AddFolderToPath(CharMiss, rte.SoundDir());
    //AddFolderToPath(PartyBump, rte.SoundDir());
    //AddFolderToPath(PartyStep, rte.SoundDir());
    //AddFolderToPath(DeathMusic, rte.SoundDir());


    IntroMusic.Clear(); 
    if (globalData.version < _VERSION_0710_)
    {
      CString tmp;
      ar >> tmp;
      //AddFolderToPath(tmp, rte.SoundDir());
      IntroMusic.sounds.AddHead(tmp);
    }
    else
        IntroMusic.Serialize(ar);

    CreditsMusic.Clear();
    if (globalData.version < _VERSION_525)
    {
        //Do nothing because no outro music existing in prior versions - not even a default one
    }
    else
        CreditsMusic.Serialize(ar);

    CampMusic.Clear();
    if (globalData.version >= _VERSION_0910_)
    {
      CampMusic.Serialize(ar);
    }

    //AddFolderToPath(CharHit,    rte.SoundDir());
    //AddFolderToPath(CharMiss,   rte.SoundDir());
    //AddFolderToPath(PartyBump,  rte.SoundDir());
    //AddFolderToPath(PartyStep,  rte.SoundDir());
    //AddFolderToPath(DeathMusic, rte.SoundDir());
    //AddDesignFolderToPath(IntroMusic);
  }
}

#ifdef UAFEDITOR

const char *JKEY_CHARHIT = "charHit";
const char *JKEY_CHARMISS = "charMiss";
const char *JKEY_PARTYBUMP = "partyBump";
const char *JKEY_PARTYSTEP = "partyStep";
const char *JKEY_DEATHMUSIC = "deathMusic";
const char *JKEY_INTROMUSIC = "introMusic";
const char* JKEY_CREDITSMUSIC = "creditsMusic";
const char *JKEY_CAMPMUSIC = "campMusic";

void GLOBAL_SOUND_DATA::Export(JWriter& jw)
{
  jw.NameAndValue(JKEY_CHARHIT, CharHit);
  jw.NameAndValue(JKEY_CHARMISS, CharMiss);
  jw.NameAndValue(JKEY_PARTYBUMP, PartyBump);
  jw.NameAndValue(JKEY_PARTYSTEP, PartyStep);
  jw.NameAndValue(JKEY_DEATHMUSIC, DeathMusic);
  IntroMusic.Export(jw, JKEY_INTROMUSIC);
  CreditsMusic.Export(jw, JKEY_CREDITSMUSIC);
  CampMusic.Export(jw, JKEY_CAMPMUSIC);
}

void GLOBAL_SOUND_DATA::Import(JReader& jr)
{
  jr.NameAndValue(JKEY_CHARHIT, CharHit);
  jr.NameAndValue(JKEY_CHARMISS, CharMiss);
  jr.NameAndValue(JKEY_PARTYBUMP, PartyBump);
  jr.NameAndValue(JKEY_PARTYSTEP, PartyStep);
  jr.NameAndValue(JKEY_DEATHMUSIC, DeathMusic);
  IntroMusic.Import(jr, JKEY_INTROMUSIC);
  CreditsMusic.Import(jr, JKEY_CREDITSMUSIC);
  CampMusic.Import(jr, JKEY_CAMPMUSIC);
}



void GLOBAL_SOUND_DATA::CrossReference(CR_LIST *pCRList)
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceType = CR_TYPE_global;
  CRReference.m_referenceName = "Global Sounds";
  pCRList->CR_AddSoundReference(CharHit, &CRReference);
  pCRList->CR_AddSoundReference(CharMiss, &CRReference);
  pCRList->CR_AddSoundReference(PartyBump, &CRReference);
  pCRList->CR_AddSoundReference(PartyStep, &CRReference);
  pCRList->CR_AddSoundReference(DeathMusic, &CRReference);
  IntroMusic.CrossReference(pCRList, &CRReference);
  CreditsMusic.CrossReference(pCRList, &CRReference);
  CampMusic.CrossReference(pCRList, &CRReference);
}
#endif
  

GLOBAL_SOUND_DATA& GLOBAL_SOUND_DATA::operator =(const GLOBAL_SOUND_DATA& src)
{
  if (&src == this)
    return *this;
  hCharHit = src.hCharHit;
  hCharMiss = src.hCharMiss;
  hPartyBump = src.hPartyBump;
  hPartyStep = src.hPartyStep;
  hDeathMusic = src.hDeathMusic;
  //hIntroMusic = src.hIntroMusic;

  CharHit = src.CharHit;
  CharMiss = src.CharMiss;
  PartyBump = src.PartyBump;
  PartyStep = src.PartyStep;
  DeathMusic = src.DeathMusic;
  IntroMusic = src.IntroMusic;
  CreditsMusic = src.CreditsMusic;
  CampMusic = src.CampMusic;
  return *this;
}

//*****************************************************************************
#ifdef UAFEDITOR
CString SPECIAL_OBJECT_DATA::GetSpecialObjectText()
{
  CString tmp;
  tmp.Format("Special Item Event: %s", name);
  return tmp;
}
#endif


void SPECIAL_OBJECT_DATA::Save(CAR& car) const
{
	obj_asl.Save(car, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
}

void SPECIAL_OBJECT_DATA::PrepareForRestore(void) 
{
	temp_asl.Clear();
}

void SPECIAL_OBJECT_DATA::Restore(CArchive& ar) 
{
  temp_asl.Serialize(ar, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
}

void SPECIAL_OBJECT_DATA::Restore(CAR& car) 
{
  temp_asl.Serialize(car, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
}

void SPECIAL_OBJECT_DATA::CommitRestore(void) 
{
	obj_asl.CommitRestore(&temp_asl);
	temp_asl.Clear();
}

void SPECIAL_OBJECT_DATA::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);
  if (ar.IsStoring())
  {
    AS(ar,name);
    ar << id;
    ar << stage;
    ar << ExamineEvent;
    AS(ar, ExamineLabel);
    ar << CanBeDropped;
  }
  else
  {
    DAS(ar,name);
    ar >> id;
    ar >> stage;
    if (ver >= _VERSION_0810_)
    {
      ar >> ExamineEvent;
      DAS(ar,ExamineLabel);
    }
    if (ver >= _VERSION_0830_)
      ar >> CanBeDropped;
  }
  obj_asl.Serialize(ar, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
}

void SPECIAL_OBJECT_DATA::Serialize(CAR &ar, double ver)
{
  if (ar.IsStoring())
  {
    AS(ar,name);
    ar << id;
    ar << stage;
    ar << ExamineEvent;
    AS(ar, ExamineLabel);
    ar << CanBeDropped;
  }
  else
  {
    DAS(ar,name);
    ar >> id;
    ar >> stage;
    if (ver >= _VERSION_0810_)
    {
      ar >> ExamineEvent;
      DAS(ar,ExamineLabel);
    }
    if (ver >= _VERSION_0830_)
      ar >> CanBeDropped;
  }
  obj_asl.Serialize(ar, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
}

#ifdef UAFEDITOR

extern const char *JKEY_EXAMINEEVENT;
extern const char *JKEY_EXAMINELABEL;
const char *JKEY_CANBEDROPPED = "canBeDropped";
const char *JKEY_STAGE        = "stage";
extern const char *JKEY_ID;

void SPECIAL_OBJECT_DATA::Export(JWriter& jw)
{

    //AS(ar,name);
  jw.StartList();
  jw.NameAndValue(JKEY_NAME, name);
    //ar << id;
  jw.NameAndValue(JKEY_ID, id);
    //ar << stage;
  jw.NameAndValue(JKEY_STAGE, stage);
    //ar << ExamineEvent;
  jw.NameAndValue(JKEY_EXAMINEEVENT, ExamineEvent);
    //AS(ar, ExamineLabel);
  jw.NameAndValue(JKEY_EXAMINELABEL, ExamineLabel);
    //ar << CanBeDropped;
  jw.NameAndValue(JKEY_CANBEDROPPED, CanBeDropped);
    //obj_asl.Serialize(ar, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
  obj_asl.Export(jw);
  jw.EndList();
}
void SPECIAL_OBJECT_DATA::Import(JReader& jr)
{
  jr.StartList();
    //AS(ar,name);
  jr.NameAndValue(JKEY_NAME, name);
    //ar << id;
  jr.NameAndValue(JKEY_ID, id);
    //ar << stage;
  jr.NameAndValue(JKEY_STAGE, stage);
    //ar << ExamineEvent;
  jr.NameAndValue(JKEY_EXAMINEEVENT, ExamineEvent);
    //AS(ar, ExamineLabel);
  jr.NameAndValue(JKEY_EXAMINELABEL, ExamineLabel);
    //ar << CanBeDropped;
  jr.NameAndValue(JKEY_CANBEDROPPED, CanBeDropped);
    //obj_asl.Serialize(ar, "SPECIAL_OBJECT_DATA_ATTRIBUTES");
  obj_asl.Import(jr);
  jr.EndList();
}
#endif


SPECIAL_OBJECT_DATA& SPECIAL_OBJECT_DATA::operator =(const SPECIAL_OBJECT_DATA& src)
{
  if (&src == this)
    return *this;
  name = src.name;
  id = src.id;
  stage = src.stage;
  ExamineEvent=src.ExamineEvent;
  ExamineLabel=src.ExamineLabel;
  CanBeDropped=src.CanBeDropped;
  obj_asl.Copy(src.obj_asl);
  temp_asl.Copy(src.temp_asl);
  return *this;
}

//*****************************************************************************
int SPECIAL_OBJECT_LIST::GetNextKey()
{
  int key=0;
  if (GetCount() > 0)
  {
    key = items.GetTail().id;
    if (key >= INT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_MONSTERS.
      // Gaps occur when deletions are made to the monster list
      POSITION pos = items.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = items.GetNext(pos).id;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

BOOL SPECIAL_OBJECT_LIST::CanBeDropped(int index) const
{
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    return items.PeekAtPos(pos).CanBeDropped;
  else
    return FALSE;  
}

void SPECIAL_OBJECT_LIST::SetCanBeDropped(int index, BOOL val)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    items.GetAtPos(pos).CanBeDropped=val;
}

CString SPECIAL_OBJECT_LIST::GetExamineLabel(int index) const
{
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    return items.PeekAtPos(pos).ExamineLabel;
  else
    return "";
}

DWORD SPECIAL_OBJECT_LIST::GetExamineEvent(int index) const
{
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    return items.PeekAtPos(pos).ExamineEvent;
  else
    return 0;
}

void SPECIAL_OBJECT_LIST::SetExamineLabel(int index, CString label)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    items.GetAtPos(pos).ExamineLabel = label;
}

void SPECIAL_OBJECT_LIST::SetExamineEvent(int index, DWORD event)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    items.GetAtPos(pos).ExamineEvent = event;
}

BOOL SPECIAL_OBJECT_LIST::DeleteSpecialObjectEvents(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {
    globalData.eventData.DeleteEvents(items.PeekAtPos(pos).ExamineEvent);
    items.GetAtPos(pos).ExamineLabel="EXAMINE";
    return TRUE;
  }
  else
    return FALSE;
}


void SPECIAL_OBJECT_LIST::Save(CAR& car) const
{
	int count;
	POSITION so_pos;
	count=items.GetCount();
	car << count;
	so_pos=items.GetHeadPosition();
	while (so_pos!=NULL) 
	{
		car << items.PeekAtPos(so_pos).name;
		items.PeekAtPos(so_pos).Save(car);
		items.PeekNext(so_pos);
	};
}

void SPECIAL_OBJECT_LIST::Restore(CArchive& ar) 
{
	int count;
	CString name;
	if (globalData.version<_ASL_LEVEL_) return;
	POSITION pos=items.GetHeadPosition();
  // Let items know they will be asked to CommitRestore.  They
	// can make a copy of their data or otherwise note that 
	// if no Restore comes along they do something special.
	// Probably will want to clear their temporary copy of
	// ATTRIBUTES in case no restore operation clears it.
	while (pos!=NULL) items.GetNext(pos).PrepareForRestore();
	ar >> count;
	for (int i=0; i<count; i++) {
		ar >> name;
		pos=items.GetHeadPosition();
		while (pos!=NULL)
		{
			if (name==items.PeekAtPos(pos).name)
			{
				items.GetAtPos(pos).Restore(ar);
				break;
			};
			items.GetNext(pos);
		}
		if (pos==NULL)
		{
			SPECIAL_OBJECT_DATA junk;
			junk.Restore(ar); // discard
			CString msg;
			msg.Format("SAVE-FILE SPECIAL_OBJECT_DATA \"%s\" is not in design\n",
				name);
			WriteDebugString(msg);
		};
	};
}

void SPECIAL_OBJECT_LIST::Restore(CAR& car) 
{
	int count;
	CString name;
	POSITION pos=items.GetHeadPosition();
  // Let items know they will be asked to CommitRestore.  They
	// can make a copy of their data or otherwise note that 
	// if no Restore comes along they do something special.
	// Probably will want to clear their temporary copy of
	// ATTRIBUTES in case no restore operation clears it.
	while (pos!=NULL) items.GetNext(pos).PrepareForRestore();
	car >> count;
	for (int i=0; i<count; i++) {
		car >> name;
		pos=items.GetHeadPosition();
		while (pos!=NULL)
		{
			if (name==items.PeekAtPos(pos).name)
			{
				items.GetAtPos(pos).Restore(car);
				break;
			};
			items.GetNext(pos);
		}
		if (pos==NULL)
		{
			SPECIAL_OBJECT_DATA junk;
			junk.Restore(car); // discard
			CString msg;
			msg.Format("SAVE-FILE SPECIAL_OBJECT_DATA \"%s\" is not in design\n",
				name);
			WriteDebugString(msg);
		};
	};
}

void SPECIAL_OBJECT_LIST::CommitRestore(void) {
	POSITION p;
	p=items.GetHeadPosition();
	while (p!=NULL) items.GetNext(p).CommitRestore();
}

void SPECIAL_OBJECT_LIST::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);

  if (ar.IsStoring())
  {      
    ar << GetCount();
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
      items.GetNext(pos).Serialize(ar, ver);
  }
  else
  {
    Clear();
    SPECIAL_OBJECT_DATA data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      AddItemWithCurrKey(data);
    }
  }
}

void SPECIAL_OBJECT_LIST::Serialize(CAR &ar, double ver)
{
  if (ar.IsStoring())
  {      
    ar << GetCount();
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
      items.GetNext(pos).Serialize(ar, ver);
  }
  else
  {
    Clear();
    SPECIAL_OBJECT_DATA data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      AddItemWithCurrKey(data);
    }
  }
}

#ifdef UAFEDITOR

//const char *JKEY_SPECIALOBJECTS = "specialObjects";
void SPECIAL_OBJECT_LIST::Export(JWriter& jw, const char *type)
{
  POSITION pos = GetHeadPosition();
  jw.StartArray(type);
  while (pos != NULL)
  {
    jw.NextEntry();
    items.GetNext(pos).Export(jw);
  };
  jw.EndArray();
}

void SPECIAL_OBJECT_LIST::Import(JReader& jr, const char *type)
{
  jr.StartArray(type);
  while (jr.Optional(), jr.NextEntry())
  {
    SPECIAL_OBJECT_DATA item;
    item.Import(jr);
    AddItem(item);
  };
  jr.EndArray();
}




void SPECIAL_OBJECT_LIST::CrossReference(CR_LIST *pCRList, CR_TYPE crType)
{
  POSITION pos;
  for (pos = items.GetHeadPosition(); pos!=NULL; items.GetNext(pos))
  {
    const SPECIAL_OBJECT_DATA *p;
    p = &items.PeekAtPos(pos);
    pCRList->CR_AddResource(crType, p->name, p->id);
  };
}

#endif

SPECIAL_OBJECT_LIST& SPECIAL_OBJECT_LIST::operator=(const SPECIAL_OBJECT_LIST& src)
{    
  if (&src == this)
    return *this;
  Clear();
  m_Max = const_cast<SPECIAL_OBJECT_LIST&>(src).m_Max;
  POSITION pos = const_cast<SPECIAL_OBJECT_LIST&>(src).GetHeadPosition();
  while (pos != NULL)
    AddItemWithCurrKey(const_cast<SPECIAL_OBJECT_LIST&>(src).GetNext(pos));
  return *this;
}  

BOOL SPECIAL_OBJECT_LIST::HaveItem(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  return (items.FindKeyPos(index) != NULL);
}

const CString SPECIAL_OBJECT_LIST::GetName(const int index) const
{
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    return items.PeekAtPos(pos).name;
  else
    return "No Item";
}

void SPECIAL_OBJECT_LIST::SetName(const int index, const CString &name)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
    items.GetAtPos(pos).name = name;
}

// called when loading from scripts, or
// when adding from editor GUI
int SPECIAL_OBJECT_LIST::AddItem(SPECIAL_OBJECT_DATA &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  if (items.GetCount() < m_Max)
  {
    data.id = GetNextKey();
    items.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

// called when loading binary monster data that
// was saved with key info
int SPECIAL_OBJECT_LIST::AddItemWithCurrKey(const SPECIAL_OBJECT_DATA &data)
{
  if (items.GetCount() < m_Max)
  {
    items.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

BOOL SPECIAL_OBJECT_LIST::DeleteItem(const int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {
    items.RemoveAt(pos);
    return TRUE;
  }
  else
    return FALSE;
}

BOOL SPECIAL_OBJECT_LIST::GetItem(const int index, SPECIAL_OBJECT_DATA &item) 
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {
    item = items.PeekAtPos(pos);
    return TRUE;
  }
  return FALSE;
}

BOOL SPECIAL_OBJECT_LIST::SetItem(const int index, const SPECIAL_OBJECT_DATA &item)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {
    items.GetAtPos(pos) = item;
    return TRUE;
  }
  return FALSE;
}

WORD SPECIAL_OBJECT_LIST::GetStage(const int index) const
{
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {      
    return items.PeekAtPos(pos).stage;
  }
  return 0;
}

void SPECIAL_OBJECT_LIST::SetStage(const int index, const WORD stage)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = items.FindKeyPos(index)) != NULL)
  {      
    items.GetAtPos(pos).stage = stage;
  }
}

int SPECIAL_OBJECT_LIST::GetItemKeyUsingSequence(int SeqNum) const
{
  int count = 0;
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
  {
    count++;
    if (count == SeqNum)
      return (PeekAtPos(pos).id);
    PeekNext(pos);
  }
  return 0;
}

//*****************************************************************************

void QUEST_DATA::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);
  if (ar.IsStoring())
  {
    AS(ar,name);
    ar << (int)state;
    ar << stage;
    ar << id;
  }
  else
  {
    DAS(ar,name);
    int temp;
    ar >> temp;
    state = (QuestStateType)temp;
    ar >> stage;
    ar >> id;
  }
  quest_asl.Serialize(ar, "QUEST_DATA_ATTRIBUTES");
}

void QUEST_DATA::Serialize(CAR &ar, double ver)
{
  if (ar.IsStoring())
  {
    AS(ar,name);
    ar << (int)state;
    ar << stage;
    ar << id;
  }
  else
  {
    DAS(ar,name);
    int temp;
    ar >> temp;
    state = (QuestStateType)temp;
    ar >> stage;
    ar >> id;
  }
  quest_asl.Serialize(ar, "QUEST_DATA_ATTRIBUTES");
}

#ifdef UAFEDITOR

const char *JKEY_QUESTS = "quests";
extern const char *JKEY_NAME;
const char *JKEY_STATE = "state";
extern const char *JKEY_ID;

void QUEST_DATA::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_NAME, name);
  jw.NameAndEnum(JKEY_STATE, state);
  jw.NameAndValue(JKEY_STAGE, stage);;
  jw.NameAndValue(JKEY_ID, id);
  quest_asl.Export(jw);
  jw.EndList();
}
void QUEST_DATA::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_NAME, name);
  jr.NameAndEnum(JKEY_STATE, state);
  jr.NameAndValue(JKEY_STAGE, stage);;
  jr.NameAndValue(JKEY_ID, id);
  quest_asl.Import(jr);
  jr.EndList();
}
#endif

QUEST_DATA& QUEST_DATA::operator=(const QUEST_DATA& src)
{
  if (&src == this)
    return *this;
  name = src.name;
  state = src.state;
  stage = src.stage;
  id = src.id;
  quest_asl.Copy(src.quest_asl);
  return *this;
}

//*****************************************************************************

int QUEST_LIST::GetNextKey()
{
  int key=0;
  if (GetCount() > 0)
  {
    key = quests.GetTail().id;
    if (key >= INT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_MONSTERS.
      // Gaps occur when deletions are made to the monster list
      POSITION pos = quests.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = 0;
      int currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = quests.GetNext(pos).id;
        if (currKey != prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          prevKey = currKey; 
      }
      VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = 1;
  return key;
}

QUEST_LIST& QUEST_LIST::operator=(const QUEST_LIST& src)
{
  if (&src == this)
    return *this;
  Clear();
  POSITION pos = const_cast<QUEST_LIST&>(src).GetHeadPosition();
  while (pos != NULL)
    AddQuestWithCurrKey(const_cast<QUEST_LIST&>(src).GetNext(pos));
  return *this;
}

void QUEST_LIST::Reset() 
{
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
    quests.GetNext(pos).Reset();
}

void QUEST_LIST::Serialize(CArchive &ar, double ver)
{
//  CObject::Serialize(ar);

  if (ar.IsStoring())
  {      
    ar << GetCount();
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
      quests.GetNext(pos).Serialize(ar, ver);
  }
  else
  {
    Clear();
    QUEST_DATA data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      AddQuestWithCurrKey(data);
    }
  }
}

void QUEST_LIST::Serialize(CAR &ar, double ver)
{
  if (ar.IsStoring())
  {      
    ar << GetCount();
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
      quests.GetNext(pos).Serialize(ar, ver);
  }
  else
  {
    Clear();
    QUEST_DATA data;
    int count;
    ar >> count;
    for (int i=0;i<count;i++)
    {
      data.Clear();
      data.Serialize(ar, ver);
      AddQuestWithCurrKey(data);
    }
  }
}

#ifdef UAFEDITOR

//const char *JKEY_QUESTS = "quests";
void QUEST_LIST::Export(JWriter& jw)
{
  POSITION pos = GetHeadPosition();
  jw.StartArray(JKEY_QUESTS);
  while (pos != NULL)
  {
    jw.NextEntry();
    quests.GetNext(pos).Export(jw);
  };
  jw.EndArray();
}
void QUEST_LIST::Import(JReader& jr)
{
  jr.StartArray(JKEY_QUESTS);
  while (jr.Optional(), jr.NextEntry())
  {
    QUEST_DATA questData;
    questData.Import(jr);
    quests.Insert(questData, questData.id);
  };
  jr.EndArray();
}
#endif


BOOL QUEST_LIST::HaveQuest(int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  return (quests.FindKeyPos(index) != NULL);
}

const CString QUEST_LIST::GetName(const int id) const
{
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return quests.PeekAtPos(pos).name;
  else
    return "No Quest";
}

void QUEST_LIST::SetName(const int id, const CString &name)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    quests.GetAtPos(pos).name = name;
}

WORD QUEST_LIST::GetStage(const int id) const
{
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return quests.PeekAtPos(pos).stage;
  else
    return MAX_QUEST_STAGE+1;
}

int QUEST_LIST::FindQuest(const CString& questName) const
{
  POSITION pos = GetHeadPosition();
  while (pos != NULL)
  {
    if (quests.PeekAtPos(pos).name == questName)
    {
      return quests.PeekAtPos(pos).id;
    };
    quests.PeekNext(pos);
  };
  return -1;
}

WORD QUEST_LIST::GetStage(const CString& questName) const
{
  return GetStage(FindQuest(questName));
}


void QUEST_LIST::SetStage(const int id, const WORD stage)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    WORD temp = min(stage, MAX_QUEST_STAGE);
    quests.GetAtPos(pos).stage = temp;

    if (quests.PeekAtPos(pos).stage > MAX_QUEST_STAGE)
      quests.GetAtPos(pos).stage = MAX_QUEST_STAGE;
    if (quests.PeekAtPos(pos).stage < 0)
      quests.GetAtPos(pos).stage = 0;

    if (quests.PeekAtPos(pos).stage == QUEST_COMPLETED_STAGE)
      SetComplete(id);
    else if (quests.PeekAtPos(pos).stage == QUEST_FAILED_STAGE)
      SetFailed(id);
    else if (quests.PeekAtPos(pos).stage == 0)
      SetNotStarted(id);
    else
      SetInProgress(id);
  }
  WriteDebugString("Quest %u, SetStage(%u)\n", id,stage);
}

void QUEST_LIST::SetStage(const CString& questName, const WORD stage)
{
  SetStage(FindQuest(questName), stage);
}

// cannot add to a quest and make it be failed
void QUEST_LIST::IncStage(const int id, const WORD value)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    if (quests.PeekAtPos(pos).stage < QUEST_COMPLETED_STAGE)
    {
      long result = quests.PeekAtPos(pos).stage + value;
      //quests.GetAt(pos).stage = quests.GetAt(pos).stage += value;
      if (result > QUEST_COMPLETED_STAGE) result = QUEST_COMPLETED_STAGE;
      if (result < 0) result = 0;
      quests.GetAtPos(pos).stage = (WORD)result;

      if (quests.PeekAtPos(pos).stage == QUEST_COMPLETED_STAGE)
        SetComplete(id);
      else if (quests.PeekAtPos(pos).stage == QUEST_FAILED_STAGE)
        SetFailed(id); // shouldn't get here
      else if (quests.PeekAtPos(pos).stage == 0)
        quests.GetAtPos(pos).state = QS_NotStarted;
      else
        SetInProgress(id);

      WriteDebugString("Quest %u, IncStage()=%u\n", id, quests.PeekAtPos(pos).stage);
    }
  }
}

BOOL QUEST_LIST::IsPresent(const int id) const
{
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return (quests.PeekAtPos(pos).state != QS_NotStarted);
  else
    return FALSE;
}

BOOL QUEST_LIST::IsInProgress(const int id) const
{ 
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return (quests.PeekAtPos(pos).state == QS_InProg);
  else
    return FALSE;
}

BOOL QUEST_LIST::IsComplete(const int id) const
{ 
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return (quests.PeekAtPos(pos).state == QS_Complete);
  else
    return FALSE;
}

BOOL QUEST_LIST::IsFailed(const int id) const
{ 
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return (quests.PeekAtPos(pos).state == QS_Failed);
  else
    return FALSE;
}

BOOL QUEST_LIST::StageEqual(const int id, const int stage) const
{
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
    return (quests.PeekAtPos(pos).stage == stage);
  else
    return FALSE;
}

void QUEST_LIST::SetInProgress(const int id) 
{ 
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quests.GetAtPos(pos).state = QS_InProg;
    if (quests.PeekAtPos(pos).stage == 0)
      quests.GetAtPos(pos).stage = 1;
    if (quests.PeekAtPos(pos).stage >= QUEST_COMPLETED_STAGE)
      quests.GetAtPos(pos).stage = 1;
    if (quests.PeekAtPos(pos).stage == QUEST_FAILED_STAGE)
      quests.GetAtPos(pos).stage = 1;
  }
  WriteDebugString("Quest %u, SetInProgress()\n", id);
}

void QUEST_LIST::SetComplete(const int id) 
{ 
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quests.GetAtPos(pos).stage = QUEST_COMPLETED_STAGE;
    quests.GetAtPos(pos).state = QS_Complete;
  }
  WriteDebugString("Quest %u, SetComplete()\n", id);
}

void QUEST_LIST::SetFailed(const int id) 
{ 
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quests.GetAtPos(pos).stage = QUEST_FAILED_STAGE;
    quests.GetAtPos(pos).state = QS_Failed;
  }
  WriteDebugString("Quest %u, SetFailed()\n", id);
}

void QUEST_LIST::SetNotStarted(const int id) 
{ 
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quests.GetAtPos(pos).stage = 0;
    quests.GetAtPos(pos).state = QS_NotStarted;
  }
  WriteDebugString("Quest %u, SetNotStarted()\n", id);
}

// called when loading from scripts, or
// when adding from editor GUI
int QUEST_LIST::AddQuest(QUEST_DATA &data)
{
  if (quests.GetCount() < MAX_QUESTS)
  {
    data.id = GetNextKey();
    quests.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

// called when loading binary monster data that
// was saved with key info
int QUEST_LIST::AddQuestWithCurrKey(const QUEST_DATA &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  if (quests.GetCount() < MAX_QUESTS)
  {
    quests.Insert(data, data.id);
    return data.id;
  }
  else
    return 0;
}

BOOL QUEST_LIST::DeleteQuest(const int index)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(index)) != NULL)
  {
    quests.RemoveAt(pos);
    return TRUE;
  }
  else
    return FALSE;
}

BOOL QUEST_LIST::GetQuest(const int id, QUEST_DATA &quest) 
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quest = quests.GetAtPos(pos);
    return TRUE;
  }
  return FALSE;
}

BOOL QUEST_LIST::SetQuest(const int id, const QUEST_DATA &quest)
{
//  CSingleLock sLock(&m_CS, TRUE);
  POSITION pos;
  if ((pos = quests.FindKeyPos(id)) != NULL)
  {
    quests.GetAtPos(pos) = quest;
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************

ROW_OVERRIDES::~ROW_OVERRIDES(void)
{
  if (m_cellOverrides != NULL)
  {
    free(m_cellOverrides);
    m_cellOverrides = NULL;
  };
  m_numCol = 0;
}

void ROW_OVERRIDES::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    ar << m_numCol;
    if (m_numCol > 0)
    {
      ar.Write(m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
    };
  }
  else
  {
    ar >> m_numCol;
    m_cellOverrides = (CELL_OVERRIDE *)malloc(m_numCol*sizeof(CELL_OVERRIDE));
    ar.Read(m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
  };
}

void ROW_OVERRIDES::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << m_numCol;
    if (m_numCol > 0)
    {
      //car.Write(m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
      car.Serialize((char *)m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
    };
  }
  else
  {
    car >> m_numCol;
    m_cellOverrides = (CELL_OVERRIDE *)malloc(m_numCol*sizeof(CELL_OVERRIDE));
    //car.Read(m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
    car.Serialize((char *)m_cellOverrides, m_numCol*sizeof(m_cellOverrides[0]));
  };
}


WALL_OVERRIDES::~WALL_OVERRIDES(void)
{
  Clear();
}

void WALL_OVERRIDES::Clear(void)
{
  if (m_numRow > 0)
  {
    int i;
    for (i=0; i<m_numRow; i++)
    {
      if (m_row[i] != NULL)
      {
        delete m_row[i];
        m_row[i] = NULL;
      };
    };
  };
  m_numRow = 0;
  if (m_row != NULL)
  {
    free(m_row);
    m_row = NULL;
  };
}

void WALL_OVERRIDES::operator = (const WALL_OVERRIDES& src)
{
  int i;
  Clear();
  if (src.m_numRow == 0) return;
  m_numRow = src.m_numRow;
  m_row = (ROW_OVERRIDES **)malloc(m_numRow*sizeof(m_row[0]));
  if (m_row == NULL) die(0xdee83);
  for (i=0; i<m_numRow; i++)
  {
    m_row[i] = new ROW_OVERRIDES;
    *(m_row[i]) = *(src.m_row[i]);
  };
}

/*
CELL_OVERRIDE *WALL_OVERRIDES::FindCellOverride(OVERRIDE_TYPE ovt, int x, int y, int facing)
{
  if (y >= m_numRow) return NULL;
  if (m_row[y] == NULL) return NULL;
  if (x >= m_row[y]->m_numCol) return NULL;
  if (ovt >= NUM_OVERRIDE_TYPE) return NULL;
  if (facing > 3) return NULL;
  return m_row[y]->m_cellOverrides + x;
}
*/

int LEVEL_STATS::GetMapOverride(OVERRIDE_TYPE ovt, int x, int y, int facing)
{
  int n, adj=0;
  CELL_OVERRIDE *pCellOverride;
  ROW_OVERRIDES *pRowOverride;
  switch (ovt)
  {
    case WALL_OVERRIDE_USER:
    case DOOR_OVERRIDE_USER:
    case BACKGROUND_OVERRIDE_USER:
    case OVERLAY_OVERRIDE_USER:
    case BLOCKAGE_OVERRIDE:
         break;
    case WALL_OVERRIDE_INDEX:
         adj = useWallIndex?0:1;
         ovt = WALL_OVERRIDE_USER;
         break;
    case DOOR_OVERRIDE_INDEX:
         adj = useDoorAndOverlayIndex?0:1;
         ovt = DOOR_OVERRIDE_USER;
         break;
    case BACKGROUND_OVERRIDE_INDEX:
         adj = useDoorAndOverlayIndex?0:1;
         ovt = BACKGROUND_OVERRIDE_USER;
         break;
    case OVERLAY_OVERRIDE_INDEX:
         adj = useDoorAndOverlayIndex?0:1;
         ovt = OVERLAY_OVERRIDE_USER;
         break;
    default:  die(0xd18a7);
  };
  if (area_height <= 0) return -1;
  y = y % area_height;
  if (y < 0) y = y + area_height;
  x = x % area_width;
  if (x < 0) x = x+area_width;
  facing = facing % 4;
  if (facing < 0) facing = facing + 4;

  if (y >= m_wallOverrides.m_numRow)
  {
    return -1;
  };
  if (m_wallOverrides.m_row[y] == NULL)
  {
    return -1;
  };
  pRowOverride = m_wallOverrides.m_row[y];
  if (x >= pRowOverride->m_numCol)
  {
    return -1;
  };
  pCellOverride = pRowOverride->m_cellOverrides + x;
  n = pCellOverride->m_overrides[ovt][facing];
  if (n == 255) return -1;
  
  return n - adj;  // conver 'user' value to 'index' value
}

void LEVEL_STATS::SetMapOverride(OVERRIDE_TYPE ovt, int x, int y, int facing, unsigned int value)
{
  CELL_OVERRIDE *pCellOverride;
  ROW_OVERRIDES *pRowOverride;
  int adj=0;
  switch (ovt)
  {
    case WALL_OVERRIDE_USER:
    case DOOR_OVERRIDE_USER:
    case BACKGROUND_OVERRIDE_USER:
    case OVERLAY_OVERRIDE_USER:
    case BLOCKAGE_OVERRIDE:
         break;
    case WALL_OVERRIDE_INDEX:
         adj = useWallIndex?0:1;
         ovt = WALL_OVERRIDE_USER;
         break;
    case DOOR_OVERRIDE_INDEX:
         adj = useDoorAndOverlayIndex?0:1;
         ovt = DOOR_OVERRIDE_USER;
         break;
    case BACKGROUND_OVERRIDE_INDEX:
         adj = useDoorAndOverlayIndex?0:1;
         ovt = BACKGROUND_OVERRIDE_USER;
         break;
    case OVERLAY_OVERRIDE_INDEX:
         adj = 1;
         ovt = OVERLAY_OVERRIDE_USER;
         break;
    default:  die(0xd18a7);
  };
  if (value > 255) value = 255;
  if (value != 255)
  {
    value = value+adj;  // convert 'index' value to 'user' value
  };
  if (ovt >= NUM_OVERRIDE_TYPE) return;
  if (area_height <= 0) return;

  y = y % area_height;
  if (y < 0) y = y + area_height;
  x = x % area_width;
  if (x < 0) x = x+area_width;
  facing = facing % 4;
  if (facing < 0) facing = facing + 4;

  if (y >= m_wallOverrides.m_numRow)
  {
    int i;
    if (value == 255) return;
    m_wallOverrides.m_row = (ROW_OVERRIDES **)realloc(m_wallOverrides.m_row,
                                                      (y+1)*sizeof(ROW_OVERRIDES *));
    if (m_wallOverrides.m_row == NULL)
    {
#ifdef UAFEngine
      die(0xae56b8);
#else
      exit(1);
#endif
    };
    for (i=m_wallOverrides.m_numRow; i<y+1; i++)
    {
      m_wallOverrides.m_row[i] = NULL;
    };
    m_wallOverrides.m_numRow = y+1;
  };
  if (m_wallOverrides.m_row[y] == NULL)
  {
    if (value == 255) return;
    m_wallOverrides.m_row[y] = new ROW_OVERRIDES;
  };
  pRowOverride = m_wallOverrides.m_row[y];
  if (x >= pRowOverride->m_numCol)
  {
    int i;
    if (value == 255) return;
    pRowOverride->m_cellOverrides = (CELL_OVERRIDE *)realloc(pRowOverride->m_cellOverrides,
                                                              (x+1)*sizeof(CELL_OVERRIDE));
    for (i=pRowOverride->m_numCol; i<x+1; i++)
    {
      pRowOverride->m_cellOverrides[i].Clear();
    };
    pRowOverride->m_numCol = x+1;
  };
  pCellOverride = pRowOverride->m_cellOverrides + x;
  pCellOverride->m_overrides[ovt][facing] = value;
}



int GetMapOverride(OVERRIDE_TYPE ovt, int *parameters)
{
  // Notice that parameter[0] is the level starting with level 1!!
  // There is no level 0.  The first level is level 1.
  int n;
  if (parameters[0] < 1) return -1;
  if (parameters[0] > MAX_LEVELS) return -1;
  n = globalData.levelInfo.stats[parameters[0]-1].GetMapOverride
                                                          (ovt, 
                                                           parameters[1],  // x
                                                           parameters[2],  // y
                                                           parameters[3]   // facing
                                                          );
  if (n < 0) return 255;
  return n;
}

void SetMapOverride(OVERRIDE_TYPE ovt, int *parameters)
{
  // Notice that parameter[0] is the level starting with level 1!!
  // There is no level 0.  The first level is level 1.
  if (parameters[0] < 1) return;
  if (parameters[0] > MAX_LEVELS) return;
  globalData.levelInfo.stats[parameters[0]-1].SetMapOverride(ovt, 
                                                           parameters[1],  // x
                                                           parameters[2],  // y
                                                           parameters[3],  // facing
                                                           parameters[4]   // value
                                                          );
}

void WALL_OVERRIDES::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    int i;
    ar << m_numRow;
    for (i=0; i<m_numRow; i++)
    {
      if (m_row[i] != NULL)
      {
        ar << i;
        m_row[i]->Serialize(ar);
      }
      else
      {
        ar << -1;   // The row does not exist
      };
    };
  }
  else
  {
    Clear();
    ar >> m_numRow;
    if (m_numRow > 0)
    {
      int i;
      m_row = (ROW_OVERRIDES **)malloc(m_numRow * sizeof(int *));
      if (m_row == NULL)
      {
#ifdef UAFEngine
        die (0xc31ca1);
#else
        exit(1);
#endif
      };
      for (i=0; i<m_numRow; i++)
      {
        int rowNum;
        ar >> rowNum;
        if (rowNum >= 0)
        {
          m_row[rowNum] = new ROW_OVERRIDES;
          m_row[rowNum]->Serialize(ar);
        }
        else
        {
          m_row[i] = NULL;
        };
      };
    };
  };
}

void WALL_OVERRIDES::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    int i;
    car << m_numRow;
    for (i=0; i<m_numRow; i++)
    {
      if (m_row[i] != NULL)
      {
        car << i;
        m_row[i]->Serialize(car);
      }
      else
      {
        car << -1;   // The row does not exist
      };
    };
  }
  else
  {
    Clear();
    car >> m_numRow;
    if (m_numRow > 0)
    {
      int i;
      m_row = (ROW_OVERRIDES **)malloc(m_numRow * sizeof(int *));
      if (m_row == NULL)
      {
#ifdef UAFEngine
        die (0xc31ca1);
#else
        exit(1);
#endif
      };
      for (i=0; i<m_numRow; i++)
      {
        int rowNum;
        car >> rowNum;
        if (rowNum >= 0)
        {
          m_row[rowNum] = new ROW_OVERRIDES;
          m_row[rowNum]->Serialize(car);
        }
        else
        {
          m_row[i] = NULL;
        };
      };
    };
  };
}

#ifdef UAFEDITOR

void WALL_OVERRIDES::Export(JWriter& jw, const char* name)
{
  jw.StartList(name);
    jw.StartArray("rows");
    if (m_numRow != 0)
      NotImplemented(0x3ab6d9, false);
//  pos = sounds.GetHeadPosition();
//  while (pos != NULL)
//  {
//    CString* pSound;
//    pSound = &sounds.GetNext(pos);
//    jw.NameAndValue(JKEY_SOUND, *pSound);
//  };
    jw.EndArray();
  jw.EndList();
}
#endif

void CELL_ITEM::Serialize(CAR& car)
{
  int temp;
  if (car.IsStoring())
  {
    car << m_itemID;
    car << m_charges;
    car << m_qty;
    car << m_identified;
    car << m_paid;
    temp = m_type;
    car << temp;
    car << m_cursed;
  }
  else
  {
    car >> m_itemID;
    car >> m_charges;
    car >> m_qty;
    car >> m_identified;
    car >> m_paid;
    car >> temp;
    m_type = (CELL_ITEM_TYPE) temp;
    car >> m_cursed;
  };
}

void CELL_ITEM::operator =(const ITEM *pItem)
{
  m_itemID = pItem->itemID;
  m_charges = pItem->charges;
  m_qty = pItem->qty;
  m_identified = pItem->identified;
  m_paid = pItem->paid;
  m_type = CIT_DroppedItem;
  m_cursed = pItem->cursed;
}

void CELL_ITEM::operator =(const CELL_ITEM *pSrc)
{
  m_itemID = pSrc->m_itemID;
  m_charges = pSrc->m_charges;
  m_qty = pSrc->m_qty;
  m_identified = pSrc->m_identified;
  m_paid = pSrc->m_paid;
  m_type = pSrc->m_type;
  m_cursed = pSrc->m_cursed;
}

CELL_ROW_CONTENTS::~CELL_ROW_CONTENTS(void)
{
  Clear();
}

void CELL_ROW_CONTENTS::Clear(void)
{
  unsigned int i;
  for (i=0; i<m_numItem; i++)
  {
    delete m_pCellItem[i];
  };
  m_numItem = 0;
  free (m_pCellItem);
  m_pCellItem = NULL;
}

void CELL_ROW_CONTENTS::operator =(const CELL_ROW_CONTENTS *pSrc)
{
  unsigned int i;
  Clear();
  m_row = pSrc->m_row;
  m_numItem = pSrc->m_numItem;
  if (m_numItem == 0) return;
  m_pCellItem = (CELL_ITEM **)malloc(m_numItem * sizeof(m_pCellItem[0]));
  if (m_pCellItem == NULL)
  {
    die(0xe0ae7);
  };
  for (i=0; i<m_numItem; i++)
  {
    m_pCellItem[i] = new CELL_ITEM;
    *m_pCellItem[i] = pSrc->m_pCellItem[i];
  };
}

void CELL_ROW_CONTENTS::Serialize(CAR& car)
{
  unsigned int i;
  if (car.IsStoring())
  {
    car << m_row;
    car << m_numItem;
    for(i=0; i<m_numItem; i++)
    {
      m_pCellItem[i]->Serialize(car);
    };
  }
  else
  {
    Clear();
    car >> m_row;
    car >> m_numItem;
    if (m_numItem == 0) return;
    m_pCellItem = (CELL_ITEM **)malloc(m_numItem*sizeof(m_pCellItem[0]));
    for (i=0; i<m_numItem; i++)
    {
      m_pCellItem[i] = new CELL_ITEM;
      m_pCellItem[i]->Serialize(car);
    };
  };
}

void CELL_ROW_CONTENTS::Add(CELL_ITEM *pCellItem)
{
  m_pCellItem = (CELL_ITEM **)realloc(m_pCellItem, (m_numItem+1)*sizeof(m_pCellItem[0]));
  m_pCellItem[m_numItem] = pCellItem;
  m_numItem++;
}

int CELL_ROW_CONTENTS::GetCount(void ) const
{
  return m_numItem;
}


CELL_COLUMN_CONTENTS::~CELL_COLUMN_CONTENTS(void)
{
  Clear();
}

void CELL_COLUMN_CONTENTS::Clear(void)
{
  unsigned int i;
  for (i=0; i<m_numRow; i++)
  {
    delete m_pCellRowContents[i];
  };
  m_numRow = 0;
  free (m_pCellRowContents);
  m_pCellRowContents = NULL;
}

void CELL_COLUMN_CONTENTS::Add(CELL_ITEM *pCellItem, unsigned int y)
{
  unsigned int i;
  for (i=0; i<m_numRow; i++)
  {
    if (m_pCellRowContents[i]->Row() == y) break;
  };
  if (i == m_numRow)
  {
    m_numRow++;
    m_pCellRowContents = (CELL_ROW_CONTENTS **)
             realloc(m_pCellRowContents, m_numRow*sizeof(m_pCellRowContents[0]));
    m_pCellRowContents[i] = new CELL_ROW_CONTENTS;
    m_pCellRowContents[i]->Row(y);
  };
  m_pCellRowContents[i]->Add(pCellItem);
}

int CELL_COLUMN_CONTENTS::GetCount(unsigned int y) const
{
  unsigned int i;
  for (i = 0; i < m_numRow; i++)
  {
    if (m_pCellRowContents[i]->Row() == y) break;
  };
  if (i == m_numRow) return 0;
  return m_pCellRowContents[i]->GetCount();
}

void CELL_COLUMN_CONTENTS::operator = (const CELL_COLUMN_CONTENTS *pSrc)
{
  unsigned int i;
  Clear();
  m_numRow = pSrc->m_numRow;
  m_column = pSrc->m_column;
  if (m_numRow == 0) return;
  m_pCellRowContents = (CELL_ROW_CONTENTS **)malloc(m_numRow*sizeof(m_pCellRowContents[0]));
  if (m_pCellRowContents == NULL)
  {
    die(0x44a0cb);
  };
  for (i=0; i< m_numRow; i++)
  {
    m_pCellRowContents[i] = new CELL_ROW_CONTENTS;
    *m_pCellRowContents[i] = pSrc->m_pCellRowContents[i];
  };
}

void CELL_COLUMN_CONTENTS::Serialize(CAR& car)
{
  unsigned int i;
  if (car.IsStoring())
  {
    car << m_column;
    car << m_numRow;
    for (i=0; i<m_numRow; i++)
    {
      m_pCellRowContents[i]->Serialize(car);
    };
  }
  else
  {
    Clear();
    car >> m_column;
    car >> m_numRow;
    m_pCellRowContents = (CELL_ROW_CONTENTS **)
           malloc(m_numRow * sizeof(m_pCellRowContents[0]));
    if (m_pCellRowContents == NULL)
    {
      die(0xca332e);
    };
    for (i=0; i<m_numRow; i++)
    {
      m_pCellRowContents[i] = new CELL_ROW_CONTENTS;
      m_pCellRowContents[i]->Serialize(car);
    };
  };
}

CELL_LEVEL_CONTENTS::~CELL_LEVEL_CONTENTS(void)
{
  Clear();
}

void CELL_LEVEL_CONTENTS::Clear()
{
  unsigned int i;
  for(i=0; i<m_numColumn; i++)
  {
    delete m_pCellColumnContents[i];
  };
  m_numColumn = 0;
  free (m_pCellColumnContents);
  m_pCellColumnContents = NULL;
}

void CELL_LEVEL_CONTENTS::Add(CELL_ITEM *pCellItem, unsigned int x, unsigned int y)
{
  unsigned int i;
  for(i=0; i<m_numColumn; i++)
  {
    if (m_pCellColumnContents[i]->Column() == x) break;
  };
  if (i == m_numColumn)
  {
    m_numColumn++;
    m_pCellColumnContents = (CELL_COLUMN_CONTENTS **)
             realloc(m_pCellColumnContents, m_numColumn*sizeof(m_pCellColumnContents[0]));
    m_pCellColumnContents[i] = new CELL_COLUMN_CONTENTS;
    m_pCellColumnContents[i]->Column(x);
  };
  m_pCellColumnContents[i]->Add(pCellItem, y);
}

int CELL_LEVEL_CONTENTS::GetCount(unsigned int x, unsigned int y) const
{
  unsigned int i;
  for (i = 0; i < m_numColumn; i++)
  {
    if (m_pCellColumnContents[i]->Column() == x) break;
  };
  if (i == m_numColumn) return 0;
  return m_pCellColumnContents[i]->GetCount(y);
}


void CELL_LEVEL_CONTENTS::operator = (const CELL_LEVEL_CONTENTS& src)
{
  unsigned int i;
  Clear();
  if (src.m_numColumn == 0) return;
  m_numColumn = src.m_numColumn;
  m_pCellColumnContents = (CELL_COLUMN_CONTENTS **)
         malloc(m_numColumn * sizeof (m_pCellColumnContents[0]));
  if (m_pCellColumnContents == NULL)
  {
    die(0x565ab);
  };
  for (i=0; i<m_numColumn; i++)
  {
    m_pCellColumnContents[i] = new CELL_COLUMN_CONTENTS;
    *m_pCellColumnContents[i] = src.m_pCellColumnContents[i];
  };
}

void CELL_LEVEL_CONTENTS::Serialize(CAR& car)
{
  unsigned int i;
  if (car.IsStoring())
  {
    car << m_numColumn;
    for (i=0; i<m_numColumn; i++)
    {
        m_pCellColumnContents[i]->Serialize(car);
    };
  }
  else
  {
    Clear();
    car >> m_numColumn;
    m_pCellColumnContents = (CELL_COLUMN_CONTENTS **)malloc(m_numColumn*sizeof(m_pCellColumnContents[0]));
    if (m_pCellColumnContents == NULL)
    {
      die(0x6ccd03);
    }
    for(i=0; i<m_numColumn; i++)
    {
      m_pCellColumnContents[i] = new CELL_COLUMN_CONTENTS;
      m_pCellColumnContents[i]->Serialize(car);
    };
  };
}

#ifdef UAFEDITOR
void CELL_LEVEL_CONTENTS::Export(JWriter& jw, const char* name)
{
  jw.StartList(name);
    jw.StartArray("columns");
    if (m_numColumn != 0)
      NotImplemented(0xc3a03, false);
 // while (pos != NULL)
 // {
 //   CString* pSound;
 //   pSound = &sounds.GetNext(pos);
 //   jw.NameAndValue(JKEY_SOUND, *pSound);
 // };
    jw.EndArray();
  jw.EndList();
}
#endif

LEVEL_STATS& LEVEL_STATS::operator =(const LEVEL_STATS& src)
{
  if (&src == this)
    return *this;
  area_height = src.area_height;
  area_width = src.area_width;
  used = src.used;
  overland = src.overland;
  level_name = src.level_name;
  AVStyle = src.AVStyle;
  bgSounds=src.bgSounds;
  StepSound=src.StepSound;
  BumpSound=src.BumpSound;

	level_asl.Copy(src.level_asl);
  temp_asl.Copy(src.temp_asl);

  m_wallOverrides = src.m_wallOverrides;
  m_cellContents = src.m_cellContents;
  //NotImplemented(0x8da6c, false);  // Need to copy m_wallOverrides and m_cellContents

  for (int i=0;i<MAX_ENTRY_POINTS;i++)
    entryPoints[i] = src.entryPoints[i];
  return *this;
}

#ifdef UAFEDITOR
BOOL LEVEL_STATS::operator ==(LEVEL_STATS& src)
{
  if (&src == this) return TRUE;
  if (area_height != src.area_height) return FALSE;
  if (area_width != src.area_width) return FALSE;
  if (used != src.used) return FALSE;
  if (overland != src.overland) return FALSE;
  if (level_name != src.level_name) return FALSE;
  if (AVStyle != src.AVStyle) return FALSE;
  if (!(bgSounds==src.bgSounds)) return FALSE;
  if (StepSound != src.StepSound) return FALSE;
  if (BumpSound != src.BumpSound) return FALSE;
	if ( !(level_asl==src.level_asl) ) return FALSE;
  for (int i=0;i<MAX_ENTRY_POINTS;i++)
    if ( !(entryPoints[i] == src.entryPoints[i]) ) return FALSE;
  NotImplemented(0x8da6d, false);  // Need to copy m_wallOverrides and m_cellContents
  return TRUE;
}
#endif
void LEVEL_STATS::Clear()
{
  m_version = 0;  // 20110910 PRS
  area_height=0;area_width=0;used=FALSE;overland=FALSE;level_name="";
  StepSound="";
  BumpSound="";
  AVStyle=AnyView;
  bgSounds.Clear();
  for (int i=0;i<MAX_ENTRY_POINTS;i++)
  {
    entryPoints[i].x=0;
    entryPoints[i].y=i;
    entryPoints[i].facing=0;
  }

  level_asl.Clear();
  m_wallOverrides.Clear();
  m_cellContents.Clear();
}

void LEVEL_STATS::SetDefaults()
{
  area_height=50;
  area_width=50;
  overland=FALSE;
  AVStyle=AnyView;
  bgSounds.Clear();
  bgSounds.UseNightMusic=FALSE;
  bgSounds.EndTime=600;
  bgSounds.StartTime=1800;
  m_wallOverrides.Clear();
  m_cellContents.Clear();
  //NotImplemented(0x8da6f, false);  // Need to clear m_wallOverrides and m_cellContents
}

void LEVEL_STATS::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  int i;

  PreSerialize(ar.IsStoring());

  if (ar.IsStoring())
  {
    ar << area_height;
    ar << area_width;
    ar << used;
    ar << overland;
    ar << (int)AVStyle;

    AS(ar,level_name);

    for (i=0;i<MAX_ENTRY_POINTS;i++)
    {
      ar << entryPoints[i].x;  
      ar << entryPoints[i].y;  
      // ar << entryPoints[i].facing;  stored as attribute
    }
    AS(ar,StepSound);
    AS(ar,BumpSound);
    bgSounds.backgroundSounds.Serialize(ar);
    bgSounds.nightSounds.Serialize(ar);
    ar << bgSounds.UseNightMusic;
    ar << bgSounds.EndTime;
    ar << bgSounds.StartTime;
    m_version = LEVEL_STATS_VERSION;
    StoreIntAsASL(level_asl, m_version,"__LEVEL_STATS_VERSION", ASLF_MODIFIED);
    level_asl.Serialize(ar, "LEVEL_STATS_ATTRIBUTES");
    level_asl.Delete("__LEVEL_STATS_VERSION");
    m_wallOverrides.Serialize(ar);
    NotImplemented(0x8da70, false);  // Need to clear m_wallOverrides and m_cellContents
  }
  else
  {
    ar >> area_height;
    ar >> area_width;
    ar >> used;
    ar >> overland;
    if (globalData.version >= _VERSION_0576_)
    {
      ar >> i;
      AVStyle = (GlobalAreaViewStyle)i;
    }

    DAS(ar,level_name);

    POINT pt;
    for (i=0;i<MAX_ENTRY_POINTS;i++)
    {
      ar >> pt;
      entryPoints[i].x = pt.x;
      entryPoints[i].y = pt.y;
      entryPoints[i].facing = 0;
    }

    if (globalData.version >= _VERSION_0640_)
    {
      DAS(ar,StepSound);
      DAS(ar,BumpSound);
      StripFilenamePath(StepSound);
      StripFilenamePath(BumpSound);
      //if (StepSound != "") StepSound = rte.SoundDir() + StepSound;
      //if (BumpSound != "") BumpSound = rte.SoundDir() + BumpSound;
    }
    else
    {
      StepSound="";
      BumpSound="";
    }

    if (globalData.version >= _VERSION_0710_)
    {
      bgSounds.backgroundSounds.Serialize(ar);
      bgSounds.nightSounds.Serialize(ar);
      ar >> bgSounds.UseNightMusic;
      ar >> bgSounds.EndTime;
      ar >> bgSounds.StartTime;
    }
    level_asl.Serialize(ar, "LEVEL_STATS_ATTRIBUTES");
    RetrieveIntFromASL(level_asl, m_version, "__LEVEL_STATS_VERSION");
    level_asl.Delete("__LEVEL_STATS_VERSION");
    m_wallOverrides.Clear();
    m_cellContents.Clear();
    if (m_version >= 1)
    {
      m_wallOverrides.Serialize(ar);
    };
  }

  

  PostSerialize(ar.IsStoring());
}

void LEVEL_STATS::Serialize(CAR &car)
{
  int i;
  PreSerialize(car.IsStoring());
  if (car.IsStoring())
  {
    car << area_height;
    car << area_width;
    car << used;
    car << overland;
    car << (int)AVStyle;

    AS(car,level_name);

    for (i=0;i<MAX_ENTRY_POINTS;i++)
    {
      car << entryPoints[i].x;  
      car << entryPoints[i].y;  
      // ar << entryPoints[i].facing;  stored as attribute
    }

    AS(car,StepSound);
    AS(car,BumpSound);
    bgSounds.backgroundSounds.Serialize(car);
    bgSounds.nightSounds.Serialize(car);
    car << bgSounds.UseNightMusic;
    car << bgSounds.EndTime;
    car << bgSounds.StartTime;
	m_wallOverrides.Serialize(car);
	m_cellContents.Serialize(car);
  }
  else
  {
    car >> area_height;
    car >> area_width;
    car >> used;
    car >> overland;
    if (globalData.version >= _VERSION_0576_)
    {
      car >> i;
      AVStyle = (GlobalAreaViewStyle)i;
    }

    DAS(car,level_name);
//    ar >> level_name;
//
//    if (level_name == "*")
//      level_name="";     

    POINT pt;
    for (i=0;i<MAX_ENTRY_POINTS;i++)
    {
      //car >> pt;
      car >> pt.x;
      car >> pt.y;
      entryPoints[i].x = pt.x;
      entryPoints[i].y = pt.y;
      entryPoints[i].facing = 0;
    }
    if (globalData.version >= _VERSION_0640_)
    {
      DAS(car,StepSound);
      DAS(car,BumpSound);
      StripFilenamePath(StepSound);
      StripFilenamePath(BumpSound);
      // 20140421 PRS
      //if (StepSound != "") StepSound = rte.SoundDir() + StepSound;
      //if (BumpSound != "") BumpSound = rte.SoundDir() + BumpSound;
    }
    else
    {
      StepSound="";
      BumpSound="";
    }
    if (globalData.version >= _VERSION_0710_)
    {
      bgSounds.backgroundSounds.Serialize(car);
      bgSounds.nightSounds.Serialize(car);
      car >> bgSounds.UseNightMusic;
      car >> bgSounds.EndTime;
      car >> bgSounds.StartTime;
    }
	if (globalData.version >= _CELL_CONTENTS_VERSION)
	{
  	m_wallOverrides.Serialize(car);
	  m_cellContents.Serialize(car);
	  //NotImplemented(0x8da73, false);  // Need to serialize m_wallOverrides and m_cellContents
	};
  }
  level_asl.Serialize(car, "LEVEL_STATS_ATTRIBUTES");
  //m_wallOverrides.Serialize(car);

  PostSerialize(car.IsStoring());
}

#ifdef UAFEDITOR

extern const char *JKEY_HEIGHT;
extern const char *JKEY_WIDTH;
const char *JKEY_OVERLAND = "overland";
const char *JKEY_AVSTYLE = "avStyle";
extern const char *JKEY_NAME;
const char *JKEY_STEPSOUND = "stepSound";
const char *JKEY_BUMPSOUND = "bumpSound";
const char *JKEY_LEVELINFO = "levelInfo";
const char *JKEY_ENTRYPOINTS = "entryPoints";
extern const char *JKEY_ENTRY;
const char *JKEY_XY = "xy";
extern const char *JKEY_FACING;
extern const char *JKEY_INDEX;


CONFIG_ITEM_STATUS LEVEL_STATS::Export(JWriter& jw)
{
  int i;
  jw.StartList(JKEY_LEVELINFO);
  jw.NameAndValue(JKEY_HEIGHT, area_height);
  jw.NameAndValue(JKEY_WIDTH, area_width);
  jw.NameAndValue(JKEY_OVERLAND, overland);
  jw.NameAndEnum(JKEY_AVSTYLE, AVStyle);

  jw.NameAndValue(JKEY_NAME, level_name);
  jw.StartArray(JKEY_ENTRYPOINTS);
  for (i=0;i<MAX_ENTRY_POINTS;i++)
  {
    int j[2];
    jw.ListEntry();
    jw.Linefeed(false);
    jw.NameAndValue(JKEY_INDEX, i); 
    j[0] = entryPoints[i].x;
    j[1] = entryPoints[i].y;
    jw.NameAndValue(JKEY_XY, j, 2); 
    jw.NameAndEnum(JKEY_FACING, entryPoints[i].facing, 4, FacingText); 
    jw.EndList();
    jw.Linefeed(true);
  }
  jw.EndArray();
  jw.NameAndValue(JKEY_STEPSOUND, StepSound);
  jw.NameAndValue(JKEY_BUMPSOUND, BumpSound);
  bgSounds.backgroundSounds.Export(jw, "backgroundSounds");
  bgSounds.nightSounds.Export(jw, "nightSounds");
  level_asl.Export(jw);
  m_wallOverrides.Export(jw, "wallOverrides");
  m_cellContents.Export(jw, "cellContents");
  // NotImplemented(0x8da75, false);  // Need to export m_wallOverrides and m_cellContents
  jw.EndList();
  return CONFIG_STAT_ok;
}

CONFIG_ITEM_STATUS LEVEL_STATS::Import(JReader& jr)
{
  int i;
  jr.StartList(JKEY_LEVELINFO);
  jr.NameAndValue(JKEY_HEIGHT, area_height);
  jr.NameAndValue(JKEY_WIDTH, area_width);
  jr.NameAndValue(JKEY_OVERLAND, overland);
  jr.NameAndEnum(JKEY_AVSTYLE, AVStyle);

  jr.NameAndValue(JKEY_NAME, level_name);
  jr.StartArray(JKEY_ENTRYPOINTS);
  for (i=0;i<MAX_ENTRY_POINTS;i++)
  {
    int j[2];
    jr.NextEntry();
    jr.StartList();
    jr.Linefeed(false);
    jr.NameAndValue(JKEY_INDEX, i); 
    jr.NameAndValue(JKEY_XY, j, 2);
    entryPoints[i].x = j[0]; 
    entryPoints[i].y = j[1];
    jr.NameAndEnum(JKEY_FACING, entryPoints[i].facing, 4, FacingText); 
    jr.EndList();
    jr.Linefeed(true);
  }
  jr.EndArray();
  jr.NameAndValue(JKEY_STEPSOUND, StepSound);
  jr.NameAndValue(JKEY_BUMPSOUND, BumpSound);
  bgSounds.backgroundSounds.Import(jr, "backgroundSounds");
  bgSounds.nightSounds.Import(jr, "nightSounds");
  level_asl.Import(jr);
  NotImplemented(0x8da76, false);  // Need to import m_wallOverrides and m_cellContents
  jr.EndList();
  return CONFIG_STAT_ok;
}


#endif

void LEVEL_STATS::PreSerialize(BOOL IsStoring)
{
	if (IsStoring)
	{
		// move fields that need to be serialized
		// as attributes into the attribute map  

		if (used)
		{
			CString key;
			// Entry Point facings    
			for (int i = 0; i < MAX_ENTRY_POINTS; i++)
			{
				key.Format("EPFace_%i", i);
				StoreIntAsASL(level_asl, entryPoints[i].facing, key);
			};
		};
		//NotImplemented(0x8da77, false);  // Need to preserialize m_wallOverrides and m_cellContents
		m_wallOverrides.PreSerialize();
		m_cellContents.PreSerialize();
	};
}

void LEVEL_STATS::PostSerialize(BOOL IsStoring)
{
  if (IsStoring)
  {
    CString key;
    for (int i=0;i<MAX_ENTRY_POINTS;i++)
    {
      key.Format("EPFace_%i", i);
      level_asl.Delete(key);
    };
  }
  else
  {
    // retrieve fields that are serialized as
    // attributes from the attribute map

    if (used)
    {
      CString key;
      int temp;
      // Entry Point facings    
      for (int i=0;i<MAX_ENTRY_POINTS;i++)
      {
        key.Format("EPFace_%i", i);
        RetrieveIntFromASL(level_asl, temp, key);
        level_asl.Delete(key);
        entryPoints[i].facing = (BYTE)temp;
      };
    };
  };
  m_wallOverrides.PostSerialize();
  m_cellContents.PostSerialize();
}


void LEVEL_STATS::Save(CAR& car) 
{
  m_version = LEVEL_STATS_VERSION;
  StoreIntAsASL(level_asl, m_version, "__LEVEL_STATS_VERSION", ASLF_MODIFIED);
  level_asl.Save(car, "LEVEL_STATS_ATTRIBUTES");
  level_asl.Delete("__LEVEL_STATS_VERSION");
  m_wallOverrides.Serialize(car);
  m_cellContents.Serialize(car);
};

void LEVEL_STATS::Restore(CArchive& ar) 
{
  temp_asl.Serialize(ar, "LEVEL_STATS_ATTRIBUTES");
  RetrieveIntFromASL(temp_asl, m_version, "__LEVEL_STATS_VERSION");
  temp_asl.Delete("__LEVEL_STATS_VERSION");
  if (m_version >= 1)
  {
    m_wallOverrides.Serialize(ar);
    NotImplemented(0x8da7b, false);  // Need to postserialize m_wallOverrides and m_cellContents
  };
}

void LEVEL_STATS::Restore(CAR& car) 
{
  temp_asl.Serialize(car, "LEVEL_STATS_ATTRIBUTES");
  RetrieveIntFromASL(temp_asl, m_version, "__LEVEL_STATS_VERSION");
  temp_asl.Delete("__LEVEL_STATS_VERSION");
  if (m_version >= 1)
  {
    m_wallOverrides.Serialize(car);
    if (m_version >= 2)
    {
      m_cellContents.Serialize(car);
    };
    //NotImplemented(0x8da7c, false);  // Need to restore m_wallOverrides and m_cellContents
  };
}

void LEVEL_STATS::CommitRestore(void) 
{
	level_asl.CommitRestore(&temp_asl);
	temp_asl.Clear();
}




//*****************************************************************************

LEVEL_INFO& LEVEL_INFO::operator =(const LEVEL_INFO& src)
{
 if (&src == this)
   return *this;
 numLevels = src.numLevels;
 for (int i=0;i<MAX_LEVELS;i++)
   stats[i] = src.stats[i];
 return *this;
}

void LEVEL_INFO::Clear() 
{ 
 numLevels=0; 
 for (int i=0;i<MAX_LEVELS;i++)
   Clear(i);
}

void LEVEL_INFO::Clear(int level) 
{ 
 stats[level].Clear();
 stats[level].level_name.Format("Level %i", level+1);
}

void LEVEL_INFO::SaveSounds(void)
{
  for (int i=0;i<MAX_LEVELS;i++)
  {
    ::SaveSound(stats[i].StepSound, i, rte.SoundDir());
    ::SaveSound(stats[i].BumpSound, i, rte.SoundDir());
    stats[i].bgSounds.backgroundSounds.SaveSounds(i);
    stats[i].bgSounds.nightSounds.SaveSounds(i);
  }
}

int LEVEL_INFO::GetEPX(int ep)
{
  return (stats[GetCurrentLevel()].entryPoints[ep].x); 
}

int LEVEL_INFO::GetEPY(int ep)
{
  return (stats[GetCurrentLevel()].entryPoints[ep].y); 
}

BYTE LEVEL_INFO::GetEPFacing(int ep)
{
  return (stats[GetCurrentLevel()].entryPoints[ep].facing); 
}

void LEVEL_INFO::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);

  DWORD count = 0;
  DWORD i;
  for (i=0;i<MAX_LEVELS;i++)
  {
    if (stats[i].used)
      count++;
  }

  if (ar.IsStoring())
  {
    ar << numLevels;
    ar << count;
    for (i=0;i<MAX_LEVELS;i++)
    {
      if (stats[i].used)
      {
        ar << i;
        stats[i].Serialize(ar);
      }
    }
  }
  else
  {
    ar >> numLevels;

    if (globalData.version >= _VERSION_0566_)
    {
      ar >> count;
      DWORD index;
      for (i=0;i<count;i++)
      {
        ar >> index;
        stats[index].Serialize(ar);
      }
    }
    else
    {
      for (i=0;i<MAX_LEVELS;i++)
          stats[i].Serialize(ar);
    }
  }
}

void LEVEL_INFO::Serialize(CAR &car)
{
  DWORD count = 0;
  DWORD i;
  for (i=0;i<MAX_LEVELS;i++)
  {
    if (stats[i].used)
      count++;
  }

  if (car.IsStoring())
  {
    car << numLevels;
    car << count;
    for (i=0;i<MAX_LEVELS;i++)
    {
      if (stats[i].used)
      {
        car << i;
        stats[i].Serialize(car);
      }
    }
  }
  else
  {
    car >> numLevels;
    car >> count;
    DWORD index;
    for (i=0;i<count;i++)
    {
      car >> index;
      stats[index].Serialize(car);
    }
  }
}


void LEVEL_INFO::Save(CAR& car) {
	for (int i=0; i<MAX_LEVELS; i++) 
	{
		stats[i].Save(car);
	};
}

void LEVEL_INFO::Restore(CArchive& ar) 
{
	for (int i=0; i<MAX_LEVELS; i++) 
	{
		stats[i].Restore(ar);
	};
}

void LEVEL_INFO::Restore(CAR& car) 
{
	for (int i=0; i<MAX_LEVELS; i++) 
	{
		stats[i].Restore(car);
	};
}

void LEVEL_INFO::CommitRestore(void) 
{
	for (int i=0; i<MAX_LEVELS; i++) 
	{
		stats[i].CommitRestore();
	};
}


#ifdef UAFEDITOR
void LEVEL_INFO::CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference)
{
  int i;
  for (i=0; i<numLevels; i++)
  {
    stats[i].CrossReference(pCRList, i);
  };
}
#endif

//*****************************************************************************

GLOBAL_VAULT_DATA& GLOBAL_VAULT_DATA::operator =(const GLOBAL_VAULT_DATA& src)
{
  if (&src == this)
    return *this;

  money = src.money;
  items = src.items;
  return *this;
}

void GLOBAL_VAULT_DATA::Clear() 
{ 
  money.Clear();
  items.Clear();
}

void GLOBAL_VAULT_DATA::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  double ver;
  // older designs stored vault data in game.dat file
  // instead of saved game file
  //if (version < _VERSION_0661_)
  //  ver = version;
  //else
    ver = globalData.SaveGameVersion;

  if (ar.IsStoring())
  {
    money.Serialize(ar, ver);
  }
  else
  {
    money.Clear();

    if (ver < _VERSION_0661_)
    {
      int temp;
      ar >> temp; money.Coins[CopperType] = temp;
      ar >> temp; money.Coins[ElectrumType] = temp;
      ar >> temp; money.Coins[GoldType] = temp;
      ar >> temp; money.Coins[PlatinumType] = temp;
      ar >> temp; money.Coins[SilverType] = temp;
      ar >> temp; { for (int g=0;g<temp;g++) money.AddGem(); }
      ar >> temp; { for (int j=0;j<temp;j++) money.AddJewelry(); }
    }
    else
    {
      money.Serialize(ar, ver);
    }
  }

  items.Serialize(ar,ver);
}

void GLOBAL_VAULT_DATA::Serialize(CAR &ar)
{
  double ver;
  // older designs stored vault data in game.dat file
  // instead of saved game file
  //if (version < _VERSION_0661_)
  //  ver = version;
  //else
    ver = globalData.SaveGameVersion;

  if (ar.IsStoring())
  {
    money.Serialize(ar, ver);
  }
  else
  {
    money.Clear();
    if (globalData.SaveGameVersion < _VERSION_0661_)
    {
      int temp;
      ar >> temp; money.Coins[CopperType] = temp;
      ar >> temp; money.Coins[ElectrumType] = temp;
      ar >> temp; money.Coins[GoldType] = temp;
      ar >> temp; money.Coins[PlatinumType] = temp;
      ar >> temp; money.Coins[SilverType] = temp;
      ar >> temp; { for (int g=0;g<temp;g++) money.AddGem(); }
      ar >> temp; { for (int j=0;j<temp;j++) money.AddJewelry(); }
    }
    else
    {
      money.Serialize(ar, ver);
    }
  }
  items.Serialize(ar,ver);
}

#ifdef UAFEDITOR
void GLOBAL_VAULT_DATA::CrossReference(CR_LIST *pCRList)
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceName = "Global Vault Data";
  CRReference.m_referenceType = CR_TYPE_global;
  items.CrossReference(pCRList, &CRReference);
}
#endif
//*****************************************************************************

GLOBAL_STATS& GLOBAL_STATS::operator =(const GLOBAL_STATS& src)
{
  if (&src == this) return *this;
  Clear();
  version = src.version;
  designName = src.designName;
  startLevel = src.startLevel;
  currLevel = src.currLevel;
  startX = src.startX;
  startY = src.startY;
  startFacing = src.startFacing;
  useAreaView = src.useAreaView;
  startTime = src.startTime;
  startExp = src.startExp;
  startExpType=src.startExpType;
 // UNUSED_startEquip = src.UNUSED_startEquip;
  startPlatinum = src.startPlatinum;
  startGem = src.startGem;
  startJewelry = src.startJewelry;

  //minPartySize = src.minPartySize;
  //maxPartySize = src.maxPartySize;
  minPCs = src.minPCs;
  maxParty_maxPCs = src.maxParty_maxPCs;

  //allowCharCreate = src.allowCharCreate;
  flags = src.flags;
  DungeonTimeDelta = src.DungeonTimeDelta;
  DungeonSearchTimeDelta = src.DungeonSearchTimeDelta;
  WildernessTimeDelta = src.WildernessTimeDelta;
  WildernessSearchTimeDelta = src.WildernessSearchTimeDelta;
  AutoDarkenViewport = src.AutoDarkenViewport;
  AutoDarkenAmount = src.AutoDarkenAmount;
  StartDarken = src.StartDarken;
  EndDarken = src.EndDarken;
  m_MapArt = src.m_MapArt;
  UseAVZones = src.UseAVZones;
  MagicAdjFont = src.MagicAdjFont;
  /*   prs 20191220
   *WhiteFont = src.WhiteFont;
   *YellowFont = src.YellowFont;
   *OrangeFont = src.OrangeFont;
   *RedFont = src.RedFont;
   *GreenFont = src.GreenFont;
   *BlueFont = src.BlueFont;
   */
  // Instead of the above:
// prs 20191227  {
// prs 20191227    int font;
// prs 20191227    for (font = 0; font < 10; font++) availableFonts[font] = src.availableFonts[font];
// prs 20191227  };
  fontLib = src.fontLib;

  Custom1Font=src.Custom1Font;
  KeyboardFont = src.KeyboardFont;
  KeyboardHighlightFont=src.KeyboardHighlightFont;
  HighlightFont = src.HighlightFont;
  //TitleBgArt = src.TitleBgArt;
  titleData = src.titleData;
  creditsData = src.creditsData;
  IconBgArt = src.IconBgArt;
  BackgroundArt = src.BackgroundArt;
  MapArtSurf = src.MapArtSurf;
  BackgroundSurf=src.BackgroundSurf;
  keyData = src.keyData;
  specialItemData = src.specialItemData;
  charData = src.charData;
  
  for (int i=0;i<MAX_GLOBAL_VAULTS;i++)    
    vault[i] = src.vault[i];

  levelInfo = src.levelInfo;
  questData = src.questData;
  sounds = src.sounds;
  logfont=src.logfont;
/*
  startEquip=src.startEquip;
  ClericEquip=src.ClericEquip;
  FighterEquip=src.FighterEquip;
  MagicUserEquip=src.MagicUserEquip;
  ThiefEquip=src.ThiefEquip;
  RangerEquip=src.RangerEquip;
  PaladinEquip=src.PaladinEquip;
  DruidEquip=src.DruidEquip;
*/
  moneyData = src.moneyData;
  m_diffLvlData=src.m_diffLvlData;
  journalData=src.journalData;
  SaveGameVersion=src.SaveGameVersion;
  eventData = const_cast<GLOBAL_STATS&>(src).eventData;
  fixSpellBook=src.fixSpellBook;
  global_asl.Copy(src.global_asl);
  temp_asl.Copy(src.temp_asl);
  
  return *this;
}

void GLOBAL_STATS::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  int i;
  POSITION pos;

  if (ar.IsStoring())
  {
    ar << version;
    ar << GetDesignName();
    ar << startLevel;
    ar << startX;
    ar << startY;
    ar << startFacing;
    ar << startTime;
    ar << startExp;
    ar << startExpType;
    {
      //ar << UNUSED_startEquip;
      long int junk = 0;
      ar << junk;
    };
    ar << startPlatinum;
    ar << startGem;
    ar << startJewelry;   
    
    ar << DungeonTimeDelta;
    ar << DungeonSearchTimeDelta;
    ar << WildernessTimeDelta;
    ar << WildernessSearchTimeDelta;
    ar << AutoDarkenViewport;
    ar << AutoDarkenAmount;
    ar << StartDarken;
    ar << EndDarken;

    //ar << minPartySize;
    //ar << maxPartySize;
    ar << minPCs;
    ar << maxParty_maxPCs;

    //ar << allowCharCreate;    
    ar << flags;
    
    //StripFilenamePath(MapArt);
    AS(ar, m_MapArt);

    if (strlen(logfont.lfFaceName) == 0)
      FillDefaultFontData("SYSTEM", 16, &logfont);

    ar.Write(&logfont, sizeof(logfont));
    //StripFilenamePath(IconBgArt);

    AS(ar, IconBgArt);
    //AddFolderToPath(IconBgArt, rte.IconArtDir());
    //StripFilenamePath(BackgroundArt);

    StripFilenamePath(BackgroundArt);
    AS(ar, BackgroundArt);
    //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());
  
    i = global_SmallPicImport.GetCount();
   ar << i;
   pos = global_SmallPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     global_SmallPicImport.GetAt(pos)->picType=SmallPicDib;;
     global_SmallPicImport.GetAt(pos)->SetDefaults();
     global_SmallPicImport.GetNext(pos)->Serialize(ar, version, rte.PicArtDir());
   }

   i = global_IconPicImport.GetCount();
   ar << i;
   pos = global_IconPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     global_IconPicImport.GetAt(pos)->picType=IconDib;
     global_IconPicImport.GetAt(pos)->SetDefaults();
     global_IconPicImport.GetNext(pos)->Serialize(ar, version, rte.PicArtDir());
   }
   titleData.Serialize(ar);
   creditsData.Serialize(ar);
  }
  else
  {
    titleData.Clear();
    creditsData.Clear();

    ar >> version;
    DAS(ar,designName);
    ar >> startLevel;
    ar >> startX;
    ar >> startY;
    ar >> startFacing;
    ar >> startTime;
    ar >> startExp;
    if (version >= _VERSION_0770_)
      ar >> startExpType;
    {
      //ar >> UNUSED_startEquip;
      long int junk;
      ar >> junk;
    };
    ar >> startPlatinum;
    ar >> startGem;
    ar >> startJewelry;

    if (version >= _VERSION_0574_)
    {
     ar >> DungeonTimeDelta;
     ar >> DungeonSearchTimeDelta;
     ar >> WildernessTimeDelta;
     ar >> WildernessSearchTimeDelta;
    }

    if (version >= _VERSION_0620_)
    {
      ar >> AutoDarkenViewport;
      ar >> AutoDarkenAmount;
      ar >> StartDarken;
      ar >> EndDarken;
    }

    if (version >= _VERSION_0575_)
    {
      //ar >> minPartySize;
      //ar >> maxPartySize;
      ar >> minPCs;
      ar >> maxParty_maxPCs;
      if (GetMaxPartySize() == 0) SetMaxPartySize(GetMaxPCs() + 2);

      //ar >> allowCharCreate;    
      ar >> flags;
    }

    DAS(ar,m_MapArt);
    //MapArt = rte.MapArtDir() + MapArt;

    if (version < _VERSION_0830_)
    {
      CString font="";
      BYTE fontSize=16;
      ar >> font;
      if (version >= _VERSION_0681_)
        ar >> fontSize;
      
      FillDefaultFontData(font, fontSize, &logfont);
    }
    else
    {
      ar.Read(&logfont, sizeof(logfont));
    }

    if (version < _VERSION_0800_)
    {
      CString TitleBgArt("");
      DAS(ar,TitleBgArt);
      //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
      TITLE_SCREEN data;
      data.TitleBgArt = TitleBgArt;
      data.DisplayBy = TITLE_SCREEN::tsFadeIn;
      data.UseTrans = FALSE;
      titleData.AddTitle(data);
    }

    if (version >= _VERSION_0660_)
    {
      DAS(ar,IconBgArt);
      //AddFolderToPath(IconBgArt, rte.IconArtDir());

      DAS(ar,BackgroundArt);
      //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());
      if (version < 0.9599)
      {
        StripFilenamePath(BackgroundArt);
        StripFilenamePath(IconBgArt);
      };
    }

    if (version >= _VERSION_0566_)
    {
        if (version < _VERSION_525)
        {
          // For older versions - Load the old CreditsBgArt filepath in as a single screen in the sequence
          CString CreditsBgArt;
          DAS(ar, CreditsBgArt);
          StripFilenamePath(CreditsBgArt);
          TITLE_SCREEN data;
          data.TitleBgArt = CreditsBgArt;
          data.DisplayBy = TITLE_SCREEN::tsFadeIn;
          data.UseTrans = FALSE;
          creditsData.AddTitle(data);
        }
    }

    int count;
    PIC_DATA data;

    ar >> count;
    for (i=0;i<count;i++)
    {
      data.Serialize(ar, version, rte.PicArtDir());
      global_SmallPicImport.AddTail(data);
    }

    data.Clear();
    ar >> count;
    CString icon;
    for (i=0;i<count;i++)
    {
      if (version < _VERSION_0640_)
      {        
        DAS(ar,icon);
        //AddFolderToPath(icon, rte.IconArtDir());
        data.filename = icon;
        data.picType=IconDib;
        data.SetDefaults();
      }
      else
      {
        data.Serialize(ar, version, rte.IconArtDir());
      }
      global_IconPicImport.AddTail(data);
    }
    if (version >= _VERSION_0800_)
      titleData.Serialize(ar);
    if (version >= _VERSION_525)
      creditsData.Serialize(ar);
  }

  global_asl.Serialize(ar, "GLOBAL_STATS_ATTRIBUTES");
  HBarVPArt.Serialize(ar);
  VBarVPArt.Serialize(ar);
  FrameVPArt.Serialize(ar);
  VBarCombArt.Serialize(ar);
  HBarCombArt.Serialize(ar);
  CombatWallArt.Serialize(ar);
  CombatCursorArt.Serialize(ar);
  CombatDeathIconArt.Serialize(ar);
  if (version >= _VERSION_526 || ar.IsStoring())
      CharViewFrameVPArt.Serialize(ar);
  if (version >= 0.930204)
      CombatPetrifiedIconArt.Serialize(ar);
  CombatDeathArt.Serialize(ar);
  if (version >= _VERSION_0575_)
      CursorArt.Serialize(ar, version, rte.CursorArtDir());

  if (!ar.IsStoring())
  {
    //HBarVPArt.name          = rte.WindowArtDir() + HBarVPArt.name;
    //VBarVPArt.name          = rte.WindowArtDir() + VBarVPArt.name;
    //FrameVPArt.name         = rte.WindowArtDir() + FrameVPArt.name;
    //VBarCombArt.name        = rte.CombatArtDir() + VBarCombArt.name;
    //HBarCombArt.name        = rte.CombatArtDir() + HBarCombArt.name;
    //CombatWallArt.name      = rte.CombatArtDir() + CombatWallArt.name;
    //CombatCursorArt.name    = rte.CombatArtDir() + CombatCursorArt.name;
    //CombatDeathIconArt.name = rte.CombatArtDir() + CombatDeathIconArt.name;
    if (version >= 0.930204)
    {
        //CombatPetrifiedIconArt.name = rte.CombatArtDir() + CombatPetrifiedIconArt.name;
    };
    //CombatDeathArt.name = rte.CombatArtDir() + CombatDeathArt.name;
  };

  sounds.Serialize(ar);
  keyData.Serialize(ar, version);
  specialItemData.Serialize(ar, version);
  questData.Serialize(ar, version);
  charData.Serialize(ar, version);

  if (version < _VERSION_0661_)
  {
    // only versions prior to 0.661 stored
    // vault data, all others store it
    // in the saved game file.
    SaveGameVersion=version;
    vault[0].Serialize(ar);
  }

  levelInfo.Serialize(ar);

  if (ar.IsStoring())
  {
/*
    startEquip.Serialize(ar,version);
    ClericEquip.Serialize(ar,version);
    FighterEquip.Serialize(ar,version);
    MagicUserEquip.Serialize(ar,version);
    ThiefEquip.Serialize(ar,version);
    RangerEquip.Serialize(ar,version);
    PaladinEquip.Serialize(ar,version);
    DruidEquip.Serialize(ar,version);
*/
    moneyData.Serialize(ar,version);
    m_diffLvlData.Serialize(ar);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);
    eventData.Serialize(ar, version);
    eventData.SetEventSource(GlobalEventSrc);
    journalData.Serialize(ar);
  }
  else
  {
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      if (version >= _VERSION_0575_)
        startEquip.Serialize(ar,version);
      if (version >= _VERSION_0830_)
      {
        ClericEquip.Serialize(ar,version);
        FighterEquip.Serialize(ar,version);
        MagicUserEquip.Serialize(ar,version);
        ThiefEquip.Serialize(ar,version);
        RangerEquip.Serialize(ar,version);
        PaladinEquip.Serialize(ar,version);
        DruidEquip.Serialize(ar,version);
      }
    };
    // We need to move this information to the Class database.
    // For each of the existing classes we will make
    // a list of the starting items:  The basic item plus
    // the items specific to each of that class' baseclasses.
    //    So we have loops:
    //      For each class
    //         For each baseclass in that class
    //             For each item in basic equipment
    //             For each item is specific equipment
    {
      int c, nc;
      nc = classData.GetCount(); // Number of classes
      for (c=0; c<nc; c++)       // For each 'c'lass
      {
        CLASS_DATA *pClass;
        int b, nb;
        pClass = classData.GetClass(c);
        pClass->m_startingEquipment = startEquip;  // Add the 'Base Items'.
        nb = pClass->GetCount(); // Number of baseclasses
        for (b=0; b<nb; b++)     // For each 'b'aseclass
        {
          const BASECLASS_ID *pBaseclassID;
          pBaseclassID = pClass->PeekBaseclassID(b);
          if (*pBaseclassID == "fighter")   pClass->m_startingEquipment.Append(FighterEquip);
          if (*pBaseclassID == "cleric")    pClass->m_startingEquipment.Append(ClericEquip);
          if (*pBaseclassID == "magicUser") pClass->m_startingEquipment.Append(MagicUserEquip);
          if (*pBaseclassID == "thief")     pClass->m_startingEquipment.Append(ThiefEquip);
          if (*pBaseclassID == "ranger")    pClass->m_startingEquipment.Append(RangerEquip);
          if (*pBaseclassID == "paladin")   pClass->m_startingEquipment.Append(PaladinEquip);
          if (*pBaseclassID == "druid")     pClass->m_startingEquipment.Append(DruidEquip);
        };
      };
    };

#endif
    if (version >= _VERSION_0642_)
      moneyData.Serialize(ar,version);
    if (version >= _VERSION_0697_)
      m_diffLvlData.Serialize(ar);
    if (version >= _VERSION_0681_)
      eventData.Serialize(ar, version);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);

    if ((version >= _VERSION_06991_)&&(version<=_VERSION_0842_))
    {
      // load it, but don't do anything with it.
      SPELL_SPECIAL_DATA tempData;
      tempData.Serialize(ar, version);
    }

    if (version >= _VERSION_0780_)
      journalData.Serialize(ar);
  }  

  if (ar.IsStoring())
  {
    fixSpellBook.Serialize(ar, version);
  }
  else
  {
    if (version >= _VERSION_0909_)
    {
      fixSpellBook.Serialize(ar, version);
    }
  }
}




void GLOBAL_STATS::Serialize(CAR& car)
{
  //CObject::Serialize(ar);
  int i;
  POSITION pos;

  if (car.IsStoring())
  {
    car << version;
    car << GetDesignName();
    car << startLevel;
    car << startX;
    car << startY;
    car << startFacing;
    car << startTime;
    car << startExp;
    car << startExpType;
    {
      long int junk=0;
      //car << UNUSED_startEquip;
      car << junk;
    };
    car << startPlatinum;
    car << startGem;
    car << startJewelry;   
    
    car << DungeonTimeDelta;
    car << DungeonSearchTimeDelta;
    car << WildernessTimeDelta;
    car << WildernessSearchTimeDelta;
    car << AutoDarkenViewport;
    car << AutoDarkenAmount;
    car << StartDarken;
    car << EndDarken;

    //car << minPartySize;
    //car << maxPartySize;
    car << minPCs;
    car << maxParty_maxPCs;

    //car << allowCharCreate;    
    car << flags;
    
    //StripFilenamePath(MapArt);
    AS(car, m_MapArt);

    if (strlen(logfont.lfFaceName) == 0)
      FillDefaultFontData("SYSTEM", 16, &logfont);

    //car.Write(&logfont, sizeof(logfont));
    car.Serialize((char *)&logfont, sizeof(logfont));
    StripFilenamePath(IconBgArt);

    AS(car, IconBgArt);
    //AddFolderToPath(IconBgArt, rte.IconArtDir());
    //StripFilenamePath(BackgroundArt);

    StripFilenamePath(BackgroundArt);
    AS(car, BackgroundArt);
    //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());

   i = global_SmallPicImport.GetCount();
   car << i;
   pos = global_SmallPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     global_SmallPicImport.GetAt(pos)->picType=SmallPicDib;;
     global_SmallPicImport.GetAt(pos)->SetDefaults();
     global_SmallPicImport.GetNext(pos)->Serialize(car, version, rte.PicArtDir());
   }

   i = global_IconPicImport.GetCount();
   car << i;
   pos = global_IconPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     global_IconPicImport.GetAt(pos)->picType=IconDib;
     /*
      * SetDefaults was removed 20160608 PRS
      * It was unconditionally setting NumFrames to 2.
      * Manikus did not like that.
     global_IconPicImport.GetAt(pos)->SetDefaults();
      */
     global_IconPicImport.GetNext(pos)->Serialize(car, version, rte.PicArtDir());
   }
   titleData.Serialize(car);
   creditsData.Serialize(car);
  }
  else
  {
    titleData.Clear();
    creditsData.Clear();
    {
      __int64 temp;
      car.Serialize((char *)&temp, sizeof(temp));
      if (temp == 0xfabcdefabcdefabfI64)
      {
        car >> version;
        car.Compress(true);
        car >> version;
      }
      else
      {
        version = (double)temp;
      };
    }
    DAS(car,designName);
    car >> startLevel;
    car >> startX;
    car >> startY;
    car >> startFacing;
    car >> startTime;
    car >> startExp;
    if (version >= _VERSION_0770_)
      car >> startExpType;
    {
      long int junk;
      //car >> UNUSED_startEquip;
      car >> junk;
    };
    car >> startPlatinum;
    car >> startGem;
    car >> startJewelry;

    if (version >= _VERSION_0574_)
    {
     car >> DungeonTimeDelta;
     car >> DungeonSearchTimeDelta;
     car >> WildernessTimeDelta;
     car >> WildernessSearchTimeDelta;
    }

    if (version >= _VERSION_0620_)
    {
      car >> AutoDarkenViewport;
      car >> AutoDarkenAmount;
      car >> StartDarken;
      car >> EndDarken;
    }

    if (version >= _VERSION_0575_)
    {
      //car >> minPartySize;
      //car >> maxPartySize;
      car >> minPCs;
      car >> maxParty_maxPCs;
      if (GetMaxPartySize() == 0) SetMaxPartySize(GetMaxPCs() + 2);

      //car >> allowCharCreate;    
      car >> flags;
    }

    DAS(car,m_MapArt);
    //MapArt = rte.MapArtDir() + MapArt;

    if (version < _VERSION_0830_)
    {
      CString font="";
      BYTE fontSize=16;
      car >> font;
      if (version >= _VERSION_0681_)
        car >> fontSize;
      
      FillDefaultFontData(font, fontSize, &logfont);
    }
    else
    {
      car.Serialize((char *)&logfont, sizeof(logfont));
    }

    if (version < _VERSION_0800_)
    {
      CString TitleBgArt("");
      DAS(car,TitleBgArt);
      //AddFolderToPath(TitleBgArt, rte.TitleArtDir());
      TITLE_SCREEN data;
      data.TitleBgArt = TitleBgArt;
      data.DisplayBy = TITLE_SCREEN::tsFadeIn;
      data.UseTrans = FALSE;
      titleData.AddTitle(data);
    }

    if (version >= _VERSION_0660_)
    {
      DAS(car,IconBgArt);
      //AddFolderToPath(IconBgArt, rte.IconArtDir());

      DAS(car,BackgroundArt);
      //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());
      if (version < 0.998918)
      {
        StripFilenamePath(BackgroundArt);
        StripFilenamePath(IconBgArt);
      };
    }

    if (version >= _VERSION_0566_)
    {
        if (version < _VERSION_525)
        {
            // For older versions - Load the old CreditsBgArt filepath in as a single screen in the sequence
            CString CreditsBgArt;
            DAS(car, CreditsBgArt);
            StripFilenamePath(CreditsBgArt);
            TITLE_SCREEN data;
            data.TitleBgArt = CreditsBgArt;
            data.DisplayBy = TITLE_SCREEN::tsFadeIn;
            data.UseTrans = FALSE;
            creditsData.AddTitle(data);
        }
    }

    int count;
    PIC_DATA data;

    car >> count;
    for (i=0;i<count;i++)
    {
      data.Serialize(car, version, rte.PicArtDir());
      global_SmallPicImport.AddTail(data);
    }

    data.Clear();
    car >> count;
    CString icon;
    for (i=0;i<count;i++)
    {
      if (version < _VERSION_0640_)
      {        
        DAS(car,icon);
        //AddFolderToPath(icon, rte.IconArtDir());
        data.filename = icon;
        data.picType=IconDib;
        data.SetDefaults();
      }
      else
      {
        data.Serialize(car, version, rte.IconArtDir());
      }
      global_IconPicImport.AddTail(data);
    }
    if (version >= _VERSION_0800_)
      titleData.Serialize(car);
    if (version >= _VERSION_525)
      creditsData.Serialize(car);
 }

  global_asl.Serialize(car, "GLOBAL_STATS_ATTRIBUTES");
  HBarVPArt.Serialize(car);
  VBarVPArt.Serialize(car);
  FrameVPArt.Serialize(car);
  VBarCombArt.Serialize(car);
  HBarCombArt.Serialize(car);
  CombatWallArt.Serialize(car);
  CombatCursorArt.Serialize(car);
  CombatDeathIconArt.Serialize(car);
  if (version >= _VERSION_526 || car.IsStoring())
      CharViewFrameVPArt.Serialize(car);
  if (version >= 0.930204)
      CombatPetrifiedIconArt.Serialize(car);
  CombatDeathArt.Serialize(car);
  if (version >= _VERSION_0575_)
      CursorArt.Serialize(car, version, rte.CursorArtDir());

  if (!car.IsStoring())
  {
    //HBarVPArt.name          = rte.WindowArtDir() + HBarVPArt.name;
    //VBarVPArt.name          = rte.WindowArtDir() + VBarVPArt.name;
    //FrameVPArt.name         = rte.WindowArtDir() + FrameVPArt.name;
    //VBarCombArt.name        = rte.CombatArtDir() + VBarCombArt.name;
    //HBarCombArt.name        = rte.CombatArtDir() + HBarCombArt.name;
    //CombatWallArt.name      = rte.CombatArtDir() + CombatWallArt.name;
    //CombatCursorArt.name    = rte.CombatArtDir() + CombatCursorArt.name;
    //CombatDeathIconArt.name = rte.CombatArtDir() + CombatDeathIconArt.name;
    if (version >= 0.930204)
    {
        //CombatPetrifiedIconArt.name = rte.CombatArtDir() + CombatPetrifiedIconArt.name;
    };
    //CombatDeathArt.name = rte.CombatArtDir() + CombatDeathArt.name;
  };

  sounds.Serialize(car);
  keyData.Serialize(car, version);
  specialItemData.Serialize(car, version);
  questData.Serialize(car, version);
  charData.Serialize(car, version);

  if (version < _VERSION_0661_)
  {
    // only versions prior to 0.661 stored
    // vault data, all others store it
    // in the saved game file.
    SaveGameVersion=version;
    vault[0].Serialize(car);
  }

  levelInfo.Serialize(car);

  if (car.IsStoring())
  {
    //startEquip.Serialize(car,version);
    //ClericEquip.Serialize(car,version);
    //FighterEquip.Serialize(car,version);
    //MagicUserEquip.Serialize(car,version);
    //ThiefEquip.Serialize(car,version);
    //RangerEquip.Serialize(car,version);
    //PaladinEquip.Serialize(car,version);
    //DruidEquip.Serialize(car,version);
    moneyData.Serialize(car,version);
    m_diffLvlData.Serialize(car);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);
    eventData.Serialize(car, version);
    eventData.SetEventSource(GlobalEventSrc);
    journalData.Serialize(car);
  }
  else
  {
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      if (version >= _VERSION_0575_)
        startEquip.Serialize(car,version);
      if (version >= _VERSION_0830_)
      {
        ClericEquip.Serialize(car,version);
        FighterEquip.Serialize(car,version);
        MagicUserEquip.Serialize(car,version);
        ThiefEquip.Serialize(car,version);
        RangerEquip.Serialize(car,version);
        PaladinEquip.Serialize(car,version);
        DruidEquip.Serialize(car,version);
      }
    };
    // We need to move this information to the Class database.
    // For each of the existing classes we will make
    // a list of the starting items:  The basic item plus
    // the items specific to each of that class' baseclasses.
    //    So we have loops:
    //      For each class
    //         For each baseclass in that class
    //             For each item in basic equipment
    //             For each item is specific equipment
    {
      int c, nc;
      nc = classData.GetCount(); // Number of classes
      for (c=0; c<nc; c++)       // For each 'c'lass
      {
        CLASS_DATA *pClass;
        int b, nb;
        pClass = classData.GetClass(c);
        pClass->m_startingEquipment = startEquip;  // Add the 'Base Items'.
        nb = pClass->GetCount(); // Number of baseclasses
        for (b=0; b<nb; b++)     // For each 'b'aseclass
        {
          const BASECLASS_ID *pBaseclassID;
          pBaseclassID = pClass->PeekBaseclassID(b);
          if (*pBaseclassID == "fighter")   pClass->m_startingEquipment.Append(FighterEquip);
          if (*pBaseclassID == "cleric")    pClass->m_startingEquipment.Append(ClericEquip);
          if (*pBaseclassID == "magicUser") pClass->m_startingEquipment.Append(MagicUserEquip);
          if (*pBaseclassID == "thief")     pClass->m_startingEquipment.Append(ThiefEquip);
          if (*pBaseclassID == "ranger")    pClass->m_startingEquipment.Append(RangerEquip);
          if (*pBaseclassID == "paladin")   pClass->m_startingEquipment.Append(PaladinEquip);
          if (*pBaseclassID == "druid")     pClass->m_startingEquipment.Append(DruidEquip);
        };
      };
    };

#endif
    if (version >= _VERSION_0642_)
      moneyData.Serialize(car,version);
    if (version >= _VERSION_0697_)
      m_diffLvlData.Serialize(car);
    if (version >= _VERSION_0681_)
      eventData.Serialize(car, version);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);

    if ((version >= _VERSION_06991_)&&(version<=_VERSION_0842_))
    {
      // load it, but don't do anything with it.
      SPELL_SPECIAL_DATA tempData;
      tempData.Serialize(car, version);
    }

    if (version >= _VERSION_0780_)
      journalData.Serialize(car);
  }  

  if (car.IsStoring())
  {
    fixSpellBook.Serialize(car, version);
  }
  else
  {
    if (version >= _VERSION_0909_)
    {
      fixSpellBook.Serialize(car, version);
    }
  }
}

#ifdef UAFEDITOR

extern const char *JKEY_VERSION;
const char *JKEY_DESIGNNAME= "designName";
const char *JKEY_STARTLEVEL= "startLevel";
const char *JKEY_STARTXY= "startXY";
const char *JKEY_STARTFACING= "startFacing";
extern const char *JKEY_STARTTIME;
const char *JKEY_STARTEXP= "startExp";
const char *JKEY_STARTEXPTYPE= "startExpType";
const char *JKEY_STARTEQUIP= "startEquip";
const char *JKEY_STARTPLATINUM= "startPlatinum";
const char *JKEY_STARTGEM= "startGem";
const char *JKEY_STARTJEWELRY= "startJewelry";   
const char *JKEY_DUNGEONTIMEDELTA= "dungeonTimeDelta";
const char *JKEY_DUNGEONSEARCHDELTA= "dungeonSearchDelta";
const char *JKEY_WILDERNESSTIMEDELTA= "wildernessTimeDelta";
const char *JKEY_WILDERNESSSEARCHDELTA= "wildernessSearchDelta";
const char *JKEY_AUTODARKEN= "autoDaken";
const char *JKEY_AUTODARKENAMOUNT= "autoAarkenAmount";
const char *JKEY_STARTDARKEN= "startDarken";
const char *JKEY_ENDDARKEN= "endDarken";
//const char *JKEY_MINPARTYSIZE= "minPartySize";
const char *JKEY_MAXPARTYSIZE= "maxPartySize";
const char *JKEY_ALLOWCHARCREATE= "allowCharCreate";   
const char *JKEY_DEADATZEROHP = "deadAtZeroHP";
const char *JKEY_MAPART= "mapArt";
const char *JKEY_ICONBGART= "IconBgArt";
const char *JKEY_BACKGROUNDART= "BackgroundArt";
const char *JKEY_LOGFONT="logFont";
const char *JKEY_TITLEDATA="titleData";
const char* JKEY_CREDITSDATA = "creditsData";
const char *JKEY_SMALLPIC="smallPic";
const char *JKEY_ICONPIC="iconPic";
const char *JKEY_HBARVPART = "hBarVPArt";
const char *JKEY_VBARVPART = "vBarVPArt";
const char *FRAMEVPART = "frameVPArt";
const char *JKEY_VBARCOMBART = "vBarCombArt";
const char *JKEY_HBARCOMBART = "hBarCombArt";
const char *JKEY_COMBATWALLART = "combatWallArt";
const char *JKEY_COMBATCURSORART = "combatCursorArt";
const char *JKEY_COMBATDEATHICONART = "combatDeathIcon";
const char *JKEY_COMBATPETRIFIEDICONART = "combatPetrifidIcon";
const char *JKEY_COMBATDEATHART = "combatDeathArt";
const char* JKEY_CHARVIEWFRAMEVPART = "charViewFrameVPArt";
const char *JKEY_CURSORART = "cursorArt";
const char *JKEY_SMALLPICS = "smallPics";
const char *JKEY_ICONPICS = "iconPics";
const char *JKEY_MINPCS = "minPCs";
const char *JKEY_MAXPCS = "maxPCs";
extern const char *JKEY_SPECIALKEYS;
extern const char *JKEY_SPECIALITEMS;



CONFIG_ITEM_STATUS GLOBAL_STATS::Export(JWriter& jw)
{
  POSITION pos;
  {
    int i[2];
    jw.NameAndValue(JKEY_VERSION, version);
    jw.NameAndValue(JKEY_DESIGNNAME, GetDesignName());
    // External level numbers start at 1.
    jw.NameAndValue(JKEY_STARTLEVEL, startLevel+1); 
    i[0] = startX;
    i[1] = startY;
    jw.NameAndValue(JKEY_STARTXY, i, 2);
    jw.NameAndValue(JKEY_STARTFACING, startFacing);
    jw.NameAndValue(JKEY_STARTTIME, startTime);
    jw.NameAndValue(JKEY_STARTEXP, startExp);
    jw.NameAndValue(JKEY_STARTEXPTYPE, startExpType);
    //jw.NameAndValue(JKEY_STARTEQUIP, UNUSED_startEquip);
    jw.NameAndValue(JKEY_STARTPLATINUM, startPlatinum);
    jw.NameAndValue(JKEY_STARTGEM, startGem);
    jw.NameAndValue(JKEY_STARTJEWELRY, startJewelry);   
    
    jw.NameAndValue(JKEY_DUNGEONTIMEDELTA, DungeonTimeDelta);
    jw.NameAndValue(JKEY_DUNGEONSEARCHDELTA, DungeonSearchTimeDelta);
    jw.NameAndValue(JKEY_WILDERNESSTIMEDELTA, WildernessTimeDelta);
    jw.NameAndValue(JKEY_WILDERNESSSEARCHDELTA, WildernessSearchTimeDelta);
    jw.NameAndValue(JKEY_AUTODARKEN, AutoDarkenViewport);
    jw.NameAndValue(JKEY_AUTODARKENAMOUNT, AutoDarkenAmount);
    jw.NameAndValue(JKEY_STARTDARKEN, StartDarken);
    jw.NameAndValue(JKEY_ENDDARKEN, EndDarken);

    //jw.NameAndValue(JKEY_MINPARTYSIZE, minPartySize);
    //jw.NameAndValue(JKEY_MAXPARTYSIZE, maxPartySize);
    jw.NameAndValue(JKEY_MINPCS, GetMinPCs());
    jw.NameAndValue(JKEY_MAXPCS, GetMaxPCs());
    jw.NameAndValue(JKEY_MAXPARTYSIZE, GetMaxPartySize());
  

    //jw.NameAndValue(JKEY_ALLOWCHARCREATE, allowCharCreate);    
    jw.NameAndValue(JKEY_ALLOWCHARCREATE, GetAllowCharCreate());    
    jw.NameAndValue(JKEY_DEADATZEROHP, GetDeadAtZeroHP());    
    
    //StripFilenamePath(MapArt);
    jw.NameAndValue(JKEY_MAPART, m_MapArt);

    if (strlen(logfont.lfFaceName) == 0)
      FillDefaultFontData("SYSTEM", 16, &logfont);

    jw.NameAndValue(JKEY_LOGFONT, (char *)&logfont, sizeof(logfont));
    //StripFilenamePath(IconBgArt);

    jw.NameAndValue(JKEY_ICONBGART, IconBgArt);
    //AddFolderToPath(IconBgArt, rte.IconArtDir());
    //StripFilenamePath(BackgroundArt);

    StripFilenamePath(BackgroundArt);
    jw.NameAndValue(JKEY_BACKGROUNDART, BackgroundArt);
    //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());

   //i = global_SmallPicImport.GetCount();
   //car << i;
   pos = global_SmallPicImport.GetHeadPosition();
   jw.StartArray(JKEY_SMALLPICS);
   while (pos != NULL)
   {
     PIC_DATA *pPic;
     pPic = global_SmallPicImport.GetNext(pos);
     //pPic->picType=SmallPicDib;
     //pPic->SetDefaults();
     //global_SmallPicImport.GetNext(pos).Serialize(car, version, rte.PicArtDir());
     jw.NextEntry();
     pPic->Export(jw,"");
   }
   jw.EndArray();

   //i = global_IconPicImport.GetCount();
   //car << i;
   pos = global_IconPicImport.GetHeadPosition();
   jw.StartArray(JKEY_ICONPICS);
   while (pos != NULL)
   {
     //global_IconPicImport.GetAt(pos).picType=IconDib;
     //global_IconPicImport.GetAt(pos).SetDefaults();
     //global_IconPicImport.GetNext(pos).Serialize(car, version, rte.PicArtDir());
     jw.NextEntry();
     global_IconPicImport.GetNext(pos)->Export(jw, "");
   };
   jw.EndArray();
   titleData.Export(jw);
   creditsData.Export(jw);
  }
  global_asl.Export(jw);
  HBarVPArt.Export(jw, JKEY_HBARVPART);
  VBarVPArt.Export(jw, JKEY_VBARVPART);
  FrameVPArt.Export(jw, FRAMEVPART);
  VBarCombArt.Export(jw, JKEY_VBARCOMBART);
  HBarCombArt.Export(jw,JKEY_HBARCOMBART);
  CombatWallArt.Export(jw, JKEY_COMBATWALLART);
  CombatCursorArt.Export(jw, JKEY_COMBATCURSORART);
  CombatDeathIconArt.Export(jw,JKEY_COMBATDEATHICONART);
  CombatPetrifiedIconArt.Export(jw, JKEY_COMBATPETRIFIEDICONART);
  CombatDeathArt.Export(jw, JKEY_COMBATDEATHART);
  CharViewFrameVPArt.Export(jw, JKEY_CHARVIEWFRAMEVPART);
  CursorArt.Export(jw, JKEY_CURSORART);
  sounds.Export(jw);
  keyData.Export(jw, JKEY_SPECIALKEYS);
  specialItemData.Export(jw, JKEY_SPECIALITEMS);
  questData.Export(jw);
  //We do this one character per file.  charData.Export(jw);
  //levelInfo.Serialize(ar);
  moneyData.Export(jw);
  m_diffLvlData.Export(jw);
  eventData.m_level = GLOBAL_ART;
  eventData.SetEventSource(GlobalEventSrc);
  eventData.Export(jw);
  eventData.SetEventSource(GlobalEventSrc);
  journalData.Export(jw);
  fixSpellBook.Export(jw);
  return CONFIG_STAT_ok;
}


CONFIG_ITEM_STATUS GLOBAL_STATS::Import(JReader& jr, BOOL quiet)
{
//  POSITION pos;
  {
    int i[2];
    int temp;
    jr.NameAndValue(JKEY_VERSION, version);
    jr.NameAndValue(JKEY_DESIGNNAME, designName);
    jr.NameAndValue(JKEY_STARTLEVEL, startLevel);
    startLevel--;  // The internal numbering scheme starts at zero.
    jr.NameAndValue(JKEY_STARTXY, i, 2);
    startX = i[0];
    startY = i[1];
    jr.NameAndValue(JKEY_STARTFACING, startFacing);
    jr.NameAndValue(JKEY_STARTTIME, startTime);
    jr.NameAndValue(JKEY_STARTEXP, startExp);
    jr.NameAndValue(JKEY_STARTEXPTYPE, startExpType);
    //jr.NameAndValue(JKEY_STARTEQUIP, UNUSED_startEquip);
    jr.NameAndValue(JKEY_STARTPLATINUM, startPlatinum);
    jr.NameAndValue(JKEY_STARTGEM, startGem);
    jr.NameAndValue(JKEY_STARTJEWELRY, startJewelry);   
    
    jr.NameAndValue(JKEY_DUNGEONTIMEDELTA, DungeonTimeDelta);
    jr.NameAndValue(JKEY_DUNGEONSEARCHDELTA, DungeonSearchTimeDelta);
    jr.NameAndValue(JKEY_WILDERNESSTIMEDELTA, WildernessTimeDelta);
    jr.NameAndValue(JKEY_WILDERNESSSEARCHDELTA, WildernessSearchTimeDelta);
    jr.NameAndValue(JKEY_AUTODARKEN, AutoDarkenViewport);
    jr.NameAndValue(JKEY_AUTODARKENAMOUNT, AutoDarkenAmount);
    jr.NameAndValue(JKEY_STARTDARKEN, StartDarken);
    jr.NameAndValue(JKEY_ENDDARKEN, EndDarken);

    //jr.NameAndValue(JKEY_MINPARTYSIZE, minPartySize);
    //jr.NameAndValue(JKEY_MAXPARTYSIZE, maxPartySize);
    jr.NameAndValue(JKEY_MINPCS, minPCs);
    jr.NameAndValue(JKEY_MAXPARTYSIZE, temp);
    SetMaxPartySize(temp);
    jr.NameAndValue(JKEY_MAXPCS, temp);
    SetMaxPCs(temp);

    {
      BOOL allowCharCreate;
      jr.NameAndValue(JKEY_ALLOWCHARCREATE, allowCharCreate);    
      SetAllowCharCreate(allowCharCreate);
    };
    
    {
      BOOL deadAtZeroHP;
      jr.NameAndValue(JKEY_DEADATZEROHP, deadAtZeroHP);    
      SetDeadAtZeroHP(deadAtZeroHP);
    };
    
    //StripFilenamePath(MapArt);
    jr.NameAndValue(JKEY_MAPART, m_MapArt);

    if (strlen(logfont.lfFaceName) == 0)
      FillDefaultFontData("SYSTEM", 16, &logfont);

    jr.NameAndValue(JKEY_LOGFONT, (char *)&logfont, sizeof(logfont));
    //StripFilenamePath(IconBgArt);

    jr.NameAndValue(JKEY_ICONBGART, IconBgArt);
    //AddFolderToPath(IconBgArt, rte.IconArtDir());
    //StripFilenamePath(BackgroundArt);

    StripFilenamePath(BackgroundArt);
    jr.NameAndValue(JKEY_BACKGROUNDART, BackgroundArt);
    //AddFolderToPath(BackgroundArt, rte.BackgroundArtDir());

   //i = global_SmallPicImport.GetCount();
   //car << i;
   //pos = global_SmallPicImport.GetHeadPosition();
   jr.StartArray(JKEY_SMALLPICS);
   global_SmallPicImport.RemoveAll();
   while (jr.Optional(), jr.NextEntry())
   {
     PIC_DATA pic;
     //pPic = &global_SmallPicImport.GetNext(pos);
     //pPic->picType=SmallPicDib;
     //pPic->SetDefaults();
     //global_SmallPicImport.GetNext(pos).Serialize(car, version, rte.PicArtDir());
     pic.Import(jr,NULL);
     global_SmallPicImport.AddTail(pic);
   }
   jr.EndArray();

   //i = global_IconPicImport.GetCount();
   //car << i;
   //pos = global_IconPicImport.GetHeadPosition();
   jr.StartArray(JKEY_ICONPICS);
   global_IconPicImport.RemoveAll();
   while (jr.Optional(), jr.NextEntry())
   {
     PIC_DATA pic;
     //global_IconPicImport.GetAt(pos).picType=IconDib;
     //global_IconPicImport.GetAt(pos).SetDefaults();
     //global_IconPicImport.GetNext(pos).Serialize(car, version, rte.PicArtDir());
     pic.Import(jr, NULL);
     global_IconPicImport.AddTail(pic);
   };
   jr.EndArray();
   titleData.Import(jr);
   creditsData.Import(jr);
  }
  global_asl.Import(jr);
  HBarVPArt.Import(jr, JKEY_HBARVPART);
  VBarVPArt.Import(jr, JKEY_VBARVPART);
  FrameVPArt.Import(jr, FRAMEVPART);
  VBarCombArt.Import(jr, JKEY_VBARCOMBART);
  HBarCombArt.Import(jr,JKEY_HBARCOMBART);
  CombatWallArt.Import(jr, JKEY_COMBATWALLART);
  CombatCursorArt.Import(jr, JKEY_COMBATCURSORART);
  CombatDeathIconArt.Import(jr,JKEY_COMBATDEATHICONART);
  CombatPetrifiedIconArt.Import(jr, JKEY_COMBATPETRIFIEDICONART);
  CombatDeathArt.Import(jr, JKEY_COMBATDEATHART);
  CharViewFrameVPArt.Import(jr, JKEY_CHARVIEWFRAMEVPART);
  CursorArt.Import(jr, JKEY_CURSORART);
  sounds.Import(jr);
  keyData.Import(jr, JKEY_SPECIALKEYS);
  specialItemData.Import(jr, JKEY_SPECIALITEMS);
  questData.Import(jr);
  charData.Import(quiet);
  //levelInfo.Serialize(ar);
  moneyData.Import(jr);
  m_diffLvlData.Import(jr);
  eventData.m_level = GLOBAL_ART;
  eventData.SetEventSource(GlobalEventSrc);
  eventData.Import(jr);
  eventData.SetEventSource(GlobalEventSrc);
  journalData.Import(jr);
  fixSpellBook.Import(jr);
  return CONFIG_STAT_ok;
}



#endif

/*
void GLOBAL_STATS::Serialize(CAR &ar)
{
  int i;
  POSITION pos;

  if (ar.IsStoring())
  {
    CString name(GetDesignName());
    ar << version;
    ar << name;
    ar << startLevel;
    ar << startX;
    ar << startY;
    ar << startFacing;
    ar << startTime;
    ar << startExp;
    ar << startExpType;
    ar << UNUSED_startEquip;
    ar << startPlatinum;
    ar << startGem;
    ar << startJewelry;   
    
    ar << DungeonTimeDelta;
    ar << DungeonSearchTimeDelta;
    ar << WildernessTimeDelta;
    ar << WildernessSearchTimeDelta;
    ar << AutoDarkenViewport;
    ar << AutoDarkenAmount;
    ar << StartDarken;
    ar << EndDarken;
    ar << minPartySize;
    ar << maxPartySize;
    ar << allowCharCreate;    
    
    StripFilenamePath(MapArt);
    AS(ar,MapArt);
    AddDesignFolderToPath(MapArt);

    if (strlen(logfont.lfFaceName) == 0)
      FillDefaultFontData("SYSTEM", 16, &logfont);
    //ar << font;
    //ar << fontSize;
    ar.Write(&logfont, sizeof(logfont));

    //StripFilenamePath(TitleBgArt);
    //AS(ar,TitleBgArt);
    //AddDesignFolderToPath(TitleBgArt);
     
    StripFilenamePath(IconBgArt);
    AS(ar,IconBgArt);
    AddDesignFolderToPath(IconBgArt);

    StripFilenamePath(BackgroundArt);
    AS(ar,BackgroundArt);
    AddDesignFolderToPath(BackgroundArt);

    StripFilenamePath(CreditsBgArt);
    AS(ar,CreditsBgArt);
    AddDesignFolderToPath(CreditsBgArt);

   i = m_SmallPicImport.GetCount();
   ar << i;

   pos = m_SmallPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     m_SmallPicImport.GetAt(pos).picType=SmallPicDib;
     m_SmallPicImport.GetAt(pos).SetDefaults();
     m_SmallPicImport.GetNext(pos).Serialize(ar, version);
   }

   i = m_IconPicImport.GetCount();
   ar << i;
   pos = m_IconPicImport.GetHeadPosition();
   while (pos != NULL)
   {
     m_IconPicImport.GetAt(pos).picType=IconDib;
     m_IconPicImport.GetAt(pos).SetDefaults();
     m_IconPicImport.GetNext(pos).Serialize(ar, version);
   }
   titleData.Serialize(ar);
  }
  else
  {
    titleData.Clear();

    ar >> version;
    DAS(ar,designName);
    ar >> startLevel;
    ar >> startX;
    ar >> startY;
    ar >> startFacing;
    ar >> startTime;
    ar >> startExp;
    if (version >= _VERSION_0770_)
      ar >> startExpType;
    ar >> UNUSED_startEquip;
    ar >> startPlatinum;
    ar >> startGem;
    ar >> startJewelry;

    if (version >= _VERSION_0574_)
    {
     ar >> DungeonTimeDelta;
     ar >> DungeonSearchTimeDelta;
     ar >> WildernessTimeDelta;
     ar >> WildernessSearchTimeDelta;
    }

    if (version >= _VERSION_0620_)
    {
      ar >> AutoDarkenViewport;
      ar >> AutoDarkenAmount;
      ar >> StartDarken;
      ar >> EndDarken;
    }

    if (version >= _VERSION_0575_)
    {
      ar >> minPartySize;
      ar >> maxPartySize;
      ar >> allowCharCreate;    
    }

    DAS(ar,MapArt);
    AddDesignFolderToPath(MapArt);

    if (version < _VERSION_0830_)
    {
      CString font="";
      BYTE fontSize=16;
      ar >> font;
      if (version >= _VERSION_0681_)
        ar >> fontSize;
      FillDefaultFontData(font, fontSize, &logfont);
    }
    else
    {
      ar.Read(&logfont, sizeof(logfont));
    }

    if (version < _VERSION_0800_)
    {
      CString TitleBgArt("");
      DAS(ar,TitleBgArt);
      AddDesignFolderToPath(TitleBgArt);
      TITLE_SCREEN data;
      data.TitleBgArt = TitleBgArt;
      data.DisplayBy = TITLE_SCREEN::tsFadeIn;
      data.UseTrans = FALSE;
      titleData.AddTitle(data);
    }

    if (version >= _VERSION_0660_)
    {
      DAS(ar,IconBgArt);
      AddDesignFolderToPath(IconBgArt);

      DAS(ar,BackgroundArt);
      AddDesignFolderToPath(BackgroundArt);
    }

    if (version >= _VERSION_0566_)
    {
      DAS(ar,CreditsBgArt);
      AddDesignFolderToPath(CreditsBgArt);
    }

    int count;
    PIC_DATA data;

    ar >> count;
    for (i=0;i<count;i++)
    {
      data.Serialize(ar, version);
      m_SmallPicImport.AddTail(data);
    }

    data.Clear();
    ar >> count;
    CString icon;
    for (i=0;i<count;i++)
    {
      if (version < _VERSION_0640_)
      {        
        DAS(ar,icon);
//        ar >> icon;
//        if (icon == "*") icon = "";
        AddDesignFolderToPath(icon);
        data.filename = icon;
        data.picType=IconDib;
        data.SetDefaults();
      }
      else
      {
        data.Serialize(ar, version);
      }
      m_IconPicImport.AddTail(data);
    }
    if (version >= _VERSION_0800_)
      titleData.Serialize(ar);
  }

  global_attributes.Serialize(ar);
  HBarVPArt.Serialize(ar);
  VBarVPArt.Serialize(ar);
  FrameVPArt.Serialize(ar);
  VBarCombArt.Serialize(ar);
  HBarCombArt.Serialize(ar);
  CombatWallArt.Serialize(ar);
  CombatCursorArt.Serialize(ar);
  CombatDeathIconArt.Serialize(ar);
  CombatDeathArt.Serialize(ar);
  if (version >= _VERSION_0575_)
    CursorArt.Serialize(ar, version);
  sounds.Serialize(ar);
  keyData.Serialize(ar, version);
  specialItemData.Serialize(ar, version);
  questData.Serialize(ar, version);
  charData.Serialize(ar, version);
  
  // only versions prior to 0.661 stored
  // vault data, all others store it
  // in the saved game file.
  if (ar.IsStoring())
  {   
    ASS ERT(FALSE);
    ar << MAX_GLOBAL_VAULTS;
    for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
      vault[i].Serialize(ar);
  }
  else
  {
    ASS ERT(FALSE);
    // if loading old design with only one vault
    if (version < _VERSION_0910_)
    {
      vault[0].Serialize(ar);
    }
    else
    {
      int numvaults;
      ar >> numvaults;
      if (numvaults != MAX_GLOBAL_VAULTS)
      {
        TRACE("*** ERROR: Invalid numvaults read from design file\n");
        ASS ERT(FALSE);
        if (numvaults > MAX_GLOBAL_VAULTS)
          numvaults = MAX_GLOBAL_VAULTS;
      }
      for (int i=0;i<numvaults;i++)
        vault[i].Serialize(ar);
    }
  }
  
  levelInfo.Serialize(ar);

  if (ar.IsStoring())
  {
    startEquip.Serialize(ar,version);
    ClericEquip.Serialize(ar,version);
    FighterEquip.Serialize(ar,version);
    MagicUserEquip.Serialize(ar,version);
    ThiefEquip.Serialize(ar,version);
    RangerEquip.Serialize(ar,version);
    PaladinEquip.Serialize(ar,version);
    DruidEquip.Serialize(ar,version);
    moneyData.Serialize(ar,version);
    m_diffLvlData.Serialize(ar);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);
    eventData.Serialize(ar, version);
    eventData.SetEventSource(GlobalEventSrc);
    //m_spellSpecialData.Serialize(ar);
    journalData.Serialize(ar);
  }
  else
  {
    if (version >= _VERSION_0575_)
      startEquip.Serialize(ar,version);
    if (version >= _VERSION_0830_)
    {
      ClericEquip.Serialize(ar,version);
      FighterEquip.Serialize(ar,version);
      MagicUserEquip.Serialize(ar,version);
      ThiefEquip.Serialize(ar,version);
      RangerEquip.Serialize(ar,version);
      PaladinEquip.Serialize(ar,version);
      DruidEquip.Serialize(ar,version);
    }
    if (version >= _VERSION_0642_)
      moneyData.Serialize(ar,version);
    if (version >= _VERSION_0697_)
      m_diffLvlData.Serialize(ar);
    if (version >= _VERSION_0681_)
      eventData.Serialize(ar, version);
    eventData.m_level = GLOBAL_ART;
    eventData.SetEventSource(GlobalEventSrc);

    if ((version >= _VERSION_06991_)&&(version<=_VERSION_0842_))
    {
      // load it, but don't do anything with it.
      SPELL_SPECIAL_DATA tempData;
      tempData.Serialize(ar);
    }

    if (version >= _VERSION_0780_)
      journalData.Serialize(ar);
  }  

  if (ar.IsStoring())
  {
    fixSpellBook.Serialize(ar, version);
  }
  else
  {
    if (version >= _VERSION_0909_)
    {
      fixSpellBook.Serialize(ar, version);
    }
  }
}
*/


void GLOBAL_STATS::Save(CAR& car) 
{
  global_asl.Save(car, "GLOBAL_STATS_ATTRIBUTES");
  combatTreasure.Serialize(car, SaveGameVersion);
}

void GLOBAL_STATS::Restore(CArchive& ar) 
{
#ifdef mapaNames
	temp_asl.Serialize(ar);
#else
  temp_asl.Serialize(ar, "GLOBAL_STATS_ATTRIBUTES");
#endif
}

void GLOBAL_STATS::Restore(CAR& car) 
{
#ifdef mapaNames
	temp_asl.Serialize(car);
#else
  temp_asl.Serialize(car, "GLOBAL_STATS_ATTRIBUTES");
#endif
  combatTreasure.Serialize(car, SaveGameVersion);
}

void GLOBAL_STATS::CommitRestore(void) 
{
	global_asl.CommitRestore(&temp_asl);
	temp_asl.Clear();
}

#ifdef UAFEDITOR
void GLOBAL_STATS::CrossReferenceEquipment(ITEM_LIST *pItemList, const char *etype, CR_LIST *pCRList)
{
  CR_REFERENCE CRReference;
  CRReference.m_referenceType = CR_TYPE_global;
  CRReference.m_referenceName = etype;
  CRReference.m_referenceName += " Starting Equipment";
  pItemList->CrossReference(pCRList, &CRReference);
}


void GLOBAL_STATS::CrossReference(CR_LIST *pCRList)
{
  int i;
  CR_REFERENCE CRReference;
  CRReference.m_referenceName = "Global Stats";
  CRReference.m_referenceType = CR_TYPE_global;
  pCRList->CR_AddPicReference(m_MapArt, &CRReference);
  pCRList->CR_AddPicReference(IconBgArt, &CRReference);
  pCRList->CR_AddPicReference(BackgroundArt, &CRReference);
  CrossReferenceEquipment(&startEquip,     "Basic", pCRList);
  CrossReferenceEquipment(&ClericEquip,    "Cleric", pCRList);
  CrossReferenceEquipment(&FighterEquip,   "Fighter", pCRList);
  CrossReferenceEquipment(&MagicUserEquip, "Magic User", pCRList);
  CrossReferenceEquipment(&ThiefEquip,     "Thief", pCRList);
  CrossReferenceEquipment(&RangerEquip,    "Ranger", pCRList);
  CrossReferenceEquipment(&PaladinEquip,   "Paladin", pCRList);
  CrossReferenceEquipment(&DruidEquip,     "Druid", pCRList);
  specialItemData.CrossReference(pCRList, CR_TYPE_specialItem);
  keyData.CrossReference(pCRList, CR_TYPE_specialKey);
  charData.CrossReference(pCRList);
  raceData.CrossReference(pCRList);
  abilityData.CrossReference(pCRList);
  baseclassData.CrossReference(pCRList);
//  traitData.CrossReference(pCRList);
  spellgroupData.CrossReference(pCRList);
  classData.CrossReference(pCRList);
  spellData.CrossReference(pCRList);
  for (i=0; i<MAX_GLOBAL_VAULTS; i++)
  {
    vault[i].CrossReference(pCRList);
  };
  levelInfo.CrossReference(pCRList, &CRReference);
  {
    CR_EVENT_INFO crEI;
    crEI.m_anchorLocation = "(global)";
    crEI.m_pCRList = pCRList;
    eventData.CrossReference(&crEI);
  };
  titleData.CrossReference(pCRList, &CRReference);
  creditsData.CrossReference(pCRList, &CRReference);
  fixSpellBook.CrossReference(pCRList);
  sounds.CrossReference(pCRList);
}

#endif


GLOBAL_STATS::GLOBAL_STATS() : keyData(MAX_SPECIAL_KEYS),
// ASLs named "GLOBAL_STATS_ATTRIBUTES"
                 global_asl(),
                 temp_asl(),
                 specialItemData(MAX_SPECIAL_ITEMS),
// prs 20191220                   WhiteFont(-1),YellowFont(-1),OrangeFont(-1),RedFont(-1),GreenFont(-1),BlueFont(-1),HighlightFont(-1),KeyboardFont(-1),
                 fontLib(), HighlightFont(-1), KeyboardFont(-1),
                 MapArtSurf(-1),BackgroundSurf(-1),Custom1Font(-1),MagicAdjFont(-1),KeyboardHighlightFont(-1)
{ 
   version = PRODUCT_VER;
   SaveGameVersion = 0.0;
   startLevel = 0;
   currLevel = 0;
   startX = 0;
   startY = 0;
   startFacing = FACE_EAST;
   startTime = 800; // 24-hour format
   startExp = 0;
   startExpType=START_EXP_VALUE;
   //UNUSED_startEquip = 0;
   startPlatinum = START_PLATINUM;
   startGem = 0;
   startJewelry = 0;

   minPCs = 1;
   SetMaxPCs(MAX_PARTY_PCS);
   SetMaxPartySize(MAX_PARTY_PCS+2);
   //minPartySize = 1;
   //maxPartySize = MAX_PARTY_CHARACTERS;

   flags = 0;
   //allowCharCreate = TRUE;
   SetAllowCharCreate(TRUE);
   SetDeadAtZeroHP(FALSE);
   useAreaView = FALSE;
   DungeonTimeDelta = 1;
   DungeonSearchTimeDelta = DungeonTimeDelta*10;
   WildernessTimeDelta = 12 * 60; // 12 hours
   WildernessSearchTimeDelta = 18 * 60; // 18 hours
   AutoDarkenViewport=FALSE;
   AutoDarkenAmount=256;
   StartDarken=0;
   EndDarken=0;
   designName="";
   m_MapArt="";
   UseAVZones=FALSE;
   //font = "System";
   //fontSize=16;
   FillDefaultFontData("SYSTEM", 16, &logfont);

   //TitleBgArt="";
   titleData.Clear();
   creditsData.Clear();
   IconBgArt="";
   BackgroundArt="";
   
   MapArtSurf = -1; 
   BackgroundSurf=-1;

/*   
  startEquip.Clear();
  ClericEquip.Clear();
  FighterEquip.Clear();
  MagicUserEquip.Clear();
  ThiefEquip.Clear();
  RangerEquip.Clear();
  PaladinEquip.Clear();
  DruidEquip.Clear();
*/

  charData.Clear();
  ReleaseSurfaces();
  keyData.Clear();
  specialItemData.Clear();
  questData.Clear();
#ifndef UAFEDITOR
  sounds.Clear(TRUE);
#endif
  for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
    vault[i].Clear();

  levelInfo.Clear(); 
  global_asl.Clear();
  moneyData.Clear();
  m_diffLvlData.Clear();
  eventData.Clear();
  eventData.SetEventSource(GlobalEventSrc);
  journalData.Clear();
  fixSpellBook.Clear();
}

void GLOBAL_STATS::Clear(BOOL ctor, BOOL npcclear)
{
   version = PRODUCT_VER;
   startLevel = 0;
   currLevel = 0;
   startX = 0;
   startY = 0;
   startFacing = FACE_EAST;
   startTime = 800; // 24-hour format
   startExp = 0;
   startExpType=START_EXP_VALUE;
   //UNUSED_startEquip = 0;
   startPlatinum = START_PLATINUM;
   startGem = 0;
   startJewelry = 0;

   //minPartySize = 1;
   //maxPartySize = MAX_PARTY_CHARACTERS;
   minPCs = 1;
   SetMaxPCs(MAX_PARTY_PCS);
   SetMaxPartySize(MAX_PARTY_PCS+2);

   flags = 0;
   //allowCharCreate = TRUE;
   SetAllowCharCreate(TRUE);
   SetDeadAtZeroHP(FALSE);
   useAreaView = FALSE;
   DungeonTimeDelta = 1;
   DungeonSearchTimeDelta = DungeonTimeDelta*10;
   WildernessTimeDelta = 12 * 60; // 12 hours
   WildernessSearchTimeDelta = 18 * 60; // 18 hours
   AutoDarkenViewport=FALSE;
   AutoDarkenAmount=256;
   StartDarken=0;
   EndDarken=0;
   designName="";
   m_MapArt="";
   UseAVZones=FALSE;
   //font = "";
   //fontSize=16;
   FillDefaultFontData("SYSTEM", 16, &logfont);
   //TitleBgArt="";
   titleData.Clear();
   creditsData.Clear();
   IconBgArt="";
   BackgroundArt="";

#ifdef UAFEDITOR
   if (!ctor)
   {
     designName="Untitled";
     //font = "System"; // required by game engine
     m_MapArt = AREAVIEWART;//.Format("%s%s", global_editorResourceDir, AREAVIEWART); // required by game engine
     IconBgArt = ede.TemplateBackgroundArtDir() + DEFAULT_IB;//.Format("%s%s", global_editorResourceDir, DEFAULT_IB);
     CString TitleBgArt("");
     TitleBgArt = ede.TemplateBackgroundArtDir() + DEFAULT_TITLE_BG;//.Format("%s%s", global_editorResourceDir, DEFAULT_TITLE_BG);
     titleData.SetDefault(TitleBgArt);
     CString CreditsBgArt("");
     CreditsBgArt = ede.TemplateBackgroundArtDir() + DEFAULT_CREDITS_BG;
     creditsData.SetDefault(CreditsBgArt);
   }
#else
   if (!ctor)
   {
     // this stuff should be in the design data files, but not 
     // all of the older file formats include it.
     //font = "System";
     m_MapArt = AREAVIEWART;
     IconBgArt = DEFAULT_IB;
     CString TitleBgArt("");
     TitleBgArt = DEFAULT_TITLE_BG;
     titleData.SetDefault(TitleBgArt);
     CString CreditsBgArt(""); 
     CreditsBgArt = DEFAULT_CREDITS_BG;
     creditsData.SetDefault(CreditsBgArt);
   }
#endif

/*   
  startEquip.Clear();
  ClericEquip.Clear();
  FighterEquip.Clear();
  MagicUserEquip.Clear();
  ThiefEquip.Clear();
  RangerEquip.Clear();
  PaladinEquip.Clear();
  DruidEquip.Clear();
*/

  if (npcclear) charData.Clear();
  ReleaseSurfaces();

  keyData.Clear();
  specialItemData.Clear();
  questData.Clear();
  sounds.Clear(ctor);
  
  for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
    vault[i].Clear();

  levelInfo.Clear();
  global_asl.Clear();
#ifdef UAFEDITOR
  CString vertext;
  vertext.Format("%1.4f", version);
  global_asl.Update("RunAsVersion", vertext, ASLF_EDITOR);
  global_asl.Update("GuidedTourVersion", vertext, ASLF_EDITOR);
  global_asl.Update("ItemUseEventVersion", vertext, ASLF_EDITOR);
  global_asl.Update("SpecialItemKeyQtyVersion", vertext, ASLF_EDITOR);
#endif
  moneyData.Clear();
  m_diffLvlData.Clear();
  eventData.Clear();
  eventData.SetEventSource(GlobalEventSrc);
  moneyData.SetUADefaults();
  m_diffLvlData.SetUADefaults();
  //m_spellSpecialData.Clear();
  journalData.Clear();
  fixSpellBook.Clear();
}

void GLOBAL_STATS::AddMoney(int which, MONEY_SACK &src) 
{ 
  if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
    vault[which].money.Add(src); 
}

void GLOBAL_STATS::TransferMoney(int which, MONEY_SACK &src) 
{ 
  if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
    vault[which].money.Transfer(src); 
}

double GLOBAL_STATS::VaultTotal(int which)
{
  double total = 0.0;

  if (which < 0)
  {
    // check all vaults
    for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
      total += vault[i].money.Total();
  }
  else
  {
    if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
      total = vault[which].money.Total();
  }

  return total;
}

BOOL GLOBAL_STATS::VaultHasMoney(int which)
{
  if (which < 0)
  {
    // check all vaults
    for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
    {
      if (!vault[i].money.IsEmpty())
        return TRUE;
    }
  }
  else
  {
    if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
      return (!vault[which].money.IsEmpty());
  }

  return FALSE;
}

void GLOBAL_STATS::UpdateMoneyInVault(int which, itemClassType type, int qty)
{
  if ((which < 0) || (which >= MAX_GLOBAL_VAULTS))
    return;

  if (qty == 0)
    return;

  switch (type)
  {
  case GemType:
    if (qty > 0)
    {
      for (int i=0;i<qty;i++) 
        vault[which].money.AddGem();
    }
    else
    {
      vault[which].money.RemoveMultGems(abs(qty));
    }
    break;

  case JewelryType:
    if (qty > 0)
    {
      for (int i=0;i<qty;i++) 
        vault[which].money.AddJewelry();
    }
    else
    {
      vault[which].money.RemoveMultJewelry(abs(qty));
    }
    break;

  default:
    if (qty < 0)
      vault[which].money.Subtract(type, abs(qty));
    else
      vault[which].money.Add(type, qty);
    break;
  }
}

BOOL GLOBAL_STATS::AddItemToVault(int which, ITEM item)
{
  if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
    return (vault[which].items.AddItem(item));

  return FALSE;
}

BOOL GLOBAL_STATS::RemoveItemFromVault(int which, int index, int qty)
{
  if ((which >= 0) && (which < MAX_GLOBAL_VAULTS))
    return (vault[which].items.deleteItem(index, qty));

  return FALSE;
}

void GLOBAL_STATS::SaveSounds()
{
  ::SaveSound(sounds.CharHit,    GLOBAL_SOUND, rte.SoundDir());
  ::SaveSound(sounds.CharMiss,   GLOBAL_SOUND, rte.SoundDir());
  ::SaveSound(sounds.PartyBump,  GLOBAL_SOUND, rte.SoundDir());
  ::SaveSound(sounds.PartyStep,  GLOBAL_SOUND, rte.SoundDir());
  ::SaveSound(sounds.DeathMusic, GLOBAL_SOUND, rte.SoundDir());
  //saveSound(sounds.IntroMusic, GLOBAL_SOUND);

  POSITION pos = sounds.IntroMusic.sounds.GetHeadPosition();
  while (pos!=NULL)
    ::SaveSound(sounds.IntroMusic.sounds.GetNext(pos), GLOBAL_SOUND, rte.SoundDir());

  pos = sounds.CreditsMusic.sounds.GetHeadPosition();
  while (pos != NULL)
      ::SaveSound(sounds.CreditsMusic.sounds.GetNext(pos), GLOBAL_SOUND, rte.SoundDir());

  pos = sounds.CampMusic.sounds.GetHeadPosition();
  while (pos!=NULL)
    ::SaveSound(sounds.CampMusic.sounds.GetNext(pos), GLOBAL_SOUND, rte.SoundDir());

  levelInfo.SaveSounds();
}

void GLOBAL_STATS::SaveArt()
{ 
  POSITION pos;
  int i,n;
   // save common pics (frame bars, combat common pics, etc)
   ::SaveArt(HBarVPArt.name             ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir());
   ::SaveArt(VBarVPArt.name             ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir()); 
   ::SaveArt(FrameVPArt.name            ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir());
   ::SaveArt(VBarCombArt.name           ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir());
   ::SaveArt(HBarCombArt.name           ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir());
   ::SaveArt(CombatCursorArt.name       ,CommonDib      , GLOBAL_ART, TRUE, rte.CursorArtDir());
   ::SaveArt(CombatDeathIconArt.name    ,CommonDib      , GLOBAL_ART, TRUE, rte.CombatArtDir());
   ::SaveArt(CombatPetrifiedIconArt.name,CommonDib      , GLOBAL_ART, TRUE, rte.CombatArtDir());
   ::SaveArt(CombatDeathArt.name        ,CommonDib      , GLOBAL_ART, TRUE, rte.CombatArtDir());
   ::SaveArt(CharViewFrameVPArt.name    ,CommonDib      , GLOBAL_ART, TRUE, rte.WindowArtDir());
   ::SaveArt(CursorArt.filename         ,CommonDib      , GLOBAL_ART, TRUE, rte.CursorArtDir());
   ::SaveArt(m_MapArt                   ,CommonDib      , GLOBAL_ART, TRUE, rte.MapArtDir());
   ::SaveArt(IconBgArt                  ,CommonDib      , GLOBAL_ART, TRUE, rte.IconArtDir());   ::SaveArt(BackgroundArt              ,TransBufferDib , GLOBAL_ART, TRUE, rte.BackgroundArtDir());

   // save pre-gen character pics (smallpics, icons, sprites)

   WriteDebugString("charData.SaveArt\n");
   //pos = charData.GetHeadPosition();
   //while (pos != NULL)
   //  charData.GetNext(pos).SaveArt();
   n = charData.GetCount();
   for (i=0; i<n; i++)
   {
     charData.GetCharacter(i)->SaveArt();
   };

   WriteDebugString("SmallPicImport.SavePicData\n");
   pos = global_SmallPicImport.GetHeadPosition();
   while (pos != NULL)
     global_SmallPicImport.GetNext(pos)->SavePicData(GLOBAL_ART);

   WriteDebugString("IconPicImport.SavePicData\n");
   pos = global_IconPicImport.GetHeadPosition();
   while (pos != NULL)
     global_IconPicImport.GetNext(pos)->SavePicData(GLOBAL_ART);
   
   pos = titleData.Titles.GetHeadPosition();
   while (pos != NULL)
     ::SaveArt(titleData.Titles.GetNext(pos).TitleBgArt, TitleDib, GLOBAL_ART, TRUE, rte.TitleArtDir());

   pos = creditsData.Titles.GetHeadPosition();
   while (pos != NULL)
       ::SaveArt(creditsData.Titles.GetNext(pos).TitleBgArt, TitleDib, GLOBAL_ART, TRUE, rte.TitleArtDir());

   eventData.saveUsedEventArt(GLOBAL_ART);
}

BOOL GLOBAL_STATS::ConfirmUsedLevels()
{
  BOOL success = TRUE;
  int i;

  for (i=0;((i<MAX_LEVELS) && (success));i++)
  {
    if (levelInfo.stats[i].used)
    {
      if (!levelInfo.stats[i].level_name.IsEmpty())
      {
        success = levelExists(i);

        if (!success)
        {
           WriteDebugString("Used level %i does not exist: %s\n", i,levelInfo.stats[i].level_name);
           levelInfo.stats[i].used = FALSE;
        }
      }
      else 
      {
        WriteDebugString("level %i used=TRUE when level_name is empty: %s\n", i,levelInfo.stats[i].level_name);
        levelInfo.stats[i].used = FALSE;
      }
    }
  }

  return success;
}

bool foundFont;

int CALLBACK EnumFontFamExProc(
                                const struct tagLOGFONTA *,
                                const struct tagTEXTMETRICA *,
                                unsigned long,
                                long)


//
//  const ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
//  const NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
//  unsigned long FontType,             // type of font
//  long lParam             // application-defined data
{
  foundFont = true;
  return 1;
}



// prs 20191221 BOOL GLOBAL_STATS::LoadFonts(COLORREF green, COLORREF yellow, COLORREF red, COLORREF blue, COLORREF orange, COLORREF keyb, COLORREF BackColor, COLORREF TransColor, COLORREF Custom)
BOOL GLOBAL_STATS::LoadSystemFonts(COLORREF keyb, 
                                   COLORREF BackColor, 
                                   COLORREF TransColor, 
                                   COLORREF Custom)
{
#ifdef UAFEngine
  ReleaseFonts();
 
  
  {
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    foundFont = false;
    ::EnumFontFamiliesEx(   dc.GetSafeHdc(),              // handle to device context
                            &logfont,     // LPLOGFONT lpLogfont,  // pointer to logical font information
                            EnumFontFamExProc,
                                    // pointer to callback function
                            NULL,   //LPARAM lParam,        // application-supplied data
                            0);     //DWORD dwFlags         // reserved; must be zero
    if (!foundFont)
    {
      CString msg;
      msg.Format("Cannot find specified font named %s", logfont.lfFaceName);
      WriteDebugString(msg+'\n');
      MsgBoxInfo(msg, "Warning");
    };
  };

  
  
  //fontSize = max(8, fontSize);
  WriteDebugString("Creating fonts from %s\n", logfont.lfFaceName);
  /*   prs 20191220
  //WhiteFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, White, BackColor, TransColor);
  WhiteFont = GraphicsMgr.CreateBitmappedFont(&logfont, White, BackColor, TransColor);
  if (WhiteFont < 0) return FALSE;
  
  //YellowFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, yellow, BackColor, TransColor);
  YellowFont = GraphicsMgr.CreateBitmappedFont(&logfont, yellow, BackColor, TransColor);
  if (YellowFont < 0) return FALSE;
  
  OrangeFont = GraphicsMgr.CreateBitmappedFont(&logfont, orange, BackColor, TransColor);
  if (OrangeFont < 0) return FALSE;
  
  //RedFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, red, BackColor, TransColor);
  RedFont = GraphicsMgr.CreateBitmappedFont(&logfont, red, BackColor, TransColor);
  if (RedFont < 0) return FALSE;
  
  //GreenFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, green, BackColor, TransColor);
  GreenFont = GraphicsMgr.CreateBitmappedFont(&logfont, green, BackColor, TransColor);
  if (GreenFont < 0) return FALSE;
  
  //BlueFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, blue, BackColor, TransColor);
  BlueFont = GraphicsMgr.CreateBitmappedFont(&logfont, blue, BackColor, TransColor);
  if (BlueFont < 0) return FALSE;
  */

  {
// prs 20191227    int font;
    LOGFONT tempFont;
    tempFont = logfont;
    /*  prs 20191227
    for (font = 0; font < 10; font++)
    {
      availableFonts[font].LoadFont(tempFont, BackColor, TransColor);
    };
    */
    // PRS 20200208 fontLib.LoadFonts(tempFont, BackColor, TransColor);


  };

  //Custom1Font = GraphicsMgr.CreateBitmappedFont(font, fontSize, Custom, BackColor, TransColor);
  Custom1Font = GraphicsMgr.CreateBitmappedFont(&logfont, Custom, BackColor, TransColor);
  if (Custom1Font < 0) return FALSE;

  //KeyboardFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, keyb, BackColor, TransColor);
  KeyboardFont = GraphicsMgr.CreateBitmappedFont(&logfont, keyb, BackColor, TransColor);
  if (KeyboardFont < 0) return FALSE;
  
  //KeyboardHighlightFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, keyb, White, red);
// prs 20191221  KeyboardHighlightFont = GraphicsMgr.CreateBitmappedFont(&logfont, keyb, White, red);
  KeyboardHighlightFont = GraphicsMgr.CreateBitmappedFont(
                                  &logfont,
                                  keyb,
    // prs 20191227                      globalData.availableFonts[0].color[whiteColor], 
                                  globalData.fontLib.GetColor(0, whiteColor),
                                  globalData.fontLib.GetColor(0, redColor));
// prs 20191227                      globalData.availableFonts[0].color[redColor]);

  if (KeyboardHighlightFont < 0) return FALSE;

  //HighlightFont = GraphicsMgr.CreateBitmappedFont(font, fontSize, Black, White, red);
// prs 20191221  HighlightFont = GraphicsMgr.CreateBitmappedFont(&logfont, Black, White, RGB(255,0,0));
  HighlightFont = GraphicsMgr.CreateBitmappedFont(
    &logfont,
    globalData.fontLib.GetColor(0, blackColor),
    globalData.fontLib.GetColor(0, whiteColor),
    globalData.fontLib.GetColor(0, redColor));
                      // prs 20191227
                      //globalData.availableFonts[0].color[blackColor],
                      //globalData.availableFonts[0].color[whiteColor],
                      //globalData.availableFonts[0].color[redColor]);
  if (HighlightFont < 0) return FALSE;
  
  MagicAdjFont = GraphicsMgr.CreateBitmappedFont(&logfont, RGB(255,128,0), BackColor, TransColor);
  if (MagicAdjFont < 0) return FALSE; 

#endif
  return TRUE;
}

void GLOBAL_STATS::ReleaseSurfaces()
{
  if (MapArtSurf >= 0)
    GraphicsMgr.ReleaseSurface(MapArtSurf);
  MapArtSurf = -1; 
  if (BackgroundSurf >= 0)
    GraphicsMgr.ReleaseSurface(BackgroundSurf);

  BackgroundSurf = -1; 
  ReleaseFonts();
}



/* prs 20191227
void AVAIL_FONT::ReleaseFonts(void)
{
  int colorNum;
  for (colorNum = whiteColor; colorNum <= silverColor; colorNum++)
  {
    if (fontNum[colorNum] >= 0)
    {
      GraphicsMgr.ReleaseSurface(fontNum[colorNum]);
    };
    fontNum[colorNum] = -1;
  };
};

BOOL AVAIL_FONT::LoadFont(LOGFONT& logFont, COLORREF backColor, COLORREF transColor)
{
  int colorNum;
  for (colorNum = whiteColor; colorNum <= silverColor; colorNum++)
  {
    strcpy_s(logFont.lfFaceName, 32, fontName);
    //      long int origFont; 
    //      origFont = availableFonts[font].fontNum;
    //      availableFonts[font].fontNum = -1;
    fontNum[colorNum] = GraphicsMgr.CreateBitmappedFont(
                                    &logFont, 
                                    color[colorNum], 
                                    backColor, 
                                    transColor);
    if (fontNum[colorNum] < 0)
    {
      fontNum[colorNum] = -1;
      return FALSE;
    };
  };
  return TRUE;
}
*/

void FONT_DESC::ReleaseFont(void)
{
  if (m_fontHandle >= 0)
  {
    GraphicsMgr.ReleaseSurface(m_fontHandle);
  };
  m_fontHandle = -1;
}

void FONT_DESC::LoadFont(void)
{
  if (m_fontHandle >= 0)
  {
    die(0x44a0df);
  }
  if (m_fontHandle == -2)
  {
    return;
  };
  LOGFONT temp;
  temp = globalData.logfont;
  temp.lfItalic = m_italic;
  temp.lfWeight = m_bold?999:001;
  strcpy_s(temp.lfFaceName, 32, (LPCSTR)m_fontName);
#ifdef UAFEngine
  m_fontHandle = GraphicsMgr.CreateBitmappedFont(
                     &temp, 
                     m_color, 
                     m_backColor, 
                     m_transColor);
  if (m_fontHandle < 0)
  {
    m_fontHandle = -2;
  };
#else
  die(0x5a9bc1);
#endif
}

void FONT_DESC::operator=(const FONT_DESC& src)
{
  m_fontName   = src.m_fontName;
  m_fontHandle = src.m_fontHandle;
  m_color      = src.m_color;
  m_backColor  = src.m_backColor;
  m_transColor = src.m_transColor;
  m_italic     = src.m_italic;
  m_bold       = src.m_bold;
}

void FONT_LIBRARY::operator =(const FONT_LIBRARY& src)
{
  int i, numFont;
  numFont = sizeof(m_fonts) / sizeof(m_fonts[0]);
  for (i = 0; i < numFont; i++)
  {
    m_fonts[i] = src.m_fonts[i];
  };
}

COLORREF FONT_LIBRARY::GetColor(int baseFontNumber, FONT_COLOR_NUM colorNum)
{
  return m_fonts[baseFontNumber + colorNum].m_color;
}

long int FONT_LIBRARY::CheckFont(int baseFontNumber, 
                                 FONT_COLOR_NUM colorNum,
                                 BOOL custom)
{
  int fnum;
  fnum = baseFontNumber + colorNum;
  if ((fnum < 0) || (fnum >= NUMTEXTFONT))
  {
    return m_fonts[0].m_fontHandle;
  };
  fnum = custom ? fnum + 100 : fnum;
  if (m_fonts[fnum].m_fontHandle == -1)
  {
    m_fonts[fnum].LoadFont();
  };
  if (m_fonts[fnum].m_fontHandle >= 0)
  {
    return m_fonts[fnum].m_fontHandle;
  };
  return m_fonts[0].m_fontHandle;
}

long int FONT_LIBRARY::GetFont(int baseFontNumber, 
                               FONT_COLOR_NUM colorNum,
                               BOOL custom)
{
  int fontHandle;
  // check errors; load font.
  fontHandle = CheckFont(baseFontNumber, colorNum, custom);  
  return fontHandle;
}

//void FONT_LIBRARY::LoadFonts(LOGFONT& logFont, COLORREF backColor, COLORREF transColor)
void FONT_LIBRARY::LoadFonts(void)
{
  SetFontColor(0, whiteColor,        RGB(255, 255, 255));
  SetFontColor(0, blackColor,        RGB(  0,   0,   0));
  SetFontColor(0, greenColor,        RGB(  0, 255,   0));
  SetFontColor(0, redColor,          RGB(255,   0,   0));
  SetFontColor(0, blueColor,         RGB(128, 128, 255));
  SetFontColor(0, magentaColor,      RGB(255,   0, 255));
  SetFontColor(0, cyanColor,         RGB(  0, 255, 255));
  SetFontColor(0, orangeColor,       RGB(255, 128,   0));
  SetFontColor(0, brightOrangeColor, RGB(255, 128,   0));
  SetFontColor(0, silverColor,       RGB(192, 192, 192));
  SetFontColor(0, yellowColor,       RGB(255, 255,   0))
    ;
}

void FONT_LIBRARY::ReleaseFonts(void)
{
  int font;
  for (font = 0; font < 2*NUMTEXTFONT; font++)
  {
    m_fonts[font].ReleaseFont();
  }
}

void FONT_LIBRARY::SetFontColor(int baseFontNumber, FONT_COLOR_NUM colorNum, COLORREF color)
{
  m_fonts[baseFontNumber + colorNum].ReleaseFont();
  m_fonts[baseFontNumber + colorNum].m_color = color;
  m_fonts[baseFontNumber + colorNum + NUMTEXTFONT].ReleaseFont();
  m_fonts[baseFontNumber + colorNum + NUMTEXTFONT].m_color = color;
}

void FONT_LIBRARY::SetCustomColor(COLORREF color)
{
  int i;
  for (i = 0; i < NUMTEXTFONT; i++)
  {
    m_fonts[NUMTEXTFONT + i].m_color = color;
  };
}

void FONT_LIBRARY::SetFontType(int fontNum, const CString& fontType)
{
  if (fontType.CompareNoCase("italic") == 0)
  {
    m_fonts[fontNum].m_italic = true;
    m_fonts[fontNum + NUMTEXTFONT].m_italic = true;
  };
  if (fontType.CompareNoCase("regular") == 0)
  {
    m_fonts[fontNum].m_italic = false;
    m_fonts[fontNum].m_bold = false;
    m_fonts[fontNum + NUMTEXTFONT].m_italic = false;
    m_fonts[fontNum + NUMTEXTFONT].m_bold = false;
  };
  if (fontType.CompareNoCase("bold") == 0)
  {
    m_fonts[fontNum].m_bold = true;
    m_fonts[fontNum + NUMTEXTFONT].m_bold = true;
  };
}

void FONT_LIBRARY::SetFontName(int baseFontNumber, const CString& name)
{
  m_fonts[baseFontNumber].ReleaseFont();
  m_fonts[baseFontNumber].m_fontName = name;
  m_fonts[baseFontNumber+NUMTEXTFONT].ReleaseFont();
  m_fonts[baseFontNumber+NUMTEXTFONT].m_fontName = name;
}

void GLOBAL_STATS::ReleaseFonts()
{
  /*  prs 20191220
  if (WhiteFont >= 0)
    GraphicsMgr.ReleaseSurface(WhiteFont);
  WhiteFont = -1;
  if (YellowFont >= 0)
    GraphicsMgr.ReleaseSurface(YellowFont);
  YellowFont = -1;
  if (OrangeFont >= 0)
    GraphicsMgr.ReleaseSurface(OrangeFont);
  OrangeFont = -1;
  if (RedFont >= 0)
    GraphicsMgr.ReleaseSurface(RedFont);
  RedFont = -1;
  if (GreenFont >= 0)
    GraphicsMgr.ReleaseSurface(GreenFont);
  GreenFont = -1;
  if (BlueFont >= 0)
    GraphicsMgr.ReleaseSurface(BlueFont);
  BlueFont = -1;
  */
  {
// prs 20191227    int font;
    /* prs 20191227
    for (font = 0; font < 10; font++)
    {
      availableFonts[font].ReleaseFonts();
    };
    */
    fontLib.ReleaseFonts();



  };
  if (KeyboardFont >= 0)
    GraphicsMgr.ReleaseSurface(KeyboardFont);
  KeyboardFont=-1;  
  if (KeyboardHighlightFont >= 0)
    GraphicsMgr.ReleaseSurface(KeyboardHighlightFont);
  KeyboardHighlightFont=-1;
  if (HighlightFont >= 0)
    GraphicsMgr.ReleaseSurface(HighlightFont);
  HighlightFont = -1;  
  if (Custom1Font >= 0)
    GraphicsMgr.ReleaseSurface(Custom1Font);
  Custom1Font = -1;
  if (MagicAdjFont >= 0)
    GraphicsMgr.ReleaseSurface(MagicAdjFont);
  MagicAdjFont=-1;
}

//*****************************************************************************

void saveSounds(GLOBAL_STATS& data)
{
  data.SaveSounds();
}

void saveArt(GLOBAL_STATS& data) 
{
  data.SaveArt();
}

void GetKeyNames(GLOBAL_STATS& data, CString (*names)[8])
{
  int index=1;
  while (index <= 8)
  {    
    int key = data.keyData.GetItemKeyUsingSequence(index);
    if (key > 0)
      (*names)[index-1] = data.keyData.GetName(key);
      else (*names)[index-1]="";
    index++;
  }
}

long int GetKeyboardFont(GLOBAL_STATS& data)
{
  return data.KeyboardFont;
}

long int GetKeyboardHighlightFont(GLOBAL_STATS& data)
{
  return data.KeyboardHighlightFont;
}

long int GetHighlightFont(GLOBAL_STATS& data)
{
  return data.HighlightFont;
}

double GetGameVersion(GLOBAL_STATS& data)
{
  return data.version;
}

void DeleteLevelASL(GLOBAL_STATS& data, int level, const CString& key)
{
  data.levelInfo.stats[level].level_asl.Delete(key);
}

CString LookupGlobalASL(GLOBAL_STATS& data, const CString& key)
{
  return data.global_asl.Lookup(key);
}

void InsertGlobalASL(
          GLOBAL_STATS& data,
          const CString& key,
          const CString& value,
          int flags)
{
  data.global_asl.Insert(key,value,flags);
}

void InsertLevelASL(
          GLOBAL_STATS& data,
          int level,
          const CString& key,
          const CString& value,
          int flags)
{
  data.levelInfo.stats[level].level_asl.Insert(key,value,flags);
}

CString GetDesignName(GLOBAL_STATS& data)
{
  return data.designName;
}

void SetGameVersion(GLOBAL_STATS& data, double version)
{
  data.version=version;
}

BYTE CurrLevelWidth() 
{ 
  return globalData.levelInfo.stats[globalData.currLevel].area_width; 
}

BYTE CurrLevelHeight() 
{ 
  return globalData.levelInfo.stats[globalData.currLevel].area_height; 
}


void PlayCharHit(BOOL play) 
{ 
  if ((play) && PlaySoundEffects)
  {
    if (!ValidSound(globalData.sounds.hCharHit))
      globalData.sounds.hCharHit = LoadSound(globalData.sounds.CharHit);
    if (!PlaySound(globalData.sounds.hCharHit))
      globalData.sounds.hCharHit=-1;
  }
  else
    StopSound(globalData.sounds.hCharHit);
}

void PlayCharMiss(BOOL play) 
{ 
  if (play && PlaySoundEffects)
  {
    if (!ValidSound(globalData.sounds.hCharMiss))
      globalData.sounds.hCharMiss = LoadSound(globalData.sounds.CharMiss);
    if (!PlaySound(globalData.sounds.hCharMiss))
      globalData.sounds.hCharMiss=-1;
  }
  else
    StopSound(globalData.sounds.hCharMiss);
}

void PlayPartyStep(BOOL play) 
{ 
  if (    play 
       && PlaySoundEffects
       && (globalData.levelInfo.stats[globalData.currLevel].level_asl.Find("Supress Step Sound")==NULL)
     )
  {
    if (!ValidSound(globalData.sounds.hPartyStep))
    {
      CString tmp;
      if (globalData.levelInfo.stats[globalData.currLevel].StepSound != "")
        tmp = globalData.levelInfo.stats[globalData.currLevel].StepSound;
      else
        tmp = globalData.sounds.PartyStep;
      globalData.sounds.hPartyStep = LoadSound(tmp);
    }
    if (!PlaySound(globalData.sounds.hPartyStep))
      globalData.sounds.hPartyStep=-1;
  }
  else
    StopSound(globalData.sounds.hPartyStep);
}

void PlayPartyBump(BOOL play) 
{ 
  if (    play 
       && PlaySoundEffects
       && (globalData.levelInfo.stats[globalData.currLevel].level_asl.Find("Supress Bump Sound")==NULL)
     )  
  {
    if (!ValidSound(globalData.sounds.hPartyBump))
    {
      CString tmp;
      if (globalData.levelInfo.stats[globalData.currLevel].BumpSound != "")
        tmp = globalData.levelInfo.stats[globalData.currLevel].BumpSound;
      else
        tmp = globalData.sounds.PartyBump;
      globalData.sounds.hPartyBump = LoadSound(tmp);
    }
    if (!PlaySound(globalData.sounds.hPartyBump))
      globalData.sounds.hPartyBump=-1;
  }
  else
    StopSound(globalData.sounds.hPartyBump);
}

void PlayIntro(BOOL play) 
{ 
  if (play)
  {
    if (pSndMgr != NULL)
    {  
      pSndMgr->StopBgndQueue();
      POSITION pos = globalData.sounds.IntroMusic.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(globalData.sounds.IntroMusic.sounds.GetNext(pos),FALSE);
      pSndMgr->PlayBgndQueue();
    }  
  }
  else
    pSndMgr->StopBgndQueue();
}

void PlayOuttro(BOOL play)
{
    if (play)
    {
        if (pSndMgr != NULL)
        {
            pSndMgr->StopBgndQueue();
            POSITION pos = globalData.sounds.CreditsMusic.sounds.GetHeadPosition();
            while (pos != NULL)
                pSndMgr->QueueBgndSound(globalData.sounds.CreditsMusic.sounds.GetNext(pos), FALSE);
            pSndMgr->PlayBgndQueue();
        }
    }
    else
        pSndMgr->StopBgndQueue();
}

void PlayCampMusic(BOOL play) 
{ 
  if (pSndMgr == NULL)
    return;
  if (!pSndMgr->GetMusicEnable()) 
    return;

  if (globalData.sounds.CampMusic.sounds.GetCount() == 0)
    return;
  
  pSndMgr->StopBgndQueue();

  if (play)
  {    
    POSITION pos = globalData.sounds.CampMusic.sounds.GetHeadPosition();

    while (pos != NULL)
      pSndMgr->QueueBgndSound(globalData.sounds.CampMusic.sounds.GetNext(pos),FALSE);

    pSndMgr->PlayBgndQueue();
  }
}

void PlayDeath(BOOL play) 
{ 
  if (play && PlaySoundEffects)
  {
    if (!ValidSound(globalData.sounds.hDeathMusic))
      globalData.sounds.hDeathMusic = LoadSound(globalData.sounds.DeathMusic);
    if (!PlaySound(globalData.sounds.hDeathMusic, FALSE))
      globalData.sounds.hDeathMusic=-1;
  }
  else
    StopSound(globalData.sounds.hDeathMusic);
}

void StopAllSounds()
{
  globalData.sounds.ClearSounds();  
  itemData.ClearSounds();
  spellData.ClearSounds();
  monsterData.ClearSounds();
  //combatData.ClearSounds(); auto cleared when combat ends

  if (pSndMgr == NULL) return;
  pSndMgr->StopAll();
  //WriteDebugString("StopAllSounds\n");
}

void ReleaseAllSounds()
{
  globalData.sounds.ClearSounds();  
  itemData.ClearSounds();
  spellData.ClearSounds();
  monsterData.ClearSounds();
  //combatData.ClearSounds(); auto cleared when combat ends

  if (pSndMgr == NULL) return;
  pSndMgr->Clear();
  //WriteDebugString("RemoveAllSounds\n");
}

int messageMapInitStatus = 0;
MESSAGEMAP *messageMap;
const char* getGameText(const char* id) {
    if (messageMapInitStatus == -1) {  // -1 indicates failed status
        return id;
    }
    if (messageMapInitStatus == 0) {
        messageMap = new MESSAGEMAP();
        messageMapInitStatus = 1;
        if (FileExists(rte.DataDir() + MESSAGEMAP_FILE_NAME)) {
            messageMap->LoadFile(rte.DataDir() + MESSAGEMAP_FILE_NAME);
        }
        else {
            messageMapInitStatus = -1;
            return id;
        }
    }
    return messageMap->getTextFromId(id);
}

void initInventoryRects(mCArray<RECT, RECT&> &rects) {
    if (rects.GetSize() < Items_Per_Page) {
        rects.SetSize(Items_Per_Page);
    }
    for (int idx = 0; idx < Items_Per_Page; idx++) {
        rects.GetAt(idx).left = 0;
        rects.GetAt(idx).right = 0;
        rects.GetAt(idx).top = 0;
        rects.GetAt(idx).bottom = 0;
    };
}

#ifdef UAFEngine
void CheckAndPlayBackgroundMusic(int &UsingDayMusic)
{
  // no background music during combat, or during encamp (encamp has it's own music)
  if (!IsCombatActive())
  { 
    bool usebg = true;
    if (GetCurrEventType() == CONTROL_Encamp)
    {
      // see if camp music is available
      if (globalData.sounds.CampMusic.sounds.GetCount() > 0)
        usebg = false;
    }

    if (usebg)
    {
      int currZone = levelData.GetCurrZone(party.Posx, party.Posy);
      ZONE &zdata = levelData.zoneData.zones[currZone];
      LEVEL_STATS &data = globalData.levelInfo.stats[GetCurrentLevel()];

      BOOL znight = (   (zdata.bgSounds.UseNightMusic) 
                     && (zdata.bgSounds.nightSounds.sounds.GetCount()>0)
                     && (IsTriggerTimeInCurrentTime(zdata.bgSounds.StartTime, zdata.bgSounds.EndTime)));

      BOOL lnight = (   (data.bgSounds.UseNightMusic) 
                     && (data.bgSounds.nightSounds.sounds.GetCount()>0)
                     && (IsTriggerTimeInCurrentTime(data.bgSounds.StartTime, data.bgSounds.EndTime)));


      BOOL nightTrigger = (znight || lnight);

      if (UsingDayMusic==1) // playing day music
      {
        if (nightTrigger)
        {
          // switch to night music
          StartBackgroundMusic();
          UsingDayMusic=0;
        }
      }
      else if (UsingDayMusic==0) // playing night music
      {
        if (!nightTrigger)
        {
          // switch to day music
          StartBackgroundMusic();
          UsingDayMusic=1;
        }
      }
      else // day music=-1 at program start
      {
        // music should already be playing,
        // just init day/night flag
        UsingDayMusic = !nightTrigger;
      }
    }
  }
}
#endif // UAFEngine