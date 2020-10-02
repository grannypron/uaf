/******************************************************************************
* Filename: ASL.h (formerly attrib.h)
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

#if !defined __ASL_H__
#define __ASL_H__

void die(const char *msg);

class JWriter;
class JReader;

enum TYMER_TYPE
{
  TYMER_TYPE_NONE = 0,
  TYMER_TYPE_SPELLEFFECT,
  TYMER_TYPE_ROUND,
};


class TYMER_ENT
{
  friend class MINUTE_TYMER;
  friend class ROUND_TYMER;
  CArray<int, int> systemIntegers;
                      // systemIntegers[0] = remaining time (arbitrary units)
  CArray<CString, CString&> userStrings;   
  CArray<CString, CString&> systemStrings; 
                      // systemStrings[0] = SA name
                      // systemStrings[1] = Script name
                      // systemStrings[2] = Search id
  void Clear(void)
  {
    systemIntegers.RemoveAll();
    systemStrings.RemoveAll();
    userStrings.RemoveAll();
  };
};


class MINUTE_TYMER : TYMER_ENT
{
  void Clear(void)
  {
    TYMER_ENT::Clear();
  };
};

class ROUND_TYMER : TYMER_ENT
{
  void Clear(void)
  {
    TYMER_ENT::Clear();
  };
};

class TYMERS
{
  CArray<int, int> index;  // Binary timer tree.
  CArray<TYMER_ENT, TYMER_ENT&> timer;
public:
  void operator =(const TYMERS& src){die("Not Needed?"); };
  TYMER_ENT *AddMinuteTimer(int numMinute);
};

class TYMER_DATA
{
  mCArray<TYMERS, TYMERS&> timers;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Timer,      /* array data name      */ \
                                  CString,    /* Search variable type */ \
                                  timerID,    /* search variable name */ \
                                  timers,     /* array name           */ \
                                  TYMERS,     /* array data type      */ \
                                  TYMER_DATA) /* type of *this        */

};


class ASLENTRY {
  friend int ASLENTRY_COMPARE_KEY(int a);
  friend int ASLENTRY_COMPARE_ENTRY(int a, int b);

  friend class A_ASLENTRY_L;
  friend struct SCRIPT_CONTEXT;
  friend class CAR;
	friend void AFXAPI SerializeElements(class CArchive &ar,
  		                                 class ASLENTRY *atr,
	  									                 int count);

//#ifdef _DEBUG
//  int id;
//#endif
	unsigned char m_flags;
  CString m_key;
	CString m_value;
  ASLENTRY(void) { Clear(); };
//ASLENTRY(LPCSTR key, LPCSTR value, unsigned char flags);
public:
  ASLENTRY(const CString& key, const CString& value, unsigned char flags);
	~ASLENTRY(void);
  void operator = (const ASLENTRY& src) {m_flags=src.m_flags;m_key=src.m_key;m_value=src.m_value;};
  BOOL operator == (const ASLENTRY& src) const;
  void Clear(void) { m_key.Empty(); m_value.Empty(); m_flags = 0; };
  inline CString Value(void) const { return m_value;};
  inline void Value(const CString& value) { m_value = value; };
  inline CString Key(void) const {return m_key;};
//  inline void Key(const CString& key) { m_key = key; };
  inline unsigned char Flags(void) const {return m_flags;};
  inline void Flags(unsigned char flags) {m_flags = flags;};
  inline const CString *pValue(void) const { return &m_value;};
  inline const CString *pKey(void) const {return &m_key;};
  int GetDataSize(void) const;
  // Compare functions return if (keyData>m_key) then 1; else 0 or -1
  int Compare(const ASLENTRY *keyData)const{return keyData->m_key.Compare(m_key);};
  int Compare(LPCTSTR keyData)const{return -m_key.Compare(keyData);};
  void Serialize(CAR *pCar) const;
  void DeSerialize(CAR *pCar);
  void Serialize(CArchive *pAr) const;
  void DeSerialize(CArchive *pAr);
#ifdef UAFEDITOR
  void Export(JWriter& jw) const;
  void Import(JReader& jw);
#endif
};


#define ASLF_READONLY     1   // Can't be deleted or changed or saved.  READONLY
                              //  attributes cannot be created during gameplay.
							                //  So ASLF_READONLY implies ASLF_DESIGN.
