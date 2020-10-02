/******************************************************************************
* Filename: class.h
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
#ifndef __CLASS_H__
#define __CLASS_H__

#include "SharedQueue.h"
#include "specAb.h"
#include "items.h"


#define MAX_RACES 9999
#define MAX_BASE_CLASSES 9999
#define MAX_CLASSES 9999
#define MAX_ABILITIES 6
#define MAX_TRAITS 500
#define MAX_SPELLGROUPS 500

struct CONFIG_PKT;
class  CLASS_DATA;
class  BASECLASS_STATS;
class  ALLOWED_CLASSES;
class  CAR;
struct AREA_MAP_DATA;

typedef DWORD abilityType;
#define abilityUnknown 0xffffffff

typedef DWORD spellType;
#define spellUnknown 0xffffffff

//typedef DWORD spellgroupType;
//#define spellgroupUnknown 0xffffffff

typedef DWORD traitType;
#define traitUnknown 0xffffffff

//typedef DWORD baseclassKey;
//#define baseclassUnknown 0xffffffff


enum SYS_SKILLS // Skills referenced by the runtime engine
{
  skill_THAC0 = 1,
  skill_END  // User-defined skills can start here
};

enum _DBTYPE {
  UNKNOWN_DB = 1,
  LEVEL_DB, // Not a database really.  The character's level.
  SPELL_DB,
  ITEM_DB,
  RACE_DB,
  BASECLASS_DB,
  CLASS_DB,
  ABILITY_DB,
  TRAIT_DB,
  SPELLGROUP_DB,
  LITERAL_DB, // Also not a database. refKey is literal value
  DICE_DB, // Not a db, indicates dice roll info for compiled RDR data
  GENDER_DB, // Not a db
  RACEDB_ID,       // These '_ID' types are followed
  ITEMDB_ID,       // by a counted string, naming the
  SPELLDB_ID,      // entry in the database.
  CLASSDB_ID,
  BASECLASSDB_ID,
};

// order of precedence for ajo operators (highest to lowest)
//
//  *,/
//  +, -
//  >, >=, <, <=
//  ==
//  &
//  |
//  &&
//  ||

enum ADJOP 
{
  AJO_none=0,
  AJO_gear=1,
  AJO_plus,
  AJO_equality,
  AJO_greater,
  AJO_greaterEqual,
  AJO_andBit,
  AJO_slash,
  AJO_andLogical,
  AJO_continue,
  AJO_minus,
  AJO_less,
  AJO_lessEqual,
  AJO_orBit,
  AJO_orLogical
};


enum eventDistType;
enum chainTriggerType;
enum eventTriggerType;
enum eventFacingType;
enum eventStepType;
enum encounterButtonResultType;
enum labelPostChainOptionsType;
enum eventDirType;
enum eventSurpriseType;
enum eventTurnUndeadModType;
enum passwordActionType;
enum eventPartyAffectType;
enum spellSaveEffectType;
enum spellSaveVersusType;
enum takeItemQtyType;
enum takeItemsAffectsType;
enum taleOrderType;
enum costFactorType;
enum MathOperationType;
enum MultiItemCheckType;
enum GlobalAreaViewStyle;
enum BlockageType;
enum alignmentType;
enum creatureSizeType;
enum QuestStateType;
enum QuestAcceptType;
enum MappingType;
enum charStatusType;
enum ACTION;
enum ACTION_CONDITION;
enum VALUE_MODIFICATION;

#ifdef UAFEDITOR
#define JWTAB 3
#define maxTab 199

enum ES
{
  ES_ok = 0,
  ES_EOF,
  ES_syntax,
  ES_missingColon,
  ES_newlineInString,
  ES_missingList,
  ES_missingArray,
  ES_illegalValue,
  ES_missingData,
};


class JWriter
{
  CFile *m_f;
  int indent;
  bool linefeed;
  ES   errorStatus;
  CArray<int, int> lineCount; // lineCount[indent];
  char tabs[maxTab+1];
  void OpenBrace(void);
  void OpenBracket(void);
  void Colon(void);
  void Name(const char *name);
  void String(const char *str);
  void MaybeComma(void);
  void Tab(void);
 public:
  JWriter(CFile *cf){m_f=cf; indent=0;memset(tabs,' ',199);tabs[199]=0;linefeed=true;};
  void NameAndColon(const char *name);
  void QuotedString(const char *str);
  void NameAndValue(const char *name, const CString& value);
  void NameAndValue(const char *name, const unsigned char& value);
  void NameAndBinary(const char *name, const CString& value);
  void NameAndQuotedValue(const char *name, const CString& value);
  void NameAndNonBlank(const char *name, const CString& value);
  void NameAndQuotedNonBlank(const char *name, const CString& value);
  void NameAndMultiLineString(const char *name, const CString& value);
  void NameAndValue(const char *name, const int& value);
  void NameAndValue(const char *name, const long& value);
  void NameAndValue(const char *name, const WORD& value);
  void NameAndNonZero(const char *name, const int& value){if(value!=0)NameAndValue(name,value);};
  void NameAndNonNegative(const char *name, const int& value){if(value>=0)NameAndValue(name,value);};
  void NameAndValue(const char *name, const DWORD& value);
  void NameAndNonZero(const char *name, const DWORD& value){if(value!=0)NameAndValue(name,value);};
  void NameAndValue(const char *name, const LONGLONG& value);
  void NameAndNonZero(const char *name, const LONGLONG& value){if(value!=0)NameAndValue(name,value);};
  void NameAndValue(const char *name, const double& value);
  void NameAndValue(const char *name, const float& value);
  void NameAndBool(const char *name, const bool& value);
  void NameAndEnum(const char *name, const DWORD& value, DWORD numValue, const char *text[]);
  void NameAndEnum(const char *name, const int& value, DWORD numValue, const char *text[]);
  void NameAndEnum(const char *name, const BYTE& value, DWORD numValue, const char *text[]);
  void NameAndEnum(const char *name, const chainTriggerType& value);
  void NameAndEnum(const char *name, const itemClassType& value);
  void NameAndEnum(const char *name, const eventTriggerType& value);
  void NameAndEnum(const char *name, const genderType& value);
  void NameAndEnum(const char *name, const creatureSizeType& value);
  void NameAndEnum(const char *name, const alignmentType& value);
  void NameAndEnum(const char *name, const eventFacingType& value);
  void NameAndEnum(const char *name, const eventStepType& value);
  void NameAndEnum(const char *name, const encounterButtonResultType& value);
  void NameAndEnum(const char *name, const labelPostChainOptionsType& value);
  void NameAndEnum(const char *name, const eventDistType& value);
  void NameAndEnum(const char *name, const eventDirType& value);
  void NameAndEnum(const char *name, const eventSurpriseType& value);
  void NameAndEnum(const char *name, const eventTurnUndeadModType& value);
  void NameAndEnum(const char *name, const passwordActionType& value);
  void NameAndEnum(const char *name, const eventPartyAffectType& value);
  void NameAndEnum(const char *name, const spellSaveEffectType& value);
  void NameAndEnum(const char *name, const spellSaveVersusType& value);
  void NameAndEnum(const char *name, const takeItemsAffectsType& value);
  void NameAndEnum(const char *name, const takeItemQtyType& value);
  void NameAndEnum(const char *name, const taleOrderType& value);
  void NameAndEnum(const char *name, const costFactorType& value);
  void NameAndEnum(const char *name, const MathOperationType& value);
  void NameAndEnum(const char *name, const MultiItemCheckType& value);
  void NameAndEnum(const char *name, const GlobalAreaViewStyle& value);
  void NameAndEnum(const char *name, const BlockageType& value);
  void NameAndEnum(const char *name, const SurfaceType& value);
  void NameAndEnum(const char *name, const QuestStateType& value);
  void NameAndEnum(const char *name, QuestAcceptType& value);
  void NameAndEnum(const char *name, const MappingType& value);
  void NameAndEnum(const char *name, const charStatusType& value);
  void NameAndEnum(const char *name, const VALUE_MODIFICATION& value);
  void NameAndEnum(const char *name, const ACTION_CONDITION& value);
  void NameAndEnum(const char *name, const ACTION& value);
  void NameAndFlags(const char *name, const BYTE& value, int num, const char *text[]);
  void NameAndFlags(const char *name, const DWORD& value, int num, const char *text[]);
  void NameAndBool(const char *name, const BOOL& value){NameAndBool(name, value!=0);};
  void NameAndDice(const char *name, const int& n, const int& s, const int& b);
  void NameAndValue(const char *name, const int *val, int num);
  void NameAndValue(const char *name, const char *val, int num);
  void NewLine(void);

  void StartList(void);
  void StartList(const char *name);
  void EndList(void);

  void StartArray(void);
  void StartArray(const char *name);
  void ListEntry(void)  {NextEntry(); StartList();};
  void ArrayEntry(void) {NextEntry(); StartArray();};
  void NextEntry();
  void EndArray(void);

  void Linefeed(bool lf){linefeed=lf;};
};

struct JR_DATA;

struct JR_LIST
{
  JR_DATA      *first;
  JR_DATA      *last;
  int          lineNum;
  JR_LIST(void){first=last=NULL;};
  ~JR_LIST(void);
};

struct JR_DATA
{
  CString       name;
  CString       stringValue;
  JR_LIST      *listValue; // If non-null then value is another list
  JR_LIST      *arrayValue; // A list of nameless values
  JR_DATA      *next;  
  int           lineNum;
  JR_DATA(void) {next = NULL; listValue = NULL; arrayValue = NULL;};
  ~JR_DATA(void);
};

// Ensure that the 'optional' flag is removed by any function
// that wants to test for optional data.
// Now the instance variable 'm_optional' should never be referenced
// by the code.
class OPT
{
public:
  bool opt;
  OPT(bool *optional)
  {
    opt = *optional;
    *optional = false;
  };
};

#define USEOPTIONAL  OPT opt(&m_optional);
#define ISOPTIONAL   (opt.opt)


class JReader
{
  enum TT
  {
    TT_String,
    TT_Colon,
    TT_OpenBrace,
    TT_CloseBrace,
    TT_OpenBracket,
    TT_CloseBracket,
    TT_Comma,
  };
  CFile *m_f;
private:
  CString token;
  TT      tokenType;
  ES      errStat;
  CString errMessage;
  void    SetError(ES e);
  BOOL    nextCharValid;
  void    GetChar(void);
  BOOL    Whitespace(void);
  void    GetToken(void);
  void    GetParseList(JR_LIST **pList);
  void    GetParseArray(JR_LIST **pList);
  char    thisChar;
  char    nextChar;
  CArray<char, char>tokenArray;
  int     tokenSize;
  void    JRError(const char *msg);
  JR_DATA *m_root;
  int     lineNum;
  bool    m_optional;
  int     errLine;
  int     listStackSize;
  JR_DATA *pCurData;
  JR_LIST *pCurList;
  CArray<JR_LIST *, JR_LIST *>listStack;
  bool    FindAndRemoveName(const char *name);
  bool    FindAndRemoveOpenBrace(const char *name);
  bool    FindAndRemoveOpenBracket(const char *name);
  bool    OpenBrace(void);
  bool    OpenBracket(void);
public:
  JReader(CFile *cf){m_f = cf; m_root=pCurData=NULL;pCurList=NULL;m_optional=false;};
  ~JReader(void);
  void    Clear(void);
  ES      GetError(void);
  CString GetErrorMessage(void);
  int     Initialize(void);
  inline  void Optional(){m_optional = true;};
  void    DeleteName();
  void    EndList(void);
  void    Colon(void);
  void    Linefeed(bool){return;}; // Just so read looks more like write
  bool    Name(const char *name);
  void    String(const char *str);
  void    QuotedString(char term);
  bool    NameAndValue(const char *name,  CString& value);
  bool    NameAndBinary(const char *name, CString& value);
  bool    NameAndQuotedNonBlank(const char *name,  CString& value)
          {
            Optional(); 
            if (NameAndValue(name, value)) return true;
            value.Empty(); return false;
          };
  bool    NameAndNonBlank(const char *name,  CString& value)
          {
            Optional(); 
            if (NameAndValue(name, value)) return true;
            value.Empty(); return false;
          };
  bool    NameAndQuotedValue(const char *name, CString& value){return NameAndValue(name, value);};
  bool    NameAndMultiLineString(const char *name, CString& value);
  bool    NameAndValue(const char *name,  unsigned char& value);
  bool    NameAndValue(const char *name,  char& value);
  bool    NameAndValue(const char *name, int& value);
  bool    NameAndValue(const char *name, long& value);
  bool    NameAndValue(const char *name, WORD& value);
  bool    NameAndNonZero(const char *name, int& value)
          {
            Optional(); 
            if (NameAndValue(name, value)) return true;
            value = 0; return false;
          };
  bool    NameAndNonNegative(const char *name, int& value)
          {
            Optional(); 
            if (NameAndValue(name, value)) return true;
            value = -1; return false;
          };
  bool    NameAndNonZero(const char *name, DWORD& value)
          {
            Optional(); 
            if (NameAndValue(name, value)) return true;
            value = 0; return false;
          };
  bool    NameAndValue(const char *name, DWORD& value);
  bool    NameAndValue(const char *name, LONGLONG& value);
  bool    NameAndValue(const char *name, double& valuef);
  bool    NameAndValue(const char *name, float& value);
  bool    NameAndBool(const char *name, bool& value);
  bool    NameAndBool(const char *name, BYTE& value);
  bool    NameAndEnum(const char *name, DWORD& value, DWORD numValue, const char *text[]);
  bool    NameAndEnum(const char *name, int& value, DWORD numValue, const char *text[]);
  bool    NameAndEnum(const char *name, BYTE& value, DWORD numValue, const char *text[]);
  bool    NameAndEnum(const char *name, eventDistType& value);
  bool    NameAndEnum(const char *name, itemClassType& value);
  bool    NameAndEnum(const char *name, genderType& value);
  bool    NameAndEnum(const char *name, creatureSizeType& value);
  bool    NameAndEnum(const char *name, alignmentType& value);
  bool    NameAndEnum(const char *name, chainTriggerType& value);
  bool    NameAndEnum(const char *name, takeItemsAffectsType& value);
  bool    NameAndEnum(const char *name, takeItemQtyType& value);
  bool    NameAndEnum(const char *name, eventTriggerType& value);
  bool    NameAndEnum(const char *name, eventType& value);
  bool    NameAndEnum(const char *name, eventPartyAffectType& value);
  bool    NameAndEnum(const char *name, eventFacingType& value);
  bool    NameAndEnum(const char *name, eventStepType& value);
  bool    NameAndEnum(const char *name, eventDirType& value);
  bool    NameAndEnum(const char *name, eventTurnUndeadModType& value);
  bool    NameAndEnum(const char *name, eventSurpriseType& value);
  bool    NameAndEnum(const char *name, encounterButtonResultType& value);
  bool    NameAndEnum(const char *name, labelPostChainOptionsType& value);
  bool    NameAndEnum(const char *name, spellSaveEffectType& value);
  bool    NameAndEnum(const char *name, spellSaveVersusType& value);
  bool    NameAndEnum(const char *name, passwordActionType& value);
  bool    NameAndEnum(const char *name, MathOperationType& value);
  bool    NameAndEnum(const char *name, MultiItemCheckType& value);
  bool    NameAndEnum(const char *name, taleOrderType& value);
  bool    NameAndEnum(const char *name, costFactorType& value);
  bool    NameAndEnum(const char *name, GlobalAreaViewStyle& value);
  bool    NameAndEnum(const char *name, BlockageType& value);
  bool    NameAndEnum(const char *name, SurfaceType& value);
  bool    NameAndEnum(const char *name, QuestStateType& value);
  bool    NameAndEnum(const char *name, MappingType& value);
  bool    NameAndEnum(const char *name, QuestAcceptType& value);
  bool    NameAndEnum(const char *name, charStatusType& value);
  bool    NameAndEnum(const char *name, VALUE_MODIFICATION& value);
  bool    NameAndEnum(const char *name, ACTION_CONDITION& value);
  bool    NameAndEnum(const char *name, ACTION& value);
  bool    NameAndFlags(const char *name, BYTE& value, int num, const char *text[]);
  bool    NameAndFlags(const char *name, DWORD& value, int num, const char *text[]);
  bool    NameAndBool(const char *name, BOOL& value);
  bool    NameAndDice(const char *name, int& n, int& s, int& b);

  bool    StartList(const char *name);
  bool    StartList();

  bool    StartArray(const char *name);
  bool    StartArray(void);
  bool    ListEntry() {return NextEntry() && StartList();};
  bool    ArrayEntry() {return NextEntry() && StartArray();};
  bool    NextEntry();
  bool    EndArray(void);

  bool    NameAndValue(const char *name, int *val, int num);
  bool    NameAndValue(const char *name, char *val, int num);
};


#endif

#define CArchiveBufSize 7
#define CArchiveBufCur 9
#define CArchiveBufMax 10
#define CArchiveBufStart 11



class CAR //: public CArchive
{
public:
  CArchive ar;
public:
  inline BOOL IsStoring(void){return ar.IsStoring();};     //Added 20121208 PRS
  inline BOOL IsLoading(void){return ar.IsLoading();};     //Added 20121208 PRS
  inline const CFile *GetFile(void) const {return ar.GetFile();}; //Added 20121208 PRS
  inline DWORD GetPosition(void) const
  {
    
    const int *par;
    par = (int *)&ar;
    return     GetFile()->GetPosition()  // End of current buffer
            -  par[CArchiveBufMax]      // Start of current buffer
            +  par[CArchiveBufCur]
            +  m_bufferIndex/8;
  };
  inline BOOL ReadString(CString& str)
  {
    if (m_compressType == 0)
    {
      return ar.ReadString(str);  //Added 20121208 PRS
    }
    else
    {
      die("Not Needed?"); // Not Implemented(0xc3a17, false);
      return FALSE;
    };
  };
private:
  inline void Write(void *buf, unsigned int num) {ar.Write(buf, num);}; //Added 20121208 PRS
  inline UINT Read (void *buf, unsigned int num) 
  {
    UINT result;
    result = ar.Read(buf, num);
    return result;
  }; //Added 20121208 PRS
  class CODES
  {
  private:
    unsigned __int16 m_hashTable[9973];
    unsigned int m_codes[8192];
    unsigned __int16 m_nextCode;
    int m_numCodes;
    int m_numMisses;
  public:
    CODES(void);
    ~CODES(void);
    void Clear(void);
    unsigned __int16 AddCode(unsigned int code);
    void PrintStats(void);
    void HashDump(FILE *f);
    bool Full(void) { return m_nextCode == 8190;};
    unsigned __int16 FindCode(unsigned int code); // zero if no find
  };
  class DDATA
  {
    friend class CAR;
    char m_stack[1000];
    unsigned __int16 m_prefix[8192];
    unsigned char   m_postfix[8192];
  };
  class CDATA
  {
    friend class CAR;
  };
private:
  char m_compressType;
  CMap <CString, LPCSTR, unsigned int, unsigned int> stringIndex;
  CArray <CString, CString&> m_stringArray;
  unsigned int m_nextIndex;
  DDATA *m_dData; // Allocated at run time on heap.  We like
  CDATA *m_cData; // to keep these large items off the stack.
  CODES *m_codes;
  void compress (const char *chars, int num);
  void decompress (char *chars, int num);
  unsigned __int16 m_w;
  unsigned int m_bufferIndex;
  unsigned int m_buffer[13]; // 52 bytes
  unsigned int m_stkLen;
  unsigned __int16 m_OC;
  unsigned int m_numCode;
  unsigned char m_C;
public:
  CAR(CFile* pFile, UINT nMode);
  ~CAR(void);
  void Close(void);
  void Flush(void);
  void Compress(bool compress);
  char GetCompression() const { return m_compressType; }
  void WriteCount(unsigned int n);
  void WriteCount(DWORD n);
  void WriteCount(int n);
  CString CanonicalString(CString& str);
  DWORD ReadCount(void);

  CAR& operator << (int Int);
  CAR& operator >> (int& Int);

  CAR& operator << (unsigned int UInt);
  CAR& operator >> (unsigned int& UInt);

  CAR& operator << (unsigned long ULong);
  CAR& operator >> (unsigned long& ULong);

  CAR& operator << (long ULong);
  CAR& operator >> (long& ULong);

  CAR& operator << (BYTE byte);
  CAR& operator >> (BYTE& byte);

  CAR& operator << (char Char);
  CAR& operator >> (char& Char);

  CAR& operator << (short Short);
  CAR& operator >> (short& Short);

  CAR& operator << (double Double);
  CAR& operator >> (double& Double);

  CAR& operator << (float Float);
  CAR& operator >> (float& Float);

  CAR& operator << (unsigned short UShort);
  CAR& operator >> (unsigned short& UShort);

  CAR& operator << (const CString& str);
  CAR& operator >> (CString& str);

  CAR& operator << (const char *);
  CAR& operator >> (char *);

  CAR& Serialize   (A_ASLENTRY_L& attr, LPCSTR mapName);
  CAR& DeSerialize (A_ASLENTRY_L& attr, LPCSTR mapName);

  CAR& Serialize(char *buf, int n);


  //CAR& operator << (AREA_MAP_DATA& attr);
  //CAR& operator >> (AREA_MAP_DATA& attr);

  CAR& operator << ( CArray<WORD, WORD> &warray);
  CAR& operator >> ( CArray<WORD, WORD> &warray);
  CAR& operator << ( CArray<DWORD, DWORD> &warray);
  CAR& operator >> ( CArray<DWORD, DWORD> &warray);
};


const int MAX_SPELL_NAME = 50;
const int MAX_SPELLS = 0x000FFFFF;
const int NO_SPELL = -1;

struct SQueuedSpecAb;
/*
class GLOBAL_SPELL_ID
{
  friend class  SPELL_DATA_TYPE;
  friend struct SPELL_LIST;
  friend struct PENDING_SPELL_LIST;
  friend struct SPELL_TEXT_LIST;
  friend struct SPELL_EFFECTS_DATA;
  friend struct SQueuedSpecAb;
  friend int CompareSpellsForNewSpellList( const void *arg1, const void *arg2 );
public:
  GLOBAL_SPELL_ID(void){m_key=-1;};
private:
  GLOBAL_SPELL_ID(int n){m_key=n;};
  int m_key;
public:
  GLOBAL_SPELL_ID(const GLOBAL_SPELL_ID& gsID){m_key=gsID.m_key;};

  // you should not be using UniqueIdentifier() if there is any reasonable 
  // way to avoid it.
  inline DWORD UniqueIdentifier(void) const {return m_key^0x5cdf;};
  inline void UniqueIdentifier(DWORD n){m_key=n^0x5cdf;};

  //This function must be removed when we can figure out 
  //what is going on in some of the places that appear confused
  //about spell IDs!
  DWORD IllegalConversion(void) const {return m_key;};
  GLOBAL_SPELL_ID IllegalConversion(int n){m_key = n;return *this;};

  inline void Clear(void){m_key = -1;};
  inline bool IsValidSpell(void) const {return (m_key>=0) && (m_key<=MAX_SPELLS);};
  //GLOBAL_SPELL_ID(int n){m_key = n;};
  void Serialize(CArchive& ar, double version); //{ar.IsStoring()?(ar<<m_key):ar>>m_key;};
  void Serialize(CAR& car, double version); //{car.IsStoring()?(car<<m_key):car>>m_key;};
  inline GLOBAL_SPELL_ID operator =(const GLOBAL_SPELL_ID& gsID){m_key=gsID.m_key;return *this;};
//  inline bool operator ==(int n){return m_giID==n;};
//  inline bool operator !=(int n){return m_giID!=n;};
  inline bool operator ==(const GLOBAL_SPELL_ID gsID)const{return m_key==gsID.m_key;};
  inline bool operator !=(const GLOBAL_SPELL_ID gsID)const{return m_key!=gsID.m_key;};
//  inline bool operator > (int n){return m_giID>n;};
//  inline bool operator >=(int n){return m_giID>=n;};
};
*/
/*
class GLOBAL_ITEM_ID
{
  enum { GemItem=-2, JewelryItem=-3 };
  friend class ITEM_DATA_TYPE;
  friend struct SPELL_EFFECTS_DATA;
  friend BOOL AssignItem(BYTE slot, struct ITEM &data, BOOL id); // Import--assign original keys
  friend struct SQueuedSpecAb;
  int m_key;
  GLOBAL_ITEM_ID(int n){m_key=n;};
public:
  GLOBAL_ITEM_ID(void){m_key=-1;};
  GLOBAL_ITEM_ID(const GLOBAL_ITEM_ID& giID){m_key=giID.m_key;};

  // You should not be using UniqueIdentifier() if there is any reasonable 
  // way to avoid it.  You can use these to extract an 'int' from the
  // GLOBAL_ITEM_ID but you should not use that 'int' for any purpose
  // except to build another GLOBAL_ITEM_ID.  We fiddle with the value
  // to make it obvious that these two functions are complementary
  // and that the 'int' you receive is worthless for any other purpose.
  inline DWORD UniqueIdentifier(void) const {return m_key^0x5cdf;};
  inline void UniqueIdentifier(DWORD n){m_key=n^0x5cdf;};

  DWORD IllegalConversion(void){return m_key;};
  void IllegalConversion(int n){m_key = n;};


  inline void Clear(void){m_key = -1;};
  inline bool IsValidItem(void){return m_key>=0;};
  inline bool IsNoItem(void){return m_key==-1;};
//  GLOBAL_ITEM_ID(int n){m_giID = n;};
  void Serialize(CArchive& ar){ar.IsStoring()?(ar<<m_key):ar>>m_key;};
  void Serialize(CAR& car){car.IsStoring()?(car<<m_key):car>>m_key;};
//  inline GLOBAL_ITEM_ID operator =(int n){m_giID=n;return *this;};
//  inline bool operator ==(int n){return m_giID==n;};
//  inline bool operator !=(int n){return m_giID!=n;};
  inline bool operator ==(const GLOBAL_ITEM_ID giID) const {return m_key==giID.m_key;};
  inline bool operator !=(const GLOBAL_ITEM_ID giID) const {return m_key!=giID.m_key;};
  inline bool IsMoney(void){return (m_key==GemItem)||(m_key==JewelryItem);};
  inline bool IsGemType(void){return m_key==GemItem;};
  inline GLOBAL_ITEM_ID CreateGem(void){return GLOBAL_ITEM_ID(GemItem);};
  inline GLOBAL_ITEM_ID CreateJewelry(void){return GLOBAL_ITEM_ID(JewelryItem);};
//  inline bool operator > (int n){return m_giID>n;};
//  inline bool operator >=(int n){return m_giID>=n;};
};
*/
//bool IsGemType(const ITEM_ID& itemID);
/*
class SPELLBOOK_SPELL_ID
{
  friend class SPELL_DATA_TYPE;
  SPELLBOOK_SPELL_ID(int n){m_key=n;};
public:
  int m_key;
  SPELLBOOK_SPELL_ID(void){m_key=-1;};
  SPELLBOOK_SPELL_ID(const SPELLBOOK_SPELL_ID& gsID){m_key=gsID.m_key;};

  //SPELLBOOK_SPELL_ID ConvertFromGlobal(GLOBAL_SPELL_ID gsID){m_key=gsID.m_key;return *this;};
  //GLOBAL_SPELL_ID ConvertToGlobal(void)
  //{
  //  GLOBAL_SPELL_ID gsID;
  //  gsID.IllegalConversion(m_key);
  //  return gsID;
  //};

  // you should not be using UniqueIdentifier() if there is any reasonable 
  // way to avoid it.
  //inline DWORD UniqueIdentifier(void){return m_key^0x5cdfa;};
  //inline void UniqueIdentifier(DWORD n){m_key=n^0x5cdfa;};

  //This function must be removed when we can figure out 
  //what is going on in some of the places that appear confused
  //about spell IDs!
  DWORD IllegalConversion(void){return m_key;};
  void IllegalConversion(int n){m_key = n;};

  inline void Clear(void){m_key = -1;};
  inline bool IsValidSpell(void){return m_key>=0;};
  //GLOBAL_SPELL_ID(int n){m_key = n;};
  void Serialize(CArchive& ar){ar.IsStoring()?(ar<<m_key):ar>>m_key;};
  inline SPELLBOOK_SPELL_ID operator =(const SPELLBOOK_SPELL_ID& gsID){m_key=gsID.m_key;return *this;};
//  inline bool operator ==(int n){return m_giID==n;};
//  inline bool operator !=(int n){return m_giID!=n;};
  inline bool operator ==(const SPELLBOOK_SPELL_ID gsID)const{return m_key==gsID.m_key;};
  inline bool operator !=(const SPELLBOOK_SPELL_ID gsID)const{return m_key!=gsID.m_key;};
//  inline bool operator > (int n){return m_giID>n;};
//  inline bool operator >=(int n){return m_giID>=n;};
};
*/


