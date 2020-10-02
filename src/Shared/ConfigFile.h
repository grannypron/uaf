/******************************************************************************
* Filename: ConfigFile.h
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
#ifndef __CONFIG_FILE_H__
#define __CONFIG_FILE_H__

enum CONFIG_ITEM_STATUS;
struct CONFIGID;
class DICEPLUS;
class ABILITY_LIMITS;
class ADJUSTMENT;
struct SPELL_EFFECTS_DATA;
//struct MONSTER_ATTACK_DETAILS;
struct ATTACK_DETAILS;

// A class to help you read / edit files of the form
//    \(BEGIN)   item 1
//    name = dodad
//    size=3.5
//    weight=17
//    \(END)
//
//    \(BEGIN)
//
// et cetera.
//
// You crate an instance of class CONFIG_FILE.
// 
// You call methods:
//     ReadFile    To read the file and create objects
//                 described by the file.  You provide
//                 a CArchive object and functions
//                 to check and store values the individual
//                 values.
//     EditFile    To edit a file that has been read.
//                 You provide functions to check and
//                 store values.
//     WriteFile   To write the file.  You provide a
//                 CArchive object.
//
// The functions mentioned above have the form:
//    CONFIG_FILE_STATUS  f (CONFIG_PKT *pkt);
//
// If the status is other than CONFIG_ITEM_OK then there should
// be an error message in errorMsg.
//
// We also provide some global decoding functions for things like:
// 
//   -integers within range
CONFIG_ITEM_STATUS CONFIG_DECODE_integer(
      const CString& value,
      CString& errorMsg,
      int min,
      int max,
      int& result);
CONFIG_ITEM_STATUS CONFIG_DECODE_long(
      const CString& value,
      CString& errorMsg,
      int min,
      int max,
      long& result,
      long DEFAULT=0);
//   -float (actually double) within range
//   -Bitmaps (such as "class = fighter + thief + NBAcoach")
CONFIG_ITEM_STATUS CONFIG_DECODE_flags(
      const CString& value,
      CString& errorMsg,
      CONFIGID *names,
      unsigned int& result);
//   -General syntax <numDice>d<numSides>[+<levelBonus>/level][max <maximum>]
//   -Dice  5/8-3  means ((5 dice each of eight sides)  minus 3 bonus)
//    In this case a number between 2 and 37 inclusive.
//      2d6+3 +1/level max 10
//      means 1 additional point per level with a 10 point overall maximum
CONFIG_ITEM_STATUS CONFIG_DECODE_dice(
      const CString& value,
      CString& errorMsg,
      unsigned int& nDice,
      unsigned int& nSides,
      int& nBonus,
      int *levelBonus=NULL,
      int *maximum=NULL
      );
//   -files (we take care of converting unix to DOS syntax and will
//           check the file is present, if asked)
CONFIG_ITEM_STATUS CONFIG_DECODE_file(
      const CString& value,
      CString& errorMsg,
      CString& result,
      const CString& Default="");
//   -enumerated possibilities.  Like foot | waist | head | finger | body 
//    The first item in the list is the default if value is all blank.
CONFIG_ITEM_STATUS CONFIG_DECODE_enum(
      const CString& value,
      CString  *errorMsg,
      const CONFIGID *possibilities,
      int& result);
//   -a string of arbitrary characters
CONFIG_ITEM_STATUS CONFIG_DECODE_string(
      const CString& value,
      CString& errorMsg,
      CString& result);
//   -BOOL  'yes', 'no', 'true', or 'false'
CONFIG_ITEM_STATUS CONFIG_DECODE_BOOL(
      const CString& value,
      CString& errorMsg,
      BOOL& result,
      BOOL DEFAULT=FALSE);
//   -floating point (double)
CONFIG_ITEM_STATUS CONFIG_DECODE_float(
      const CString& value,
      CString& errorMsg,
      double& result,
      double DEFAULT=0.0);
//   -List of enumerated integer values
CONFIG_ITEM_STATUS CONFIG_DECODE_enumList (
      const CString& value,
      CString& errorMsg,
      CONFIGID *ids,
      CList <int,int> **list);
// animated image specification
CONFIG_ITEM_STATUS CONFIG_DECODE_SpriteData (
    const CString& value,
    CString& errorMsg,
    CString &filename,
    int &nTimeDelay,
    int &nNumFrames,
    int &nFrameWidth,
    int &nFrameHeight,
    int &nStyle,
    DWORD &MaxLoops,
    DWORD &nFlags,
    int   &AllowCentering,
    int   &UseAlpha,
    WORD  &AlphaValue);
CONFIG_ITEM_STATUS CONFIG_DECODE_dicePlus (
    const CString& value,
    CString& errorMsg,
    DICEPLUS *result,
    int numdice=1, int numsides=6, int bonus=0, int min=0, int max=999999);
CONFIG_ITEM_STATUS CONFIG_DECODE_AbilityRequirement (
    const CString& value,
    CString& errorMsg,
    ABILITY_REQ *abReq);
typedef CArray<ADJUSTMENT,ADJUSTMENT&> ADJUSTMENTListType;
CONFIG_ITEM_STATUS CONFIG_DECODE_adjust(
    const CString& value,
    CString& errorMsg,
    ADJUSTMENTListType &list);
    //ADJUSTMENT *adjust);
CONFIG_ITEM_STATUS CONFIG_DECODE_MonsterAttack(
		  const CString& value,
		  CString& errorMsg,
      //MONSTER_ATTACK_DETAILS *data);
      ATTACK_DETAILS* data);

//
// And here are some functions to help you encode your
// values:
// A signed integer
void CONFIG_ENCODE_integer(
    CString& destination,
    int integer);
// A longword.
void CONFIG_ENCODE_long(
    CString& result,
    long value);
// A bitmap (like "class = ranger + cleric")
void CONFIG_ENCODE_flags (
    CString& result,
    const CONFIGID *ids,
    unsigned int flags);
void CONFIG_ENCODE_flags (
    CString& result,
    const CONFIGID *ids,
    long flags);
void CONFIG_ENCODE_flags (
    CString& result,
    const CONFIGID *ids,
    unsigned char flags);
void CONFIG_ENCODE_flags (
    CString& result,
    const CONFIGID *ids,
    unsigned long flags);
void CONFIG_ENCODE_flags (
    CString& result,
    const CONFIGID *ids,
    WORD flags);
// dice in format 3D6 + 5 [3/level] [max 7]
void CONFIG_ENCODE_dice (
    CString& result,
    int nDice,
    int nSides,
    int nBonus,
    int levelBonus=0x40000000,  // Means omit
    int maximum=0x40000000);    // means omit
// filename.  Changes backslashes to slashes
void CONFIG_ENCODE_file (
    CString& result,
    const CString& filename);
// enumerated values.  Like male or female
void CONFIG_ENCODE_enum(
    CString& result,
    const CONFIGID *ids,
    int value);
// A boolean type  "yes" or "no"  See CONFIG_ENCODE_bool for "true" or "false"
void CONFIG_ENCODE_BOOL (
    CString& result,
    BOOL value);
// A float (actually double)
void CONFIG_ENCODE_float (
    CString& result,
    double value);
// animated image specification
void CONFIG_ENCODE_SpriteData (
    CString& result,
    const CString &filename,
    int   &nTimeDelay,
    int   &nNumFrames,
    int   &nFrameWidth,
    int   &nFrameHeight,
    int   &nStyle,
    DWORD &nMaxLoops,
    DWORD &nFlags,
    int   &nAllowCentering,
    int   &nUseAlpha,
    WORD  &nAlphaValue
    );
void CONFIG_ENCODE_dicePlus (
    CString& result,
    const DICEPLUS& dice);
void CONFIG_ENCODE_SpellEffect (
		CString& result,
    SPELL_EFFECTS_DATA *data);
CONFIG_ITEM_STATUS CONFIG_DECODE_SpellEffect (
		const CString& value,
		CString& errorMsg,
    SPELL_EFFECTS_DATA *data);
void CONFIG_ENCODE_MonsterAttack(
            CString &result,
            //const MONSTER_ATTACK_DETAILS *data);
            const ATTACK_DETAILS* data);

enum CONFIG_DATA_TYPE
{ 
  // Before your function is called we fill in most of the
  // packet with default values.
  //   errorMsg     ="";
  //   objectNumber (=0 to n)
  //   helpMsg      = ID from keyword table
  //   type         (=type of data)
  //   key          (keyword from text file)
  //   value        (value from text file)
  //   status       =CONFIG_STAT_ok
  //   ids          =NULL
  //   listNum      =0, 1, 2, .... when IsStoring==true
  // All functions should fill in:  status   if not 'ok'
  //                  errorMsg if not 'ok'
  //
  // At least one of the keywords should result in an 'object name' and
  // should set the field objectName in the packet.
  CONFIG_DATA_object=1,   // The object as a whole.
  CONFIG_DATA_end,        // An entry to mark the end of list
  CONFIG_DATA_dice,
  CONFIG_DATA_exDice,
  CONFIG_DATA_integer,
  CONFIG_DATA_long,
  CONFIG_DATA_LONG,    // Automatic conversion to/from CString value.
  CONFIG_DATA_DOUBLE,
  CONFIG_DATA_DWORD,
  CONFIG_DATA_BOOL,
  CONFIG_DATA_string,
  CONFIG_DATA_file,
  CONFIG_DATA_flags,      // Must set pkt->ids
  CONFIG_DATA_float,
  CONFIG_DATA_enum,       // Fill set pkt->ids
  CONFIG_DATA_spritedata,
  CONFIG_DATA_dicePlus,   // 3d9 + 12 + 1/level + 1/female + 2/Mage + 3/Gnome
  CONFIG_DATA_AbilityReq, // eg: Strength 3/18(60)
  CONFIG_DATA_adjust,     // eg: Constitution == 3 * 2 / 5
  CONFIG_DATA_spellEffect,
  CONFIG_DATA_unknown
};

enum CONFIG_FILE_FUNC
{
  CONFIG_FUNC_beginObject,    // Initialize for new object  "\(BEGIN)"
  CONFIG_FUNC_checkValue,     // And store it in the object  "xxx = yyy"
  CONFIG_FUNC_storeObject,    // All done, "\(END)"
  CONFIG_FUNC_getFirstObject, // Fetch first object to temporary area.
  CONFIG_FUNC_getNextObject   // Fetch next object to temporary area.
};

       
struct CONFIG_PKT;

struct CONFIGID 
{
  CString m_name;
  void  (*m_f)( CONFIG_PKT *pkt);

  union 
  {
    CONFIG_DATA_TYPE  m_dataType;
    int               m_enumVal;
  } m_id;

  int   m_helpID;
  int   m_max; // How many of these can appear in definition of an object.
  bool  m_readOnly; // can this value be written?
  bool  m_grouped; // multi-line items are grouped (associated) by sequential numbering
                   // somekey 1
                   // somerelatedkey 1
                   // somekey 2
                   // somerelatedkey 2

  CONFIGID (void) { ASSERT(0);};  // For Lint

  CONFIGID ( LPCSTR name, 
             CONFIG_DATA_TYPE type, 
             void f( CONFIG_PKT *pkt),
             int helpID=0,
             int max=1,
             bool read=false,
             bool group=false) 
  {
    m_name=name; 
    m_id.m_dataType=type; 
    m_f=f; 
    m_helpID=helpID;
    m_max=max;
    m_readOnly=read;
    m_grouped=group;
  };

  CONFIGID (LPCSTR name,int value) 
  {
    m_name=name; 
    m_id.m_enumVal=value; 
    m_f=NULL; 
    m_helpID=0;
    m_max=1;
    m_readOnly=false;
    m_grouped=false;
  };
};



struct CONFIG_PKT {
  // A structure we use to pass information to and from
  // here and there.  The program that uses CONFIG_FILE must
  // provide functions to fill in the appropriate values
  // in the CONBFIG pkt for each kind if information in the
  // config file.  For example: if a line like
  //        class = fighter 
  // is allowed then the program must provide a function
  // to tell us about the allowed values, help information,
  // and so on.
  CString errorMsg;  // A pointer to an error message
  int helpMsgID;   // A stringID of a help message in the resuurce file.
  CONFIG_FILE_FUNC func;    // What we want you to do in your function.
  CONFIG_DATA_TYPE type;    // Tell us what kind of data you expect.
                // Your responsibility for putting information
                // in the packet depends on this type. qv.
  CONFIG_ITEM_STATUS status;// Result of checking the data.
  CONFIGID *ids;            // Pointer to array of CStrings and 
                // values like:
                            //     "Fighter"  0x01
                            //     "Cleric"   0x02
                            //     ""         0x04  // blank=end of array
  CString  key;  // The keyword from the input line eg: age=30
  CString  value;  // The value from the input line.      key=value
  long     longValue;   // The keyword loop process will format this integer <-> value
  DWORD    DWordValue;
  double   doubleValue;
  BOOL     boolValue;
  int      keyNum;
  CString  objectName;
  int      objectNumber;
  int      insertPoint; // Where to insert in object's itemList.
  bool     IsStoring;
  int      listNum;  // When storing multiple items under same key.
  POSITION listPos;  // When storing multiple items under same key.
  void     AddLongValue(int n); // Encode additional ",%d" onto value
  CONFIGID *newIDs;

};


class CONFIG_FILE
{
  class OBJECTS;
  class OBJECT;
public:
  class LINE_ITEM
  {
    friend class CONFIG_FILE;
    friend class OBJECT;
  private:
    CString             m_line;
    CString             m_key;
    CString             m_value;
    CString             m_errorMsg;
    CONFIG_ITEM_STATUS  m_status;
    bool                m_toBeDeleted;
    int                 m_linenumber;
  public:
    LINE_ITEM(void);
    ~LINE_ITEM(void);
    void CreateComment(char *line);
    void CreateDelimiter(char *line);
  };
private:
  class OBJECT
  {
    friend class OBJECTS;
  private:
    int         m_allocatedSize;
    int         m_numberOfItems;
    CString     m_objectName;
    LINE_ITEM **m_itemList;
    bool        m_toBeDeleted;
  public:
    OBJECT(void);
    ~OBJECT(void);
    void AddTail(LINE_ITEM& item); 
    LINE_ITEM *GetLine(int itemNum);
    void MarkItemsToDelete(void);
    void RemoveDeletedLines(void);
    int DeleteKeys(const CString& key);
    void WriteFile(CArchive& ar);
    void InsertKeyValue(
              int& insertPoint,
              const CString& key,
              const CString& value);
  };
  class OBJECTS 
  {
  private:
    OBJECT **m_objectList;
    int     m_numberOfObjects;
    int     m_allocatedSize;
  public:
    OBJECTS(void);
    ~OBJECTS(void);
    void AddTail(int objectNumber,LINE_ITEM& lineItem);
    LINE_ITEM *GetLine(int objectNumber,int itemNumber);
    int NumberOfObjects(void) const {return m_numberOfObjects;};
    int NumberOfItems(int objNum) const;
    void MarkAllToDelete(void); // Mark all objects 'to-be-deleted'
    int FindObject(const CString& name);  // -1 if no such object in list
    int AddObject(int objNum=-1);  // Add at end if objNum < 0.  Return object number
    void SetName(int objectNumber, const CString& name);
    void ClearToDelete(int objectNumber);
    void MarkItemsToDelete(int objectNumber);
    int DeleteKeys(int objectNumber, const CString& key);
    void RemoveDeletedLines(int objectNumber);
    void WriteFile(CArchive& ar);
    void InsertKeyValue(
                int objectNumber,
                int& insertPoint,
                const CString& key,
                const CString& value);
  };
private:
  OBJECTS  m_objects;
  CONFIGID *m_ids;
  void (*m_objectFunction)(CONFIG_PKT *pkt);
  CString  m_filePath;
  int m_numErrors;  // Errors after the 5th are not printed
  BOOL isEscapeChar(const char ch);
  CONFIG_ITEM_STATUS split(
    LINE_ITEM *lineItem,
  //  const CString& src, 
    CString& key, 
    CString& value,
    int *valueColumn=NULL);
  CONFIG_ITEM_STATUS DecodeObject(
     int objectNumber,
     CONFIGID *ids,
     CString& objectName);  // Returns name of object
public:
  CONFIG_FILE(void);
  ~CONFIG_FILE(void);
  CONFIG_ITEM_STATUS ReadFile(CArchive& ar);
  CONFIG_ITEM_STATUS ReadFile(CAR& car);
  CONFIG_ITEM_STATUS WriteFile(CArchive& ar, CONFIGID *ids);
  CONFIG_ITEM_STATUS DecodeFile(CONFIGID *ids, bool commit);
  CONFIG_ITEM_STATUS GetObjectInfo(
      int objectNumber, 
      CString &objectName);
  bool Exists(int objectNumber) const;
  CString GetFileName(void);
  bool GetLineInfo(
    int objectNumber,
    int lineNumber,
    CString& line,
    CONFIG_ITEM_STATUS& status,
    int *helpMsgID,
    CString *errorMsg,
    int *valueColumn=NULL);
  CString GetLineText(int objNum, int lineNum);
  int NumberOfItems(int objNum) const;
  void PerhapsLogError(LINE_ITEM *pLine, const CString& msg);
  void PerhapsLogError(int objNum, int lineNum, const char *msg);
};

#endif