#define ASLF_MODIFIED     2   // Changed during gameplay.  The initial insertion
                              //  of an attribute during gameplay does not set
                              //   this bit
#define ASLF_DESIGN       4   // Created by UAFWinEd.  Info only.  Not used.
#define ASLF_SYSTEM       8   // System attribute.  Info only.  Not used.


// just to save typing the two flags throughout the editor code
#define ASLF_EDITOR (ASLF_READONLY | ASLF_DESIGN)

// ASLs will be saved in the .PTY files unless READONLY.
// When restoring a game, all ASLs except READONLY will be discarded before
//  reading the savefile.

//class CAR;


// Replacement for CMap'ed ASLs ***************************************8


class BTreeNode;

union NDPT // NodePointer/Data 
{
  int        data;
  BTreeNode *pNode;
};




class BTreeNode
{
  friend class BTree;
  friend void CheckNodes(BTreeNode *);
//#ifdef _DEBUG
//  int id;
//#endif
  NDPT      m_left;
  NDPT      m_right;
  int       m_data;
  unsigned _int16 m_cntL; // Count of entries.Left/Right
  unsigned _int16 m_cntR;
  // ****** Functions ********
  int FindNode(int key);
};


struct BTREE_ENVIRONMENT;

class BTree
{
  //friend class A_ASLENTRY_L;
private:
  friend void CheckNodes(BTreeNode *);
  friend void CheckList(bool print=false);
protected:
  BTreeNode *m_root;
private:
  //mutable int m_foundData;
  //********************** FUNCTIONS **************
  int   RemoveData(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  int   AddNode(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  int   DeleteNode(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  int   RemoveSmallest(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  int   RemoveLargest(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  void  PushLeft(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  void  PushRight(BTreeNode *pTop, BTREE_ENVIRONMENT *pEnv);
  void  ClearNode(BTreeNode *pNode, BTREE_ENVIRONMENT *pEnv);
  int   FindNode(BTreeNode *pNode, BTREE_ENVIRONMENT *pEnv) const;
public:
  BTree(void){m_root = NULL;};
  ~BTree(void);
  int         Insert(BTREE_ENVIRONMENT *pEnv);
  int         Delete(BTREE_ENVIRONMENT *pEnv);
  int         Find(BTREE_ENVIRONMENT *pEnv)const;
  inline int  GetCount(void)const {return (m_root==NULL)?0:1+m_root->m_cntL+m_root->m_cntR;};
  void        Clear(BTREE_ENVIRONMENT *pEnv);
  POSITION    GetStartPosition(void) const {return (m_root==NULL)?NULL:(POSITION)1;};
  int         GetNextAssoc(POSITION& pos) const;
  void        Swap(BTree& pTree);
};

class A_CStringPAIR_L;
class JWriter;
class JReader;

class CStringPAIR;
enum CBRESULT;
enum CBFUNC;

// Associated ASLVALUE List
class A_ASLENTRY_L:protected BTree
{
  friend class CAR;

  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
	A_ASLENTRY_L&   operator = (const A_ASLENTRY_L& attr);
  A_ASLENTRY_L    (const A_ASLENTRY_L& specDef);


  /****************************************************
   *    Functions
   ***************************************************/

public:
  void            Swap(A_ASLENTRY_L& list);
  void            Copy(const A_ASLENTRY_L& list);
  void            Copy(const A_CStringPAIR_L& list);
  void            Copy(CArray<CStringPAIR, CStringPAIR&> *pArray); 

  A_ASLENTRY_L    (void):BTree(){};
	CArchive&       Serialize(CArchive& ar, LPCSTR mapName);
	CAR&            Serialize(CAR& ar, LPCSTR mapName);
	CAR&            Save(CAR& ar, LPCSTR mapName) const; // Serialize if not ASLF_READONLY
#ifdef UAFEDITOR
  void            Export(JWriter& jw);
  bool            Import(JReader& jr);
#endif

  ~A_ASLENTRY_L(void);

  int             GetCount(void)const{return BTree::GetCount();};
	void            CommitRestore(const A_ASLENTRY_L *src);
	BOOL            Insert(const CString& key, const CString& data, unsigned char flags);
  BOOL            Insert(const ASLENTRY *pEntry);
	CString         Delete  (LPCSTR key);
	const CString&  Lookup(LPCSTR key) const;
  ASLENTRY       *Find(LPCTSTR key) const;
  BOOL            operator == (const A_ASLENTRY_L& attr) const;
	void            Clear(void);
  int             GetDataSize(void) const;
  POSITION        GetStartPosition(void) const {return BTree::GetStartPosition();};
  void            GetNextAssoc(POSITION &pos, const ASLENTRY **ppEntry) const;

  CString RunScripts(LPCSTR                  scriptName,
                     CBRESULT              (*fnc)(CBFUNC func, CString *scriptResult, void *pkt),
                     void                   *pkt,
                     enum SCRIPT_SOURCE_TYPE SA_Source_Type,
                     LPCSTR                  SA_Source_Name) const;


#ifdef UAFEDITOR
  void            Update(LPCSTR key, LPCSTR value, unsigned char flags);
#endif
};

class CChooseASLs;

class CStringPAIR
{ 
  friend class A_CStringPAIR_L;
  friend class CChooseOptions;
  friend class CWhoTriesDlg;
  friend class A_ASLENTRY_L;
  CString m_key;
  CString m_value;
  CStringPAIR(const CStringPAIR& src);
public:
  CStringPAIR(void);
  CStringPAIR(const CString& key, const CString& value);
  CStringPAIR& operator =(const CStringPAIR& src);
  CStringPAIR& operator =(const ASLENTRY& src);
  void Clear(void){m_key=""; m_value="";};
  const CString& Key(void) const {return m_key;};
  const CString& Value(void) const {return m_value;};
  void Value(const CString& value){m_value = value;};
  int  Compare(LPCSTR key) const;
  int  Compare(const CStringPAIR *b) const ;
  bool operator == (const CStringPAIR& p) const;
};

class A_CStringPAIR_L:protected BTree
{
  friend class CAR;

  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
	A_CStringPAIR_L&   operator = (const A_CStringPAIR_L& attr);
  A_CStringPAIR_L    (const A_CStringPAIR_L& specDef);

public:
  A_CStringPAIR_L(void):BTree(){};
  ~A_CStringPAIR_L(void);
	CArchive& Serialize(CArchive& ar);
	CAR&      Serialize(CAR& car);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  void Import(JReader& jr);
#endif

	CArchive& Save(CArchive& ar);

  void          Copy(const A_CStringPAIR_L& src);
  void          Copy(const A_ASLENTRY_L& src);
  int           GetCount(void)const{return BTree::GetCount();};
	void          CommitRestore(const A_CStringPAIR_L *src);
	BOOL          Insert(const CString& key, const CString& data);
  BOOL          Insert(const CStringPAIR& pEntry);
#ifdef UAFEDITOR
  void          Update(LPCSTR key, LPCSTR value);
#endif
	CString       Delete(LPCSTR key);
  CStringPAIR  *Find(LPCSTR key) const;
  const         CString& Lookup(LPCSTR key) const;
  BOOL          operator == (const A_CStringPAIR_L& attr) const;
	void          Clear(void);
  int           GetDataSize(void) const;
  POSITION      GetStartPosition(void) const {return BTree::GetStartPosition();};
  void          GetNextAssoc(POSITION &pos, const CStringPAIR **ppEntry) const;
};


class A_CString_L:protected BTree
{
  friend class CAR;

  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
private:
  // These are private because if they are made public it is FAR TOO EASY
  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
  // If you must COPY then use the Copy function explicitly and put a comment
  // justifying your actions!!!
	A_CString_L&   operator = (const A_CString_L& attr);
  A_CString_L    (const A_CString_L& specDef);

public:
  A_CString_L(void):BTree(){};
  ~A_CString_L(void);
	//CArchive& Serialize(CArchive& ar);
	//CAR&      Serialize(CAR& car);
	//CArchive& Save(CArchive& ar);

  //void          Copy(const A_CStringPAIR_L& src);
  //void          Copy(const A_ASLENTRY_L& src);
  int           GetCount(void)const{return BTree::GetCount();};
	//void          CommitRestore(const A_CStringPAIR_L *src);
	BOOL          Insert(const CString& key);
  //BOOL          Insert(const CStringPAIR& pEntry);
//#ifdef UAFEDITOR
//  void          Update(LPCSTR key, LPCSTR value);
//#endif
//	CString       Delete(LPCSTR key);
//  CString      *Find(LPCSTR key) const;
//  const         CString& Lookup(LPCSTR key) const;
//  BOOL          operator == (const A_CStringPAIR_L& attr) const;
//	void          Clear(void);
//  int           GetDataSize(void) const;
//  POSITION      GetStartPosition(void) const {return BTree::GetStartPosition();};
//  void          GetNextAssoc(POSITION &pos, const CStringPAIR **ppEntry) const;
};


class CStringInteger
{ 
  friend class A_CStringInteger_L;
private:
  CString m_key;
  int     m_value;
  CStringInteger(const CStringInteger& src);
public:
  CStringInteger(void);
  CStringInteger& operator =(const CStringInteger& src);
  void  Clear(void){m_key=""; m_value=0x8000000;};
  const CString& Key(void) const {return m_key;};
  int   Value(void) const {return m_value;};
  void  Value(int value){m_value = value;};
  int   Compare(LPCSTR key) const;
  int   Compare(const CStringInteger *b) const ;
  bool  operator == (const CStringInteger& p) const;
};




class A_CStringInteger_L:protected BTree
{
  /****************************************************
   *  Constructors and copy operators
   ***************************************************/
//private:
//  // These are private because if they are made public it is FAR TOO EASY
//  // to cause these CPU-intensive copies accidentally.  It is seldom necessary.
//  // If you must COPY then use the Copy function explicitly and put a comment
//  // justifying your actions!!!
//	A_CStringPAIR_L&   operator = (const A_CStringPAIR_L& attr);
//  A_CStringPAIR_L    (const A_CStringPAIR_L& specDef);

public:
  A_CStringInteger_L(void):BTree(){};
  ~A_CStringInteger_L(void);
//	CArchive& Serialize(CArchive& ar);
//	CAR&      Serialize(CAR& car);
//	CArchive& Save(CArchive& ar);

//  void          Copy(const A_CStringPAIR_L& src);
//  void          Copy(const A_ASLENTRY_L& src);
  int           GetCount(void)const{return BTree::GetCount();};
//	void          CommitRestore(const A_CStringPAIR_L *src);
	BOOL          Insert(const CString& key, int data);
//  BOOL          Insert(const CStringPAIR& pEntry);
//#ifdef UAFEDITOR
//  void          Update(LPCSTR key, LPCSTR value);
//#endif
//	CString       Delete(LPCSTR key);
  int          *Find(LPCSTR key) const;
//  int           Lookup(LPCSTR key) const;
//  BOOL          operator == (const A_CStringPAIR_L& attr) const;
	void          Clear(void);
//  int           GetDataSize(void) const;
//  POSITION      GetStartPosition(void) const {return BTree::GetStartPosition();};
//  void          GetNextAssoc(POSITION &pos, const CStringPAIR **ppEntry) const;
};




//lint -e762
template <> void AFXAPI SerializeElements<ASLENTRY>(CArchive& ar, 
															ASLENTRY* val,
															int count);
//lint +e762




// A class to manipulate delimited strings.
// Such strings are of the format:
//    "5.first6.second5.third"
// A DELIMITED_STRING may contain multiple occurances of the same
//  substring:
//     "5.first28.A somewhat longer sub-string5.first"
// A DELIMITED_STRING can contain empty substrings:
//     "5.first0.5.third"
// But an empty DELIMITED_STRING contains no strings...not even empty ones.
//
// The numbers (followed by a decimal point) define the length of
// each sub-string in decimal.

// Why all this fuss????  So we can put multiple strings in a single
// attribute.  The first use was to list allowable MultiClass values
// for a race.  And it really is not that much of a fuss.
class DELIMITED_STRING
{
public:
  CString m_string;
  DELIMITED_STRING(void);
  ~DELIMITED_STRING(void);
  void operator = (const CString& str);
  bool Contains(const CString& str) const;
  bool IsLegal(void) const;
  int Count(void) const;
  CString GetAt(int index) const;
  CString Head(void) const;
  CString Tail(void) const;
  //CString Fetch(int index) const;
  CString Result(void); // The resulting delimited string
  void Insert(const CString& str);  //Inserts at the end
  void Delete(const CString& str);
  bool Next(int *col, CString *substring) const;  // Set col to -1 for first.
};



class DEBUG_STRINGS
{
  A_CString_L m_debugStrings;
public:
  DEBUG_STRINGS(void){};
  BOOL AlreadyNoted(LPCSTR debugString);
};

extern DEBUG_STRINGS debugStrings;

#endif // !defined __ASL_H__