class GENERIC_REFERENCE
{
public:
  char    m_refType;  // Which database?
  CString m_refName;  // For editor..Which item in databse?
  int     m_refKey;   // For runtime..Which item in database?
public:
  GENERIC_REFERENCE(void);
  ~GENERIC_REFERENCE(void);
  GENERIC_REFERENCE& operator =(const GENERIC_REFERENCE& src);
  GENERIC_REFERENCE& operator =(const CString& name);
  BOOL operator ==(const GENERIC_REFERENCE& src) const;
  void Clear(void);
  int GetDataSize(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  int Serialize(CArchive& car);
  void PostSerialize(BOOL IsStoring);
  BOOL LookupRefKey(void);
  BOOL LookupReferenceData(double &result);
  BOOL Evaluate(double &result);
  void AmbiguousRefType(_DBTYPE dbType);
};

class ADJUSTMENT
{
  // Adjustments take the form:
  //
  //   [[[[ reference [op1 par1]] op2 par2] op3 [par3]]
  // opn between -32768 and 32767 inclusive.  For example.
  //
  //     strength > -3  * 4 / 2
  //
  // I don't know if that makes much sense but it means that if
  // strength is greater than -3 then multiply by 4 and divide by 2
  // Else zero.
  //
  // All the following are legal and equivalent
  //   Height Adjust  =  Level - 0 * +1 / 1  ; Means (+1*(level-0))/1
  //   Height Adjust  =  Level - 0 * 1 / 1
  //   Height Adjust  =  level * 1 / 1
  //   Height Adjust  =  level / 1
  //   Height Adjust  =  level * 1
  //   Height Adjust  =  level
  // 
public:
  enum { MAX_ADJ=3 };

  short   m_parameter[MAX_ADJ]; // Three parameters A, B, and C
  char    m_operator[MAX_ADJ];
  GENERIC_REFERENCE m_reference;
public:
  ADJUSTMENT (void);
  ADJUSTMENT (const ADJUSTMENT& src);
  ADJUSTMENT(char value, CString name);
  ~ADJUSTMENT(void);
  void Clear(void);
  ADJUSTMENT& operator = (const ADJUSTMENT& src);
  BOOL operator ==(const ADJUSTMENT& src) const;
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  int Serialize(CArchive& car);
  void PostSerialize(BOOL IsStoring);
  int GetDataSize(void);
  double ApplyOperatorsToInput(double &input);
  BOOL Evaluate(double &result);
  BOOL EvaluateBinary(double &result);
  CString EncodeOperator(int op);
  CString EncodeText();
  BOOL Add(int param, char oper);
  BOOL HasTwoOperands();
};

typedef CArray<ADJUSTMENT, ADJUSTMENT&> DICEPLUSADJLIST;

class DICEPLUS
{ // To record things like:
  //     2d9 + 12 + 3*level
public:
  /*
  char              m_numDice;  // Yes.  It can be negative.
  BYTE              m_numSides; // Must be positive
  char              m_bonus;    // Yes...It can be negative
  int               m_min;      // 
  int               m_max;      // 
  char              m_sign;     // +/-, +1/-1
  CArray<ADJUSTMENT, ADJUSTMENT&>  m_adjust;
  */
  CString m_Text;
  mutable CString m_Bin;

public:
  DICEPLUS(void);
  DICEPLUS(const char *text){m_Text = text;};
  ~DICEPLUS(void);
  DICEPLUS &operator =(const DICEPLUS& src);
  bool operator!=(const DICEPLUS &rhs) const { return (!(*this == rhs)); }
  bool operator==(const DICEPLUS &rhs) const
  {
    if (m_Text.CompareNoCase(rhs.m_Text)!=0) return false;
    /*
    if (m_numDice != rhs.m_numDice) return false;
    if (m_numSides != rhs.m_numSides) return false;
    if (m_bonus != rhs.m_bonus) return false;
    if (m_min != rhs.m_min) return false;
    if (m_max != rhs.m_max) return false;
    if (m_adjust.GetSize() != rhs.m_adjust.GetSize()) return false;
    if (m_sign != rhs.m_sign) return false;
    for (int i=0;i<m_adjust.GetSize();i++)
      if (!(m_adjust[i] == rhs.m_adjust[i])) return false;
    */
    return true;
  }
  void Clear(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  int Serialize(CArchive& car);
  void CrossReference(class CR_LIST *pCRList, class CR_REFERENCE *pCRReference)const;
  //void AddAdjust(int value, const CString& name);
  //void AddAdjust(ADJUSTMENT& adjust);
  void AddAdjust(CString &name);
  int GetDataSize(void) const;
  CString EncodeText();
  BOOL DecodeText(CString data, CString& err);
  BOOL DecodeText(CString data);
  BOOL Roll(double &result) const;
  BOOL MaxRoll(double &result) const; // return max possible result

  BOOL Compile() const;
  //int compileRDR(CString& name, int *array);
  //int interpretRDR(int *array);
};


extern "C" int compileDicePlusRDR(CString& name, char *array);
int interpretDicePlusRDR(int *array);
int interpretDicePlusRDR(char *array);


// default number of ability references (str,int,wis,dex,con,cha)

#ifdef UAFEDITOR
#define ABREFKEY_COUNT 6
#endif
/*
class ABILITY_REFERENCE 
{
public:
  abilityType m_abilityKey; // one of the ABREFKEY_* values, for str/int/wis, etc
  CString     m_abilityName;
public:
  ABILITY_REFERENCE(void);
  ~ABILITY_REFERENCE(void);
  ABILITY_REFERENCE& operator =(const ABILITY_REFERENCE& src);
  void Clear(void);
  int GetDataSize(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  void PostSerialize(BOOL IsStoring);
};
*/



class SPELL_REFERENCE
{
public:
  //GLOBAL_SPELL_ID m_spellKey;
  SPELL_ID   m_spellID;
public:
  SPELL_REFERENCE(void);
  ~SPELL_REFERENCE(void);
  SPELL_REFERENCE& operator =(const SPELL_REFERENCE& src);
  void Clear(void);
  int GetDataSize(void) const;
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car, double version);
  void PostSerialize(BOOL IsStoring);
};

class SPELLGROUP_REFERENCE
{
public:
  //spellgroupType m_spellgroupKey;
  //CString        m_spellgroupName;
  SPELLGROUP_ID m_spellgroupID;
public:
  SPELLGROUP_REFERENCE(void) {};
  ~SPELLGROUP_REFERENCE(void) {};
  SPELLGROUP_REFERENCE& operator =(const SPELLGROUP_REFERENCE& src) {m_spellgroupID = src.m_spellgroupID;return *this;};
  void Clear(void);
  int GetDataSize(void) const;
  void PreSerialize(BOOL IsStoring) {};
  void Serialize(CAR& car, const CString& version);
  void PostSerialize(BOOL IsStoring) {};
};


/*
class RACE_REFERENCE
{
public:
  //raceType  m_raceKey;
  //CString   m_raceName;
  RACE_ID     m_raceID;
public:
  RACE_REFERENCE(void) {};
  ~RACE_REFERENCE(void) {};
  RACE_REFERENCE& operator =(const RACE_REFERENCE& src) {m_raceID = src.m_raceID; return *this;};
  void Clear(void);
  int GetDataSize() { return m_raceID.GetDataSize() + sizeof(*this);};
  void PreSerialize(BOOL IsStoring){};
  int Serialize(CAR& car, int intVer);
  void PostSerialize(BOOL IsStoring){};
};
*/

// used if no entry for an ability in the config file
#define DEFAULT_MIN_ABILITY_LIMIT 3
#define DEFAULT_MAX_ABILITY_LIMIT 18

class ABILITY_REQ
{
public:
  //ABILITY_REFERENCE m_ability;
  ABILITY_ID m_abilityID;
  short   m_min; // Positive
  short   m_minMod;
  short   m_max; // Positive
  short   m_maxMod;
public:
  ABILITY_REQ(void);
  ABILITY_REQ(int min, int minmod, int max, int maxmod);
  ~ABILITY_REQ(void);
  ABILITY_REQ& operator =(const ABILITY_REQ& a);
  BOOL operator ==(const ABILITY_REQ& a) const
  {
    return    (m_abilityID==a.m_abilityID)
           && (m_min==a.m_min)
           && (m_minMod==a.m_minMod)
           && (m_max==a.m_max)
           && (m_maxMod==a.m_maxMod);
  };
  void Clear(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference) const;

  //abilityType GetKey(void) const { return m_ability.m_abilityKey; }
  //CString Name(void) const { return m_ability.m_abilityName; }
  CString Name(void) const { return m_abilityID.UniqueName(); }
  short GetMin(void) const { return m_min; }
  short GetMinMod(void) const { return m_minMod; }
  short GetMax(void) const { return m_max; }
  short GetMaxMod(void) const { return m_maxMod; }

  int GetDataSize(void) const;
  CString Encode(void) const; // Convert to string such as "Strength 8/18(50)"
};

/*
  ** SAMPLE RACE **

\(BEGIN) Human
name = Human
Height        =  2d10  +  59 + 1*Male
Weight        =  6d10  + 100 + 40*Male
Age           = 1d4   +  15
MaxAge        = 2d20  +  90
BaseMovement  = 12
AbilityRequirement = Strength       3/18
AbilityRequirement = Dexterity      3/18
AbilityRequirement = Constitution   3/18
AbilityRequirement = Intelligence   3/18
AbilityRequirement = Wisdom         3/18
AbilityRequirement = Charisma       3/18
\(END)
*/

struct SKILL
{
  SKILL_ID skillID;
  int  value;
  void Serialize(CAR& car); // Added 20121208 PRS
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};

#define NoSkill 0x80000000

struct BONUS_XP
{
  ABILITY_ID   abilityID;
  int          bonus[HIGHEST_CHARACTER_PRIME];
  char         bonusType; // eg: '%' for percentage
  CAR&         Serialize(CAR& car);
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  //void         UpdateSkillValue(const CHARACTER *pChar, double *val) const;
  BOOL         operator ==(const BONUS_XP& b) const
  {
    return     (abilityID==b.abilityID)
            && (bonusType == b.bonusType)
            && (memcmp(bonus,b.bonus,sizeof(bonus))==0);
  };
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};


struct SKILL_ADJUSTMENT_ABILITY
{
  SKILL_ID     skillID;
  ABILITY_ID   abilityID;
  short        skillAdj[HIGHEST_CHARACTER_PRIME];
  char         adjType; // eg: '%' for percentage
  CAR&         Serialize(CAR& car);
  //void         UpdateSkillValue(const CHARACTER *pChar, double baseVal, bool minimize) const;
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  void         UpdateSkillValue(SKILL_COMPUTATION& SC) const;
  BOOL         operator ==(const SKILL_ADJUSTMENT_ABILITY& s) const
  {
    return     (skillID==s.skillID)
            && (abilityID==s.abilityID)
            && (adjType == s.adjType)
            && (memcmp(skillAdj,s.skillAdj,sizeof(skillAdj))==0);
  };
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};

struct SKILL_ADJUSTMENT_BASECLASS
{
  SKILL_ID     skillID;
  BASECLASS_ID baseclassID;
  short        skillAdj[HIGHEST_CHARACTER_LEVEL];
  char         adjType;
  CAR&         Serialize(CAR& car);
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  //void         UpdateSkillValue(int baseclassLevel, 
  //                              double baseVal, 
  //                              double *pBestBaseclassAdj,
  //                              bool minimize) const;
  void         UpdateSkillValue(SKILL_COMPUTATION& SC) const; 
  BOOL         operator ==(const SKILL_ADJUSTMENT_BASECLASS& s) const
  {
    return     (skillID==s.skillID)
            && (baseclassID==s.baseclassID)
            && (adjType == s.adjType)
            && (memcmp(skillAdj,s.skillAdj,sizeof(skillAdj))==0);
  };
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};

struct SKILL_ADJUSTMENT_RACE
{
  SKILL_ID     skillID;
  RACE_ID      raceID;
  short        skillAdj;
  char         adjType;
  CAR&         Serialize(CAR& car);
  //void         UpdateSkillValue(double val, double *pBestAdjRace) const;
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  void         UpdateSkillValue(SKILL_COMPUTATION& SC) const;
  BOOL         operator ==(const SKILL_ADJUSTMENT_RACE& s) const
  {
    return     (skillID==s.skillID)
            && (raceID==s.raceID)
            && (adjType == s.adjType)
            && (skillAdj==s.skillAdj);
  };
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};

struct SKILL_ADJUSTMENT_SCRIPT
{
  SKILL_ID     skillID;
  CString      specialAbilityName;
  CString      scriptName;
  CAR&         Serialize(CAR& car);
  void CrossReference(class CR_LIST* pCRList, class CR_REFERENCE* pCRReference)const;
  double       UpdateSkillValue(double val) const;
  BOOL         operator ==(const SKILL_ADJUSTMENT_SCRIPT& s) const
  {
    return (skillID==s.skillID)&&(specialAbilityName==s.specialAbilityName);
  };
  void Encode(CONFIG_PKT *pkt) const;
  void Decode(CONFIG_PKT *pkt);
};

class RACE_DATA
{
private:

  friend class RACE_DATA_TYPE;
  friend static void raceObject(CONFIG_PKT *pkt);
  friend static void DraceSpecialAbility(CONFIG_PKT *pkt);
  friend static void DraceMultiClass (CONFIG_PKT *pkt);
  friend static void DraceKey(CONFIG_PKT *pkt);
  friend int ReadRaceDB(LPCSTR file,
                        RACE_DATA_TYPE *racePointer,
                        CONFIG_FILE *pConfigFile);


  //mutable           DWORD m_raceKey;
  int  preSpellNameKey;
	A_ASLENTRY_L      m_race_asl;
  A_ASLENTRY_L      m_temp_asl;

public:
  CString     m_name;
  DICEPLUS    m_weight;
  DICEPLUS    m_height;
  DICEPLUS    m_age;
  DICEPLUS    m_maxAge;
  DICEPLUS    m_baseMovement; // Positive
  BOOL        m_canChangeClass;
  BOOL        m_dwarfResistance;
  BOOL        m_gnomeResistance;
  long        m_findSecretDoor;
  long        m_findSecretDoorSearching;
private:
  mCArray<ABILITY_REQ, ABILITY_REQ&> m_abilityRequired;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(AbilityReq,        /* array data name      */ \
                                  ABILITY_ID,        /* Search variable type */ \
                                  abilityID,         /* search variable name */ \
                                  m_abilityRequired, /* array name           */ \
                                  ABILITY_REQ,       /* array data type      */ \
                                  RACE_DATA)         /* type of *this        */
  int LocateAbilityReq(const CString& abilityName) const;
private:
  mCArray<SKILL, SKILL&>m_skills;
  mCArray<SKILL_ADJUSTMENT_ABILITY,   SKILL_ADJUSTMENT_ABILITY&>   m_skillAdjustmentsAbility;
  mCArray<SKILL_ADJUSTMENT_BASECLASS, SKILL_ADJUSTMENT_BASECLASS&> m_skillAdjustmentsBaseclass;
  mCArray<SKILL_ADJUSTMENT_RACE,      SKILL_ADJUSTMENT_RACE&>      m_skillAdjustmentsRace;
  mCArray<SKILL_ADJUSTMENT_SCRIPT,    SKILL_ADJUSTMENT_SCRIPT&>    m_skillAdjustmentsScript;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Skill,      /* array data name      */ \
                                  SKILL_ID,   /* Search variable type */ \
                                  skillID,    /* search variable name */ \
                                  m_skills,   /* array name           */ \
                                  SKILL,      /* array data type      */ \
                                  RACE_DATA)  /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjAbility,           /* array data name      */ \
                                  SKILL_ID,                  /* Search variable type */ \
                                  skillID,                   /* search variable name */ \
                                  m_skillAdjustmentsAbility, /* array name           */ \
                                  SKILL_ADJUSTMENT_ABILITY,  /* array data type      */ \
                                  RACE_DATA)                 /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjBaseclass,           /* array data name      */ \
                                  SKILL_ID,                    /* Search variable type */ \
                                  skillID,                     /* search variable name */ \
                                  m_skillAdjustmentsBaseclass, /* array name           */ \
                                  SKILL_ADJUSTMENT_BASECLASS,  /* array data type      */ \
                                  RACE_DATA)                   /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjRace,             /* array data name      */ \
                                  SKILL_ID,                 /* Search variable type */ \
                                  skillID,                  /* search variable name */ \
                                  m_skillAdjustmentsRace,   /* array name           */ \
                                  SKILL_ADJUSTMENT_RACE,    /* array data type      */ \
                                  RACE_DATA)                /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjScript,           /* array data name      */ \
                                  SKILL_ID,                 /* Search variable type */ \
                                  skillID,                  /* search variable name */ \
                                  m_skillAdjustmentsScript, /* array name           */ \
                                  SKILL_ADJUSTMENT_SCRIPT,  /* array data type      */ \
                                  RACE_DATA)                /* type of *this        */

void GetSkillValue(SKILL_COMPUTATION& SC) const;

void UpdateSkillValue(SKILL_COMPUTATION& SC) const;


  
  SPECIAL_ABILITIES m_specAbs;
  CString RunRaceScripts(LPCSTR     scriptName, 
                         CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                         void      *pkt,
                         LPCSTR     comment
                         ) const
  {
    return m_specAbs.RunScripts(scriptName, 
                                fnc, 
                                pkt,
                                comment,
                                ScriptSourceType_Race, 
                                m_name);
  };

public:
  RACE_DATA();
  RACE_DATA(const RACE_DATA &src);
  ~RACE_DATA();
  RACE_DATA *operator=(const RACE_DATA &src);
  RACE_ID RaceID(void) const {RACE_ID raceID; raceID = m_name; return raceID;};
  int GetDataSize(void) const;

  void Clear(void);
  void LogUndefinedMultiClassReferences(void);
  int Serialize(CAR &car, const CString& version);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);
#ifdef UAFEDITOR
  void CrossReference(class CR_LIST *pCRList) const;
#endif
  //void AddAbilityRequirement(ABILITY_LIMITS& abReq);
  void AddAbilityRequirement(ABILITY_REQ& abReq);
  void AddAllowedClass(const CString& className);


  CString Name(void) const { return m_name; }
  DWORD GetWeight(void) const;
  DWORD GetHeight(void) const;
  DWORD GetStartAge(void) const;
  DWORD GetMaxAge(void) const;
  DWORD GetBaseMovement(void) const;
  //short GetMinAbility(abilityType type) const;
  //short GetMaxAbility(abilityType type) const;
  short GetMinAbility(const CString& abilityName) const;
  short GetMaxAbility(const CString& abilityName) const;

  //Check to see if a particular class is allowed.
  bool IsAllowedClass(const CString& className) const;
};



class RACE_DATA_TYPE
{
private:
  //POSITION         m_pos;
  //CCriticalSection m_CS;
  //OrderedQueue<RACE_DATA, MAX_RACES> m_RaceData;
  mCArray<RACE_DATA, RACE_DATA&> m_RaceData;
  // No locking... for internal use
  // Return key associated with the named object, or zero
  //raceType m_FindName (const CString& name);
public:
  RACE_DATA_TYPE();
  ~RACE_DATA_TYPE();
  void Clear()
  { 
    //CSingleLock sLock(&m_CS, TRUE); 
    m_RaceData.RemoveAll(); 
  };
  void operator =(const RACE_DATA_TYPE& src);
  void LoadUADefaults(void);
  int Serialize(CAR &car);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);

  void Save(CArchive& ar);
  void Restore(CArchive& ar); // Restore to temporary location
  void CommitRestore(void);
  void CrossReference(class CR_LIST* pCRList);
  int GetCount() const 
  { 
    //CSingleLock sLock(&m_CS, TRUE); 
    //return m_RaceData.GetCount(); 
    return m_RaceData.GetSize(); 
  };
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Race,RACE_ID,raceID,m_RaceData,RACE_DATA,RACE_DATA_TYPE)
  //int        LocateRace (const RACE_ID& raceID) const;
  //RACE_DATA *GetRace    (const RACE_ID& raceID) {return GetRace(LocateRace(raceID));};
  //const RACE_DATA *PeekRace   (const RACE_ID& raceID) const {return PeekRace(LocateRace(raceID));};
  //RACE_DATA *GetRace    (int i) {return (i>=0)?&m_RaceData[i]:NULL;};
  //const RACE_DATA *PeekRace   (int raceIndex) const {return const_cast<RACE_DATA_TYPE*>(this)->GetRace(raceIndex);};


  //POSITION GetHeadPosition() { return m_RaceData.GetHeadPosition(); }


  //      RACE_DATA& GetAtPos (POSITION pos)       { return m_RaceData.GetAtPos(pos); }
  //const RACE_DATA& PeekAtPos(POSITION pos) const { return m_RaceData.PeekAtPos(pos); }


  //RACE_DATA& GetNext(POSITION &pos) { return m_RaceData.GetNext(pos); }
  //int GetKeyAt(POSITION pos) { return m_RaceData.GetKeyAt(pos); }

  //raceType GetNextRace();

  // With locking...for public use
  // Return key associated with the named object, or zero
  //raceType FindName(const CString& name);
  //CString GetRaceName(raceType race);
  CString GetRaceName(const RACE_ID& raceID);
  BOOL HaveSpell(DWORD index);
  int GetDataSize(void) const;

  

  // Called when loading from scripts, or
  // when adding from editor GUI
  //raceType AddRace(const RACE_DATA &data);
  void AddRace(const RACE_DATA &data);

  // Called when loading binary race data that
  // was saved with key info
  //raceType AddRaceWithCurrKey(const RACE_DATA &data);

  //BOOL SetRace(DWORD index, const RACE_DATA &race);
  //BOOL RemoveRace(raceType race);
/*
  DWORD GetWeight(raceType race);  
  DWORD GetHeight(raceType race);
  DWORD GetStartAge(raceType race);
  DWORD GetMaxAge(raceType race);
  DWORD GetBaseMovement(raceType race);
  short GetMinAbility(raceType race,abilityType type) const;
  short GetMaxAbility(raceType race,abilityType type) const;

  //BOOL GetRaceData(raceType race, RACE_DATA *raceData);
  bool IsAllowedClass(raceType race, const CString& className) const;
*/
  DWORD GetWeight      (const RACE_ID& raceID) const;  
  DWORD GetHeight      (const RACE_ID& raceID) const;
  DWORD GetStartAge    (const RACE_ID& raceID) const;
  DWORD GetMaxAge      (const RACE_ID& raceID) const;
  DWORD GetBaseMovement(const RACE_ID& raceID) const;
  bool  IsAllowedClass (const RACE_ID& raceID, const CString& className) const;
  //short GetMinAbility  (const RACE_ID& raceID, abilityType type) const;
  //short GetMaxAbility  (const RACE_ID& raceID, abilityType type) const;
  BOOL  HasDwarfResistance(const RACE_ID& raceID) const;
  BOOL  HasGnomeResistance(const RACE_ID& raceID) const;

  //BOOL GetRaceData(raceType race, RACE_DATA *raceData);  
  //RACE_DATA *GetRaceData(const CString& raceName);
  //RACE_DATA *GetRaceData(raceType race);

#ifdef UAFEDITOR
  void ExpandDefaultFilenames();
  void CollapseDefaultFilenames();
  RACE_ID FindPreVersionSpellNamesRaceID(DWORD id) const;
#endif
};



/*
  ** SAMPLE ABILITY **

\(BEGIN)
name = Strength
abbreviation = STR
roll = 3d6 min 3 max 18 
modify = 18
adjust = -1 * Halfling
\(END)
*/

const double NoSkillAdj = -987656789.0;

class ABILITY_DATA
{
  friend class ABILITY_DATA_TYPE;
public:
  //mutable DWORD m_abilityKey;
  CString  m_name;
  CString  m_abbreviation;
  DICEPLUS m_roll;
  BYTE     m_modify;  // Ability score to modify.  Like Strength = 18(34)
  SPECIAL_ABILITIES m_specAbs;
  double bestSkillAdj; // Not serialized.  Used temporarily by GetAdjSkillValue.
  bool operator ==(const ABILITY_DATA& src) const;
  ABILITY_DATA& operator =(const ABILITY_DATA& src);
public:
  ABILITY_DATA(void);
  ABILITY_DATA(ABILITY_DATA& src);
  ~ABILITY_DATA(void);
  void Clear(void);
  int GetDataSize(void) const;
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car, const CString& version);
  void PostSerialize(BOOL IsStoring);
  //void AddAdjust(ADJUSTMENT& adjust);
#ifdef UAFEDITOR
  void CrossReference(class CR_LIST* pCRList);
#endif
  void AddAdjust(CString &name);
  BOOL RollAbility(double &result) const;
};

class ABILITY_DATA_TYPE
{
private:
  // No locking... for internal use
  // Return key associated with the named object, or zero
  abilityType m_FindName (const CString& name) const;
  //POSITION         m_pos;
  //OrderedQueue<ABILITY_DATA, MAX_ABILITIES> m_AbilityData;
  mCArray<ABILITY_DATA, ABILITY_DATA&> m_abilityData;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(AbilityData,       /* array data name      */ \
                                  ABILITY_ID,        /* Search variable type */ \
                                  abilityID,         /* search variable name */ \
                                  m_abilityData,     /* array name           */ \
                                  ABILITY_DATA,      /* array data type      */ \
                                  ABILITY_DATA_TYPE) /* type of *this        */
  int LocateAbilityData(const CString& abilityName) const;

  //CCriticalSection m_CS;

  ABILITY_DATA_TYPE(void);
  ~ABILITY_DATA_TYPE(void);
  void Clear(void);
  void LoadUADefaults(void);
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void CrossReference(class CR_LIST* pCRList);
  void PostSerialize (BOOL IsStoring);
  //unsigned int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_AbilityData.GetCount(); }
  //unsigned int GetCount() { return m_AbilityData.GetCount(); }
  unsigned int GetCount() { return m_abilityData.GetSize(); }
  int GetDataSize(void);  
  void ClearAdjustments(double val);
  double TotalAdjustments(double val);
  //void UpdateSkillValues(SKILL_COMPUTATION& SC);
  //abilityType GetKeyByIndex(int index) const;
  //CString GetName(abilityType type) const;
  //CString GetShortName(abilityType type) const;
  CString GetShortName(const CString& abilityName) const;

  //abilityType GetNextAbility();

  //POSITION GetHeadPosition() { return m_AbilityData.GetHeadPosition(); }


  //      ABILITY_DATA& GetAtPos (POSITION pos)       { return m_AbilityData.GetAtPos(pos); }
  //const ABILITY_DATA& PeekAtPos(POSITION pos) const { return m_AbilityData.PeekAtPos(pos); }


  //ABILITY_DATA& GetNext(POSITION &pos) { return m_AbilityData.GetNext(pos); }
  //abilityType GetKeyAt(POSITION pos) { return m_AbilityData.GetKeyAt(pos); }
  //ABILITY_DATA *GetAbilityData(const CString& abilityName);
  //abilityType FindName(const CString& name);
  // Called when loading from scripts, or
  // when adding from editor GUI
  //abilityType AddAbility(const ABILITY_DATA &data);
  void AddAbility(const ABILITY_DATA &data);

  // Called when loading binary ability data that
  // was saved with key info
  //abilityType AddAbilityWithCurrKey(const ABILITY_DATA &data);

  //BOOL RollAbility(abilityType ability, double &result);
  BOOL RollAbility(const CString& abilityName, double &result) const ;
};

struct HIT_DICE_LEVEL_BONUS
{
  CString ability;
  BASECLASS_ID baseclassID;
  char bonusValues[HIGHEST_CHARACTER_PRIME];
  bool operator ==(const HIT_DICE_LEVEL_BONUS& src) const;
  void Serialize(CAR& car);
  void CrossReference(class CR_LIST *pCRList, class CR_REFERENCE *CRReference) const;
};


/*
    ** SAMPLE CLASS **

\(BEGIN)
name = Cleric/Fighter/Magic User
baseclass = cleric
baseclass = fighter
baseclass = magicUser
\(END)
*/

class CLASS_DATA
{
  friend class CLASS_DATA_TYPE;
  friend static void DclassBaseclass(CONFIG_PKT *pkt);
  friend static void DclassHitDiceBonus(CONFIG_PKT *pkt);
  friend static void DclassHitDiceLevelBonus(CONFIG_PKT *pkt);
private:
  BASECLASS_LIST m_baseclasses;
// See BASE_CLASS_DATA  CArray<ABILITY_LIMITS, ABILITY_LIMITS&> m_abilityLimits;
public:
  //char hitDiceBonusValues[HIGHEST_CHARACTER_PRIME];
  //int GetHitDiceBonus(int prime) const;
  int DetermineCharHitDice(const CHARACTER *pChar) const;

  //CString hitDiceBonusAbility;
private:
  mCArray<HIT_DICE_LEVEL_BONUS, HIT_DICE_LEVEL_BONUS&>hitDiceLevelBonus;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(HitDiceLevelBonus,     /* array data name      */ \
                                  CString,               /* Search variable type */ \
                                  baseclass,             /* search variable name */ \
                                  hitDiceLevelBonus,     /* array name           */ \
                                  HIT_DICE_LEVEL_BONUS,  /* array data type      */ \
                                  CLASS_DATA)            /* type of *this        */

  DICEPLUS strengthBonusDice;
  //mutable DWORD m_classKey;
  int preSpellNameKey;
  CString m_name;
  SPECIAL_ABILITIES m_specialAbilities;
  CString RunClassScripts(LPCSTR     scriptName, 
                          CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                          void      *pkt,
                          LPCSTR     comment) const
  {
    return m_specialAbilities.RunScripts(scriptName, 
                                         fnc, 
                                         pkt,
                                         comment,
                                         ScriptSourceType_Class, 
                                         m_name);
  };
public:
  CLASS_DATA(void);
  ~CLASS_DATA(void);
  CLASS_DATA& operator =(const CLASS_DATA& src);
  BOOL operator ==(const CLASS_DATA& src) const;
  void Clear(void);

  ITEM_LIST m_startingEquipment;
  BASECLASS_ID hitDiceBaseclassID;


  //The following should someday become unnecessary.  When
  //classType is defined as a DWORD like spellType, abilityType, etc.
  //For now, classType is an enum.i
  //classType WhatIsYourClassType(void) const {return (classType)m_classKey;};
  CLASS_ID ClassID(void) const {CLASS_ID cid; cid = m_name; return cid;};
  CLASS_ID WhatIsYourClassType(void) const {return ClassID();};
  int GetDataSize(void) const;



  int GetCount(void) const{return m_baseclasses.GetCount();};
  int                 LocateBaseclass(const BASECLASS_ID& baseclassID) const;
  const BASECLASS_ID *PeekBaseclassID(int i) const {return m_baseclasses.PeekBaseclassID(i);};
#ifdef UAFEDITOR
  int   LocateBaseclassID(const char *baseclassName) 
  {
    BASECLASS_ID baseclassID;
    baseclassID = baseclassName;
    return m_baseclasses.LocateBaseclassID(baseclassID);
  };
#endif
  //void AddBaseclass(baseclassKey bclass);
  void AddBaseclass(const BASECLASS_ID& baseclassID);
       // If not already present.
  //baseclassKey GetNextBaseclass(baseclassKey bck);
  //void AddAbilityLimit(abilityType ab, 
  //                     int min, int minmod, 
  //                     int max, int maxmod);
  void AddAbilityLimit(ABILITY_ID abilityID, 
                       int min, int minmod, 
                       int max, int maxmod);
       // Or replace the current one.
  int Serialize(CAR &car);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);
  //ABILITYLIMITS GetAbilityLimits(abilityType abtype) const; // Min/Max allowed for this class
  ABILITYLIMITS GetAbilityLimits(ABILITY_ID abilityID) const; // Min/Max allowed for this class
#ifdef UAFEDITOR
  void CrossReference(class CR_LIST* pCRList);
#endif
    WORD GetAllowedAlignments(void) const;

  int GetCharTHAC0(const CHARACTER *pCharacter) const;
  void ComputeCharSavingThrows(const CHARACTER *, int *ppd, int *rsw, int *pp, int *br, int *sp) const;
  void GetThiefSkills(const CHARACTER *pChar, 
                      int *pp,
                      int *ol,
                      int *ft,
                      int *ms,
                      int *hs,
                      int *hn,
                      int *cw,
                      int *rl) const;


  //bool IsRaceAllowed(raceType race) const;
  bool IsRaceAllowed(const RACE_ID& raceID) const;
  void CreateAllowedSchoolList(SCHOOL_LIST *pSchoolList) const;
  BOOL MatchSpellBaseclass(const SPELL_DATA *pSpell) const;
  BOOL MatchSpellSchool(const SCHOOL_LIST *pSchoolList) const;
  const BASECLASS_LIST *BaseclassList(void) const {return &m_baseclasses;};

};
/*
   ** SAMPLE BASECLASS **

\(BEGIN)
name = cleric
Prime Requisite = Wisdom
AbilityRequirement = Wisdom 9/99
Allowed Alignment = any
Allowed race = Human
Allowed race = Halfling
Allowed race = Half-Elf
Allowed race = Gnome
Allowed race = Elf
Allowed race = Dwarf
ExpLevel = 0
ExpLevel = 1500
ExpLevel = 3000
ExpLevel = 6000
ExpLevel = 13000
ExpLevel = 27500
ExpLevel = 55000
ExpLevel = 110000
ExpLevel = 225000
ExpLevel = 450000
\(END)
*/

class CASTING_INFO
{
public:
    CASTING_INFO(void):schoolID() 
    {
      Clear();
    };
    void Clear(void)
    {
      memset(m_spellLimits[0],         0, sizeof(m_spellLimits));
      memset(m_maxSpellLevelsByPrime,  0, sizeof(m_maxSpellLevelsByPrime));
      memset(m_maxSpellsByPrime,       0, sizeof(m_maxSpellsByPrime));
      schoolID.Empty();
      primeAbility.Empty();
    };
    void Serialize(CAR& car);

    SCHOOL_ID schoolID;
    CString primeAbility;  // Wisdom, etc.
    // This array is defined for each baseclass and magic school by
    // the variable 'Spell Limits'.
    //
    // The table attached to a particular baseclass and magic school.
    // The rows are indexed by the baseclass level.
    // Each row is indexed by a spell level and provides
    // the maximum number of spells that can be learned 
    // at that spell level.
    //
    // So this table answers the question:
    // How many level 3 Cleric spells can be learned by a level 5 Paladin?
    // The answer must meet the limits imposed by the
    // 1 - m_maxSpellLevels
    // 2 - m_MaxSpells
    BYTE m_spellLimits[HIGHEST_CHARACTER_LEVEL][MAX_SPELL_LEVEL];


    // The maximum spell level allowed this baseclass and school of magic.
    // Indexed by prime attribute score.
    // It is defined in the text files by the variable 'Max Spell Levels'.
    // For example, a baseclass Magic User with intelligence
    // of 9 might have a highest spell level of 4 in the Magic User school..
    BYTE m_maxSpellLevelsByPrime[HIGHEST_CHARACTER_PRIME];


    // This array is defined for each baseclass by
    // the variable 'Max Spells' in the Baseclass.txt file.
    //
    // This array is indexed by prime attribute score.
    // For example, the MagicUser school of magic commonly
    // uses Intelligence as the prime attribute.  The
    // character can 'Know' as many spells in the MagicUser
    // school as his intelligence will allow.
    // This table, indexed by intelligence, provides
    // that number.
    //
    // For example, an intelligence of 9 might
    // allow the character to 'Know' a maximum of 6 spells from
    // the MagicUser school of magic.
    BYTE m_maxSpellsByPrime[HIGHEST_CHARACTER_PRIME];
};

/*
struct SAVE_VS
{
  SAVE_VS(void) {memset(save, 0, sizeof(save));};
  SAVE_VS(const CString& versus, const BYTE *s);
  CString versusName;
  BYTE    save[HIGHEST_CHARACTER_LEVEL];
  CAR&    Serialize(CAR& car);
  BOOL    operator ==(const SAVE_VS& s) const 
  {
    return (versusName==s.versusName) && (memcmp(save,s.save,sizeof(save))==0);
  };
};
*/
class BASE_CLASS_DATA
{
  friend class BASE_CLASS_DATA_TYPE;
public:
  //mutable DWORD m_baseclassKey;
  int m_preSpellNameKey;
  CString m_name;

//  This is unused!
//private:
//  CArray<ABILITY_ID, ABILITY_ID&> m_primeReq;
//public:
//  DEFINE_CARRAY_ACCESS_FUNCTIONS(PrimeReq,         /* array data name      */ \
//                                 ABILITY_ID,       /* Search variable type */ \
//                                 abilityID,        /* search variable name */ \
//                                 m_primeReq,       /* array name           */ \
//                                 ABILITY_ID,       /* array data type      */ \
//                                 BASE_CLASS_DATA)  /* type of *this        */

private:

private:
  mCArray<ABILITY_REQ, ABILITY_REQ&> m_abilityReq;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(AbilityReq,      /* array data name      */ \
                                  ABILITY_ID,      /* Search variable type */ \
                                  abilityID,       /* search variable name */ \
                                  m_abilityReq,    /* array name           */ \
                                  ABILITY_REQ,     /* array data type      */ \
                                  BASE_CLASS_DATA) /* type of *this        */

ABILITYLIMITS BASE_CLASS_DATA::GetAbilityReqs(ABILITY_ID abilityID) const;

private:

  mCArray<RACE_ID, RACE_ID&> m_allowedRaces;  
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(RaceID,          /* array data name      */ \
                                  RACE_ID,         /* Search variable type */ \
                                  raceID,          /* search variable name */ \
                                  m_allowedRaces,  /* array name           */ \
                                  RACE_ID,         /* array data type      */ \
                                  BASE_CLASS_DATA) /* type of *this        */

  mCArray<DWORD, DWORD> m_expLevels;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(ExpLevels,       /* array data name      */ \
                                  DWORD,           /* Search variable type */ \
                                  expLevel,        /* search variable name */ \
                                  m_expLevels,     /* array name           */ \
                                  DWORD,           /* array data type      */ \
                                  BASE_CLASS_DATA) /* type of *this        */
//private:
//
//  CArray<int, int> m_maxSpellLevelByPrime;
//public:
//  DEFINE_CARRAY_ACCESS_FUNCTIONS(MaxSpellLevel,          /* array data name      */ \
//                                 int,                    /* Search variable type */ \
//                                 masSpellLevel,          /* search variable name */ \
//                                 m_maxSpellLevelByPrime, /* array name           */ \
//                                 int,                    /* array data type      */ \
//                                 BASE_CLASS_DATA)        /* type of *this        */
  
private:  
  mCArray<CASTING_INFO, CASTING_INFO&> m_castingInfo;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(CastingInfo,     /* array data name      */ \
                                  SCHOOL_ID,       /* Search variable type */ \
                                  schoolID,        /* search variable name */ \
                                  m_castingInfo,   /* array name           */ \
                                  CASTING_INFO,    /* array data type      */ \
                                  BASE_CLASS_DATA) /* type of *this        */


  char THAC0[HIGHEST_CHARACTER_LEVEL];


  WORD m_allowedAlignments; // Bitmap of 1 << alignmentType
  SPECIAL_ABILITIES m_specAbs;
public:
                               
  CString RunBaseClassScripts(LPCSTR     scriptName, 
                              CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                              void      *pkt,
                              LPCSTR     comment) const
  {
    return m_specAbs.RunScripts(scriptName, 
                                fnc, 
                                pkt,
                                comment,
                                ScriptSourceType_Baseclass, 
                                m_name);
  };
  DICEDATA hitDice[HIGHEST_CHARACTER_LEVEL];
  void GetHitDice(int level, int *sides, int *numDice, int *constant) const;
  void DetermineCharHitDice(int *totalDice, int  *maxDice, int level) const;
private:
  mCArray<SKILL, SKILL&>m_skills;
  mCArray<SKILL_ADJUSTMENT_ABILITY,   SKILL_ADJUSTMENT_ABILITY&>   m_skillAdjustmentsAbility;
  mCArray<SKILL_ADJUSTMENT_BASECLASS, SKILL_ADJUSTMENT_BASECLASS&> m_skillAdjustmentsBaseclass;
  mCArray<SKILL_ADJUSTMENT_RACE,      SKILL_ADJUSTMENT_RACE&>      m_skillAdjustmentsRace;
  mCArray<SKILL_ADJUSTMENT_SCRIPT,    SKILL_ADJUSTMENT_SCRIPT&>    m_skillAdjustmentsScript;
  mCArray<BONUS_XP,                   BONUS_XP&>                   m_bonusXP;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Skill,      /* array data name      */ \
                                  SKILL_ID,   /* Search variable type */ \
                                  skillID,    /* search variable name */ \
                                  m_skills,   /* array name           */ \
                                  SKILL,      /* array data type      */ \
                                  BASE_CLASS_DATA)  /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjAbility,           /* array data name      */ \
                                  SKILL_ID,                  /* Search variable type */ \
                                  skillID,                   /* search variable name */ \
                                  m_skillAdjustmentsAbility, /* array name           */ \
                                  SKILL_ADJUSTMENT_ABILITY,  /* array data type      */ \
                                  BASE_CLASS_DATA)           /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjBaseclass,           /* array data name      */ \
                                  SKILL_ID,                    /* Search variable type */ \
                                  skillID,                     /* search variable name */ \
                                  m_skillAdjustmentsBaseclass, /* array name           */ \
                                  SKILL_ADJUSTMENT_BASECLASS,  /* array data type      */ \
                                  BASE_CLASS_DATA)             /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjRace,             /* array data name      */ \
                                  SKILL_ID,                 /* Search variable type */ \
                                  skillID,                  /* search variable name */ \
                                  m_skillAdjustmentsRace,   /* array name           */ \
                                  SKILL_ADJUSTMENT_RACE,    /* array data type      */ \
                                  BASE_CLASS_DATA)          /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SkillAdjScript,           /* array data name      */ \
                                  SKILL_ID,                 /* Search variable type */ \
                                  skillID,                  /* search variable name */ \
                                  m_skillAdjustmentsScript, /* array name           */ \
                                  SKILL_ADJUSTMENT_SCRIPT,  /* array data type      */ \
                                  BASE_CLASS_DATA)          /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(BonusXP,                  /* array data name      */ \
                                  ABILITY_ID,               /* Search variable type */ \
                                  abilityID,                /* search variable name */ \
                                  m_bonusXP,                /* array name           */ \
                                  BONUS_XP,                 /* array data type      */ \
                                  BASE_CLASS_DATA)          /* type of *this        */




#ifdef UAFEDITOR
void AddSkillAdjAbility  (const CString& skillName,
                          const CString& abilityName, 
                          const char  adjType, 
                          short      *adjustments,
                          int   size);

void AddSkillAdjBaseclass(const CString& skillName,
                          const CString& baseclassName,
                          const char adjType, 
                          const BYTE *adjustments,
                          int   size);

void AddSkillAdjRace     (const CString& skillName,   
                          const CString& raceName,
                          char  adjType, 
                          int   adjustment);

void AddSkill            (const CString& skillName,   
                          int   initValue);

void AddDefaultBonusXP   (const CString& abilityName,
                          int   abilityValue,
                          int   bonusValue);
#endif
//void UpdateSkillValue(const CHARACTER *pChar, 
//                      const SKILL_ID& skillID, 
//                      double baseVal,
//                      double *bestRaceAdj,
//                      double *bestBaseclassAdj,
//                      bool    minimize) const;
void UpdateSkillValue(SKILL_COMPUTATION& SC) const;



  BASE_CLASS_DATA(void);
  ~BASE_CLASS_DATA(void);
  BASE_CLASS_DATA& operator =(const BASE_CLASS_DATA& src);
  BOOL operator ==(const BASE_CLASS_DATA& src) const;
  void Clear(void);
  int GetDataSize(void) const;
  BASECLASS_ID BaseclassID(void) const {BASECLASS_ID bci; bci=m_name; return bci;};
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void PostSerialize (BOOL IsStoring);
  //void AddPrimeReq(const CString& reqName);
  //void AddAbilityRequirement(ABILITY_LIMITS& abReq);
#ifdef UAFEDITOR
  void CrossReference(class CR_LIST* pCRList) const;
#endif
  void AddAbilityRequirement(ABILITY_REQ& abReq);
  //void AddAllowedRace(RACE_REFERENCE& race);
  void AddAllowedRace(const RACE_ID& raceID);

  void AddExpLevel(DWORD exp);
  //void SetUAAbilityLimits(void); 
  void SetUAAbilityLimits(int baseclassArrayIndex); 
  //void SetUAAllowedAlignments(void);
  void SetUAAllowedAlignments(int baseclassArrayIndex);
  ABILITYLIMITS GetAbilityLimits(abilityType abType) const;
  //bool IsAllowedRace(raceType race) const;
  bool IsAllowedRace(const RACE_ID& raceID) const;
  int  GetMaxLevel(void) const;
  int  GetMaxExpForLevel(int level) const;
  int  GetMinExpForLevel(int level) const;
  int  GetLevel(DWORD exp) const;

  //void ComputeCharSavingThrow(const CString& versus, int *result, int level) const;
  void GetSkillValue(SKILL_COMPUTATION& SC) const;


  CString m_spellBonusAbility;  // Strength, etc.
private:
public:
  CArray <BYTE, BYTE> m_bonusSpells; //Byte triples prime,level,quantity
                                     // Cumulative!  At prime level 3 he gets the
                                     // bonus for levels 1, 2, and 3.
  //DEFINE_CARRAY_ACCESS_FUNCTIONS(BonusSpells,     /* array data name      */ \
  //                               BYTE,            /* Search variable type */ \
  //                               level,           /* search variable name */ \
  //                               m_bonusSpells,   /* array name           */ \
  //                               BYTE,            /* array data type      */ \
  //                               BASE_CLASS_DATA) /* type of *this        */

  //int  LocateCastingInfo(const SCHOOL_ID& schoolID);
  //const CASTING_INFO *PeekCastingInfo(int i) const {return &const_cast<BASE_CLASS_DATA*>(this)->m_castingInfo[i];};
  //CASTING_INFO *GetCastingInfo(int i) {return &m_castingInfo[i];};
  //const CASTING_INFO *PeekCastingInfo(const SCHOOL_ID& schoolID) const;
};

enum alignmentType;

class BASE_CLASS_DATA_TYPE
{
private:
  //POSITION         m_pos;
  //CCriticalSection m_CS;
  //OrderedQueue<BASE_CLASS_DATA, MAX_BASE_CLASSES> m_BaseclassData;
  mCArray<BASE_CLASS_DATA, BASE_CLASS_DATA&> m_baseclassData;
  // No locking... for internal use
  // Return key associated with the named object, or zero
  //baseclassKey m_FindName (const char *name, bool errorIfMissing=true) const;
public:
  BASE_CLASS_DATA_TYPE(void);
  ~BASE_CLASS_DATA_TYPE(void);
  //CLASS_DATA& operator [](classType type);
  BOOL operator == (const BASE_CLASS_DATA_TYPE& src) const;
  void Clear(void);
#ifdef UAFEDITOR
  void LoadUADefaults(void);
#endif
  int GetDataSize(void);
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void PostSerialize (BOOL IsStoring);
  void CrossReference(class CR_LIST* pCRList);
  //unsigned int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_BaseclassData.GetCount(); }
  int GetCount() const { return m_baseclassData.GetSize(); }



  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Baseclass,
                                  BASECLASS_ID,
                                  baseclassID,
                                  m_baseclassData,
                                  BASE_CLASS_DATA,
                                  BASE_CLASS_DATA_TYPE)

  //int LocateBaseclass(const BASECLASS_ID& baseclassID) const;
  //const BASE_CLASS_DATA *PeekBaseclass(int arrayIndex) const
  //{
  //  if (arrayIndex <0) return NULL;
  //  return &const_cast<BASE_CLASS_DATA_TYPE*>(this)->m_BaseclassData[arrayIndex];
  //};
  //const BASE_CLASS_DATA *PeekBaseclass(const BASECLASS_ID& baseclassID) const {return PeekBaseclass(LocateBaseclass(baseclassID));};
  //      BASE_CLASS_DATA *GetBaseclass(int arrayIndex);
  //      BASE_CLASS_DATA *GetBaseclass(const BASECLASS_ID& baseclassID);



  //POSITION GetHeadPosition() { return m_BaseclassData.GetHeadPosition(); }


  //      BASE_CLASS_DATA& GetAtPos (POSITION pos)       { return m_BaseclassData.GetAtPos(pos); }
  //const BASE_CLASS_DATA& PeekAtPos(POSITION pos) const { return m_BaseclassData.PeekAtPos(pos); }


  //BASE_CLASS_DATA& GetNext(POSITION &pos) { return m_BaseclassData.GetNext(pos); }
  //baseclassKey GetKeyAt(POSITION pos) { return m_BaseclassData.GetKeyAt(pos); }

  //baseclassKey GetNextBaseclass();
  
  //baseclassKey FindName(const char *name) const;
  //BASE_CLASS_DATA *GetBaseclassData(LPCSTR name);
  //BASE_CLASS_DATA *GetBaseclassData(baseclassKey baseClass);
  // Called when loading from scripts, or
  // when adding from editor GUI
  //baseclassKey AddBaseclass(const BASE_CLASS_DATA &data);
  BASECLASS_ID AddBaseclass(const BASE_CLASS_DATA &data);

  // Called when loading binary base class data that
  // was saved with key info
  //baseclassKey AddBaseclassWithCurrKey(const BASE_CLASS_DATA &data);
  //ABILITYLIMITS GetAbilityLimits(baseclassKey bType, abilityType abType);
  //WORD GetAllowedAlignments(baseclassKey bType)const;
  //bool IsAllowedRace(baseclassKey baseclass, raceType race) const;
  //bool IsAllowedAlignment(alignmentType align, const CString& baseclass);
  //int  GetMaxExpForLevel(baseclassKey bType,int level) const;
  //int  GetMinExpForLevel(baseclassKey bType,int level) const;
  //int  GetLevel(const char *baseclassName, int exp) const;

  ABILITYLIMITS GetAbilityLimits(const BASECLASS_ID& baseclassID, const ABILITY_ID& abilityID) const;
  WORD GetAllowedAlignments     (int arrayIndex) const;
  WORD GetAllowedAlignments     (const BASECLASS_ID& baseclassID) const;
  bool IsAllowedRace            (const BASECLASS_ID& baseclassID, const RACE_ID& raceID) const;
  int  GetMaxExpForLevel        (const BASECLASS_ID& baseclassID, int level) const;
  int  GetMinExpForLevel        (const BASECLASS_ID& baseclassID, int level) const;
  bool IsAllowedAlignment       (const BASECLASS_ID& baseclassID, alignmentType align) const;
  //int  GetAllowedLevel          (const BASECLASS_ID& baseclassID) const; 
  int  GetLevel                 (const BASECLASS_ID& baseclassID, int exp) const;
};


class CLASS_DATA_TYPE
{
private:
  //POSITION         m_pos;
  //PRS 20120119 CCriticalSection m_CS;
  //OrderedQueue<CLASS_DATA, MAX_CLASSES> m_ClassData;
  mCArray<CLASS_DATA, CLASS_DATA&> m_ClassData;
  // No locking... for internal use
  // Return key associated with the named object, or zero
  //classType m_FindName (const CString& name) const;
public:
  CLASS_DATA_TYPE(void);
  ~CLASS_DATA_TYPE(void);
  //CLASS_DATA& operator [](classType Class);
  //CLASS_DATA& operator [](int spellSchool);
  BOOL operator ==(const CLASS_DATA_TYPE& src) const;
  void operator =(const CLASS_DATA_TYPE& src);
  void Clear(void);
#ifdef UAFEDITOR
  void LoadUADefaults(void);
#endif
  int GetDataSize(void) const;
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void PostSerialize (BOOL IsStoring);
  //unsigned int GetCount() { /*PRS20120119 CSingleLock sLock(&m_CS, TRUE);*/ return m_ClassData.GetCount(); }
  void CrossReference(class CR_LIST* pCRList);
  int GetCount() const {return m_ClassData.GetSize(); }
//  CString Name(classType Class); // can't say "class"
//  int StatMax(classType Class, int statistic);
//   int StatMin(classType Class, int statistic);


  //POSITION GetHeadPosition() const { return m_ClassData.GetHeadPosition(); }


  //      CLASS_DATA& GetAtPos(POSITION pos)        { return m_ClassData.GetAtPos(pos); }
  //const CLASS_DATA& PeekAtPos(POSITION pos) const { return m_ClassData.PeekAtPos(pos); }
  //      CLASS_DATA& GetNext(POSITION &pos)        { return m_ClassData.GetNext(pos); }
  //const CLASS_DATA& PeekNext(POSITION &pos) const { return *m_ClassData.PeekNext(pos); }

  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Class,CLASS_ID,classID,m_ClassData,CLASS_DATA,CLASS_DATA_TYPE)
        //int         LocateClass(const CLASS_ID& classID) const;
        //CLASS_DATA *GetClass   (const CLASS_ID& classID);
        //const CLASS_DATA *PeekClass  (const CLASS_ID& classID) const {return PeekClass(LocateClass(classID));};
        //CLASS_DATA *GetClass   (int classIndex);
        //const CLASS_DATA *PeekClass  (int classIndex) const {return &const_cast<CLASS_DATA_TYPE*>(this)->m_ClassData[classIndex];};



  //classType GetKeyAt(POSITION pos) const { return (classType)m_ClassData.GetKeyAt(pos); }
  //CLASS_DATA *GetClassData(int key);
  //CLASS_DATA *GetClassData(const CString& className);

  //classType GetNextClass();
  
  //CString ClassName(classType type) const;
  //classType FindName(const CString& className);
  // Called when loading from scripts, or
  // when adding from editor GUI
  //classType AddClass(const CLASS_DATA &data);
  void AddClass(const CLASS_DATA *data);

  // Called when loading binary class data that
  // was saved with key info
  //classType AddClassWithCurrKey(const CLASS_DATA &data);
  //ABILITYLIMITS GetAbilityLimits(classType ctype, abilityType abtype); 
  //WORD GetAllowedAlignments(classType Class) const;
  //bool IsAllowedRace(classType Class, raceType race) const;
  ABILITYLIMITS GetAbilityLimits      (const CLASS_ID& classID, const ABILITY_ID& abilityID) const; 
  WORD GetAllowedAlignments           (const CLASS_ID& classID) const;
  bool IsAllowedRace                  (const CLASS_ID& classID, const RACE_ID& raceID) const;


  //int NumClasses(const CLASS_ID& classID) const;
#ifdef UAFEDITOR
  CLASS_ID FindPreVersionSpellNamesClassID(DWORD id) const;
#endif


};


//*********************************************************
// Right now (Feb 2002) a character is allowed a class if
// a) The Class has a single Base Class and that Base Class
//    names the Race of the character.
//            --or--
// b) The character's Race names the Class.
//
class ALLOWED_CLASSES
{
private:
  //classType   *m_allowedClasses;
  mCArray<CLASS_ID, CLASS_ID&> m_allowedClasses;
  //int          m_numAllowedClasses;
public:
  ALLOWED_CLASSES(void);
  ~ALLOWED_CLASSES(void);
  void Initialize(void); //Initialize to ALL classes!
  //int NumAllowedClasses(void) const;
  int GetCount(void) const {return m_allowedClasses.GetSize();};

  DEFINE_mCARRAY_ACCESS_FUNCTIONS(ClassID,CLASS_ID,classID,m_allowedClasses,CLASS_ID,ALLOWED_CLASSES)


  //classType GetAt(int i) const;
  //      CLASS_ID *GetClassID (int i);
  //const CLASS_ID *PeekClassID(int i) const;
  void RemoveAt(int i);
  //void RestrictByRace(raceType race);
  void Restrictions(const RACE_ID& raceID, const CString& gender);
};

//*********************************************************

/*
  ** SAMPLE TRAIT **

\(BEGIN)
name = Pick Pockets
adjust = "thief"         *  15
adjust = Dexterity == 9  * -15
adjust = Dexterity == 10 * -10
adjust = Dexterity == 11 *  -5
adjust = Dexterity == 17 *  +5
adjust = Dexterity == 18 * +10
adjust = Dexterity == 19 * +10
adjust = "Dwarf"         *  +5
adjust = "Half-Elf"      * +10
adjust = "Halfling"      *  +5
\(END)
*/
#ifdef USE_TRAITS
class TRAIT_DATA
{
  friend class TRAIT_DATA_TYPE;
  friend static void DtraitSpecialAbility(CONFIG_PKT *pkt);

public:
  mutable DWORD m_traitKey;
  CString  m_name;
  CString  m_abbreviation;
  //CArray<ADJUSTMENT, ADJUSTMENT&> m_adjustments;
  DICEPLUS m_roll;
  SPECIAL_ABILITIES m_specAbs;
public:
  TRAIT_DATA(void);
  TRAIT_DATA(TRAIT_DATA& src);
  ~TRAIT_DATA(void);
  TRAIT_DATA& operator =(const TRAIT_DATA& src);
   void Clear(void);
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car);
  void PostSerialize(BOOL IsStoring);
  //void AddAdjust(ADJUSTMENT& adjust);
  void AddAdjust(CString &name);
  BOOL EvaluateTrait(double &result);
};

class TRAIT_DATA_TYPE
{
private:
  // No locking... for internal use
  // Return key associated with the named object, or zero
  traitType m_FindName (const CString& name) const;
public:
  //POSITION         m_pos;
  OrderedQueue<TRAIT_DATA, MAX_TRAITS> m_TraitData;
  CCriticalSection m_CS;
  int m_numAbilities;
public:
  TRAIT_DATA_TYPE(void);
  ~TRAIT_DATA_TYPE(void);
  void Clear(void);
  void LoadUADefaults(void);
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void PostSerialize (BOOL IsStoring);
  //unsigned int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_TraitData.GetCount(); }
  unsigned int GetCount() { return m_TraitData.GetCount(); }

  traitType GetNextTrait();

  POSITION GetHeadPosition() { return m_TraitData.GetHeadPosition(); }


        TRAIT_DATA& GetAtPos (POSITION pos)       { return m_TraitData.GetAtPos(pos); }
  const TRAIT_DATA& PeekAtPos(POSITION pos) const { return m_TraitData.PeekAtPos(pos); }


  TRAIT_DATA& GetNext(POSITION &pos) { return m_TraitData.GetNext(pos); }
  traitType GetKeyAt(POSITION pos) { return m_TraitData.GetKeyAt(pos); }

  TRAIT_DATA *GetTraitData(const CString& traitName);
  traitType FindName(const CString& name);
  // Called when loading from scripts, or
  // when adding from editor GUI
  traitType AddTrait(const TRAIT_DATA &data);

  // Called when loading binary trait data that
  // was saved with key info
  traitType AddTraitWithCurrKey(const TRAIT_DATA &data);

  BOOL EvaluteTrait(traitType trait, double &result);
};
#endif
/*
Attribute modified
Set or Delta +/-,  points or %,  cumulative yes/no
affects target/targeter
duration permanent/temporal (rds/turns + x rds/turns per lvl)
*/

enum CASTING_ENVIRONMENT
{
  CAST_ENV_COMBAT    = 1,
  CAST_ENV_CAMP      = 2,
  CAST_ENV_ADVENTURE = 3,
};


struct SPELL_EFFECTS_DATA;
struct SAVING_THROW_DATA;

class AttributeScriptContexts
{
  SCRIPT_CONTEXT m_scriptContexts;
public:
  AttributeScriptContexts(SPELL_EFFECTS_DATA *pSpellEffectsData);
  AttributeScriptContexts(SAVING_THROW_DATA *pSavingThrowData);
  ~AttributeScriptContexts(void);
  CAR& Serialize(CAR& car);  //Added 20121208 PRS
};

struct SPELL_EFFECTS_DATA   // Also used for Timed Special Abilities
{
  SPELL_EFFECTS_DATA();
  SPELL_EFFECTS_DATA(const SPELL_EFFECTS_DATA &src);
  virtual ~SPELL_EFFECTS_DATA();
  void Serialize(CAR &car, double version);
  void Serialize(CArchive &ar, double version);
  void PreSerialize(BOOL IsStoring);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif
  bool operator==(const SPELL_EFFECTS_DATA &rhs) const;
  void Clear();

/*
  Effects:
  32-bits, default is all zeros

    bit     meaning
     2      0=literal delta,  1=percent
     3      0=not target      1=affects target
     4      0=not targeter    1=affects targeter
     5      0=Not cumulative, 1=is cumulative
     6      0=literal delta,  1=change current to amount
     7      1=item special ability source
     8      1=spell special ability source
     9      1=char/monster special ability source
     10     1=spell source
     11-32

  Target is equivalent to Self
*/
  enum 
  { 
    EFFECT_PCNT       =0x00000002, // set to percentage of original value
    EFFECT_TARGET     =0x00000004,
    EFFECT_TARGETER   =0x00000008,
    EFFECT_CUMULATIVE =0x00000010,
    EFFECT_ABS        =0x00000020, // set to amount. If not 2 or 32, amount is delta to original
    EFFECT_ITEMSPECAB =0x00000040, // source of effect is an item special ability
    EFFECT_SPELLSPECAB=0x00000080, // source of effect is a spell special ability
    EFFECT_CHARSPECAB =0x00000100, // source of effect is a char/monster special ability
    EFFECT_SPELL      =0x00000200, // source of effect is a spell
    EFFECT_REMOVEALL  =0x00000400, // remove all effects of this type from character
                                   // with the exception of CHARSPECAB effects (inherent ability)
    EFFECT_ONCEONLY   =0x00000800, // A lingering spell effect can either affect the target once
                                   // or once per round.
    EFFECT_APPLIED    =0x00001000, // This flag is set the first time
                                   // the effect is applied.  If the spell does not call for
                                   // repeated effects then this flag will prevent multiple effects.
    EFFECT_NONE       =0x00002000, // For example, if save is successful and 'Save Negates'
    EFFECT_SCRIPT     =0x00004000, // source of effect is a script
    EFFECT_TIMEDSA    =0x00008000, // This is a Timed Special Ability

    ALLSOURCE_MASK = EFFECT_ITEMSPECAB|EFFECT_SPELLSPECAB|EFFECT_CHARSPECAB|EFFECT_SPELL,
    SPECAB_MASK = EFFECT_ITEMSPECAB|EFFECT_SPELLSPECAB|EFFECT_CHARSPECAB,
    ALL_TARG_TYPES = EFFECT_TARGET|EFFECT_TARGETER
  };

  enum TIME_UNITS
  {
    TU_MINUTES = 1,
  };

  double ApplyChange(double src) const;
  double GetChange(void);
  void ClearChangeResult(void);
  bool ExecuteActivationScript(void);
  double ExecuteModificationScript(void);
  //double ExecuteSavingThrowScript(void);
  //void ExecuteSavingThrowFailedScript(void);
  //void ExecuteSavingThrowSucceededScript(void);
  bool CompileScripts(const CString &basename);
  CString GetEffectActivationFuncName(void);
  CString GetEffectModificationFuncName(void);
  CString GetEffectSavingThrowFuncName(void);    
  CString GetEffectSavingThrowFailedFuncName(void);    
  CString GetEffectSavingThrowSucceededFuncName(void);    
  BOOL IsReadyToExpire() const;    
  DWORD GetData() const;
  void SetData(DWORD val);
    
  DICEPLUS changeData;
  double changeResult;
  IF_KEYWORD_INDEX affectedAttr_IFKEY;


// ***********

  CString m_string1;     // This is the index key
  const CString& AffectedAttr(void) const {return m_string1;};     // This is the index key
  void    AffectedAttr(const CString& s){m_string1=s;};     // This is the index key

  CString m_string2;
  const CString& ActivationScript(void) const {return m_string2;};
  void    ActivationScript(const CString& s){m_string2=s;};
  const CString& TimedEffectSource(void) const {return m_string2;};
  void    TimedEffectSource(const CString& s){m_string2=s;};

  CString m_string3; // compiled GPDL script
  const CString& ActivationBinary(void) const {return m_string3;}; // compiled GPDL script
  void    ActivationBinary(const CString& s){m_string3=s;}; // compiled GPDL script

  CString m_string4;
  const CString& ModificationScript(void) const {return m_string4;};
  void    ModificationScript(const CString& s){m_string4=s;};
  const CString& ScriptEffectText(void) const {return m_string4;};
  void    ScriptEffectText(const CString& s){m_string4=s;};

  CString m_string5; // compiled GPDL script  
  const CString& ModificationBinary(void) const {return m_string5;}; // compiled GPDL script  
  void    ModificationBinary(const CString& s){m_string5=s;}; // compiled GPDL script  

  CString m_string6;
  const CString& SavingThrowScript(void) const {return m_string6;};
  void    SavingThrowScript(const CString& s){m_string6=s;};

  CString m_string7; // compiled GPDL script    
  //const CString& SavingThrowBinary(void) const {return m_string7;}; // compiled GPDL script    
  //void    SavingThrowBinary(const CString& s){m_string7=s;}; // compiled GPDL script    

  CString m_string8;
  const CString& SavingThrowFailedScript(void) const {return m_string8;};
  void    SavingThrowFailedScript(const CString& s){m_string8=s;};

  CString m_string9; // compiled GPDL script    
  //const CString& SavingThrowFailedBinary(void) const {return m_string9;}; // compiled GPDL script    
  //void    SavingThrowFailedBinary(const CString& s){m_string9=s;}; // compiled GPDL script    

  CString m_string10;
  const CString& SavingThrowSucceededScript(void) const {return m_string10;};
  void    SavingThrowSucceededScript(const CString& s){m_string10=s;};

  CString m_string11; // compiled GPDL script   
  //const CString& SavingThrowSucceededBinary(void) const {return m_string11;}; // compiled GPDL script    
  //void    SavingThrowSucceededBinary(const CString& s){m_string11=s;}; // compiled GPDL script    

// ****************


  DWORD flags;


private:
  DWORD m_sourceOfEffect;    // POSITION in editor, spell db key in engine
  SPELL_ID m_sourceSpellID;
  ITEM_ID  m_sourceItemID;
public:
#ifdef UAFEngine
  //void SourceOfEffect(GLOBAL_ITEM_ID giID, BOOL onceOnly)
  void SourceOfEffect(const ITEM_ID& itemID, BOOL onceOnly)
  {
    //m_sourceOfEffect=giID.m_key;
    m_sourceItemID=itemID;
    if (onceOnly)
    {
      flags |= EFFECT_ONCEONLY;
    }
    else
    {
      flags &= ~EFFECT_ONCEONLY;
    };
  };
  void SourceOfEffect(SPELL_ID spellID, BOOL onceOnly)
  {
    m_sourceSpellID=spellID;
    if (onceOnly)
    {
      flags |= EFFECT_ONCEONLY;
    }
    else
    {
      flags &= ~EFFECT_ONCEONLY;
    };
  };
  //GLOBAL_SPELL_ID SourceSpell_ID(void) const {return GLOBAL_SPELL_ID(m_sourceOfEffect);};
  SPELL_ID SourceSpell_ID(void) const {return m_sourceSpellID;};
  void            SourceChar_ID(int i, BOOL onceOnly)
  {
    m_sourceOfEffect=i;
    if (onceOnly)
    {
      flags |= EFFECT_ONCEONLY;
    }
    else
    {
      flags &= ~EFFECT_ONCEONLY;
    };
  };
  bool NoEffectWhatsoever(bool set)
  {
    if (set) flags |= EFFECT_NONE;
    return (flags & EFFECT_NONE) != 0;
  };
#endif
#ifdef UAFEDITOR
  void            SourcePosition_ID(POSITION pos){m_sourceOfEffect=(DWORD)pos;};
  POSITION        SourcePosition_ID(void){return (POSITION)m_sourceOfEffect;};
  DWORD           SourceInteger_ID(void){return m_sourceOfEffect;};
#endif

  DWORD parent; // key from activeSpellList (identifies the spell that caused this effect)
                // Or TIME_UNITS
  DWORD StopTime; // stop time (elapsed game minutes) if not same
                  // as parent spell (Stinking Cloud).
  DWORD data;  // extra data available to an effect if needed to track
               // something (MirrorImage)
  bool  m_scriptProcessed;  //Temporary...not serialized
  CHARACTER *m_pCaster;     //Temporary...not serialized
  CHARACTER *m_pTarget;     //Temporary...not serialized
};

class SPELLGROUP_DATA
{
  friend class SPELLGROUP_DATA_TYPE;
  friend static void DspellgroupSpecialAbility(CONFIG_PKT *pkt);
public:
  //mutable DWORD m_spellgroupKey;
  CString  m_name;
  mCArray <SPELL_REFERENCE, SPELL_REFERENCE&> m_spells;
  mCArray <SPELLGROUP_REFERENCE, SPELLGROUP_REFERENCE> m_spellgroups;
  SPECIAL_ABILITIES m_specAbs;
public:
  SPELLGROUP_DATA(void);
  SPELLGROUP_DATA(SPELLGROUP_DATA& src);
  SPELLGROUP_DATA& SPELLGROUP_DATA::operator =(const SPELLGROUP_DATA& src);
  ~SPELLGROUP_DATA(void);
  void Clear(void);

  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SpellReference,
                                  SPELL_ID,
                                  spellID,
                                  m_spells,
                                  SPELL_REFERENCE,
                                  SPELLGROUP_DATA)
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SpellgroupReference,  /* LocateSpellGroupReference()      */ \
                                  SPELLGROUP_ID,        /* Locate..(SPELLGROUP_REFERENCE x) */ \
                                  spellgroupID,         /* Locate...(spellgroupID)   */ \
                                  m_spellgroups,        /* m_spellgroups.GetSize()          */ \
                                  SPELLGROUP_REFERENCE, /* SPELLGROUP_REFERENCE *x = Peek   */ \
                                  SPELLGROUP_DATA)      /* *this                            */
  
  
  int GetDataSize(void) const;
  SPELLGROUP_ID SpellgroupID(void) const 
  {
    SPELLGROUP_ID spellgroupID; 
    spellgroupID=m_name;
    return spellgroupID;
  };
  void PreSerialize(BOOL IsStoring);
  int Serialize(CAR& car, CString& version);
  void PostSerialize(BOOL IsStoring);
#ifdef UAFEDITOR
  void CrossReference(class CR_LIST* pCRList);
#endif
  void AddSpell(const SPELL_ID& spellID);
  void AddSpellgroup(const SPELLGROUP_REFERENCE& spellgroupReference);
};

class SPELLGROUP_DATA_TYPE
{
private:
  // No locking... for internal use
  // Return key associated with the named object, or zero
  //spellgroupType m_FindName (const CString& name) const;
public:
  //POSITION         m_pos;
  //OrderedQueue<SPELLGROUP_DATA, MAX_SPELLGROUPS> m_SpellgroupData;
  mCArray <SPELLGROUP_DATA, SPELLGROUP_DATA&> m_spellgroups;
  //CCriticalSection m_CS;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Spellgroup,            \
                                  SPELLGROUP_ID,         \
                                  spellgroupID,          \
                                  m_spellgroups,         \
                                  SPELLGROUP_DATA,       \
                                  SPELLGROUP_DATA_TYPE)  
  

  
  SPELLGROUP_DATA_TYPE(void);
  ~SPELLGROUP_DATA_TYPE(void);
  void Clear(void);
  void LoadUADefaults(void);
  int GetDataSize(void) const;
  void PreSerialize (BOOL IsStoring);
  int Serialize (CAR& car);
  void PostSerialize (BOOL IsStoring);
  //unsigned int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_SpellgroupData.GetCount(); }
  void CrossReference(class CR_LIST* pCRList);
  unsigned int GetCount() { return m_spellgroups.GetSize(); }

  //spellgroupType GetNextSpellgroup();

  //POSITION GetHeadPosition() { return m_SpellgroupData.GetHeadPosition(); }


  //      SPELLGROUP_DATA& GetAtPos (POSITION pos)       { return m_SpellgroupData.GetAtPos(pos); }
  //const SPELLGROUP_DATA& PeekAtPos(POSITION pos) const { return m_SpellgroupData.PeekAtPos(pos); }


  //SPELLGROUP_DATA& GetNext(POSITION &pos) { return m_SpellgroupData.GetNext(pos); }
  //spellgroupType GetKeyAt(POSITION pos) { return m_SpellgroupData.GetKeyAt(pos); }


  //SPELLGROUP_DATA *GetSpellgroupData(const CString& spellName);
  //spellgroupType FindName(const CString& name);
  // Called when loading from scripts, or
  // when adding from editor GUI
  //spellgroupType AddSpellgroup(const SPELLGROUP_DATA &data);
  void AddSpellgroup(const SPELLGROUP_DATA &data);

  // Called when loading binary spellgroup data that
  // was saved with key info
  //spellgroupType AddSpellgroupWithCurrKey(const SPELLGROUP_DATA &data);
};
/*
class STRINGINDEX 
{
  // Provides 16-bit replacements for CStrings in archives
private:
  CString *m_strings;
  int      m_used;
  int      m_allocated;
  bool     m_operational;
  int m_errorcount;
public:
  STRINGINDEX(void);
  ~STRINGINDEX(void);
  void Clear(bool operational); // To turn it off or on.
  void AddString(const CString& str);
//  CString& GetString(WORD n);
//  WORD GetIndex(const CString& str);
  void Serialize(CArchive& ar, CString& string);

};
*/

#endif // #ifndef __CLASS_H__