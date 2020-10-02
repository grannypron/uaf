/******************************************************************************
* Filename: GPDLexec.cpp
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

// This is the code for the GPDL Interpreter.
// You feed it events and it will interpret the 
// code found in the GPDL binary file.
//
// Events:
//   Load          Make sure the GPDL binary file has been read.
//                 Accepted in 'UNINITIALIZED' state.
//   BeginExecute  Search for function name and start execution
//                 Accepted in 'IDLE' state.
//   BreakExecute  Terminate current execution
//                 Accepted anytime.
//   Char          A character has been received from the
//                 keyboard.  Returns:
//                    GPDL active 
//                       keystroke ignored    GPDL_IGNORED 
//                       deystroke accepted   GPDL_ACCEPTED
//                    GPDL inactive      GPDL_IDLE
//
// ****  All events return a status ***
//  GPDL_EVENT_ERROR   = Event occurred at inappropriate time.
//                      The event is ignored.
//  GPDL_NOSUCHNAME    = No such function name in index.
//  GPDL_IDLE          = Exection complete.  The only event that will
//                      be honored is 'BeginExecute'
//  GPDL_IGNORED       = Results only from Char event if the keystroke
//                      is ignored.
//  GPDL_ACCEPTED      = Results from Char event.  Keystroke welecomed.
//  GPDL_CODE_ILLOP    = Illegal operation code.  Execution complete
//                      State set to IDLE.
//  GPDL_WAIT_INPUT    = Waiting for keyboard text input from user.
//  GPDL_WAIT_ACK      = Waiting for output acknowledge.
//  GPDL_WAIT_CONTINUE = Waiting for 'continue' input from user when
//                      more output text is waiting for space on the
//                      screen.  We expect the user to press the space
//                      bar to indicate he has read what is already
//                      displayed.
//  GPDL_OVR_SP        = Data stack overflow.  State set to IDLE.
//  GPDL_OVR_RP        = Return stack overflow.  State set to IDLE.
//  GPDL_READ_ERROR    = Error reading GPDL binary file.


#include "..\Shared\stdafx.h"

#include "externs.h"
#include <malloc.h>
#include <math.h>
#include "class.h"
#include "GameEvent.h"
#include "..\Shared\GPDLopCodes.h"
#include "GPDLcomp.h"
#include "GPDLexec.h"
#include "Graphics.h"
#include "GlobalData.h"
#include "RuntimeIF.h"
#include "party.h"
#include "Monster.h"
#ifdef UAFEDITOR
#include "..\UAFWinEd\Resource.h"
#else
#include "..\UAFWin\Resource.h"
#endif
#include "ramfile.h"

void *My_malloc(int size);
void *My_realloc(void *buf, int size);
void My_free(void *addr);


#ifdef UAFEngine
void UpdateAdventureScreen(void);
void InitiateTaskMessage_SetPartyXY(int x, int y);
int  ComputeAttackDamage(const COMBATANT *pAttacker, const COMBATANT *pTarget);
CString FindEventAttribute(int depth, const CString& name);
extern CHARACTER FakeCharacter;
#else
int  ComputeAttackDamage(const COMBATANT *pAttacker, const COMBATANT *pTarget){return 1;};
#endif
extern SCRIPT_CONTEXT NULL_SCRIPT_CONTEXT;
extern BOOL logDebuggingInfo;
extern CString emptyString;
extern CHARACTER NULL_CHAR;
#ifdef UAFEngine
extern COMBATANT NULL_COMBATANT;
extern CHARACTER FakeCharacter;
#endif


#ifdef UAFEngine

#include "..\UAFWin\combatants.h"
#include "..\UAFWin\CharStatsForm.h"
#include "..\UAFWin\FormattedText.h"

BOOL HaveLineOfSight(int x0, int y0, int x1, int y1, BOOL *reflects);
BOOL IsLineOfSight(int x0, int y0, int x1, int y1);
CString GetSpellbook(CHARACTER *pChar, CString delimiters);
//CString SelectSpell(CHARACTER *pChar, CString spellname);
CString SelectSpell(CHARACTER *pChar, const SPELL_ID& spellID);
void Memorize(CHARACTER *pChar);
//int SetMemorizeCount(int charIndex, const CString& spellName, const CString& adj, const char *msg);
CString GetHighestLevelBaseclass(int charIndex);
int GetBaseclassLevel(int charIndex, const CString& baseclassName);
CString MonsterPlacementCallback(const CString& turtleCode);
int IntegerTableLookup(const CString& SAname, 
                       const CString& tableName, 
                       int            iValue,
                       const CString& function);
CString ForEachPartyMember(const CString& SAName, const CString& scriptName);
CString DelimitedStringFilter(const CString& src, 
                              const CString& filter,
                              const CString& function);
CString SkillAdjustment(
                CHARACTER *pChar,
          const CString&  skillName,
          const CString&  adjustmentName,
          const CString&  adjType,
                int       adjValue);
int RollHitPointDice(const CString& baseclassName, int low, int high);

CString GrSet        (const CString& name, const CString& x, const CString& y);
CString GrSetLinefeed(const CString& name);
CString GrMoveTo     (const CString& name);
CString GrFormat     (const CString& name);
CString GrPrint      (const CString& name, int fontNumber, BOOL customColor);
CString GrPrtLF      (const CString& name, int fontNumber, BOOL customColor);
CString GrMark       (const CString& name);
CString GrMove       (const CString& name);
CString GrTab        (const CString& name);
CString GrPic        (const CString& name);
CString GrColor      (const CString& name, int color);

int DAT_Race_Weight(CHARACTER *pChar);
int DAT_Race_Height(CHARACTER *pChar);
CString DAT_Item_CommonName(const CString& itemName);
CString DAT_Item_IDName(const CString& itemName);


#else
const int TEXTBOX_LINES = 5;
#endif

unsigned int CString2Base38(const CString& str);
BOOL FindConfigToken(const CString& name, CString& value);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GPDL_STACK  gpdlStack;

extern struct DISCOURSELINE *pDiscourseLine;

DISCOURSELINE rootDiscourseLine;
DISCOURSELINE *pDiscourseLine = &rootDiscourseLine;
// Replaced with a stack of DISCOURSELINE;  CArray<CString, CString> discourseLine;


void DISCOURSELINE::Push(void)
{
  DISCOURSELINE *pEntry;
  pEntry = new DISCOURSELINE;
  pEntry->pPrevDiscourseLine = this;
  pDiscourseLine = pEntry;
};
void DISCOURSELINE::Pop(void)
{
  DISCOURSELINE *pEntry;
  pEntry = pPrevDiscourseLine;
  delete pDiscourseLine;
  pDiscourseLine = pEntry;
};



re_registers	  re_grepReg;
CString         re_searchString;

//*************************************************************

int gpdlDlg = -1;
//CDialog *activeDlg = NULL;
const int MAX_DLG_CONTROLS = 20;

void SetMapOverride(OVERRIDE_TYPE ovt, int *params);
int GetMapOverride(OVERRIDE_TYPE ovt, int *params);



class GPDLDlg:public CDialog
{
public:
  GPDLDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL):CDialog(nIDTemplate, pParentWnd){};
  BOOL OnInitDialog(void);
  void OnOK(void);
  DECLARE_MESSAGE_MAP()
  afx_msg void OnButton12(void);
  afx_msg void OnButton13(void);
  afx_msg void OnButton14(void);
  afx_msg void OnButton15(void);
  afx_msg void OnButton16(void);
  afx_msg void OnButton17(void);
  void GPDLDlg::DoDataExchange(CDataExchange* pDX);

  BOOL m_gpdlCheck[6];
  int  m_gpdlRadio[1];
  int  m_gpdlButton[6];
  CString m_gpdlEdit[2];
};


void die(CString errorMsg)
{
  MessageBox(NULL, errorMsg, "Fatal Error in RunEvent", MB_TOPMOST|MB_OK);
  abort();
}

void die(const char *msg)
{
  CString m(msg);
  die(m);
}

#ifdef UAFEDITOR
void die(int err)
{
  CString msg;
  msg.Format("die(0x%x)", err);
  die(msg);
}
#endif

/*
CString dialogControls[MAX_DLG_CONTROLS];
CString dialogResults[MAX_DLG_CONTROLS];

BEGIN_MESSAGE_MAP(GPDLDlg,CDialog)
  ON_BN_CLICKED(IDC_GPDLBUTTON12, OnButton12)
  ON_BN_CLICKED(IDC_GPDLBUTTON13, OnButton13)
  ON_BN_CLICKED(IDC_GPDLBUTTON14, OnButton14)
  ON_BN_CLICKED(IDC_GPDLBUTTON15, OnButton15)
  ON_BN_CLICKED(IDC_GPDLBUTTON16, OnButton16)
  ON_BN_CLICKED(IDC_GPDLBUTTON17, OnButton17)
END_MESSAGE_MAP()


afx_msg void GPDLDlg::OnButton12(void)
{
  dialogControls[7] = "1";
  OnOK();
}

afx_msg void GPDLDlg::OnButton13(void)
{
  dialogControls[8] = "1";
  OnOK();
}

afx_msg void GPDLDlg::OnButton14(void)
{
  dialogControls[9] = "1";
  OnOK();
}

afx_msg void GPDLDlg::OnButton15(void)
{
  dialogControls[10] = "1";
  OnOK();
}

afx_msg void GPDLDlg::OnButton16(void)
{
  dialogControls[11] = "1";
  OnOK();
}

afx_msg void GPDLDlg::OnButton17(void)
{
  dialogControls[12] = "1";
  OnOK();
}




void GetCheck(int i, BOOL src)
{
  dialogResults[i] = src?"1":"";
}

void GetRadio(int i, int src)
{
  dialogResults[i].Format("%d",src);
}

void GetButton(int i, int src)
{
  dialogResults[i] = src?"1":"";
}

void GetEdit(int i, CString src)
{
  dialogResults[i] = src;
}


void GPDLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacter)
	DDX_Check(pDX, IDC_GPDLCHECK0, m_gpdlCheck[0]);
	DDX_Check(pDX, IDC_GPDLCHECK1, m_gpdlCheck[1]);
	DDX_Check(pDX, IDC_GPDLCHECK2, m_gpdlCheck[2]);
	DDX_Check(pDX, IDC_GPDLCHECK3, m_gpdlCheck[3]);
	DDX_Check(pDX, IDC_GPDLCHECK4, m_gpdlCheck[4]);
	DDX_Check(pDX, IDC_GPDLCHECK5, m_gpdlCheck[5]);
  DDX_Radio(pDX, IDC_GPDLRADIO6, m_gpdlRadio[0]);
  DDX_Text (pDX, IDC_GPDLEDIT19,  dialogControls[19]);
  DDX_Text (pDX, IDC_GPDLEDIT18,  dialogControls[18]);
	//}}AFX_DATA_MAP
  if (pDX->m_bSaveAndValidate)
  {
    GetCheck(0, m_gpdlCheck[0]);
    GetCheck(1, m_gpdlCheck[0]);
    GetCheck(2, m_gpdlCheck[0]);
    GetCheck(3, m_gpdlCheck[0]);
    GetCheck(4, m_gpdlCheck[0]);
    GetCheck(5, m_gpdlCheck[0]);
    GetRadio(6, m_gpdlRadio[0]);
    GetButton(12, m_gpdlButton[0]);
    GetButton(13, m_gpdlButton[1]);
    GetButton(14, m_gpdlButton[2]);
    GetButton(15, m_gpdlButton[3]);
    GetButton(16, m_gpdlButton[4]);
    GetButton(17, m_gpdlButton[5]);
    GetEdit(18, dialogControls[18]);
    GetEdit(19, dialogControls[19]);
  };
};























void HideDlgItem(CWnd *pWnd, int i)
{
  pWnd->ShowWindow((dialogControls[i]=="")?SW_HIDE:SW_SHOW);
}

void gpdlSetCheck(int i, int id)
{
  CWnd *pWnd;
  pWnd = activeDlg->GetDlgItem(id);
  if (pWnd == NULL) return;
  pWnd->SetWindowText(dialogControls[i]);
  HideDlgItem(pWnd, i);
}

void gpdlSetRadio(int i, int id)
{
  CWnd *pWnd;
  pWnd = activeDlg->GetDlgItem(id);
  if (pWnd == NULL) return;
  pWnd->SetWindowText(dialogControls[i]);
  HideDlgItem(pWnd, i);
}

void gpdlSetButton(int i, int id)
{
  CWnd *pWnd;
  pWnd = activeDlg->GetDlgItem(id);
  if (pWnd == NULL) return;
  pWnd->SetWindowText(dialogControls[i]);
  HideDlgItem(pWnd, i);
}


BOOL GPDLDlg::OnInitDialog(void)
{
  CDialog::OnInitDialog();

  switch (gpdlDlg)
  {
    case 0:
    {
      int i;
      gpdlSetCheck ( 0,IDC_GPDLCHECK0);
      gpdlSetCheck ( 1,IDC_GPDLCHECK1);
      gpdlSetCheck ( 2,IDC_GPDLCHECK2);
      gpdlSetCheck ( 3,IDC_GPDLCHECK3);
      gpdlSetCheck ( 4,IDC_GPDLCHECK4);
      gpdlSetCheck ( 5,IDC_GPDLCHECK5);
      gpdlSetRadio ( 6,IDC_GPDLRADIO6);
      gpdlSetRadio ( 7,IDC_GPDLRADIO7);
      gpdlSetRadio ( 8,IDC_GPDLRADIO8);
      gpdlSetRadio ( 9,IDC_GPDLRADIO9);
      gpdlSetRadio (10,IDC_GPDLRADIO10);
      gpdlSetRadio (11,IDC_GPDLRADIO11);
      gpdlSetButton(12,IDC_GPDLBUTTON12);
      gpdlSetButton(13,IDC_GPDLBUTTON13);
      gpdlSetButton(14,IDC_GPDLBUTTON14);
      gpdlSetButton(15,IDC_GPDLBUTTON15);
      gpdlSetButton(16,IDC_GPDLBUTTON16);
      gpdlSetButton(17,IDC_GPDLBUTTON17);
      for (i=0; i<6; i++)
      {
        m_gpdlCheck[i] = 0;
        m_gpdlButton[i] = 0;
      };
      m_gpdlRadio[0] = -1;
      dialogControls[18] = "";
      dialogControls[19] = "";
    };
    break;
  };
  return true;
} 

void gpdlGetCheck(int i, int id)
{
  
  //CWnd *pWnd;
  //pWnd = activeDlg->GetDlgItem(id);
  //if (pWnd == NULL) return;
  //pWnd->GetCheck();
  
}

void GPDLDlg::OnOK(void)
{
  
  //switch (gpdlDlg)
  //{
  //case 0:
  //    gpdlGetCheck ( 0,IDC_GPDLCHECK1);
  //    gpdlGetCheck ( 1,IDC_GPDLCHECK2);
  //    gpdlGetCheck ( 2,IDC_GPDLCHECK3);
  //    gpdlGetCheck ( 3,IDC_GPDLCHECK4);
  //    gpdlGetCheck ( 4,IDC_GPDLCHECK5);
  //    gpdlGetCheck ( 5,IDC_GPDLCHECK6);
  //    gpdlGetRadio ( 6,IDC_GPDLRADIO1);
      //gpdlGetButton(12,IDC_GPDLBUTTON1);
      //gpdlGetButton(13,IDC_GPDLBUTTON2);
      //gpdlGetButton(14,IDC_GPDLBUTTON3);
      //gpdlGetButton(15,IDC_GPDLBUTTON4);
      //gpdlGetButton(16,IDC_GPDLBUTTON5);
      //gpdlGetButton(17,IDC_GPDLBUTTON6);
  //};
  
  activeDlg->UpdateData(true);
  CDialog::OnOK();
}


void gpdlCleanup(void)
{
  if (activeDlg != NULL)
  {
    delete activeDlg;
    activeDlg = NULL;
  };
}

*/

//*************************************************************

GPDL::GPDL(void) {
  m_state=GPDL_UNINITIALIZED;
  m_RP0=1000;
  m_SP0=1000;
  m_H = 0;
  m_dataStack=NULL;
  m_returnStack=NULL;
  m_program = NULL;
  m_false="";
  m_listenText=m_false;
  m_true="1";
  SetIntermediateResult(0.0);
  pTimedSA = NULL;
}

GPDL::~GPDL(void) {
  if (m_dataStack!=NULL) {
    delete[] m_dataStack;
    m_dataStack=NULL;
  };
  if (m_returnStack!=NULL) {
    delete[] m_returnStack;
    m_returnStack=NULL;
  };
  if (m_program != NULL) 
  {
    delete[] m_program;
    m_program = NULL;
  };
  if (pTimedSA != NULL)
  {
    delete pTimedSA;
    pTimedSA = NULL;
  };
  m_state=GPDL_UNINITIALIZED;
}


#ifdef UAFEDITOR
GPDL_STATE GPDL::ReadProgram(GPDLCOMP *pGPDL)
{
  m_H = pGPDL->GetCodeSize();
  if (m_H <= 0) return GPDL_READ_ERROR;
  m_programAllocated=m_H;
  if (m_program != NULL) delete[] m_program;
  m_program = new unsigned int[m_programAllocated];
  memcpy(m_program, pGPDL->GetCodeAddress(),4*m_programAllocated);
  return GPDL_OK;
}
#endif UAFEDITOR

GPDL_STATE GPDL::ReadProgram(CArchive& ar)
{
  unsigned int i;
  ar >> m_H;
  m_programAllocated=m_H;
  if (m_program != NULL) delete[] m_program;
  m_program = new unsigned int[m_programAllocated];
  for (i=0; i<m_H; i++) {
    ar>>m_program[i];
  };
  return GPDL_OK;
}

GPDL_STATE GPDL::ReadProgram(RAM_FILE& ar)
{
  unsigned int i;
  ar >> m_H;
  m_programAllocated=m_H;
  if (m_program != NULL) delete[] m_program;
  m_program = new unsigned int[m_programAllocated];
  for (i=0; i<m_H; i++) {
    ar>>m_program[i];
  };
  return GPDL_OK;
}

//*****************************************
//  Time to be sure the file has been loaded.
//  Parameter:
//    CArchive = file to load
//  Return value:
//    0 if all is well
//    non-zero is an error code
//*****************************************
GPDL_STATE GPDL::Load(CArchive& ar) {
  if (m_state!=GPDL_UNINITIALIZED) return GPDL_EVENT_ERROR;
  if (ReadProgram(ar) != GPDL_OK)  return GPDL_READ_ERROR;
  //if (m_code.read(ar))      return GPDL_READ_ERROR;
  if (m_globals.read(ar)) return GPDL_READ_ERROR;
  if (m_index.read(ar))     return GPDL_READ_ERROR;
  m_state=GPDL_IDLE;
  if (m_dataStack!=NULL) delete []m_dataStack;
  m_dataStack=NULL;
  if (m_returnStack!=NULL) delete []m_returnStack;
  m_returnStack=NULL;
  m_dataStack=new CString[m_SP0];
  m_returnStack=new unsigned int[m_RP0];
  return GPDL_IDLE;
}


GPDL_STATE GPDL::Load(RAM_FILE& ar) {
  if (m_state!=GPDL_UNINITIALIZED) return GPDL_EVENT_ERROR;
  if (ReadProgram(ar) != GPDL_OK)  return GPDL_READ_ERROR;
  //if (m_code.read(ar))      return GPDL_READ_ERROR;
  if (m_globals.read(ar)) return GPDL_READ_ERROR;
  if (m_index.read(ar))     return GPDL_READ_ERROR;
  m_state=GPDL_IDLE;
  if (m_dataStack!=NULL) delete []m_dataStack;
  m_dataStack=NULL;
  if (m_returnStack!=NULL) delete []m_returnStack;
  m_returnStack=NULL;
  m_dataStack=new CString[m_SP0];
  m_returnStack=new unsigned int[m_RP0];
  return GPDL_IDLE;
}

#ifdef UAFEDITOR
GPDL_STATE GPDL::Load(GPDLCOMP *pGPDL)
{
  if (ReadProgram(pGPDL) != GPDL_OK) return GPDL_READ_ERROR;
  if (m_globals.read(pGPDL)) return GPDL_READ_ERROR;
  if (m_index.read(pGPDL))     return GPDL_READ_ERROR;
  m_state = GPDL_IDLE;
  if (m_dataStack!=NULL) delete []m_dataStack;
  m_dataStack=NULL;
  if (m_returnStack!=NULL) delete []m_returnStack;
  m_returnStack=NULL;
  m_dataStack=new CString[m_SP0];
  m_returnStack=new unsigned int[m_RP0];
  return GPDL_OK;
}
#endif

void GPDL::m_pushRP(unsigned int n) {
  if (m_RP<2) {
    if (m_interpStatus==GPDL_OK) m_interpStatus=GPDL_OVER_RP;
    return;
  };
  m_returnStack[--m_RP]=n;
}

unsigned int GPDL::m_popRP(void) {
  if (m_RP>=m_RP0) {
    if (m_interpStatus==GPDL_OK) m_interpStatus=GPDL_UNDER_RP;
    return 0;
  };
  return m_returnStack[m_RP++];
}

void GPDL::m_pushFP(void) {
  m_pushRP(m_FP);
}

void GPDL::m_pushPC(void) {
  m_pushRP(m_PC);
}

void GPDL::m_popPC(void) {
  m_PC=m_popRP();
}

void GPDL::m_popFP(void) {
  m_FP=m_popRP();
}

void GPDL::m_pushSP(const CString& val) {
  if (m_SP<2) {
    if (m_interpStatus==GPDL_OK) m_interpStatus=GPDL_OVER_SP;
    return;
  };
  m_dataStack[--m_SP]=val;
}

CString GPDL::m_popSP(void) {
  if (m_SP>=m_SP0) {
    if (m_interpStatus==GPDL_OK) m_interpStatus=GPDL_UNDER_SP;
    return m_false;
  }
  return m_dataStack[m_SP++];
}

void GPDL::m_popString1(void) {
  m_string1=m_popSP();
}
void GPDL::m_popString2(void) {
  m_string2=m_popSP();
}
void GPDL::m_popString3(void) {
  m_string3=m_popSP();
}
void GPDL::m_popString4(void) {
  m_string4=m_popSP();
}
void GPDL::m_popString5(void) {
  m_string5=m_popSP();
}
void GPDL::m_popString6(void) {
  m_string6=m_popSP();
}

void GPDL::m_pushString1(void) {
  m_pushSP(m_string1);
}
void GPDL::m_pushString2(void) {
  m_pushSP(m_string2);
}
void GPDL::m_pushString3(void) {
  m_pushSP(m_string3);
}
void GPDL::m_pushString4(void) {
  m_pushSP(m_string4);
}
void GPDL::m_pushEmptyString(void) {
  m_pushSP(emptyString);
}

void GPDL::m_pushUInteger(unsigned int u)
{
  char a[30];
  _ultoa(u, a, 10);
  m_string1 = a;
  m_pushString1();
}

void GPDL::m_pushInteger(int i)
{
  char a[30];
  _itoa(i, a, 10);
  m_string1 = a;
  m_pushString1();
}

inline void GPDL::m_pushUInteger1(void) 
{
  m_pushUInteger(m_uInteger1);
}

inline void GPDL::m_pushUInteger2(void)
{
  m_pushUInteger(m_uInteger2);
}

inline void GPDL::m_pushInteger1(void) 
{
  m_pushInteger(m_Integer1);
}

inline void GPDL::m_pushInteger2(void) 
{
  m_pushInteger(m_Integer2);
}

inline void GPDL::m_pushInteger3(void) 
{
  m_pushInteger(m_Integer3);
}

GPDL_STATE GPDL::m_popInteger(bool& negative, 
                unsigned int& n) {
  GPDL_STATE status;
  static CString str; // Avoid allocating each time.
  unsigned int len, i;
  unsigned int base=10;
  char c;
  negative=false;
  status=GPDL_OK;
  n=0;
  str=m_popSP();
  len=str.GetLength();
  for (i=0; i<len; i++) {
    c=str.GetAt(i);
    if ((c=='-')&&(i==0)) {
      negative=true;
      continue;
    }
    if ((c<'0') || (c>'9')) {
      if ((c=='x')||(c=='X')) {
        if ((n!=0)||(base!=10)||(i!=1)){
          status=GPDL_BADINTEGER;
          break;
        };
        base=16;
        continue;
      }
      else
      {
        if ((base==10) && (i!=0) && (c=='.')) // Floating point eg: "3.000"
        {
          for (i++; i<len; i++)
          {
            c = str.GetAt(i);
            if ( (c < '0') || (c > '9')) status = GPDL_BADINTEGER;
          };
        }
        else
        {
          status=GPDL_BADINTEGER;
        };
        break;
      };
	  /* Unreachable
      if (base!=16) {
        status=GPDL_BADINTEGER;
        break;
      };
      if ((c>='a') && (c<='f')) c+='A'-'a';
      if ((c<'A') || (c>'F')) {
        status=GPDL_BADINTEGER;
        break;
      };
      c+='9'+1-'A';
	  */
    };
    n=n*base+c-'0';
  };
  if (status!=GPDL_OK) { 
    n=0;
  };
  return status;
}

int GPDL::m_popInteger(void)
{
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  return negative?-(int)n:n;
}

void GPDL::m_popInteger1(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  m_Integer1=negative?-(int)n:n;
}

void GPDL::m_popInteger2(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  m_Integer2=negative?-(int)n:n;
}

void GPDL::m_popInteger3(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  m_Integer3=negative?-(int)n:n;
}

void GPDL::m_popInteger4(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  m_Integer4=negative?-(int)n:n;
}
void GPDL::m_popInteger5(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  m_Integer5=negative?-(int)n:n;
}


void GPDL::m_popUInteger1(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  if (negative) {
    m_uInteger1=0;
  } else m_uInteger1=n;
}

void GPDL::m_popUInteger2(void) {
  bool negative;
  unsigned int n;
  m_popInteger(negative, n);
  if (negative) {
    m_uInteger2=0;
  } else m_uInteger2=n;
}

void GPDL::m_popInts(int n)
{
  while(n>0)
  {
    n--;
    m_intArray[n] = m_popInteger();
  };
}

CHARACTER *GPDL::m_popCharacterActor(void)
{
  ActorType actor;
  CString temp;
  CHARACTER *pChar;
  temp = m_popSP();
  actor = m_StringToActor(temp);
  if (actor == NULL_ACTOR)
  {
    MsgBoxError("Script referenced non-existent actor", "Error", 1);
    return &NULL_CHAR;
  };
  pChar = GetCurrentlyActiveContext(&actor, "GPDL::m_popCharacterActor");
  return pChar;
}

#ifdef UAFEngine
COMBATANT *GPDL::m_popCombatantActor(void)
{
  ActorType actor;
  CString temp;
  COMBATANT *pCombatant;
  temp = m_popSP();
  actor = m_StringToActor(temp);
  if (actor == NULL_ACTOR)
  {
    MsgBoxError("Script referenced non-existent actor", "Error", 1);
    return &NULL_COMBATANT;
  };
  pCombatant = &GetCombatCharacterContext(actor);
  return pCombatant;
}

#endif

void GPDL::AURA_FUNCTION(AURA_FUNC func)
{
#ifdef UAFEngine
  AURA *pAURA;
  POSITION auraPos;
  bool error = false;
  {
    auraPos = combatData.GetAuraReference();
    if (auraPos == NULL)
    {
      WriteDebugString("Attempt to reference AURA outside of AURA script\n");
      error = true;
    }
  };
  pAURA = combatData.GetAURA(auraPos);
#endif
  {
    switch(func)
    {
    case $AURA_Destroy:
#ifdef UAFEngine
      if (!error)
      {
        pAURA->shape[1] = AURA_SHAPE_NULL;
        combatData.DeleteAura(auraPos);
        combatData.CheckAuraPlacement(pAURA, NULL);
      } 
      else
#endif
      {
        m_string1 = "OK";
        m_pushString1();
      };
      break;
    case $AURA_AddSA:
#ifdef UAFEngine
      if (!error)
      {
        m_popString2(); // Special Ability Parameter
        m_popString1(); // Special Ability Name
        combatData.AddAuraSA(auraPos, m_string1, m_string2);
        m_pushString1();
      }
      else
#endif
      {
        m_popString1();
      };
      break;
    case $AURA_GetSA:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();  // Special Ability Name
        m_string2 = combatData.GetAuraSA(auraPos, m_string1);
        m_pushString2();
      }
      else
#endif
      {
      };
      break;
    case $AURA_RemoveSA:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();  // Special Ability Name
        m_string2 = combatData.RemoveAuraSA(auraPos, m_string1);
        m_pushString2();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Attach:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();
        if (m_string1 == "Combatant")
        {
          pAURA->attachment[1] = AURA_ATTACH_COMBATANT;
        }
        else if (m_string1 == "CombatantFacing")
        {
          pAURA->attachment[1] = AURA_ATTACH_COMBATANT_FACING;
        }
        else if (m_string1 == "XY")
        {
          pAURA->attachment[1] = AURA_ATTACH_XY;
          pAURA->x[1] = -1;
          pAURA->y[1] = -1;
        }
        else 
        {
          WriteDebugString("Illegal AURA Attachment Parameter\n");
          pAURA->attachment[1] = AURA_ATTACH_NONE;
        };
        m_pushString1();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Combatant:
#ifdef UAFEngine
      if (!error)
      {
        m_popInteger1();
        pAURA->combatantIndex[1] = m_Integer1;
        m_string1.Empty();
        m_pushString1();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Wavelength:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();
        if (m_string1 == "Visible")  pAURA->wavelength[1] = AURA_WAVELENGTH_VISIBLE;
        if (m_string1 == "Xray")     pAURA->wavelength[1] = AURA_WAVELENGTH_XRAY;
        if (m_string1 == "Neutrino") pAURA->wavelength[1] = AURA_WAVELENGTH_NEUTRINO;
        m_pushString1();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Size:
#ifdef UAFEngine
      if (!error)
      {
        m_popInteger1();  pAURA->size4[1] = m_Integer1;
        m_popInteger1();  pAURA->size3[1] = m_Integer1;
        m_popInteger1();  pAURA->size2[1] = m_Integer1;
        m_popInteger1();  pAURA->size1[1] = m_Integer1;
        m_string1 = "OK";
        m_pushString1();
      }
      else
#endif
      {
        m_popInteger1();
        m_popInteger1();
        m_popInteger1();
      };
      break;
    case $AURA_Shape:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();
        if      (m_string1 == "Global") pAURA->shape[1] = AURA_SHAPE_GLOBAL;
        else if (m_string1 == "AnnularSector") pAURA->shape[1] = AURA_SHAPE_ANNULARSECTOR;
        else 
        {
          WriteDebugString("Illegal AURA shape\n");
          pAURA->shape[1] = AURA_SHAPE_NULL;
        };
        m_pushString1();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Spell:
#ifdef UAFEngine
      if (!error)
      {
        m_popString1();
        pAURA->spellID[1] = m_string1;
        m_pushString1();
      }
      else
#endif
      {
      };
      break;
    case $AURA_Location:
#ifdef UAFEngine
        m_popInteger1();  pAURA->y[1] = m_Integer1;
        m_popInteger1();  pAURA->x[1] = m_Integer1;
        m_string1 = "OK";
        m_pushString1();
#endif
      break;
    case $AURA_SetData:
#ifdef UAFEngine
      if (!error)
      {
        /* Really */ NotImplemented(0x49a73b, false);
      }
      else
#endif
      {
      };
      break;
    case $AURA_GetData:
#ifdef UAFEngine
      if (!error)
      {
        m_pushSP(pAURA->userData[m_popInteger()]);
      }
      else
#endif
      {
      };
      break;
    default:
        /* Really */ NotImplemented(0x49a7c5, false);

    };
  };
}

const ITEM_DATA *GPDL::m_GetItemData(void)
{
  ITEM_ID itemID;
  m_popString1();
  itemID = m_string1;
  m_string2.Empty();
  return itemData.PeekItem(itemID);
}


static const char *callLine;
static int length;

static int GetToken(void)
{
  char c;
  callLine += length;
  while (*callLine == ' ') callLine++;
  c = *callLine;
  if (    (c=='$')
       || (c=='_')
       || ((c>='A') && (c<='Z'))
       || ((c>='a') && (c<='z'))
     )
  {
    for (length=1; ;length++)
    {
      c = callLine[length];
      if (    (c=='$')
           || (c=='_')
           || ((c>='A') && (c<='Z'))
           || ((c>='a') && (c<='z'))
           || ((c>='0') && (c<='9'))
        ) continue;
      break;
    };
    return 1; // 1 is a function name
  };
  if (c == '"')
  {
    length = 1;
    while (callLine[length] != '"')
    {
      if (callLine[length] == 0) return 0;
      length++;
    };
    length++;
    return 5; // 5 is a quoted string
  };
  if ( (c>='0') && (c<='9') )
  {
    length = 1;
    while ((callLine[length]>='0') && (callLine[length]<='9')) length++;
    return 6; //6 is a number
  };
  length = 1;
  if (c == '(') return 2; // 2 is open parethesis
  if (c == ')') return 3;
  if (c == ',') return 4;
  return 0; // 0 is an error
}

GPDL_STATE GPDL::BeginExecute(CString& funcCall, GPDL_EVENT *event) 
{
  int tokenType;
  char c;
  int neededParam, headerIdx, multiplier;
  callLine = LPCTSTR(funcCall);
  length = 0;
  tokenType = GetToken();
  if (tokenType != 1)
  {
    WriteDebugString("GPDL call does not contain a function name\n");
    return GPDL_NOSUCHNAME;
  };
  m_string1 = CString(callLine,length);
  m_PC=m_index.lookup(m_string1);
  if (m_PC==0) 
  {
    m_string2.Format("GPDL cannot find specified function named %s.\n",m_string1);
    WriteDebugString(m_string2);
    return GPDL_NOSUCHNAME;
  };
  m_pGPDLevent=event;
  m_RP=m_RP0;
  m_SP=m_SP0;
  m_pushRP(0xffffffff);          // So we will know when we are done.
  m_pushRP(0xffffffff);          // Old Frame Pointer
  m_string1=m_false;
  m_pushSP(m_string1); // The empty string as default result.
  m_string2 = m_globals.peek(Peek(m_PC) & 0xffffff);
  neededParam = 0;
  multiplier = 1;
  for (headerIdx=m_string2.GetLength()-2;headerIdx>=0;headerIdx--)
  {
    c = m_string2[headerIdx];
    if (c < '0') break;
    if (c > '9') break;
    neededParam += multiplier * (c-'0');
    multiplier *= 10;
  };
  tokenType = GetToken();
  if (tokenType != 2)
  {
    WriteDebugString("GPDL function name not followed by '('\n");
    return GPDL_NOSUCHNAME;
  };
  for ( ; ; )
  { // Get another parameter
    tokenType = GetToken();
    if (tokenType == 3) break;
    if (tokenType == 5) m_string1 = CString(callLine+1, length -2);
    else if (tokenType == 6) m_string1 = CString(callLine, length);
    else
    {
      WriteDebugString("GPDL parameter syntax error\n");
      return GPDL_NOSUCHNAME;
    };
    m_pushSP(m_string1);
    neededParam--;
    tokenType = GetToken();
    if (tokenType == 3) break;
    if (tokenType != 4)
    {
      WriteDebugString("GPDL call parameters missing a comma\n");
      return GPDL_NOSUCHNAME;
    };
  };
  if (neededParam < 0) m_string1 = "many";
  if (neededParam > 0) m_string1 = "few";
  if (neededParam != 0)
  {
    m_string3.Format("GPDL function %s call has too %s parameters\n",m_string2,m_string1);
    WriteDebugString(m_string3);
    return GPDL_NOSUCHNAME;
  };
  m_FP=m_SP;
  m_interpretCount = 0;
  return m_interpret();
}

CString GPDL::BeginExecute(int entryPoint) {
  GPDL_STATE status;
  m_PC=entryPoint;
  if (m_PC==0) return m_false;
  m_pGPDLevent=NULL;
  m_RP=m_RP0;
  m_SP=m_SP0;
  m_pushRP(0xffffffff);          // So we will know when we are done.
  m_pushRP(0xffffffff);          // Old Frame Pointer
  m_string1=m_false;
  m_pushSP(m_string1); // The empty string as default result.
  m_FP=m_SP;
  status = m_interpret();
  if (status != GPDL_IDLE) return m_false;
  m_popString1();
  return m_string1;
}

CString GPDL::ExecuteScript(const CString& code, int entryPointOrdinal)
{
  GPDL_STATE status;
  int numEntry;
  const char *binaryCode;
  binaryCode = LPCTSTR(code);
  if (m_program != NULL) My_free (m_program);
  numEntry = *((int *)(binaryCode + 1)) & 0xffffff;
  m_H = *((int *)(binaryCode + 4));
  if (entryPointOrdinal > numEntry) return m_false;
  if (entryPointOrdinal <= 0) return m_false;
  m_PC = ((int *)(binaryCode + 12))[entryPointOrdinal-1];
  if (m_PC==0) return m_false;
  m_program = (unsigned int *)(binaryCode + 12 + 4*numEntry);
  if (m_dataStack==NULL) 
            m_dataStack=new CString[m_SP0];
  if (m_returnStack==NULL)
            m_returnStack=new unsigned int[m_RP0];
  m_pGPDLevent=NULL;
  m_RP=m_RP0;
  m_SP=m_SP0;
  m_pushRP(0xffffffff);          // So we will know when we are done.
  m_pushRP(0xffffffff);          // Old Frame Pointer
  m_string1=m_false;
  m_pushSP(m_string1); // The empty string as default result.
  m_FP=m_SP;
  m_interpretCount = 0;
  status = m_interpret();
  m_program = NULL;
  if (status != GPDL_IDLE) return m_false;
  m_popString1();
  return m_string1;
}

// Square window (running average) filter
#define interpretPeriod 5  // virtualGametime steps
#define interpretMax  50000 // interpretSteps per interpretPeriod
class InterpretFilter
{
  unsigned int steps[interpretPeriod+1];
  unsigned int time[interpretPeriod+1];
  unsigned int stepSum;
  int start, num;
public:
  InterpretFilter(void);
  void Add(int);
  void Clear(void);
};

InterpretFilter::InterpretFilter(void)
{
  Clear();
}

void InterpretFilter::Clear(void)
{
  int i;
  for (i=0; i<interpretPeriod; i++)
  {
    steps[i] = 0;
    start = 0;  // index of first entry containing data    
    num = 0;    // number of entries containing data
  };
}

void InterpretFilter::Add(int numSteps)
{
//  {
//    CString msg;
//    msg.Format("%d %d %s\n", numSteps, 
//                             stepSum, 
//                             (pScriptContext==NULL)?"Unknown":pScriptContext->scriptName);
//    WriteDebugString(msg);
//  };

  if (numSteps > 50000)  // That's enough for any one script!
  {
    CString msg;
    if (!debugStrings.AlreadyNoted("ELScript"))
    {
      msg.Format("Excessively long script.\n %s (%s) [%s]",
                  pScriptContext->sourceName,
                  pScriptContext->specAb.Key(),
                  (pScriptContext==NULL)?"Unknown":pScriptContext->scriptName);
      MsgBoxError(msg,"Warning", 5);
    };
  };
  // Eliminate any old counts
  while ( (num>0) && (time[start] < virtualGameTime - interpretPeriod))
  {
    stepSum -= steps[start];
    start = (start + 1) % (interpretPeriod+1);
    num--;
  };
  if (num == 0)
  {
    steps[start] = numSteps;
    time[start] = virtualGameTime;
    stepSum = numSteps;
    num = 1;
    return;
  };
  {
    int cur;
    cur = (start+num-1) % (interpretPeriod+1); // Where the previous data went.
    if (time[cur] != virtualGameTime)
    {
      cur = (cur+1) % (interpretPeriod+1);
      time[cur] = virtualGameTime;
      steps[cur] = 0;
      num++;
    };
    steps[cur] += numSteps;
    stepSum += numSteps;
  };
  if (stepSum > interpretMax)
  {
    CString msg;
    msg.Format("Excessive script execution\n"
               "Special Ability = %s\n"
               "Script name = %s",  
               (pScriptContext==NULL)?"Unknown":pScriptContext->specAb.Key(),   
               (pScriptContext==NULL)?"Unknown":pScriptContext->scriptName);
    MsgBoxError(msg,"Warning", 5);
    Clear();
  };
  return;
}


InterpretFilter interpretFilter;



CString GPDL::ExecuteScript(const CString& code, 
                            int entryPointOrdinal, 
                            CString *par,
                            int numPar)
{
  GPDL_STATE status;
  int numEntry;
  const char *binaryCode;
  binaryCode = LPCTSTR(code);
  if (m_program != NULL) My_free (m_program);
  numEntry = *((int *)(binaryCode + 1)) & 0xffffff;
  m_H = *((int *)(binaryCode + 4));
  if (entryPointOrdinal > numEntry) return m_false;
  if (entryPointOrdinal <= 0) return m_false;
  m_PC = ((int *)(binaryCode + 12))[entryPointOrdinal-1];
  if (m_PC==0) return m_false;
  m_program = (unsigned int *)(binaryCode + 12 + 4*numEntry);
  if (m_dataStack==NULL) 
            m_dataStack=new CString[m_SP0];
  if (m_returnStack==NULL)
            m_returnStack=new unsigned int[m_RP0];
  m_pGPDLevent=NULL;
  m_RP=m_RP0;
  m_SP=m_SP0;
  m_pushRP(0xffffffff);          // So we will know when we are done.
  m_pushRP(0xffffffff);          // Old Frame Pointer
  m_string1=m_false;
  m_pushSP(m_string1); // The empty string as default result.
  for (int i=0; i<numPar; i++) m_pushSP(par[i]); // A through F
  m_FP=m_SP;
  m_interpretCount = 0;
  m_discourseText.Push();
  status = m_interpret();
  m_discourseText.Pop();
  interpretFilter.Add(m_interpretCount);

  m_program = NULL;
  if (status != GPDL_IDLE) return m_false;
  m_popString1();
  return m_string1;
}

int GPDL::GetFunctionEntryPoint(const CString& name)
{
  if (m_state == GPDL_UNINITIALIZED) return -1;
  return m_index.lookup(name);
}

void GPDL::m_interpretError(const char *msg) 
{
#ifdef UAFEngine
  DoTaskStackTrace();
#endif
  CString message;
  if (!debugStrings.AlreadyNoted("GPDLIE"))
  {
    CString temp;
    message.Format("GPDL interpret error.\n%s\nPC = 0x%06x",msg,m_executionAddress);
    if (pScriptContext != &NULL_SCRIPT_CONTEXT)
    {
      message += "\nSpecial Ability: ";
      message += pScriptContext->specAb.Key();
      message += "\nScript: ";
      message += pScriptContext->scriptName;
    };
    temp.Format("\nParty location: %d(%d,%d)", party.level, party.Posx, party.Posy);
    message += temp;
    if (interpretError == "")
    {
      MsgBoxError(message);
    }
    else
    {
      interpretError = message;
    };
  };
}


#define INTERP_RETURN(status) {   \
  if (m_interpStatus!=GPDL_OK) {    \
    BreakExecute();         \
    return m_interpStatus;      \
  };                  \
  return status;            \
}


CString upperPattern(CString& str) {
  // Convert characters to uppercase unless escaped by a backslash.
  // Otherwise constructs such as \b get turned into \B...undesired!
  char *temp;
  int i;
  char c;
  int len=str.GetLength();
  temp=(char *)alloca(len+2);
  if (temp==NULL) return str;
  for (i=0; i<len; i++) {
    c=str.GetAt(i);
    if (c=='\\') {
      temp[i++]=c;
      temp[i]=str.GetAt(i);
    } else {
      if ((c>='a') && (c<='z')) temp[i]=(char)(c+'A'-'a');
      else temp[i]=c;
    };
  };
  temp[len]=0;
  return CString(temp);
}


void GPDL::m_error_message(char *msg){
  if (m_debugLevel&DBG_messageBox) {
    MsgBoxError(msg);
  };
}

/*void GPDL::m_attributeInsert(ATTRIBUTES *atr, 
               const CString& key,
               const CString& value) {
  int flags;
  flags=atr->GetFlags(key);
  if (flags==-1) flags=0; else flags|=ATF_MODIFIED;
  if ((flags&ATF_READONLY)==0)
  {
    atr->Insert(key,value,flags);
  }
  else
  {
    m_interpretError("Attempt to modify READONLY ATTRIBUTE");
  };
}
*/

  

CString GenderNames[3]={"Male", "Female", "Bishop"};

struct IF_KEYWORD_DATA;
IF_KEYWORD_DATA *LocateKeywordData(const int keyindex);
//CString GetDataAsCString(IF_KEYWORD_DATA *pData,ActorType *pActor=NULL);
//void SetDataFromCString(IF_KEYWORD_DATA *pData, CString value,ActorType *pActor=NULL);

extern CArray<COMBATANT *,COMBATANT *> newMonsters;

COMBATANT *GPDL::m_IndexToCombatant(int index, const char *msg)
{
#ifdef UAFEngine
  if (IsCombatActive())
  {
    if (   (index<0) 
        || (index  >= GetNumCombatants()))
    {
      if (    (index >= NewCombatantInstanceOffset)
           && (index < (NewCombatantInstanceOffset + newMonsters.GetSize())))
      {
        return newMonsters[index-NewCombatantInstanceOffset];
        //return newMonsters[index-NewCombatantInstanceOffset];
      };
      CString errmsg;
      errmsg.Format("Illegal combatant %i in m_IndexToCombatant", index);
      errmsg += "\n";
      MsgBoxError(errmsg, "Script Error", 1);
      //WriteStackTrace();
      return &NULL_COMBATANT;
    };    
    return combatData.GetCombatant(index);    
  }
  else
  {
    return NULL;
  };
#else
  return NULL;
#endif
}

CHARACTER *GPDL::m_IndexedCharacter(int index, const char *msg)
{
#ifdef UAFEngine
  if (IsCombatActive())
  {
    return m_IndexToCombatant(index, msg)->m_pCharacter;
  }
  else
#endif
  {
    //return globalData.charData.GetCharacterData(index);
    /* Really */ NotImplemented(0x91bbd, false); return NULL;  //return globalData.charData.GetCharacterData(index);
  };
}

CHARACTER *GPDL::m_IndexToCharacter(int index, const char *msg)
{
#ifdef UAFEDITOR
  return &NULL_CHAR;
#else
  CHARACTER *result = NULL;
  if (IsCombatActive())
  {
    result = m_IndexToCombatant(index, msg)->m_pCharacter;
  }
  else if (index == -2)
  {
    result = pScriptContext->GetCreatedCharacterContext(msg);
  }
  else
  {
    if (index < MAX_PARTY_MEMBERS)
    {
      int i;
      for (i=0;i<GetPartySize(party);i++)
      {
        if (party.characters[i].uniquePartyID == index)
        {
          break;
        }
      }
      if (i < GetPartySize(party))
      {
        result = &party.characters[i];
      }
    }
    else
    {
      //result = globalData.charData.GetCharacterData(index);
      /* Really */ NotImplemented(0x12cca, false); return NULL; //result = globalData.charData.GetCharacterData(index);
    };
  };
  if (result == NULL)
  {
    CString errmsg;
    errmsg.Format("Illegal party member %i in m_IndexToCharacter",
                  index);
    errmsg += "\n";

    m_interpretError(errmsg);
    result = &NULL_CHAR;
    //WriteStackTrace();
  };
  return result;
#endif
}


/*
void GPDL::m_getCharacterValueWithActor(int keyindex, ActorType actor)
{
  // We need to convert keyindex to a IF_KEYWORD_DATA pointer
  IF_KEYWORD_DATA *pIfKeyData = LocateKeywordData(keyindex);
  m_string1 = GetDataAsCString(pIfKeyData, &actor);
  m_pushString1();
}
*/

void GPDL::m_GetCharActor(ActorType *pActor, const char *msg)
{
  if (IsCombatActive())
  {
#ifdef UAFEngine
    COMBATANT *pCombatant;
    if (   (m_Integer1<0) 
        || (m_Integer1>=GetNumCombatants()))
    {
      if (     (m_Integer1 >= NewCombatantInstanceOffset)
           &&  (m_Integer1 < (NewCombatantInstanceOffset + newMonsters.GetSize())))
      {
        pCombatant = newMonsters[m_Integer1-NewCombatantInstanceOffset];
      }
      else
      {
        CString errmsg;
        errmsg.Format("Illegal combatant %i in m_getCharActor\n%s",
          m_Integer1, msg);
        errmsg += "\n";
        errmsg += GetActorDebugText(*pActor);
        m_interpretError(errmsg);
        //WriteStackTrace();
        m_string1 = m_false;
        m_pushString1(); // Have to supply a result.
        *pActor = INVALID_ACTOR;
        return;
      };
    }
    else
    {    
      pCombatant = combatData.GetCombatant(m_Integer1);
    };
    pCombatant->GetContext(pActor);
#else
    *pActor=NULL_ACTOR;
#endif
  }
  else
  {
#ifdef UAFEngine
    int partyIndex = -1;
//
// Why shouldn't m_integer1 be the partyIndex?  That is
// what the help file says!
// This would allow a script to go through the party members one at a time.
//
// In July 2018 I examined the functions that somehow used an integer to
// select a character.  Here are the results of that analysis:
//
    /*
      case SUBOP_SET_CHAR_SEX:             (party order or  combat order)
      case SUBOP_GET_PARTY_ACTIVECHAR:     (party order)
      case SUBOP_SetMemorizeCount:         (party order)
      case SUBOP_GetHighestLevelBaseclass: (party order)
      case SUBOP_IndexToActor:             (party order)
      case SUBOP_GetBaseclassLevel:        (party order)
      case SUBOP_IF_CHAR_ASL:              (party order)
      case SUBOP_COINCOUNT:                (party order)
      case SUBOP_GET_CHAR_SEX:             (uniqueID    or   combat order)
      case SUBOP_GET_CHAR_PERM_STR:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_CHA:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_ALIGNMENT:       (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_CLASS:           (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_CON:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_DAMAGEBONUS:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_STRMOD:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_RACE:            (uniqueID   or   combat order)
      case SUBOP_SET_CHAR_RACE:            (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_ASL:             (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_AC:              (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_ADJAC:           (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_NAME:            (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_HITPOINTS:       (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_ADJTHAC0:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_EFFAC:           (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_RDYTOTRAIN:      (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_Exp:             (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_THAC0:           (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_MAXAGE:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_MAXMOVE:         (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_AGE:             (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_STRMOD:  (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_INT:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_STR:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_DEX:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_NBRATTACKS:      (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_LIMITED_WIS:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_GENDER:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_HITBONUS:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_MORALE:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_WIS:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_DEX:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_ENC:             (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_MAGICRESIST:     (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_SIZE:            (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_UNDEAD:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_STATUS:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_INT:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_NBRHITDICE:      (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_Lvl:             (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_MAXENC:          (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_CHA:        (uniqueID   or   combat order)
      case SUBOP_GET_CHAR_PERM_CON:        (uniqueID   or   combat order)
      case SUBOP_SET_CHAR_SIZE:            (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_ALIGNMENT:       (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_UNDEAD:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_STATUS:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_CHA:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_GENDER:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MAXENC:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_WIS:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_CON:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_DEX:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_STR:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_INT:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_PERM_STRMOD:     (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_AGE:             (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MAXMOVE:         (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MAXAGE:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_Exp:             (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_ICON_INDEX:      (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MAXHITPOINTS:    (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_HITPOINTS:       (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_RDYTOTRAIN:      (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_THAC0:           (UniqueID   or  combat order)
      case SUBOP_RUN_CHAR_SCRIPTS:         (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_AC:              (UniqueID   or  combat order)
      case SUBOP_MODIFY_CHAR_ATTRIBUTE:    (UniqueID   or  combat order)
      case SUBOP_REMOVE_CHAR_MODIFICATION: (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_HITBONUS:        (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_ASL:             (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MAGICRESIST:     (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_MORALE:          (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_DAMAGEBONUS:     (UniqueID   or  combat order)
      case SUBOP_GET_CHAR_MAXHITPOINTS:    (UniqueID   or  combat order)
      case SUBOP_SET_CHAR_Lvl:             (UniqueID   or  combat order)
      case SUBOP_IndexOf:                  (uniqueID)
      */
// As you can see, almost all functions use the uniqueID of the character
// rather than the party index.  I decided that the few exceptions should be 
// treated as 'bugs'.  I attempted to fix these bugs on 13 JUly 2018.   PRS

//
    if (m_Integer1 == -2)
    {
      CHARACTER *pChar;
      pChar = pScriptContext->GetCreatedCharacterContext(msg);
      if (pChar == NULL)
      {
        *pActor = NULL_ACTOR;
      }
      else
      {
        pChar->GetContext(pActor);
      };
    }
    else if (m_Integer1 < MAX_PARTY_MEMBERS)
    {
      for (int i=0;i<GetPartySize(party);i++)
      {
        if (party.characters[i].uniquePartyID == m_Integer1)
        {
          partyIndex=i;
          break;
        }
      };
      if (partyIndex < 0)
      {
        *pActor = NULL_ACTOR;
      }
      else
      {
        party.characters[partyIndex].GetContext(pActor);
      }
    }
    else
    {
      CHARACTER *pChar;
      //pChar = globalData.charData.GetCharacterData(m_Integer1);
      /* Really */ NotImplemented(0xaff5, false); pChar=NULL; // pChar = globalData.charData.GetCharacterData(m_Integer1);
      if (pChar == NULL)
      {
        *pActor = NULL_ACTOR;
      }
      else
      {
        pChar->GetContext(pActor);
      };
    };
    if (*pActor == NULL_ACTOR)
    {
      CString errmsg;
      errmsg.Format("Illegal party member %i in m_getCharacterActor\n%s",
                    m_Integer1, msg);
      errmsg += "\n";
      errmsg += GetActorDebugText(*pActor);
      
      m_interpretError(errmsg);
      //WriteStackTrace();
      m_string1 = m_false;
      m_pushString1(); // Have to supply a result.
    };
#else
    *pActor=NULL_ACTOR;
#endif    
  }
}

/*
void GPDL::m_setCharacterValueWithActor(int keyindex, ActorType actor, const CString& value)
{
  // We need to convert keyindex to a IF_KEYWORD_DATA pointer
  IF_KEYWORD_DATA *pIfKeyData = LocateKeywordData(keyindex);
  SetDataFromCString(pIfKeyData, value, &actor);
  m_pushSP(value);
}
*/
/*
void GPDL::m_getPartyValue(int keyindex)
{
  IF_KEYWORD_DATA *pIfKeyData;
  // We need to convert keyindex to a IF_KEYWORD_DATA pointer
  pIfKeyData = LocateKeywordData(keyindex);
  m_string1 = GetDataAsCString(pIfKeyData);
  m_pushString1();
}
*/
/*
void GPDL::m_setPartyValue(int keyindex)
{
  IF_KEYWORD_DATA *pIfKeyData;
  m_popString1();  // value
  pIfKeyData = LocateKeywordData(keyindex);
  SetDataFromCString(pIfKeyData, m_string1);
  m_string1 = m_false;
  m_pushString1(); // Have to supply a result;
}
*/
/*
void GPDL::m_getGameValue(int keyindex)
{
  m_getPartyValue(keyindex); //Works the same
}
*/
/*
void GPDL::m_setGameValue(int keyindex)
{
  m_setPartyValue(keyindex); //Works the same
}
*/
void GPDL::SA_Param()
{
  const CStringPAIR *pSAC;
  pSAC = pScriptContext->GetAbility();
  if (pSAC != NULL)
  {
    m_string2 = pSAC->Value();
  }
  else
  {
    m_string2 = NO_SUCH_SA;
  };
  m_pushString2();
}

void GPDL::SA_Name()
{
  const CStringPAIR *pSAC;
  pSAC = pScriptContext->GetAbility();
  if (pSAC != NULL)
  {
    m_string2 = pSAC->Key();
  }
  else
  {
    m_string2 = NO_SUCH_SA;
  };
  m_pushString2();
}


void GPDL::SA_Param(const SPECIAL_ABILITIES *pSA)
{
  static bool error = false;
  m_popString1();
  if ( (pSA == NULL) && !error)
  {
    WriteDebugString ("Attempt to GET Special Ability parameter \"%s\" from NULL SA List\n", m_string1);
    error = true;
  };
  CStringPAIR *pAb;
  if (    (pSA == NULL)
       || ((pAb = pSA->FindAbility(m_string1)) == NULL)
     )
  {
    m_string2 = NO_SUCH_SA;
  }
  else
  {
    m_string2 = pAb->Value();
  };
  m_pushString2();
};


void GPDL::SA_SetParam(SPECIAL_ABILITIES *pSA)
{
  static bool error = false;
  m_popString2();     // Value of parameter
  m_popString1();     // Name of Special Ability
  if (pSA != NULL)
  {
    pSA->InsertAbility(m_string1, m_string2, emptyString, "Script"); 
  }
  else
  {
    if (!error)
    {
      WriteDebugString("Attempt to set SpecialAbility parameter \"%s=%s\" in NULL SA list\n", m_string1, m_string2);
      error = true;
    };
  };
  m_pushString2();
};


void GPDL::SA_Delete(SPECIAL_ABILITIES *pSA)
{
  static bool error = false;
  m_popString1();
  if (pSA != NULL)
  {
    m_string2 = pSA->DeleteAbility(m_string1);
  }
  else
  {
    if (!error)
    {
      WriteDebugString("Attempt to delete Special Ability \"%s\" from NULL SA List\n", m_string1);
      error = true;
    };
  };
  m_pushString2();
};

CHARACTER *GPDL::Dude(const char *msg){
  m_popInteger1();
  ActorType actor;
  m_GetCharActor(&actor, msg);
  /*
  // We need to convert keyindex to a IF_KEYWORD_DATA pointer
  pIfKeyData = LocateKeywordData(keyindex);
  m_string1 = GetDataAsCString(pIfKeyData, &actor);
  m_pushString1();
  */
  return GetCurrentlyActiveContext(&actor, msg);
};

void GPDL::m_GetLiteralInt(int v)
{
  m_Integer1 = v;
  m_pushInteger1();
}

void GPDL::m_SetLiteralInt(int *v)
{
  m_popInteger1();
  *v = m_Integer1;
  this->m_pushEmptyString();
}

void GPDL::m_GetCharInt(genderType (CHARACTER::*f)(DWORD) const, const char *msg)
{
  /* Really */ NotImplemented(0x4c9, false);
}

void GPDL::m_GetCharInt(int (CHARACTER::*f)(DWORD) const, const char *msg)
{
  m_Integer1 = (Dude(msg)->*f)(DEFAULT_SPELL_EFFECT_FLAGS);
  m_pushInteger1();
}

void GPDL::m_GetCharInt(int (CHARACTER::*f)() const, const char *msg)
{
  //IF_KEYWORD_DATA *pIfKeyData;
  m_Integer1 = (Dude(msg)->*f)();
  m_pushInteger1();
}

void GPDL::m_GetCharInt(int (CHARACTER::*f)(const ITEM_ID&, int, DWORD) const, const char *msg)
{
  //IF_KEYWORD_DATA *pIfKeyData;
  ITEM_ID itemid;
  m_Integer1 = (Dude(msg)->*f)(itemid, 0, DEFAULT_SPELL_EFFECT_FLAGS);
  m_pushInteger1();
}

void GPDL::m_GetCharInt(BYTE (CHARACTER::*f)(DWORD) const, const char *msg)
{
  //IF_KEYWORD_DATA *pIfKeyData;
  m_Integer1 = (Dude(msg)->*f)(DEFAULT_SPELL_EFFECT_FLAGS);
  m_pushInteger1();
}

void GPDL::m_GetCharInt(BYTE (CHARACTER::*f)() const, const char *msg)
{
  //IF_KEYWORD_DATA *pIfKeyData;
  m_Integer1 = (Dude(msg)->*f)();
  m_pushInteger1();
}

void GPDL::m_GetCharString(const CString& (CHARACTER::*f)() const, const char *msg)
{
  //IF_KEYWORD_DATA *pIfKeyData;
  m_string2 = (Dude(msg)->*f)();
  m_pushString2();
}


void GPDL::m_GetActorBOOL(BOOL (CHARACTER::*f)() const, const char *msg)
{
  CHARACTER *pDude;
  m_popString1(); // ActorType
  ActorType actor = m_StringToActor((LPCSTR)m_string1);
  pDude = GetCurrentlyActiveContext(&actor, msg);
  m_Integer1 = (int)(pDude->*f)();
  m_pushInteger1();
}

void GPDL::m_SetActorBOOL(void (CHARACTER::*f)(BOOL v), const char *msg)
{
  CHARACTER *pDude;
  m_popInteger1();  // Value
  m_popString1();   // ActorType
  ActorType actor = m_StringToActor((LPCSTR)m_string1);
  pDude = GetCurrentlyActiveContext(&actor, msg);
  (pDude->*f)(m_Integer1 != 0);
  m_pushInteger1();
}

void GPDL::m_SetCharInt(void (CHARACTER::*f)(int), const char *msg)
{
  m_popInteger1();
  (Dude(msg)->*f)(m_Integer1);
  m_pushEmptyString();
}

void GPDL::m_SetCharBOOL(void (CHARACTER::*f)(BOOL), const char *msg)
{
  m_popInteger1();
  (Dude(msg)->*f)(m_Integer1 != 0);
  m_pushEmptyString();
}

void GPDL::m_SetCharString(void (CHARACTER::*f)(const CString&), const char *msg)
{
  m_popString1();
  (Dude(msg)->*f)(m_string1);
  m_pushEmptyString();
}

void GPDL::m_SetMemorizeCount(void)
{
#ifdef UAFEngine
  m_popString1();  // adjustment
  m_popString3();  // spellname
  CHARACTER *pChar;
  int i, n;
  pChar = Dude("$SetMemorizeCount()");
  n = pChar->GetSpellCount();
  for (i=0; i<n; i++)
  {
    CHARACTER_SPELL *pCharSpell;
    pCharSpell = pChar->GetCharacterSpell(i);
    if (m_string3 == pCharSpell->spellID)
    {
      int v;
      if (m_string1.IsEmpty())
      {
        m_Integer1 = pCharSpell->memorized;
        m_pushInteger1();
        return;
      };
      sscanf(m_string1,"%d", &v);
      if ((m_string1[0] == '-') || (m_string1[0] == '+'))
      {
        pCharSpell->memorized += v;
      }
      else
      {
        pCharSpell->memorized = v;
      };
      if ( pCharSpell->memorized < 0)
      {
        pCharSpell->memorized = 0;
      };
      m_Integer1 = pCharSpell->memorized;
      m_pushInteger1();
      return;
    };
  }
  m_Integer1 = -1;
  m_pushInteger1();
  return;
#else
          m_popString1();
          m_popString2();
          m_popString3();
          m_string2 = "OK";
          m_pushString2();
#endif
}

void GPDL::m_GetHighestLevelBaseclass(void)
{
#ifdef UAFEngine
   CHARACTER *pChar;
   pChar = Dude("$GET_HIGHEST_LEVEL_BASECLASS()");
   m_string2 = pChar->GetHighestLevelBaseclass();
   m_pushString2();
#else
   m_string2 = "OK";
   m_pushString2();
#endif
  return;
}


void GPDL::m_GetBaseclassLevel(void)
{
#ifdef UAFEngine
  CHARACTER *pChar;
  BASECLASS_ID baseclassID;
  m_popString1();
  pChar = Dude("$GetBaseclassLevel");
  baseclassID = m_string1;
  m_Integer2 = pChar->GetBaseclassLevel(baseclassID);
  m_pushInteger2();
#else
  m_popString1();
#endif
};














void GPDL::m_SetCharInt(void (CHARACTER::*f)(int,int,bool), const char *msg)
{
   //pDude->SetHitPoints(data,0,pActor->m_canFinishCasting);
  /* Really */ NotImplemented(0xab6, false);
}

#define GET_CHAR_INT(f,m) m_GetCharInt(&CHARACTER::f,m);
#define SET_CHAR_INT(f,m) m_SetCharInt(&CHARACTER::f,m);
#define SET_CHAR_BOOL(f,m) m_SetCharBOOL(&CHARACTER::f,m);
#define GET_LITERAL_INT(v) m_GetLiteralInt(v);
#define SET_LITERAL_INT(v) m_SetLiteralInt(&v);

#define GET_ACTOR_BOOL(f,m) m_GetActorBOOL(&CHARACTER::f,m);
#define SET_ACTOR_BOOL(f,m) m_SetActorBOOL(&CHARACTER::f,m);

#define GET_CHAR_STRING(f,m) {m_string1=Dude(m)->f();m_pushString1();};
#define GET_LITERAL_STRING(s) m_GetLiteralString(s);
#define SET_CHAR_STRING(f,m) m_SetCharString(&CHARACTER::f,m);

#define GET_CHAR_ENUM(f,m) {m_Integer1=Dude(m)->f();m_pushInteger1();};
#define SET_CHAR_ENUM(f,t,m) {m_popInteger1(); Dude(m)->f((t)m_Integer1);};

#define GET_CHAR_FLOAT(f,m) {m_string1.Format("%1.8f", Dude(m)->f()); m_pushString1();};


GPDL_STATE GPDL::m_interpret(void) {
  unsigned int i;
  m_interpStatus=GPDL_OK;
  while (m_interpStatus==GPDL_OK) {
    m_interpretCount++;
    if(m_interpretCount > 1000000)
    {
      m_interpStatus = GPDL_EXCESSCPU;
      return m_interpStatus;
    };
    m_executionAddress = m_PC; // for error printout
    // 19 Apr PRS m_bincode=m_code.peek(m_PC++);
    m_bincode=Peek(m_PC++);
    m_opcode=(m_bincode>>24)&0xff;
    m_subop=m_bincode&0xffffff; // Also might be an address
    switch(m_opcode) {
    case BINOP_LOCALS:
      for (i=0; i<m_subop; i++)
        m_pushSP(m_false);
      break;
    case BINOP_JUMP:
      m_PC=m_subop;
      break;
    case BINOP_ReferenceGLOBAL:
      {
        int index;
        index = m_subop;
        if (index & 0x800000)
        {
          m_globals.Set(index & 0x7fffff, m_popSP());
        }
        else
        {
          m_pushSP(m_globals.peek(index));
        };
      };
      break;
    case BINOP_FETCHTEXT:
      m_string1 = ((char *)m_program) + (m_bincode & 0xffffff);
      m_pushString1();
      break;
    case BINOP_CALL:
      m_pushPC();
      m_pushFP();
      m_FP=m_SP;
      m_PC=m_subop;
      if (m_debugLevel&DBG_functionTrace) {
        int openParenLoc, LSP;
        unsigned int numPar;
        m_string1=m_globals.peek(Peek(m_PC)&0xffffff);
        openParenLoc=m_string1.Find('(');
        if (openParenLoc<0) {
          m_errorLog.log("Function has illegal entry");
          INTERP_RETURN(GPDL_ILLFUNC);
        };
        m_string2.Format("0x%06x call ",m_returnStack[m_RP+1]-1);
        m_string2+=m_string1.Left(openParenLoc+1);
        numPar=0xffffff;
        sscanf((LPCTSTR)m_string1+openParenLoc+1,"%d",&numPar);
        if (numPar>10) numPar=10;
        LSP=m_SP;
        for (unsigned int j=0; j<numPar; LSP++,j++) 
        {
          m_string1=m_dataStack[LSP];
          if (m_string1.GetLength()>20) m_string1=m_string1.Left(20);
          m_string2+=CString("\"") + m_string1 + CString("\"");
          if (j!=numPar-1) m_string2+=',';
        };
        m_string2+=CString(")");
        m_errorLog.log(m_string2);
      };
      m_PC++; // skip over entry information.
      break;
    case BINOP_FETCH_FP:
      m_subop -= (m_subop & 0x800000)<<1;
      m_pushSP(m_dataStack[m_FP+m_subop]);
      break;
    case BINOP_JUMPFALSE:
      m_popString1();
      if ((m_string1==m_false)||(m_string1=="0")) m_PC=m_subop;
      break;
    case BINOP_RETURN:
      m_SP=m_FP;
      m_popFP();
      m_popPC();
      for (;m_subop&0xfff; m_subop--) m_popString1();
        // Leave the parameters behind.
      if (m_PC==0xffffffff) {
        m_state=GPDL_IDLE;
        m_discourseText.Clear();
        INTERP_RETURN(GPDL_IDLE);
      };
      if (m_debugLevel&DBG_functionTrace) {
        m_string1="          return ";
        m_string1+=CString("\"")+m_dataStack[m_SP]+CString("\"");
        m_errorLog.log(m_string1);
      };
      break;
    case BINOP_STORE_FP:
      m_subop -= (m_subop & 0x800000)<<1; 
      m_dataStack[m_FP+m_subop]=m_popSP();
      break;
    case BINOP_SUBOP:
      switch(m_subop) {
      case SUBOP_AddCombatant:
        m_popInteger1();  // Friendly flag
        m_popString1();  // monster name
#ifdef UAFEngine
        combatData.AddMonsterToCombatants(m_string1, m_Integer1);
#endif
        m_pushSP(m_false);
        break;
      case SUBOP_ToHitComputation_Roll:
        m_Integer1 = 10;
#ifdef UAFEngine
        if (pScriptContext->pToHitComputationContext != NULL)
        {
          m_Integer1 = pScriptContext->pToHitComputationContext->Rolled();
        }
        else
        {
          if (!debugStrings.AlreadyNoted(CString("SRNTHC")))
          {
            writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
            debugSeverity = 5;
            WriteDebugString("Script Referenced Non-existent ToHitContext\n");
          };
        };
#endif
        m_pushInteger1();
        break;
      case SUBOP_Alignment:
        m_popString1();
        m_string2 = m_Alignment(m_string1);
        m_pushString2();
        break;
      case SUBOP_AlignmentGood:
        m_popString1();
        m_string2 = m_AlignmentGood(m_string1);
        m_pushString2();
        break;
      case SUBOP_AlignmentEvil:
        m_popString1();
        m_string2 = m_AlignmentEvil(m_string1);
        m_pushString2();
        break;
      case SUBOP_AlignmentLawful:
        m_popString1();
        m_string2 = m_AlignmentLawful(m_string1);
        m_pushString2();
        break;
      case SUBOP_AlignmentNeutral:
        m_popString1();
        m_string2 = m_AlignmentNeutral(m_string1);
        m_pushString2();
        break;
      case SUBOP_AlignmentChaotic:
        m_popString1();
        m_string2 = m_AlignmentChaotic(m_string1);
        m_pushString2();
        break;
      case SUBOP_GET_CHARACTER_SA:
        {
          m_popString2();  // SA name
#ifdef UAFEngine
          CHARACTER *pCharacter = m_popCharacterActor(); 
          if (pCharacter != NULL)
          {
            m_string3 = pCharacter->specAbs.GetString(m_string2);
          };
#endif
          m_pushString3();
        };
        break;
      case SUBOP_SET_CHARACTER_SA:
        {
          m_popString1();  // Parameter
          m_popString2();  // name
         // m_popInteger1(); // Index
#ifdef UAFEngine
          CHARACTER *pDude = m_popCharacterActor();
          if (pDude != NULL)
          {
            pDude->specAbs.InsertAbility(m_string2, m_string1, "Script Add SA to char ", pDude->GetName());
          };
#else
          m_popString3();
#endif
        };
        m_pushString1();
        break;
      case SUBOP_DELETE_CHARACTER_SA:
        {
          m_popString2();  // SA name
#ifdef UAFEngine
          CHARACTER *pCharacter = m_popCharacterActor(); 
          if (pCharacter != NULL)
          {
            m_string3 = pCharacter->specAbs.DeleteAbility(m_string2);
          };
#endif
          m_pushString3();
        };
        break;
      case SUBOP_GET_COMBATANT_SA:
        {
          m_popString2();  // name
#ifdef UAFEngine
          COMBATANT *pCombatant = m_popCombatantActor(); // Index
          if (pCombatant != NULL)
          {
            m_string3 = pCombatant->combatantSA.GetString(m_string2);
          };
#endif
          m_pushString3();
        };
        break;
      case SUBOP_SET_COMBATANT_SA:
        {
          m_popString1(); // Parameter
          m_popString2(); // name
#ifdef UAFEngine
          COMBATANT *pCombatant = m_popCombatantActor();
          if ((pCombatant != NULL) && (pCombatant != &NULL_COMBATANT))
          {
            if (IsCombatActive())
            {
              pCombatant->combatantSA.InsertAbility(m_string2, m_string1, "Script Add SA to combatant ", pCombatant->GetName());
            }
            else
            {
              MsgBoxError("$SET_COMBATANT_SA() called when combat not active","Error",2);
            };
          }
#else
          m_popString3();
#endif
        };
        m_pushString2();
        break;
      case SUBOP_DELETE_COMBATANT_SA:
        {
          m_popString2();  // name
#ifdef UAFEngine
          COMBATANT *pCombatant = m_popCombatantActor(); // Index
          if (pCombatant != NULL)
          {
            m_string3 = pCombatant->combatantSA.DeleteAbility(m_string2);
          };
#endif
          m_pushString3();
        };
        break;
      case SUBOP_DUMP_CHARACTER_SAS:
        {
#ifdef UAFEngine
          CHARACTER *pCharacter = m_popCharacterActor(); 
          if (pCharacter != NULL)
          {
            POSITION pos;
            pos = pCharacter->specAbs.GetHeadPosition();




            while (pos != NULL)
            {
              const CStringPAIR *pSpecAb;
              CString line;
              pSpecAb = pCharacter->specAbs.GetNextData(pos);
              line.Format(" Character Special Ability = %s; value = %s",
                                pSpecAb->Key(), pSpecAb->Value());
              m_errorLog.log(line);
            };
          };





#endif
          m_string3="Dump of Character SAs";
          m_pushString3();
        };
        break;
      case SUBOP_DelimitedStringCount:
        {
          char delimiter;
          m_Integer1 = 0;
          m_popString1();
          if (!m_string1.IsEmpty())
          {
            int col;
            int len;
            len = m_string1.GetLength();
            delimiter = m_string1[0];
            col = 1;
            while ((col>0) && (col<len))
            {
              col = m_string1.Find(delimiter, col);
              m_Integer1++;
              col++;
            };
          };
          m_pushInteger1();
        };
        break;
      case SUBOP_DelimitedStringSubstring:
        {
          int start, len;
          char delimiter;
          m_string2 = emptyString;
          m_popInteger1();
          m_popString1();
          len = m_string1.GetLength();
          start = 0;
          if ((len > 0) && (m_Integer1 >= 0))
          {
            delimiter = m_string1[0];
            while ((start>=0) && (start<len))
            {
              if (m_Integer1 == 0)
              {
                int end;
                end = m_string1.Find(delimiter,start+1);
                if (end < 0) end = len;
                m_string2 = m_string1.Mid(start+1, end-start-1);
                break;
              };
              m_Integer1--;
              start = m_string1.Find(delimiter, start+1);
            };
          };
        };
        m_pushString2();
        break;
      case SUBOP_DelimitedStringHead:
        {
          int len;
          char delimiter;
          m_string2 = emptyString;
          m_popString1();
          len = m_string1.GetLength();
          if (len > 0)
          {
            delimiter = m_string1[0];
            {
              int end;
              end = m_string1.Find(delimiter,1);
              if (end < 0) end = len;
              m_string2 = m_string1.Mid(1, end-1);
            };
          };
        };
        m_pushString2();
        break;
      case SUBOP_DelimitedStringTail:
        {
          int len;
          char delimiter;
          m_string2 = emptyString;
          m_popString1();
          len = m_string1.GetLength();
          if (len > 0)
          {
            delimiter = m_string1[0];
            {
              int end;
              end = m_string1.Find(delimiter,1);
              if (end < 0) end = len;
              m_string2 = m_string1.Right(len-end);
            };
         };
        };
        m_pushString2();
        break;
      case SUBOP_DelimitedStringFilter:
        {
          m_popString3();  // Filter NAme
          m_popString2();  // Filter string
          m_popString1();  // source string
#ifdef UAFEngine
          m_string4 = DelimitedStringFilter(m_string1, m_string2, m_string3);
#else
          m_string4.Empty();
#endif
          m_pushString4();
        };
        break;
      case SUBOP_DelimitedStringAdd:
        {
          m_popString3();  // Possible Delimiter
          m_popString2(); // New head
          m_popString1(); // original string
          if (m_string1.IsEmpty())
          {
            if (m_string3.IsEmpty())
            {
              m_string4 = '#';
            }
            else
            {
              m_string4 = m_string3[0];
            };
          }
          else
          {
            m_string4 = m_string1[0];
          }
          m_string4 += m_string2;
          m_string4 += m_string1;
          m_pushString4();
        };
        break;
      case SUBOP_GET_ITEM_SA:
        {
          ITEM_DATA *pItemData;
          m_popString2(); // SA name
          m_popString1(); // item name
          ITEM_ID itemID;
          itemID = m_string1;
          pItemData = itemData.GetItem(itemID);
          if (pItemData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pItemData->specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_SET_ITEM_SA:
      //  {
      //    ITEM_DATA *pItemData;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // item name
      //    ITEM_ID itemID;
      //    itemID = m_string1;
      //    pItemData = itemData.GetItem(itemID);
      //    if (pItemData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pItemData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_ITEM_SA:
      //  {
      //    ITEM_DATA *pItemData;
      //    m_popString2(); // SA name
      //    m_popString1(); // item name
      //    ITEM_ID itemID;
      //    itemID = m_string1;
      //    pItemData = itemData.GetItem(itemID);
      //    if (pItemData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pItemData->specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      case SUBOP_GET_SPELL_SA:
        {
          SPELL_DATA *pSpellData;
          m_popString2(); // SA name
          m_popString1(); // spell name
          SPELL_ID spellID;
          spellID = m_string1;
          pSpellData = spellData.GetSpell(spellID);
          if (pSpellData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pSpellData->specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
/*  Spell SAs are read-only
      case SUBOP_SET_SPELL_SA:
        {
          SPELL_DATA *pSpellData;
          m_popString3(); // SA value
          m_popString2(); // SA name
          m_popString1(); // spell name
          SPELL_ID spellID;
          spellID = m_string1;
          pSpellData = spellData.GetSpell(spellID);
          if (pSpellData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            pSpellData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
          };
          m_pushString1();
        };
        break;
      case SUBOP_DELETE_SPELL_SA:
        {
          SPELL_DATA *pSpellData;
          m_popString2(); // SA name
          m_popString1(); // spell name
          SPELL_ID spellID;
          spellID = m_string1;
          pSpellData = spellData.GetSpell(spellID);
          if (pSpellData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            m_string1 = pSpellData->specAbs.DeleteAbility(m_string2);
          };
          m_pushString1();
        };
        break;

 */ 
 /*
    case SUBOP_GET_SPELLGROUP_SA:
        {
          const SPELLGROUP_DATA *pSpellgroupData;
          SPELLGROUP_ID spellgroupID;
          m_popString2(); // SA name
          m_popString1(); // spellgroup name
          //pSpellgroupData = spellgroupData.GetSpellgroupData(m_string1);
          spellgroupID = m_string1;
          pSpellgroupData = spellgroupData.PeekSpellgroup(spellgroupID);
          if (pSpellgroupData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pSpellgroupData->m_specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_SET_SPELLGROUP_SA:
      //  {
      //    SPELLGROUP_DATA *pSpellgroupData;
      //    SPELLGROUP_ID spellgroupID;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // spellgroup name
      //    spellgroupID = m_string1;
      //    //pSpellgroupData = spellgroupData.GetSpellgroupData(m_string1);
      //    pSpellgroupData = spellgroupData.GetSpellgroup(spellgroupID);
      //    if (pSpellgroupData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pSpellgroupData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_SPELLGROUP_SA:
      //  {
      //    SPELLGROUP_DATA *pSpellgroupData;
      //    SPELLGROUP_ID spellgroupID;
      //    m_popString2(); // SA name
      //    m_popString1(); // spellgroup name
      //    spellgroupID = m_string1;
      //    pSpellgroupData = spellgroupData.GetSpellgroup(spellgroupID);
      //    if (pSpellgroupData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pSpellgroupData->m_specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
*/
      case SUBOP_GET_MONSTERTYPE_SA:
        {
          MONSTER_DATA *pMonsterData;
          MONSTER_ID monsterID;
          m_popString2(); // SA name
          m_popString1(); // monster name
          monsterID = m_string1;
          pMonsterData = monsterData.GetMonster(monsterID);
          if (pMonsterData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pMonsterData->specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_SET_MONSTERTYPE_SA:
      //  {
      //    MONSTER_DATA *pMonsterData;
      //    MONSTER_ID monsterID;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // monster name
      //    monsterID = m_string1;
      //    pMonsterData = monsterData.GetMonster(monsterID);
      //    if (pMonsterData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pMonsterData->specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_MONSTERTYPE_SA:
      //  {
      //    MONSTER_DATA *pMonsterData;
      //    MONSTER_ID monsterID;
      //    m_popString2(); // SA name
      //    m_popString1(); // monster name
      //    monsterID = m_string1;
      //    pMonsterData = monsterData.GetMonster(monsterID);
      //    if (pMonsterData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pMonsterData->specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      case SUBOP_GET_RACE_SA:
        {
          RACE_DATA *pRaceData;
          RACE_ID raceID;
          m_popString2(); // SA name
          m_popString1(); // race name
          raceID = m_string1;
          pRaceData = raceData.GetRace(raceID);
          if (pRaceData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pRaceData->m_specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_DELETE_RACE_SA:
      //  {
      //    RACE_DATA *pRaceData;
      //    RACE_ID raceID;
      //    m_popString2(); // SA name
      //    m_popString1(); // race name
      //    //pRaceData = raceData.GetRaceData(m_string1);
      //    raceID = m_string1;
      //    pRaceData = raceData.GetRace(raceID);
      //    if (pRaceData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pRaceData->m_specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      case SUBOP_GET_CLASS_SA:
        {
          CLASS_DATA *pClassData;
          CLASS_ID classID;
          m_popString2(); // SA name
          m_popString1(); // class name
          //pClassData = classData.GetClassData(m_string1);
          classID = m_string1;
          pClassData = classData.GetClass(classID);
          if (pClassData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pClassData->m_specialAbilities.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      case SUBOP_GET_BASECLASS_SA:
        {
          BASE_CLASS_DATA *pBaseclassData;
          BASECLASS_ID baseclassID;
          m_popString2(); // SA name
          m_popString1(); // baseclass name
          //pClassData = classData.GetClassData(m_string1);
          baseclassID = m_string1;
          pBaseclassData = baseclassData.GetBaseclass(baseclassID);
          if (pBaseclassData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pBaseclassData->m_specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_SET_CLASS_SA:
      //  {
      //    CLASS_DATA *pClassData;
      //    CLASS_ID classID;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // class name
      //    classID = m_string1;
      //    //pClassData = classData.GetClassData(m_string1);
      //    pClassData = classData.GetClass(classID);
      //    if (pClassData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pClassData->m_specialAbilities.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_CLASS_SA:
      //  {
      //    CLASS_DATA *pClassData;
      //    CLASS_ID classID;
      //    m_popString2(); // SA name
      //    m_popString1(); // class name
      //    classID = m_string1;
      //    //pClassData = classData.GetClassData(m_string1);
      //    pClassData = classData.GetClass(classID);
      //    if (pClassData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pClassData->m_specialAbilities.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;

#ifdef USE_TRAITS
      //case SUBOP_GET_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //    pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      CStringPAIR *csp;
      //      csp = pTraitData->m_specAbs.FindAbility(m_string2);
      //      if (csp == NULL) m_string1 = NO_SUCH_SA;
      //      else m_string1 = csp->Value();
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_SET_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //    pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pTraitData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_TRAIT_SA:
      //  {
      //    TRAIT_DATA *pTraitData;
      //    m_popString2(); // SA name
      //    m_popString1(); // trait name
      //   pTraitData = traitData.GetTraitData(m_string1);
      //    if (pTraitData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pTraitData->m_specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
#endif

      case SUBOP_GET_ABILITY_SA:
        {
          const ABILITY_DATA *pAbilityData;
          m_popString2(); // SA name
          m_popString1(); // ability name
          pAbilityData = abilityData.PeekAbilityData(abilityData.LocateAbilityData(m_string1));
          if (pAbilityData == NULL)
          {
            m_string1 = NO_SUCH_SA;
          }
          else
          {
            CStringPAIR *csp;
            csp = pAbilityData->m_specAbs.FindAbility(m_string2);
            if (csp == NULL) m_string1 = NO_SUCH_SA;
            else m_string1 = csp->Value();
          };
          m_pushString1();
        };
        break;
      //case SUBOP_SET_ABILITY_SA:
      //  {
      //    ABILITY_DATA *pAbilityData;
      //    m_popString3(); // SA value
      //    m_popString2(); // SA name
      //    m_popString1(); // ability name
      //    pAbilityData = abilityData.GetAbilityData(abilityData.LocateAbilityData(m_string1));
      //    if (pAbilityData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      pAbilityData->m_specAbs.InsertAbility(m_string2, m_string3, m_string1, m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      //case SUBOP_DELETE_ABILITY_SA:
      //  {
      //    ABILITY_DATA *pAbilityData;
      //    m_popString2(); // SA name
      //    m_popString1(); // ability name
      //    pAbilityData = abilityData.GetAbilityData(abilityData.LocateAbilityData(m_string1));
      //    if (pAbilityData == NULL)
      //    {
      //      m_string1 = NO_SUCH_SA;
      //    }
      //    else
      //    {
      //      m_string1 = pAbilityData->m_specAbs.DeleteAbility(m_string2);
      //    };
      //    m_pushString1();
      //  };
      //  break;
      case SUBOP_GET_HOOK_PARAM:
        m_popInteger1();
        m_string1 = emptyString;
        if ( m_Integer1 < NUMHOOKPARAM)
        {
          m_string1 = (*p_hook_parameters)[m_Integer1];
        };
        m_pushString1();
        break;
      case SUBOP_SET_HOOK_PARAM:
        m_popString1();  // The new value
        m_popInteger1(); // the index
        m_string2 = emptyString;
        if ( (m_Integer1 < NUMHOOKPARAM) && (m_Integer1 >= 0))
        {
          m_string2 = (*p_hook_parameters)[m_Integer1];
          (*p_hook_parameters)[m_Integer1] = m_string1;
        };
        m_pushString2();
        break;
      case SUBOP_SA_NAME:
#ifdef UAFEngine
        SA_Name();
#else
        m_string1.Empty();
        m_pushString1();
#endif
        break;
      case SUBOP_SA_SOURCE_TYPE:
#ifdef UAFEngine
        m_string1 = pScriptContext->GetSourceTypeName();
#else
        m_string1.Empty();
#endif
        m_pushString1();
        break;
      case SUBOP_SA_SOURCE_NAME:
#ifdef UAFEngine
        m_string1 = pScriptContext->GetSourceName();
#else
        m_string1.Empty();
#endif
        m_pushString1();
        break;
      case SUBOP_SA_PARAM_GET:
        SA_Param();
        break;
      case SUBOP_SA_PARAM_SET:
        m_popString1();  // parameter value
        pScriptContext->SetParam(m_string1);
        m_pushString1();
        break;
      case SUBOP_SA_REMOVE:
#ifdef UAFEngine
        m_string1 = pScriptContext->RemoveAbility();
#endif
        m_pushString1();
        break;
      case SUBOP_SA_ITEM_GET:
        SA_Param(pScriptContext->GetItemSAContext( "$SA_ITEM_GET()"));
        break;
      case SUBOP_SA_CHARACTER_GET:
        SA_Param(pScriptContext->GetCharacterSAContext("$SA_CHARACTER_GET()"));
        break;
      case SUBOP_SA_COMBATANT_GET:
#ifdef UAFEngine
        SA_Param(pScriptContext->GetCombatantSAContext("$SA_COMBATANT_GET()"));
#endif
        break;
      case SUBOP_SA_CLASS_GET:
        SA_Param(pScriptContext->GetClassSAContext("$SA_CLASS_GET()"));
        break;
      case SUBOP_SA_BASECLASS_GET:
        SA_Param(pScriptContext->GetBaseclassSAContext("$SA_BASECLASS_GET()"));
        break;
      case SUBOP_SA_SPELL_GET:
        SA_Param(pScriptContext->GetSpellSAContext("$SA_SPELL_GET()"));
        break;
      case SUBOP_SA_MONSTERTYPE_GET:
        SA_Param(pScriptContext->GetMonstertypeSAContext("$SA_MONSTERTYPE_GET()"));
        break;
      case SUBOP_SA_RACE_GET:
        SA_Param(pScriptContext->GetRaceSAContext("$SA_RACE_GET()"));
        break;
      case SUBOP_SA_ABILITY_GET:
        SA_Param(pScriptContext->GetAbilitySAContext("$SA_ABILITY_GET()"));
        break;

#ifdef USE_TRAITS
      //case SUBOP_SA_TRAIT_GET:
      //  SA_Param(pScriptContext->GetTraitSAContext());
      //  break;
#endif


      
      
      
      case SUBOP_COINCOUNT:
        if (IsCombatActive())
        {
          m_interpretError("Combat active in $COINCOUNT");
          m_Integer3=0;
          m_pushInteger3();
          break;
        };
        /*  **** Replaced with code that uses character uniqueID instead of party index.
        m_popInteger1();   // The coin ordinal
        if (m_Integer1 < 1) m_Integer1 = 1;
        if (m_Integer1 > MONEY_DATA_TYPE::MAX_COIN_TYPES) m_Integer1 = 1;
        
        m_popInteger2();   // The character ordinal
        if ((m_Integer2 < 1) || (m_Integer2 > GetPartySize(party)))
        {
          // assume active char 
          m_Integer2 = party.activeCharacter+1;
        };   
        
        m_Integer3 = party.characters[m_Integer2-1].money.Coins[m_Integer1-1];
        m_pushInteger3();
        */
        {
          CHARACTER *pChar;
          m_popInteger1(); // Coin Ordinal
          if (m_Integer1 > MONEY_DATA_TYPE::MAX_COIN_TYPES) m_Integer1 = 1;
          pChar = Dude("$COINCOUNT()");
          m_Integer3 = pChar->money.Coins[m_Integer1-1];
          m_pushInteger3();
        };
        break;
      case SUBOP_COINNAME:
        m_popInteger1(); // The coin ordinal;
        m_string1 = globalData.moneyData.GetCoinName(m_Integer1-1);
        m_pushString1();
        break;
      case SUBOP_COINRATE:
        m_popInteger1(); // The coin ordinal;
        m_Integer2 = globalData.moneyData.GetCoinRate(m_Integer1-1);
        m_pushInteger2();
        break;
      case SUBOP_CURR_CHANGE_BY_VAL:
        {
          // GPDL doesn't handle floats, convert to integer
          double ir = GetIntermediateResult();
          int temp=0;
          if (ir < 0.0)
            temp = ceil(ir + -0.5); // round downo
          else
            temp = floor(ir + 0.5); // round up

          m_string1.Format("%i", temp);
          m_pushString1();
        }
        break;
      case SUBOP_DEBUG:
        m_popString1();
        m_string2.Format("$DEBUG(%s)",m_string1);
        m_errorLog.log(m_string2);
        m_pushString1();  // Our 'result'
        break;
      case SUBOP_DebugWrite:
        WriteDebugString(m_dataStack[m_SP] + '\n');
        break;
      case SUBOP_DEPRECATED_DELETE_LEVEL_STATS_ASL:
        m_popString1();  // key
        DeleteLevelASL(globalData,
                       GetCurrentLevel(),
                       m_string1);
//        globalData.levelInfo.stats[globalData.currLevel]
//          .level_attributes.Delete(m_string1);
        m_pushSP(m_false);  // Must supply a result
        break;
      case SUBOP_DELETE_LEVEL_STATS_ASL:
        m_popString1();  // key
        m_popString3();  //Level
        if (m_string3 == "") m_Integer1 = GetCurrentLevel();
        else m_Integer1 = atoi(m_string3);
        DeleteLevelASL(globalData,
                       m_Integer1,
                       m_string1);
//        globalData.levelInfo.stats[globalData.currLevel]
//          .level_attributes.Delete(m_string1);
        m_pushSP(m_false);  // Must supply a result
        break;
      case SUBOP_DELETE_PARTY_ASL:
        m_popString1();  // key
        DeletePartyASL(party,m_string1);
//        party.party_attributes.Delete(m_string1);
        m_pushSP(m_false
          );  // Must supply a result
        break;
      case SUBOP_DUP:
        m_popString1();
        m_pushString1();
        m_pushString1();
        break;
      case SUBOP_FALSE:
        m_pushSP(m_false);
        break;
      case SUBOP_FORCENUMERIC:
        m_popInteger1();
        m_pushInteger1();
        break;
      case SUBOP_ISEQUAL:
        m_popString1();
        m_popString2();
        m_pushSP(m_string1==m_string2?m_true:m_false);
        break;
      case SUBOP_Gender:
        m_popString1();
        ASSERT(m_string1.GetLength() == 24);
        m_string2 = m_Gender(m_string1);
        m_pushString2();
        break;
      case SUBOP_GET_CHAR_ASL:

        
        
        
        {
          CHARACTER *pChar;
          m_popString1();  // key
          pChar = m_popCharacter("$GET_CHAR_ASL()"); // character number
#ifdef UAFEngine
          m_string2 = LookupCharacterASL(
            pChar,
            m_string1);
#else
        m_string2 = "Asl Value";
#endif
        };
        m_pushSP(m_string2);  // Must supply a result
        break;
       
        
        
        //if (IsCombatActive())
        //{
        //  m_interpretError("Combat active in $GET_CHAR_ASL");
        //  m_string2="";
        //  m_pushString2();
        //  break;
        //}
		/* Unreachable
        m_popString1();
        m_popInteger1();
        if ((m_Integer1<0) 
            || (m_Integer1>=GetPartySize(party))) 
        {
          m_interpretError("Illegal party member # in $GET_CHAR_ASL");
          break;
        };
        m_pushString2();
        break;
		*/
      case SUBOP_GET_CHAR_RACE:
        {
          CHARACTER *pChar;
          pChar = m_popCharacter("$GET_CHAR_RACE()");
          if (pChar != NULL)
          {
            m_string1 = raceData.GetRaceName(pChar->RaceID());
          }
          else
          {
            m_string1 = "NoSuchCharacter";
          };
          m_pushString1();
        };
        break;
      case SUBOP_SET_CHAR_RACE:
        {
          CHARACTER *pChar;
          m_popString1();
          pChar = m_popCharacter("$SET_CHAR_RACE()");
          if (pChar != NULL)
          {
            //raceType newRace;
            RACE_ID newRaceID;
            newRaceID = m_string1;
            if (raceData.LocateRace(newRaceID) >= 0)
            //if (newRace == raceUnknown)
            {
              pChar->RaceID(newRaceID);
            }
            else
            {
              m_string1 = "";
            };
          }
          else
          {
            m_string1 = "";
          };
          m_pushString1();
        };
        break;

      case SUBOP_GET_ISMAMMAL:     
        m_GetActorBOOL(&CHARACTER::IsMammal, "$GET_ISMAMMAL()"); break;
        {
          /*
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_ISMAMMAL, actor);      
          break;
          */
        }
      case SUBOP_GET_ISANIMAL:     
        GET_ACTOR_BOOL(IsAnimal,"$GET_ISANIMAL()"); break;
        {
          /*
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_ISANIMAL, actor);      
          break;
          */
        }
      case SUBOP_GET_ISSNAKE:      
        GET_ACTOR_BOOL(IsSnake,"$GET_ISSNAKE()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_ISSNAKE, actor);      
        }
        break;
        */
      case SUBOP_GET_ISGIANT:      
        GET_ACTOR_BOOL(IsGiant, "$GET_ISGIANT()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_ISGIANT, actor);      
        }
        break;        
        */
      case SUBOP_GET_ISALWAYSLARGE:      
        GET_ACTOR_BOOL(IsAlwaysLarge,"$GET_ISALWAYSLARGE()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_ISALWAYSLARGE, actor);      
        }
        break;
        */
      case SUBOP_GET_HASDWARFACPENALTY:      
        GET_ACTOR_BOOL(HasDwarfACPenalty,"$GET_HASDWARFACPENALTY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASDWARFACPENALTY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASGNOMEACPENALTY:      
        GET_ACTOR_BOOL(HasGnomeACPenalty, "$GET_HASGNOMEACPENALTY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASGNOMEACPENALTY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASDWARFTHAC0PENALTY:     
        GET_ACTOR_BOOL(HasDwarfTHAC0Penalty, "$GET_HASDWARFTHAC0PENALTY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASDWARFTHAC0PENALTY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASGNOMETHAC0PENALTY:      
        GET_ACTOR_BOOL(HasGnomeTHAC0Penalty, "$GET_HASGNOMETHAC0PENALTY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASGNOMETHAC0PENALTY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASRANGERDMGPENALTY:      
        GET_ACTOR_BOOL(HasRangerDmgPenalty, "$GET_HASRANGERDMGPENALTY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASRANGERDMGPENALTY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASPOISONIMMUNITY:      
        GET_ACTOR_BOOL(HasPoisonImmunity, "$GET_HASPOISONACIMMUNITY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASPOISONIMMUNITY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASDEATHIMMUNITY:      
        GET_ACTOR_BOOL(HasDeathImmunity, "$GET_HASDEATHIMMUNITY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASDEATHIMMUNITY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASCONFUSIONIMMUNITY:      
        GET_ACTOR_BOOL(HasConfusionImmunity, "$GET_HASCONFUSIONIMMUNITY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASCONFUSIONIMMUNITY, actor);      
        }
        break;
        */
      case SUBOP_GET_HASVORPALIMMUNITY:      
        GET_ACTOR_BOOL(HasVorpalImmunity, "$GET_HASVORPALIMMUNITY()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_HASVORPALIMMUNITY, actor);      
        }
        break;
        */
      case SUBOP_GET_CANBEHELDORCHARMED:      
        GET_ACTOR_BOOL(CanBeHeldOrCharmed, "$GET_CANBEHELDORCHARMED()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_CANBEHELDORCHARMED, actor);      
        }
        break;
        */
      case SUBOP_GET_AFFECTEDBYDISPELEVIL:      
        GET_ACTOR_BOOL(AffectedByDispelEvil, "$GET_AFFECTEDBYDISPELLEVIL()"); break;
        /*
        {
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_getCharacterValueWithActor(CHAR_AFFECTEDBYDISPELEVIL, actor);      
        }
        break;
        */
      //case SUBOP_SET_AFFECTEDBYDISPELEVIL:      SET_ACTOR_BOOL(AffectedByDispelEvil); break;
        /*
        {
          m_popString2();
          m_popString1(); // ActorType
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          m_setCharacterValueWithActor(CHAR_AFFECTEDBYDISPELEVIL, actor, m_string2);      
        }
        break;
        */
      case SUBOP_GET_CHAR_NAME:            GET_CHAR_STRING(GetName,"$GET_CHAR_NAME()");               break;
      case SUBOP_GET_CHAR_AC:              GET_CHAR_INT(GetBaseAC,"$GET_CHAR_AC()");                  break;
      case SUBOP_GET_CHAR_ADJAC:           GET_CHAR_INT(GetAdjAC,"$GET_CHAR_ADJAC()");                break;
      case SUBOP_GET_CHAR_EFFAC:           GET_CHAR_INT(GetEffectiveAC,"$GET_CHAR_EFFAC()");          break;
      case SUBOP_GET_CHAR_HITPOINTS:       GET_CHAR_INT(GetAdjHitPoints,"$GET_CHAR_HITPOINTS()");     break;
      case SUBOP_GET_CHAR_ADJTHAC0:        GET_CHAR_INT(GetAdjTHAC0,"$GET_CHAR_ADJTHAC0()");          break;
      case SUBOP_GET_CHAR_THAC0:           GET_CHAR_INT(GetTHAC0,"$GET_CHAR_THAC0()");                break;
      case SUBOP_GET_CHAR_RDYTOTRAIN:      GET_CHAR_INT(GetAdjReadyToTrain,"$GET_CHAR_RDYTOTRAIN()"); break;
      case SUBOP_GET_CHAR_Exp:             
        {
          m_popString1(); // class
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            BASECLASS_ID baseclassID;
            baseclassID = m_string1;
            pChar = m_popCharacter("$GET_CHAR_Exp()");
            //m_Integer1 = pChar->GetCurrExp(m_string1);
            m_Integer1 = pChar->GetCurrExp(baseclassID);
          };
#else
          {
            m_popInteger1();
            m_Integer1 = 20000;
          };
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_GET_CHAR_ICON_INDEX:      GET_CHAR_INT(GetIconIndex,"$GET_CHAR_ICON_INDEX()");          break;
      case SUBOP_GET_CHAR_AGE:             GET_CHAR_INT(GetAdjAge,"$GET_CHAR_AGE()");                    break;
      case SUBOP_GET_CHAR_MAXAGE:          GET_CHAR_INT(GetAdjMaxAge,"$GET_CHAR_MAXAGE()");              break;
      case SUBOP_GET_CHAR_MAXMOVE:         GET_CHAR_INT(GetAdjMaxMovement_GPDL,"$GET_CHAR_MAXMOVE()");   break;
      case SUBOP_GET_CHAR_LIMITED_STR:     GET_CHAR_INT(GetLimitedStr,"$GET_CHAR_LIMITED_STR()");        break;
      case SUBOP_GET_CHAR_LIMITED_STRMOD:  GET_CHAR_INT(GetLimitedStrMod,"$GET_CHAR_LIMITED_STRMOD()");  break;
      case SUBOP_GET_CHAR_LIMITED_INT:     GET_CHAR_INT(GetLimitedInt,"$GET_CHAR_LIMITED_INT()");        break;
      case SUBOP_GET_CHAR_LIMITED_WIS:     GET_CHAR_INT(GetLimitedWis,"$GET_CHAR_LIMITED_WIS()");        break;
      case SUBOP_GET_CHAR_LIMITED_DEX:     GET_CHAR_INT(GetLimitedDex,"$GET_CHAR_LIMITED_DEX()");        break;
      case SUBOP_GET_CHAR_LIMITED_CON:     GET_CHAR_INT(GetLimitedCon,"$GET_CHAR_LIMITED_CON()");        break;
      case SUBOP_GET_CHAR_LIMITED_CHA:     GET_CHAR_INT(GetLimitedCha,"$GET_CHAR_LIMITED_CHA()");        break;
      case SUBOP_GET_CHAR_ADJ_STR:         GET_CHAR_INT(GetAdjStr, "$GET_CHAR_ADJ_STR()");        break;
      case SUBOP_GET_CHAR_ADJ_STRMOD:      GET_CHAR_INT(GetAdjStrMod, "$GET_CHAR_ADJ_STRMOD()");  break;
      case SUBOP_GET_CHAR_ADJ_INT:         GET_CHAR_INT(GetAdjInt, "$GET_CHAR_ADJ_INT()");        break;
      case SUBOP_GET_CHAR_ADJ_WIS:         GET_CHAR_INT(GetAdjWis, "$GET_CHAR_ADJ_WIS()");        break;
      case SUBOP_GET_CHAR_ADJ_DEX:         GET_CHAR_INT(GetAdjDex, "$GET_CHAR_ADJ_DEX()");        break;
      case SUBOP_GET_CHAR_ADJ_CON:         GET_CHAR_INT(GetAdjCon, "$GET_CHAR_ADJ_CON()");        break;
      case SUBOP_GET_CHAR_ADJ_CHA:         GET_CHAR_INT(GetAdjCha, "$GET_CHAR_ADJ_CHA()");        break;
      case SUBOP_GET_CHAR_PERM_STR:        GET_CHAR_INT(GetPermStr,"$GET_CHAR_PERM_STR()");              break;
      case SUBOP_GET_CHAR_PERM_STRMOD:     GET_CHAR_INT(GetPermStrMod,"$GET_CHAR_PERM_STRMOD()");        break;
      case SUBOP_GET_CHAR_PERM_INT:        GET_CHAR_INT(GetPermInt,"$GET_CHAR_PERM_INT()");              break;
      case SUBOP_GET_CHAR_PERM_WIS:        GET_CHAR_INT(GetPermWis,"$GET_CHAR_PERM_WIS()");              break;
      case SUBOP_GET_CHAR_PERM_DEX:        GET_CHAR_INT(GetPermDex,"$GET_CHAR_PERM_DEX()");              break;
      case SUBOP_GET_CHAR_PERM_CON:        GET_CHAR_INT(GetPermCon,"$GET_CHAR_PERM_CON()");              break;
      case SUBOP_GET_CHAR_PERM_CHA:        GET_CHAR_INT(GetPermCha,"$GET_CHAR_PERM_CHA()");              break;
      case SUBOP_GET_CHAR_MAXENC:          GET_CHAR_INT(GetAdjMaxEncumbrance,"$GET_CHAR_MAXENC()");      break;
      case SUBOP_GET_CHAR_ENC:             GET_CHAR_INT(GetEncumbrance,"$GET_CHAR_ENC()");               break;
      case SUBOP_GET_CHAR_GENDER:          GET_CHAR_ENUM(GetAdjGender,"$GET_CHAR_GENDER()");             break;
      case SUBOP_GET_CHAR_CLASS:           GET_CHAR_STRING(GetClass,"$GET_CHAR_CLASS()");                break;
      case SUBOP_GET_CHAR_ALIGNMENT:       GET_CHAR_ENUM(GetAdjAlignment,"$GET_CHAR_ALIGNMENT()");       break;
      case SUBOP_GET_CHAR_STATUS:          GET_CHAR_ENUM(GetAdjStatus,"$GET_CHAR_STATUS()");             break;
      case SUBOP_GET_CHAR_UNDEAD:          GET_CHAR_STRING(GetUndeadType,"$GET_CHAR_UNDEAD()");          break;
      case SUBOP_GET_CHAR_SIZE:            GET_CHAR_ENUM(GetAdjSize,"$GET_CHAR_SIZE()");                 break;
      case SUBOP_GET_CHAR_MAGICRESIST:     GET_CHAR_INT(GetAdjMagicResistance,"$GET_CHAR_MAGICRESIST()");break;
      case SUBOP_GET_CHAR_Lvl: 
        {
          m_popString1(); // class
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            BASECLASS_ID baseclassID;
            baseclassID = m_string1;
            pChar = m_popCharacter("$GET_CHAR_Lvl()");
            //m_Integer1 = pChar->GetLevel(m_string1);
            m_Integer1 = pChar->GetCurrLevel(baseclassID);
          }
#else
          {
            m_popInteger1();
            m_Integer1 = 3;
          };
#endif
          m_pushInteger1();
        };
        break;
      
      
      case SUBOP_GET_CHAR_NBRHITDICE:      GET_CHAR_FLOAT(GetNbrHD,"$GET_CHAR_NBRHITDICE()");    break;
      case SUBOP_GET_CHAR_NBRATTACKS:      GET_CHAR_FLOAT(GetNbrAttacks,"$GET_CHAR_NBRATTACKS()");    break;
      case SUBOP_GET_CHAR_MORALE:          GET_CHAR_INT(GetAdjMorale,"$GET_CHAR_MORALE()");        break;
      case SUBOP_GET_CHAR_DAMAGEBONUS:     GET_CHAR_INT(GetAdjDmgBonus,"$GET_CHAR_DAMAGEBONUS()");   break;
      case SUBOP_GET_CHAR_HITBONUS:        GET_CHAR_INT(GetAdjHitBonus,"$GET_CHAR_HITBONUS()");      break;

      case SUBOP_GET_CHAR_Ready:           
        {
          DWORD rdyLoc;
          m_popInteger2();  //The position in list
          m_popString1();   //The location on body
          m_popString2();   //The actor string
          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          if (m_string1.IsEmpty()) rdyLoc=Cannot;
          else rdyLoc = CString2Base38(m_string1);
          CHARACTER *tdude = /* (CHARACTER*) */GetCurrentlyActiveContext(&actor, "$GET_CHAR_Ready");          
          if (tdude==NULL)
          {
            m_interpretError("Invalid target actor data in $GET_CHAR_Ready");
            m_pushSP(m_false);
            break;
          };

          //int index = tdude->myItems.GetReady((itemLocationType)m_Integer1);
          int index = tdude->myItems.GetReadiedItem(rdyLoc, m_Integer2);
          if (index >= 0)
          {
            //ITEM *pItem;
            //GLOBAL_ITEM_ID giID = tdude->myItems.GetItem(index);
            //m_string2 = itemData.GetItemUniqueName(giID);
            ITEM_ID itemID = tdude->myItems.GetItem(index);
            m_string2 = itemData.GetItemUniqueName(itemID);
          }
          else
          {
            m_string2 = "";
          };
          m_pushString2();
        };
        break;

      case SUBOP_SET_CHAR_Ready:
        {
          m_popString1();  // item name
          //m_popInteger1(); // body location index
          m_popString3(); // body location index
          m_Integer1 = CString2Base38(m_string3);
          m_popString2();  // actor

          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          CHARACTER *tdude = /* (CHARACTER*) */GetCurrentlyActiveContext(&actor,"$SET_CHAR_Ready");          
          if (tdude==NULL)
          {
            m_interpretError("Invalid target actor data in $SUBOP_SET_CHAR_Ready");
            m_pushSP(m_false);
            break;
          };
          ITEM_ID itemID;
          itemID = m_string1;
          tdude->myItems.Ready(itemID, m_Integer1,/* tdude->GetItemMask(), */ tdude);  
        };
        m_pushSP(m_false);
        break;

      case SUBOP_CASTSPELLONTARGETAS:
        {
          m_popString1(); // caster actor string
          m_popString2(); // spell name
          m_popString3(); // target actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string3);
          
          CHARACTER *tdude = /* (CHARACTER*) */GetCurrentlyActiveContext(&actor,"$CastSpellOnTargetAs()");          
          if (tdude==NULL)
          {
            m_interpretError("Invalid target actor data in $CastSpellOnTargetAs()");
            m_pushSP(m_false);
            break;
          }

          actor = m_StringToActor((LPCSTR)m_string1);

          CHARACTER *cdude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$CastSpellOnTargetAs");          
          if (cdude==NULL)
          {
            m_interpretError("Invalid caster actor data in $CastSpellOnTargetAs()");
            m_pushSP(m_false);
            break;
          }

          //GLOBAL_SPELL_ID spelldbkey = spellData.FindName(m_string2);
          SPELL_ID spellID;
          spellID = m_string2;
          //if (!spelldbkey.IsValidSpell())
          if (!spellID.IsValidSpell())
          {
            CString err;
            err.Format("Invalid spell name \'%s\' in $CastSpellOnTargetAs", m_string2);
            m_interpretError(err);
            m_pushSP(m_false);
            break;
          }

          //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
          SPELL_DATA *pSpell = spellData.GetSpell(spellID);
          if (pSpell == NULL)
          {
            CString err;
            err.Format("Failed spell db lookup for spell \'%s\' in $CastSpellOnTargetAs", m_string2);
            m_interpretError(err);
            m_pushSP(m_false);
            break;
          }

#ifdef UAFEngine  
          //cdude->InvokeSpellOnTarget(*tdude, -1, spelldbkey, NULL, NULL, NULL, false);
          cdude->InvokeSpellOnTarget(*tdude, -1, spellID, NULL, NULL, NULL, false, NULL);
#endif
          m_pushSP(m_true);
        }
        break;

        // cast spell on target using imaginary 30th level Cleric/MU
      case SUBOP_CASTSPELLONTARGET:
        {
          m_popString1(); // spell name
          m_popString2(); // target actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          

#ifdef UAFEngine
          CHARACTER *tdude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$CastSpellOnTarget()");          
          if ((tdude==NULL) || (tdude == &FakeCharacter))
          {
            m_interpretError("Invalid target actor data in $CastSpellOnTarget()");
            m_pushSP(m_false);
            break;
          }
#endif

          //GLOBAL_SPELL_ID spelldbkey = spellData.FindName(m_string1);
          SPELL_ID spellID;
          spellID = m_string1;
          //if (!spelldbkey.IsValidSpell())
          if (!spellID.IsValidSpell())
          {
            CString err;
            err.Format("Invalid spell name \'%s\' in $CastSpellOnTarget()", m_string1);
            m_interpretError(err);
            m_pushSP(m_false);
            break;
          }

          //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
          SPELL_DATA *pSpell = spellData.GetSpell(spellID);
          if (pSpell == NULL)
          {
            CString err;
            err.Format("Failed spell db lookup for spell \'%s\' in $CastSpellOnTarget()", m_string1);
            m_interpretError(err);
            m_pushSP(m_false);
            break;
          }

#ifdef UAFEngine    
          //CHARACTER FakeCharacter;
          // create high-level spell caster
          //
          // create Cleric/MU so either spell class can be cast
          //
          // when other spell classes are added, this will need to be 
          // changed to create specific char classes (can't have cl/mu/dr/rang/etc
          // as valid char class.
          //


          DWORD StartExp;
/*
          StartExp = 2 + 
                     GetMinClericExpForLevel(29) +
                     GetMinMagicUserExpForLevel(29);
*/ StartExp = 0; // Not Implemented(0x9e5a235, false);

          FakeCharacter.SetAlignment(ChaoticNeutral);
          //FakeCharacter.race(Human);
          RACE_ID raceID;
          raceID = "Human";
          FakeCharacter.RaceID(raceID);
          FakeCharacter.SetGender(Male);
          CLASS_ID classID;
          classID = "Fighter";
          FakeCharacter.SetClass(classID); // 
          //FakeCharacter.SetClass(C_MU); // 
          FakeCharacter.SetType(FAKE_CHARACTER_TYPE);
          FakeCharacter.SetName("TempGPDLClericMU");
          FakeCharacter.generateNewCharacter(StartExp,START_EXP_VALUE);  
          FakeCharacter.SetPermStr(18);
          FakeCharacter.SetPermInt(18);
          FakeCharacter.SetPermWis(18);
          FakeCharacter.SetPermDex(18);
          FakeCharacter.SetPermCon(18);
          FakeCharacter.SetPermCha(18);
          FakeCharacter.InvokeSpellOnTarget(*tdude, -1, spellID, NULL, NULL, NULL, false, NULL);
#else
          m_pushSP(m_true); // editor only
#endif
        }
        break;

      case SUBOP_CHAR_REMOVEALLSPELLS:
        {
          m_popString1(); // spell level
          m_popString2(); // actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          
          CHARACTER *dude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$CHAR_REMOVEALLSPELLS()");          
          if (dude==NULL)
          {
            m_interpretError("Invalid actor data in $CHAR_REMOVEALLSPELLS()");
            m_pushSP(m_false);
            break;
          }

          int count = 0;
#ifdef UAFEngine          
          int spelllvl = atoi(m_string1);

          for (int j = spelllvl; j > 0; j--)
          {
            count += dude->RemoveAllSpellEffectsByLevel(j);
          };
#endif
          m_string3.Format("%i", count);
          m_pushSP(m_string3);
        }
        break;
/*
      case SUBOP_CHAR_REMOVESPELL:
        {
          m_popString1(); // spell name
          m_popString2(); // target actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          
          CHARACTER *dude = (CHARACTER*)GetCurrentlyActiveContext(&actor);          
          if (dude==NULL)
          {
            m_interpretError("Invalid actor data in $SUBOP_CHAR_REMOVESPELL");
            m_pushSP(m_false);
            break;
          }

          BOOL wasremoved=FALSE;
#ifdef UAFEngine          
          wasremoved = dude->RemoveSpellByName(m_string1);
#endif
          m_string3.Format("%i", (wasremoved?TRUE:FALSE));
          m_pushSP(m_string3);
        }
        break;
*/
      case SUBOP_CHAR_DISPELMAGIC:
        {
          m_popString1(); // spell level
          m_popString2(); // actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          
          CHARACTER *dude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$CHAR_DISPELMAGIC()");          
          if (dude==NULL)
          {
            m_interpretError("Invalid actor data in $CHAR_DISPELMAGIC()");
            m_pushSP(m_false);
            break;
          }

          int count = 0;
#ifdef UAFEngine          
          int spelllvl = atoi(m_string1);

          count += dude->DispelSpellEffectsByLevel(spelllvl);
#endif
          m_string3.Format("%i", count);
          m_pushSP(m_string3);
        }
        break;

      case SUBOP_CHAR_REMOVEALLITEMCURSE:
        {
          m_popString1(); // actor string

          ActorType actor = m_StringToActor((LPCSTR)m_string1);
          
          CHARACTER *dude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor, "$CHAR_REMOVEALLITEMCURSE()");          
          if (dude==NULL)
          {
            m_interpretError("Invalid actor data in $CHAR_REMOVEALLITEMCURSE()");
            m_pushSP(m_false);
            break;
          }

#ifdef UAFEngine          
          dude->ClearAllItemCursedFlags();
#endif
          m_pushSP(m_true);
        }
        break;
      case SUBOP_DAT_Class_Baseclasses:
        {
          m_popString1();
          m_string2.Empty();
          CLASS_ID classID;
          classID = m_string1;
          const CLASS_DATA *pClass;
          pClass = classData.PeekClass(classID);
          if (pClass != NULL)
          {
            int j, n;
            n = pClass->GetCount();
            for (j=0; j<n; j++)
            {
              const BASECLASS_ID *pBaseclassID;
              pBaseclassID = pClass->PeekBaseclassID(j);
              m_string2 += '$';
              m_string2 += *pBaseclassID;
            };
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Baseclass_Level:
        {
          m_popInteger1();
          m_popString1();
          BASECLASS_ID baseclassID;
          baseclassID = m_string1;
          //m_Integer2 = baseclassData.GetLevel(m_string1, m_Integer1);
          m_Integer2 = baseclassData.GetLevel(baseclassID, m_Integer1);
          m_pushInteger2();
        };
        break;
      case SUBOP_DAT_Baseclass_Experience:
        {
          m_popInteger1();
          m_popString1();
          BASECLASS_ID baseclassID;
          baseclassID = m_string1;
          //m_Integer2 = baseclassData.GetMinExpForLevel(m_string1, m_Integer1);
          m_Integer2 = baseclassData.GetMinExpForLevel(baseclassID, m_Integer1);
          m_pushInteger2();
        };
        break;
      case SUBOP_GET_CHAR_SEX:
        {
          CHARACTER *pChar;
          pChar = Dude("$GET_CHAR_SEX");
          m_string1 = GenderNames[pChar->GetAdjGender()];
          m_pushString1();
        };
        break;
/*   ******* Replaced with definition that uses uniqueID of character rather than party index
      case SUBOP_SET_CHAR_SEX:
        m_popString1();  // new sex
        m_popInteger1(); // id
        
#ifdef UAFEngine
        if (m_Integer1 == -2)
        {
          CHARACTER *pChar;
          pChar = pScriptContext->GetCreatedCharacterContext("$GET_CHAR_SEX()");
          if (pChar == NULL)
          {
            m_interpretError("Illegal characer Index in $GET_CHAR_SEX");
          }
          else
          {
            pChar->SetGender(m_string1);
          };
          break;
        }
        else if (IsCombatActive())
        {
          if (   (m_Integer1<0) 
              || (m_Integer1>=GetNumCombatants())) 
          {
            m_interpretError("Illegal party member # in $SET_CHAR_SEX");
            break;
          };
          SetCombatantGender(m_Integer1, m_string1);
        }
        else
        {          
          if (   (m_Integer1<0) 
              || (m_Integer1>=GetPartySize(party))) 
          {
            m_interpretError("Illegal party member # in $SET_CHAR_SEX");
            break;
          };
          SetCharacterGender(party, m_Integer1, m_string1);
        }
#endif
        m_pushString1();
        break;
*/
      case SUBOP_GET_CHAR_TYPE:    
        {
          int type;
          CHARACTER *pDude = Dude("$GET_CHAR_TYPE()") ;
          type = pDude->GetType();      
          switch(type)
          {
          case 1: m_string1 = "@PC@"; break;
          case 2: m_string1 = "@NPC@"; break;
          case 3:
            {
              ////const MONSTER_DATA *pMonsterData;
              //if (pDude==NULL) return "";
              ////pMonsterData = monsterData.PeekMonster(pDude->monsterID);
              ////if (pMonsterData == NULL) return "";
              ////return pMonsterData->Name;
              m_string1 = pDude->monsterID; break;
            };
          default: m_string1 = "";
          };
        };
        m_pushString1();
        break;
      case SUBOP_GET_GLOBAL_ASL:
        m_popString1(); // Get key.
        m_string2=LookupGlobalASL(globalData,m_string1);
        m_pushSP(m_string2);
        break;
      case SUBOP_GET_PARTY_ASL:
        m_popString1(); // Get key.
        m_string2=LookupPartyASL(party, m_string1);
        m_pushSP(m_string2);
        break;

      case SUBOP_GET_VAULT_MONEYAVAILABLE:
        {
          m_popInteger1();
          
          double total = globalData.VaultTotal();
          
          if (m_Integer1==0) // no conversion
          {
            m_Integer2 = (int)total;
          }
          else
          {
            if ((m_Integer1 >= 1)&&(m_Integer1 <= 10))
            {
              m_Integer2 = 
                (int)globalData.moneyData.Convert(total, 
                                                  globalData.moneyData.GetBaseType(),
                                                  globalData.moneyData.GetItemClass(m_Integer1-1));
            }
            else
              m_Integer2=0;
          }
          m_pushInteger2();
        }
        break;

      case SUBOP_GET_PARTY_MONEYAVAILABLE:
        {
          m_popInteger1();

          double total = 0;
          for (int j=0;j<party.numCharacters;j++)
            total += party.characters[j].money.Total();

          if (m_Integer1==0) // no conversion
          {
            m_Integer2 = (int)total;
          }
          else
          {
            if ((m_Integer1 >= 1)&&(m_Integer1 <= 10))
            {
              m_Integer2 = 
                 (int)globalData.moneyData.Convert(total, 
                                              globalData.moneyData.GetBaseType(),
                                              globalData.moneyData.GetItemClass(m_Integer1-1));
            }
            else
              m_Integer2=0;
          }
          m_pushInteger2();
        }
        break;

      case SUBOP_GIVE_CHAR_ITEM:
        {
          m_popString1(); // item name
          m_popString2(); // actor string
          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          CHARACTER *tdude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$GIVE_CHAR_ITEM()");          
          if (tdude==NULL)
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }
          {
            //GLOBAL_ITEM_ID giID = itemData.FindName(m_string1);
            ITEM_ID itemID;
            itemID = m_string1;
            //itemData.FindItemID(itemID);
            itemData.LocateItem(itemID);
            //BOOL success = dude->addCharacterItem(giID, 
            BOOL success = tdude->addCharacterItem(itemID, 
                                                  //itemData.GetItemBundleQty(giID), 
                                                  itemData.GetItemBundleQty(itemID), 
                                                  -1, // default charges
                                                  TRUE, // is identified
                                                  -1); // no cost
            m_pushSP(success?m_true:m_false);
          }

        }
        break;

      case SUBOP_TAKE_CHAR_ITEM:
        {
          m_popString1(); // item name
          m_popString2(); // actor string
          ActorType actor = m_StringToActor((LPCSTR)m_string2);
          CHARACTER *tdude = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$TAKE_CHAR_ITEM()");          
          if (tdude==NULL)
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }
          
          //GLOBAL_ITEM_ID giID = itemData.FindName(m_string1);
          ITEM_ID itemID; 
          itemID = m_string1;
          //itemData.FindItemID(itemID);
          itemData.LocateItem(itemID);
          BOOL success = FALSE;

          POSITION pos = tdude->myItems.GetHeadPosition();
          POSITION itemPos = NULL;
          while (pos != NULL)  // First we go through the items.  The item with the given
                               // name that is also the ItemContext() will take precedence.
          {
            const ITEM *pItem;
            pItem = &tdude->myItems.PeekAtPos(pos);
            //if (pItem->m_giID == giID)
            if (pItem->itemID == itemID)
            {
              if (itemPos == NULL) 
              {
                itemPos = pos;
              };
              if (pItem->key == pScriptContext->GetItemContextKey())
              {
                itemPos = pos;  // Override any others
                break;  // No sense searching further
              };
            };
            tdude->myItems.GetNext(pos);
          };
          if  (itemPos != NULL)
          {
            success = tdude->delCharacterItem(tdude->myItems.GetKeyAt(itemPos), 
                                             //itemData.GetItemBundleQty(giID));
                                             itemData.GetItemBundleQty(itemID));
          }
          m_pushSP(success?m_true:m_false);
        }
        break;     
          
      case SUBOP_GREATER:
        m_popString1();
        m_popString2();
        m_pushSP(m_string2>m_string1?m_true:m_false);
        break;
      case SUBOP_GREATEREQUAL:
        m_popString1();
        m_popString2();
        m_pushSP(m_string2>=m_string1?m_true:m_false);
        break;
      case SUBOP_SpellAdj:
        {
          CHARACTER *pChar;
          m_popInteger4(); // The bonus
          m_popInteger3(); // The percent
          m_popInteger2(); // The last level
          m_popInteger1(); // The first level
          m_popString2();  // The Identification
          m_popString1();  // The SCHOOL_ID
          pChar = m_popCharacterActor();
#ifdef UAFEngine
          if (pChar != NULL)
          {
            m_string1 = pChar->SpellAdjustment(
                          m_string1,  // schoolID
                          m_string2,  // identification
                          m_Integer1, // first level
                          m_Integer2, // last level
                          m_Integer3, // percent
                          m_Integer4  // bonus
                          );
          };
#endif
           m_pushString1();
        };
        break;
      case SUBOP_SkillAdj:
        {
          CHARACTER *pChar;
          m_Integer2 = 0;
          m_popInteger1();
          m_popString3();
          m_popString2();
          m_popString1();
          pChar = m_popCharacterActor();
#ifdef UAFEngine
          m_string1 = m_SkillAdjustment(
                        pChar,
                        m_string1,
                        m_string2,
                        m_string3,
                        m_Integer1);
#endif
           m_pushString1();
        };
        break;
      case SUBOP_GREP:
        // bool GREP (pattern, string)
        re_Set_EGrep(); // Set egrep mode
        re_searchString = m_popSP();   // The string to search.
        m_popString2();   // The pattern
        re_searchString.MakeUpper();
        m_string2=upperPattern(m_string2);
        m_grepCResult=re_compile(LPCTSTR(m_string2), &m_grepPat);
        if (m_grepCResult!=NULL) {
          if (m_debugLevel&&DBG_messageBox) {
            CString msg;
            msg="Error compiling Grep pattern\n";
            msg+=m_string2;
            msg+="\n";
            msg+=CString(m_grepCResult);
            MsgBoxError(msg);
          };
          INTERP_RETURN(GPDL_GREPERROR);
        };
        m_Integer1=re_search(&m_grepPat,
                   LPCTSTR(re_searchString),
                   re_searchString.GetLength(),
                   0,
                   re_searchString.GetLength(),
                   &re_grepReg);
        m_pushSP(m_Integer1<0?m_false:m_true);
        break;
      case SUBOP_Wiggle:
        m_popInteger1();
        if (      ((unsigned)m_Integer1 > re_grepReg.num_regs)
              ||  (re_grepReg.start[(unsigned)m_Integer1] <0))
        {
          m_string1 = "";
        }
        else
        {
          m_string1 = re_searchString.Mid(re_grepReg.start[m_Integer1], re_grepReg.end[m_Integer1]-re_grepReg.start[m_Integer1]);
        }
        m_pushString1();
        break;
      case SUBOP_HitPoints:
        m_popString1();
        m_string2 = m_HitPoints(m_string1);
        m_pushString2();
        break;
      case SUBOP_IF_PARTY_ASL:
        m_popString1(); // Get key.
        if (ExistsPartyASL(party, m_string1))
          m_pushSP(m_true);
        else 
          m_pushSP(m_false);
        break;
      case SUBOP_IF_CHAR_ASL:
        {
          CHARACTER *pChar;
          m_popString1();   //  ASL Key
          // Replaced with code that uses UniqueID rather than party index
          //m_popInteger1();
          //if (   (m_Integer1<0) 
          //    || (m_Integer1>=GetPartySize(party))) 
          //{
            //m_interpretError("Illegal party member # in $GET_CHAR_ASL");
            //break;
          //};
          //m_string2 = LookupCharacterASL(
            //party,
            //m_Integer1,
            //m_string1);
          //m_pushString2();
          pChar = Dude("$IF_CHAR_ASL()");
          m_string2 = LookupCharacterASL(pChar, m_string1);
          m_pushString2();
        };
        break;
      case SUBOP_IS_AFFECTED_BY_SPELL:
        {
          SPELL_ID spellID;
          m_popString1(); // SpellName
          m_popString2(); // ActorType
          spellID = m_string1;
          m_string3 = m_IsCharAffectedBySpell(m_string2, spellID);
          m_pushString3();
        };
        break;

      case SUBOP_IS_AFFECTED_BY_SPELL_ATTR:
        m_popString1(); // SpellName
        m_popString2(); // ActorType
        m_string3 = m_IsCharAffectedBySpellAttribute(m_string2, m_string1);
        m_pushString3();        
        break;

      case SUBOP_IndexOf:
        m_popString1();
        m_string2 = m_IndexOf(m_string1);
        m_pushString2();
        break;
      case SUBOP_IndexToActor:
        m_string2 = m_IndexToActor();
        m_pushString2();
        break;
      case SUBOP_InParty:
        m_popString1();
        m_string2 = m_InParty(m_string1);
        m_pushString2();
        break;
      case SUBOP_LAND:
        m_popString1();
        m_popString2();
        m_pushSP(((m_string1.GetLength()!=0) && m_string2.GetLength()!=0)?
                        m_true: m_false);
        break;

      case SUBOP_LAST_ATTACKER_OF:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }

          m_popString1();
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
          COMBATANT *dude = GetCurrentlyActiveContext(&actor,"$LAST_ATTACKER_OF()")->m_pCombatant;
#else
          COMBATANT *dude = (COMBATANT*)GetCurrentlyActiveContext(&actor);
#endif
      
          if (dude==NULL) 
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();     
            break;
          }

          COMBATANT *pLastAttacker = GetLastAttackerOf(dude);
          if (pLastAttacker != NULL)
          {
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
         
      case SUBOP_LENGTH:
        m_popString1();
        m_uInteger1=m_string1.GetLength();
        m_pushUInteger1();
        break;
      case SUBOP_LESS:
        m_popString1();
        m_popString2();
        m_pushSP(m_string2<m_string1?m_true:m_false);
        break;
      case SUBOP_LESSEQUAL:
        m_popString1();
        m_popString2();
        m_pushSP(m_string2<=m_string1?m_true:m_false);
        break;

      case SUBOP_LEAST_DAMAGED_FRIENDLY:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }    
          
          COMBATANT *pLastAttacker = GetLeastDamagedFriendly();
          if (pLastAttacker != NULL)
          {
            ActorType actor;
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
      case SUBOP_LEAST_DAMAGED_ENEMY:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }
          
          COMBATANT *pLastAttacker = GetLeastDamagedEnemy();
          if (pLastAttacker != NULL)
          {
            ActorType actor;
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
        
      case SUBOP_LISTEN:
        if (m_pGPDLevent == NULL)
        {
          m_pushEmptyString();
          break;
        };
        m_discourseText.StartInput();
        m_state=GPDL_WAIT_INPUT;
        INTERP_RETURN(GPDL_WAIT_INPUT);
      case SUBOP_LOGIC_BLOCK_VALUE:
        m_popString1();
        m_string2=LookupGlobalASL(globalData,"LOGICBLOCKVALUES");
        if ( (m_string2 == "") || (m_string1 == ""))
        {
          m_string1 = "";
        }
        else
        {
          int index = 0;
          unsigned int k;
          unsigned int len = m_string2.GetLength();
          k = m_string1[0] - 'A';
          if ( (k<0) || (k>11))
          {
            m_string1 = "";
            m_pushString1();
            break;
          };
          for (i=0; i<=k; i++)
          {
            if (len < sizeof(int))
            {
              m_string1 = "";
              break;
            };
            k = *(int *)((LPCTSTR)(m_string2)+index);
            index += sizeof(int);
            len -= sizeof(int);
            if (len < k)
            {
              m_string1 = "";
              break;
            };
            if (i != k)
            {
              index += k;
              len -= k;
              continue;
            };
            m_string1 = CString((LPCTSTR)m_string2.Mid(index,k));
          };
        };
        m_pushString1();
        break;
      case SUBOP_Myself:
        m_string1 = m_Myself();
        m_pushString1();
        break;
      case SUBOP_NextCreatureIndex:
        m_popInteger1();  // The filter
#ifdef UAFEngine
        if (IsCombatActive())
        {
          if (m_dataStack[m_SP].IsEmpty())
          {
            m_popInteger2();
          }
          else
          {
            m_popInteger2();
            m_Integer2++;
          };
          for (; m_Integer2 < combatData.NumCombatants(); m_Integer2++)
          {
            COMBATANT *pCOMBATANT;
            pCOMBATANT = combatData.GetCombatant(m_Integer2);
            if (m_Integer1 & 1)
            {
              if (!pCOMBATANT->IsAlive()) continue;
            };
            if (m_Integer1 & 2)
            {
              if (pCOMBATANT->friendly) continue;
            };
            if (m_Integer1 & 4)
            {
              if (!pCOMBATANT->friendly) continue;
            };
            if (m_Integer1 & 8)
            {
              if (!pCOMBATANT->charOnCombatMap(false, true)) continue;
            };
            break;
          };
          if (m_Integer2 >= combatData.NumCombatants())
          {
            m_string1 = "";
            m_pushString1();
          }
          else
          {
            m_pushInteger2();
          };
        }
        else
        {
          /* Really */ NotImplemented(0xc43109,FALSE);
        };
#else
        m_string1 = "";
        m_pushString1();
#endif      
        break;
      case SUBOP_CharacterContext:
        m_string1 = pScriptContext->GetCharacterContext("$CharacterContext() called when no character context exists");
        m_pushString1();
        break;
      case SUBOP_ItemContext:
        m_string1 = pScriptContext->GetItemContext("$ItemContext() called when no item context exists")->UniqueName();
        m_pushString1();
        break;
      case SUBOP_ClassContext:
        m_string2 = pScriptContext->GetClassContext("$ClassContext() called when no class context exists")->m_name;
        m_pushString2();
        break;
      case SUBOP_RaceContext:
        m_string2 = pScriptContext->GetRaceContext("$RaceContext() called when no race context exists")->m_name;
        m_pushString2();
        break;
      case SUBOP_SpellContext:
        {
#ifdef UAFEngine
          m_string1 = pScriptContext->GetSpellContext("$SpellContext() called when no spell context exists")->uniqueName;
#else
          m_string1 = pScriptContext->GetSpellContext(NULL)->Name;
#endif
          m_pushString1();
        };
          break;
      case SUBOP_MonsterTypeContext:
        {
#ifdef UAFEngine
          m_string1 = pScriptContext->GetMonsterTypeContext("$MonsterTypeContext() called when no monster type context exists")->monsterID;
#else
          m_string1 = pScriptContext->GetMonsterTypeContext(NULL)->MonsterID();
#endif
          m_pushString1();
        };
          break;
      //case SUBOP_GetSpellName:
      //  {
      //    GLOBAL_SPELL_ID gsID;
      //    m_popString1();
      //    gsID = m_Spell(m_string1);
      //    m_string2 = spellData.GetSpellName(gsID);
      //    m_pushString2();
      //  };
      //  break;
      case SUBOP_GET_SPELL_Level:
        {
          SPELL_ID spellID;
          const SPELL_DATA *pSpell;
          m_popString1();
          spellID = m_string1;
          m_Integer1 = 0;
          pSpell = spellData.PeekSpell(spellID);
          if (pSpell != NULL)
          {
            m_Integer1 = pSpell->Level;
          };
          m_pushInteger1();
        };
        break;
      case SUBOP_GET_SPELL_CanBeDispelled:
        {
          SPELL_ID spellID;
          const SPELL_DATA *pSpell;
          m_popString1();
          spellID = m_string1;
          m_string2.Empty();
          pSpell = spellData.PeekSpell(spellID);
          if (pSpell != NULL)
          {
            m_string2 = pSpell->CanBeDispelled ? "1" : "";
          };
          m_pushString2();
        };
        break;
      case SUBOP_nAND:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 &= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_LOR:
        m_popString1();
        m_popString2();
        m_pushSP(((m_string1.GetLength()!=0) || (m_string2.GetLength()!=0))?
                    m_true:m_false);
        break;
      case SUBOP_nISEQUAL:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer1==m_Integer2?m_true:m_false);
        break;
      case SUBOP_IsUndead:
        m_popString1();
        m_string2 = m_IsUndead(m_string1);
        m_pushString2();
        break;
      case SUBOP_MIDDLE:
        m_popInteger2(); //count
        m_popInteger1(); //first index
        m_popString1();
        m_string2 = m_string1.Mid(m_Integer1, m_Integer2);
        m_pushString2();
        break;

      case SUBOP_MOST_DAMAGED_FRIENDLY:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }         
          
          COMBATANT *pLastAttacker = GetMostDamagedFriendly();
          if (pLastAttacker != NULL)
          {
            ActorType actor;
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
      case SUBOP_MOST_DAMAGED_ENEMY:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }          
          
          COMBATANT *pLastAttacker = GetMostDamagedEnemy();
          if (pLastAttacker != NULL)
          {
            ActorType actor;
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);   
          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
        
      case SUBOP_nGREATER:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer2>m_Integer1?m_true:m_false);
        break;
      case SUBOP_nGREATEREQUAL:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer2>=m_Integer1?m_true:m_false);
        break;
      case SUBOP_nLESS:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer2<m_Integer1?m_true:m_false);
        break;
      case SUBOP_nMINUS:
        m_popInteger1();
        m_popInteger2();
        m_Integer2-=m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_Name:
        m_popString1();
        m_string2 = m_Name(m_string1);
        m_pushString2();
        break;

      case SUBOP_NEAREST_TO:
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }
          
          m_popString1();
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
          COMBATANT *dude = GetCurrentlyActiveContext(&actor,"$NEAREST_TO()")->m_pCombatant;
#else
          COMBATANT *dude = (COMBATANT*)GetCurrentlyActiveContext(&actor);
#endif
          
          if (dude==NULL) 
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();     
            break;
          }
          
          COMBATANT *pLastAttacker = GetNearestTo(dude);
          if (pLastAttacker != NULL)
          {
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
      case SUBOP_NEAREST_ENEMY_TO:    
        {
#ifdef UAFEngine
          if (!IsCombatActive())
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();
            break;
          }
          
          m_popString1();
          ActorType actor = m_StringToActor((LPCSTR)m_string1);
#ifdef newCombatant
          COMBATANT *dude = GetCurrentlyActiveContext(&actor, "$NEAREST_ENEMY_TO()")->m_pCombatant;
#else
          COMBATANT *dude = (COMBATANT*)GetCurrentlyActiveContext(&actor);
#endif
          
          if (dude==NULL) 
          {
            m_string2 = m_ActorToString(&NULL_ACTOR);
            m_pushString2();     
            break;
          }
          
          COMBATANT *pLastAttacker = GetNearestEnemyTo(dude);
          if (pLastAttacker != NULL)
          {
            pLastAttacker->GetContext(&actor);
            m_string2 = m_ActorToString(&actor);
          }
          else
            m_string2 = m_ActorToString(&NULL_ACTOR);          
          m_pushString2();         
#else
          m_string2 = m_ActorToString(&NULL_ACTOR);
          m_pushString2();                      
#endif          
        }
        break;
        
      case SUBOP_nLESSEQUAL:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer2<=m_Integer1?m_true:m_false);
        break;
      case SUBOP_NOOP:
        break;
      case SUBOP_nNEGATE:
        m_popInteger1();
        m_Integer2 = -m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_nNOTEQUAL:
        m_popInteger1();
        m_popInteger2();
        m_pushSP(m_Integer1!=m_Integer2?m_true:m_false);
        break;
      case SUBOP_nOR:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 |= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_NOT:
        if (m_dataStack[m_SP]==m_false) m_dataStack[m_SP]=m_true;
        else m_dataStack[m_SP]=m_false;
        break;
      case SUBOP_NOTEQUAL:
        m_popString1();
        m_popString2();
        m_pushSP(m_string1==m_string2?m_false:m_true);
        break;
      case SUBOP_nPERCENT:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 %= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_nPLUS:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 += m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_nSLASH:
        m_popInteger1();
        m_popInteger2();
        if (m_Integer1 == 0)
        {
          WriteDebugString("Attempt to divide (/#) by zero ");
          m_Integer1 = 1;
        };
        m_Integer2 /= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_nTIMES:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 *= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_NUMERIC:
        if (m_popInteger(m_bool1, m_uInteger1)==GPDL_OK)
          m_pushSP(m_true);
        else m_pushSP(m_false);
        break;
      case SUBOP_nXOR:
        m_popInteger1();
        m_popInteger2();
        m_Integer2 ^= m_Integer1;
        m_pushInteger2();
        break;
      case SUBOP_OVER:
        m_popString1();
        m_popString2();
        m_pushString2();
        m_pushString1();
        m_pushString2();
        break;
      case SUBOP_LISTENTEXT:
        if (m_pGPDLevent == NULL)
        {
          m_listenText.Empty();
        };
        m_pushSP(m_listenText);
        break;
      case SUBOP_ONE:
        m_pushSP(m_true);
        break;
      case SUBOP_PARTYSIZE:
        m_Integer1=GetPartySize(party);
        m_pushInteger1();
        break;
      case SUBOP_PLUS: // string concatenation
        m_popString1();
        m_popString2();
        m_string2+=m_string1;
        m_pushString2();
        break;
      case SUBOP_POP:
        m_popString1();
        break;
      case SUBOP_RANDOM:
        m_popInteger1();
        if (m_Integer1<=0) {
          m_interpStatus=GPDL_ILLPARAM;
          break;
        };
        //m_uInteger1=rand()%m_Integer1;
        m_uInteger1=RollDice(m_Integer1, 1, 0.0)-1;
        m_pushUInteger1();
        break;

      case SUBOP_RUN_CHAR_SCRIPTS:
        {
          CString result;
          m_popString1();   // Script name
#ifdef UAFEngine
          COMBATANT *pCOMBATANT;
          if (IsCombatActive())
          {
            m_popInteger1();  // character index
            pCOMBATANT = combatData.GetCombatant(m_Integer1);
            if (pCOMBATANT != NULL)
            {
              result = pCOMBATANT->RunCombatantScripts(
                              m_string1,
                              ScriptCallback_LookForChar,
                              "N",
                              "Script function $RUN_CHAR_SCRIPTS()");
              if (result.IsEmpty() || (result[0] != 'N'))
#ifdef newCombatant
              result = pCOMBATANT->m_pCharacter->RunCharacterScripts(
#else
              result = pCOMBATANT->RunCharacterScripts(
#endif
                              m_string1,
                              ScriptCallback_LookForChar,
                              "N",
                              "Script function $RUN_CHAR_SCRIPTS()");
            };
          }
          else
          {
            CHARACTER *pCharacter;
            pCharacter = m_popCharacter("$RUN_CHAR_SCRIPTS()");
            if (pCharacter != NULL)
            {
              result = pCharacter->RunCharacterScripts(
                              m_string1,
                              ScriptCallback_LookForChar,
                              "N",
                              "Script function $RUN_CHAR_SCRIPTS()");
            };
          };
#endif
          m_pushString1();
        };
        break;
      case SUBOP_RUN_CHAR_SE_SCRIPTS:
        {
          CString result;
          m_popString1();   // Script name
          m_popString2();
#ifdef UAFEngine
          {
            CHARACTER *pCharacter;
            ActorType actor = m_StringToActor((LPCSTR)m_string2);
          
            pCharacter = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor, "$RUN_CHAR_SE_SCRIPTS()");          
            if (pCharacter==NULL)
            {
              m_interpretError("Invalid target actor data in $RUN_CHAR_SE_SCRIPTS");
              m_pushSP(m_false);
              break;
            };
            m_string1 = pCharacter->RunSEScripts(
                                        m_string1,
                                        "Script function $RUN_CHAR_SE_SCRIPTS()");
          };
#endif
          m_pushString1();
        };
        break;
      case SUBOP_RUN_CHAR_PS_SCRIPTS:
        {
          CString result;
          m_popString1();   // Script name
          m_popString2();
#ifdef UAFEngine
          {
            CHARACTER *pCharacter;
            COMBATANT *pCombatant;
            ActorType actor = m_StringToActor((LPCSTR)m_string2);
          
            pCharacter = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor, "$RUN_CHAR_PS_SCRIPTS()");          
#ifdef newCombatant
            if ((pCharacter==NULL) || ((pCombatant = pCharacter->m_pCombatant) == NULL))
#else
            if ((pCharacter==NULL) || ((pCombatant = pCharacter->GetCombatant()) == NULL))
#endif
            {
              m_interpretError("Invalid target actor data in $RUN_CHAR_PS_SCRIPTS");
              m_pushSP(m_false);
              break;
            };
            m_string1 = pCombatant->RunPSScripts(m_string1, "Script function $RUN_CHAR_PS_SCRIPTS()");
          };
#endif
          m_pushString1();
        };
        break;
      case SUBOP_RUN_AREA_SE_SCRIPTS:
        {
          CString result;
          m_popInteger1();  // Combatant #
          m_popString2();   // Script Name
#ifdef UAFEngine
          {
            COMBATANT *pCombatant;

            pCombatant = combatData.GetCombatant(m_Integer1);          
            if (pCombatant==NULL)
            {
              m_interpretError("Invalid target actor data in $RUN_AREA_SE_SCRIPTS");
              m_pushSP(m_false);
              break;
            };
            m_string1 = activeSpellList.RunSEScripts(
                               pCombatant->x, 
                               pCombatant->y, 
                               m_string2,
                               "Script function $RUN_AREA_SE_SCRIPTS()");
          };
#endif
          m_pushString1();
        };
        break;
      case SUBOP_ForEachPartyMember:
        {
          m_popString1(); // The script name
#ifdef UAFEngine
          m_popString2(); // The Special Ability name
          m_string3 = party.ForEachPartyMember(m_string2, m_string1);
          m_pushString3();
#endif
        };
        break;
      case SUBOP_ForEachPossession:
        m_popString1();  // The script name
        m_popString2();  // The actor
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          ActorType actor;
          actor = m_StringToActor((LPCSTR)m_string2);
          pCharacter = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor,"$ForEachPossession()");          
          if (pCharacter==NULL)
          {
            m_interpretError("Invalid target actor data in $ForEachPossession()");
            m_pushSP(m_false);
            break;
          };
          m_pushSP(pCharacter->ForEachPossession(m_string1));
#else
          m_pushString1();
#endif
        };
        break;
      case SUBOP_IsIdentified:
        m_popInteger1(); // The 'num'
        m_popInteger2(); // The 'key'
        m_popString1();  // The 'actor'
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          ActorType actor;
          actor = m_StringToActor((LPCSTR)m_string1);
          pCharacter = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor, "$IsIdentified()");          
          if (pCharacter==NULL)
          {
            m_interpretError("Invalid target actor data in $IsIdentifed");
            m_pushSP(m_false);
            break;
          };
          m_Integer1 = pCharacter->IsIdentified(m_Integer2, m_Integer1);
          m_pushInteger1();
#else
          m_pushSP(m_true);
#endif
        };
        break;
      case SUBOP_GetCombatantState:
#ifdef UAFEngine
        {
          COMBATANT *pCombatant;
          pCombatant = m_popCombatantActor();
          m_string1 = CombatantStateText[pCombatant->State()];
          m_pushString1();
        };
#endif
        break;
      case SUBOP_SET_PARTY_XY:
        m_popInteger1();
        m_popInteger2();
#ifdef UAFEngine
        InitiateTaskMessage_SetPartyXY(m_Integer2,m_Integer1);
#endif
        m_string1.Empty();
        m_pushString1();
        break;
      case SUBOP_REMOVE_SPELL_EFFECT:
        m_popString1();  // The script name
        m_popString2();  // The actor
#ifdef UAFEngine
        {
          CHARACTER *pCharacter;
          ActorType actor = m_StringToActor((LPCSTR)m_string2);
        
          pCharacter = /*(CHARACTER*)*/GetCurrentlyActiveContext(&actor, "$Remove_SPELL_EFFECT()");          
          if (pCharacter==NULL)
          {
            m_interpretError("Invalid target actor data in $REMOVE_SPELL_EFFECT()");
            m_pushSP(m_false);
            break;
          };
          m_string1 = pCharacter->RemoveSpellEffect(m_string1);
        };
#endif
        m_pushString1();
        break;
      case SUBOP_CALL_GLOBAL_SCRIPT:
        {
          CString result;
          m_popString1();  // Script name
          m_popString2();  // Special Ability name
#ifdef UAFEngine
          m_string1 = RunGlobalScript(m_string2, m_string1, false);
#endif
          m_pushString1();
        };
        break;
      case SUBOP_SAY:
        if (m_pGPDLevent == NULL)
        {
          break;
        };
        m_popString1();
#ifdef UAFEngine
        m_discourseText.SetNewTop();
        m_discourseText.SetText(m_string1, GetTextBoxCharWidth());
        m_discourseText.Display();
        m_state=GPDL_WAIT_ACK;
#endif        
        m_pushSP(m_false); // All functions must return a value!
        INTERP_RETURN(GPDL_WAIT_ACK);
      case SUBOP_SET_CHAR_AC:          SET_CHAR_INT(SetAC,"$SET_CHAR_AC()");            break;
      case SUBOP_SET_CHAR_HITPOINTS:   SET_CHAR_INT(SetHitPoints,"$SET_CHAR_HITPOINTS()");      break;
      case SUBOP_SET_CHAR_THAC0:       SET_CHAR_INT(SetTHAC0,"$SET_CHAR_THAC0()");          break;
      case SUBOP_SET_CHAR_RDYTOTRAIN:  SET_CHAR_BOOL(SetReadyToTrain,"$SET_CHAR_RDYTOTRAIN()");     break;
      case SUBOP_SET_CHAR_Exp:             
        {
          m_popInteger1();  // New Experience
          m_popString1(); // class
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            pChar = m_popCharacter("$SET_CHAR_Exp()");
            BASECLASS_ID baseclassID;
            baseclassID = m_string1;
            //pChar->SetCurrExp(m_string1, m_Integer1); break;
            pChar->SetCurrExp(baseclassID, m_Integer1); 
			break;
          };
#else
          m_popInteger1();
#endif
          // Unreachable m_pushInteger1();  // Have to provide an answer!
        };
        break;


      case SUBOP_SET_CHAR_Lvl:             
        {
          m_popInteger1(); //new level
          m_popString1(); // class
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            pChar = m_popCharacter("$SET_CHAR_:v;()");
            BASECLASS_ID baseclassID;
            baseclassID = m_string1;
            //pChar->SetLevel(m_string1, m_Integer1);
            pChar->SetLevel(baseclassID, m_Integer1);
          };
#else
          m_popInteger1();
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_GET_CHAR_MAXHITPOINTS:
        {
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            pChar = m_popCharacter("$GET_CHAR_MAXHITPOINTS()");
            m_Integer1 = pChar->GetMaxHitPoints();
          };
#else
          m_popInteger1();
#endif
          m_pushInteger1();
        };
        break;

      case SUBOP_SET_CHAR_MAXHITPOINTS:
        {
          m_popInteger1(); // new max
#ifdef UAFEngine
          {
            CHARACTER *pChar;
            pChar = m_popCharacter("$SET_CHAR_MAXHITPOINTS()");
            pChar->SetMaxHitPoints(m_Integer1);
          };
#else
          m_popString1();
#endif
          m_pushInteger1();
        };
        break;

      case SUBOP_SET_CHAR_ICON_INDEX:    SET_CHAR_INT(SetIconIndex,"$SET_CHAR_ICON_INDEX()");               break;
      case SUBOP_SET_CHAR_AGE:           SET_CHAR_INT(SetAge,"$SET_CHAR_AGE()");                            break;
      case SUBOP_SET_CHAR_MAXAGE:        SET_CHAR_INT(SetMaxAge,"$SET_CHAR_MAXAGE()");                      break;
      case SUBOP_SET_CHAR_MAXMOVE:       SET_CHAR_INT(SetMaxMovement,"$SET_CHAR_MAXMOVE()");                break;
      case SUBOP_SET_CHAR_PERM_STR:      SET_CHAR_INT(SetPermStr,"$SET_CHAR_PERM_STR()");                   break;
      case SUBOP_SET_CHAR_PERM_STRMOD:   SET_CHAR_INT(SetPermStrMod,"$SET_CHAR_PERM_STRMOD()");             break;
      case SUBOP_SET_CHAR_PERM_INT:      SET_CHAR_INT(SetPermInt,"$SET_CHAR_PERM_INT()");                   break;
      case SUBOP_SET_CHAR_PERM_WIS:      SET_CHAR_INT(SetPermWis,"$SET_CHAR_PERM_WIS()");                   break;
      case SUBOP_SET_CHAR_PERM_DEX:      SET_CHAR_INT(SetPermDex,"$SET_CHAR_PERM_DEX()");                   break;
      case SUBOP_SET_CHAR_PERM_CON:      SET_CHAR_INT(SetPermCon,"$SET_CHAR_PERM_CON()");                   break;
      case SUBOP_SET_CHAR_PERM_CHA:      SET_CHAR_INT(SetPermCha,"$SET_CHAR_PERM_CHA()");                   break;
      case SUBOP_SET_CHAR_MAXENC:        SET_CHAR_INT(SetMaxEncumbrance,"$SET_CHAR_MAXENC()");              break;
      case SUBOP_SET_CHAR_SEX:           SET_CHAR_STRING(SetGender,"$SET_CHAR_SEX()");                      break;
      case SUBOP_SET_CHAR_GENDER:        SET_CHAR_STRING(SetGender,"$SET_CHAR_GENDER()");                   break;
      case SUBOP_SET_CHAR_ALIGNMENT:     SET_CHAR_ENUM(SetAlignment, alignmentType,"$SET_CHAR_ALIGNMENT()");break;
      case SUBOP_SET_CHAR_STATUS:        SET_CHAR_ENUM(SetStatus,charStatusType,"$SET_CHAR_STATUS()");      break;
      case SUBOP_SET_CHAR_UNDEAD:        SET_CHAR_STRING(SetUndead,"$SET_CHAR_UNDEAD()");                   break;
      case SUBOP_SET_CHAR_SIZE:          SET_CHAR_ENUM(SetSize, creatureSizeType,"$SET_CHAR_SIZE()");       break;
      case SUBOP_SET_CHAR_MAGICRESIST:   SET_CHAR_INT(SetMagicResistance,"$SET_CHAR_<AGICRESIST()");        break;
      case SUBOP_SET_CHAR_MORALE:        SET_CHAR_INT(SetMorale,"$SET_CHAR_MORALE()");                      break;
      case SUBOP_SET_CHAR_DAMAGEBONUS:   SET_CHAR_INT(SetDmgBonus,"$SET_CHAR_DAMAGEBONUS()");               break;
      case SUBOP_SET_CHAR_HITBONUS:      SET_CHAR_INT(SetHitBonus,"$SET_CHAR_HITBONUS()");                  break;
      case SUBOP_SetMemorizeCount:       m_SetMemorizeCount();                                              break;   
      case SUBOP_GetHighestLevelBaseclass:
                                         m_GetHighestLevelBaseclass();                                      break;
      case SUBOP_GetBaseclassLevel:      m_GetBaseclassLevel();                                             break;

      case SUBOP_SET_CHAR_ASL:
        {
          CHARACTER *pChar;
          m_popString2();  // value
          m_popString1();  // key
          pChar = m_popCharacter("$GET_CHAR_ASL()"); // character number
#ifdef UAFEngine
          InsertCharacterASL(
            pChar,
            m_string1, 
            m_string2);
#endif
        };
        m_pushSP(m_string2);  // Must supply a result
        break;
      case SUBOP_MODIFY_CHAR_ATTRIBUTE:
        m_popString4();   // Source of adjustment (for identification)
        m_popString3();   // Text to display in character's stats
        m_popInteger3();  // duration
        m_popString2();   // time units
        m_popInteger2();  // amount to be added
        m_popString1();   // The attribute to be modified
        // Dude() gets this m_popString1();   // Ye Olde character
#ifdef UAFEngine
        Dude("$MODIFY_CHAR_ATTRIBUTE()")->AddTemporaryEffect(
                                   m_string1, 
                                   m_string2,
                                   m_Integer3,
                                   m_Integer2,
                                   m_string3,
                                   m_string4,
                                   "Script function $MODIFY_CHAR_ATTRIBUTE()");
#else
        m_popString1(); // The dude
#endif
        m_pushEmptyString();
        break;
      case SUBOP_REMOVE_CHAR_MODIFICATION:
#ifdef UAFEngine
        m_popString1(); // The search pattern
        m_string2 = (Dude("$REMOVE_CHAR_MODIFICTION()")->RemoveTemporaryEffect(m_string1))?"1":"";
        m_pushString2();
#else
        m_popString1();  // the pattern
        m_popString2();  // the dude
        m_pushEmptyString();
#endif
        break;  
      case SUBOP_GET_GAME_CURRLEVEL:   GET_LITERAL_INT(globalData.currLevel+1); break;
      //case SUBOP_GET_GAME_VERSION:     m_getGameValue(GAME_VERSION);   break;
      case SUBOP_GET_GAME_VERSION:
                  m_string1.Format("%1.8f", globalData.version);
                  m_pushString1();
                  break;
      case SUBOP_SET_GLOBAL_ASL:
        m_popString1();  // The value
        m_popString2();  // The key
        InsertGlobalASL(
          globalData,
          m_string2,
          m_string1);
        m_pushSP(m_string1);
//        m_dataStack[m_SP]=m_variables.Set(m_string2, m_string1);
        break;
      case SUBOP_DEPRECATED_SET_LEVEL_STATS_ASL:
        m_popString2();  // value
        m_popString1();  // key
        InsertLevelASL(
          globalData,
          GetCurrentLevel(),
          m_string1,
          m_string2);
//        m_attributeInsert(
//          globalData.levelInfo.stats[globalData.currLevel].level_attributes,
//          m_string1, 
//          m_string2);
        m_pushSP(m_false);  // Must supply a result
        break;
      case SUBOP_SET_LEVEL_STATS_ASL:
        m_popString2();  // value
        m_popString1();  // key
        m_popString3();  //Level
        if (m_string3 == "") m_Integer1 = GetCurrentLevel();
        else m_Integer1 = atoi(m_string3);
        InsertLevelASL(
          globalData,
          m_Integer1,
          m_string1,
          m_string2);
//        m_attributeInsert(
//          globalData.levelInfo.stats[globalData.currLevel].level_attributes,
//          m_string1, 
//          m_string2);
        m_pushSP(m_false);  // Must supply a result
        break;
      case SUBOP_GET_EVENT_Attribute:
        m_popString1();     // The Attribute name
        m_popInteger1();
#ifdef UAFEngine
        m_string2 = FindEventAttribute(m_Integer1, m_string1);
#else
        m_string2 = "!Found!";
#endif
        m_pushString2();
        break;
      //case SUBOP_GET_PARTY_FACING:     m_getPartyValue(PARTY_FACING);     break;
      case SUBOP_GET_PARTY_FACING:     GET_LITERAL_INT(party.facing);     break;
      case SUBOP_GET_PARTY_LOCATION:  
                m_string1.Format("/%d/%d/%d",
                         party.level+1, party.Posx, party.Posy);
                m_pushString1();
                break;
      //case SUBOP_SET_PARTY_FACING:     m_setPartyValue(PARTY_FACING);     break;
      case SUBOP_SET_PARTY_FACING:     
                m_popInteger1();
                party.facing = min(max(m_Integer1,FACE_NORTH), FACE_SE);
                break;
      //case SUBOP_GET_PARTY_DAYS:       m_getPartyValue(PARTY_DAYS);       break;
      case SUBOP_GET_PARTY_DAYS:       GET_LITERAL_INT(party.days);       break;
      //case SUBOP_SET_PARTY_DAYS:       m_setPartyValue(PARTY_DAYS);       break;
      case SUBOP_SET_PARTY_DAYS:       SET_LITERAL_INT(party.days);       break;
      //case SUBOP_GET_PARTY_HOURS:      m_getPartyValue(PARTY_HOURS);      break;
      case SUBOP_GET_PARTY_HOURS:      GET_LITERAL_INT(party.hours);      break;
      //case SUBOP_SET_PARTY_HOURS:      m_setPartyValue(PARTY_HOURS);      break;
      case SUBOP_SET_PARTY_HOURS:      SET_LITERAL_INT(party.hours);      break;
      //case SUBOP_GET_PARTY_MINUTES:    m_getPartyValue(PARTY_MINUTES);    break;
      case SUBOP_GET_PARTY_MINUTES:    GET_LITERAL_INT(party.minutes);    break;
      //case SUBOP_SET_PARTY_MINUTES:    m_setPartyValue(PARTY_MINUTES);    break;
      case SUBOP_SET_PARTY_MINUTES:    SET_LITERAL_INT(party.minutes);    break;
      //case SUBOP_GET_PARTY_TIME:       m_getPartyValue(PARTY_TIME);       break;
      case SUBOP_GET_PARTY_TIME:       GET_LITERAL_INT((party.days*24+party.hours)*60+party.minutes); 
                                                                          break;
      //case SUBOP_SET_PARTY_TIME:       m_setPartyValue(PARTY_TIME);       break;
      case SUBOP_SET_PARTY_TIME:       
                m_popInteger1();
                party.days    =m_Integer1/(24*60); 
                party.hours   =m_Integer1/60%24; 
                party.minutes =m_Integer1%60;
                m_pushEmptyString();
                break;
      //case SUBOP_GET_PARTY_ACTIVECHAR: m_getPartyValue(PARTY_ACTIVECHAR); break;
      case SUBOP_GET_PARTY_ACTIVECHAR:
            GET_LITERAL_INT(party.characters[party.activeCharacter].uniquePartyID); break;
      //case SUBOP_SET_PARTY_ACTIVECHAR: m_setPartyValue(PARTY_ACTIVECHAR); break;
      case SUBOP_SET_PARTY_ACTIVECHAR: 
                m_popInteger1();
                if (party.numCharacters>0) 
                {
                  party.activeCharacter = m_Integer1%party.numCharacters;
                }
                else 
                {
                  party.activeCharacter = 0;
                };
                m_pushEmptyString();
                break;
      case SUBOP_SET_PARTY_ASL:
        m_popString2();  // value
        m_popString1();  // key
//        m_attributeInsert(
//          party.party_attributes,
//          m_string1, 
//          m_string2);
        InsertPartyASL(
          party,
          m_string1, 
          m_string2);
        m_pushSP(m_string2);  // Must supply a result
        break;
      case SUBOP_SET_QUEST:
        {
          int j;
          char sign=' ', c;
          int value=0, oldValue;
          int valLen;
          m_string1 = m_popSP(); // Get value or increment
          m_string2 = m_popSP(); // Get quest name
          valLen = m_string1.GetLength();
          for (j=0; j<valLen; j++)
          {
            c=m_string1[j];
            if (c=='-') sign = '-';
            if (c=='+') sign = '+';
            if ( (c>='0') && (c<='9') ) value=value*10+c-'0';
          };
          if (sign != ' ')
          {
            oldValue = globalData.questData.GetStage(m_string2);
            if (sign == '+') value = oldValue + value;
            else value = oldValue - value;
          };
          globalData.questData.SetStage(m_string2, value);
          value = globalData.questData.GetStage(m_string2);
          m_string1.Format("%d",value);
          m_pushSP(m_string1);
        };
        break;
      case SUBOP_SMALL_PICTURE:
        m_popString1();
        m_pushString1(); // Must supply result.
        if (m_pGPDLevent == NULL) break;
        //if (m_pGPDLevent->pic.picType!=SmallPicDib) break;
        m_pGPDLevent->pic.FreePic();
        m_pGPDLevent->pic.picType = SmallPicDib;
        m_pGPDLevent->pic.filename=m_string1;
        m_pGPDLevent->pic.LoadPicSurfaces("");
        break;
      case SUBOP_Status:
        m_popString1();
        m_string2 = m_Status(m_string1);
        m_pushString2();
        break;
      case SUBOP_SWAP:
        m_popString1();
        m_popString2();
        m_pushString1();
        m_pushString2();
        break;

/*      case SUBOP_TARGETINDEX:
        {
          m_string1 = pScriptContext->GetTargetContext();
          if (m_string1.IsEmpty())
          {
            MsgBoxError("$TargetIndex() called when no target context exists","Error",2);
          }
          else
          {
            m_string1 = m_IndexOf(m_string1);
          };
          m_pushString1();
        };
        break;
*/
      case SUBOP_MYINDEX:
        m_string1 = m_IndexOf(m_Myself());
        m_pushString1();
        break;

      case SUBOP_TargetContext:
        m_string1 = pScriptContext->GetTargetContext("$TargetContext() called when no target context exists");
        m_pushString1();
        break;
      case SUBOP_AttackerContext:
        m_string1 = pScriptContext->GetAttackerContext("$AttackerContext() called when no attacker context exists");
        m_pushString1();
        break;
      case SUBOP_CombatantContext:
        m_string1 = pScriptContext->GetCombatantContext("$CombatantContext() called when no combatant context exists");
        m_pushString1();
        break;
      case SUBOP_iEQUAL:
        m_popString1();
        m_popString2();
        if (LongCompare(m_string1, m_string2) == 0) 
               m_pushSP(m_true); else m_pushSP(m_false);
        break;
      case SUBOP_iGREATER:
        m_popString1();
        m_popString2();
        if (LongCompare(m_string1, m_string2) < 0) 
               m_pushSP(m_true); else m_pushSP(m_false);
        break;
      case SUBOP_iLESS:
        m_popString1();
        m_popString2();
        if (LongCompare(m_string1, m_string2) > 0) 
               m_pushSP(m_true); else m_pushSP(m_false);
        break;
      case SUBOP_iPLUS:
        m_popString1();
        m_popString2();
        m_string3 = LongAdd(m_string1, m_string2);
        m_pushString3();
        break;
      case SUBOP_iMINUS:
        m_popString1();
        m_popString2();
        m_string3 = LongSubtract(m_string2, m_string1);
        m_pushString3();
        break;
      case SUBOP_iDIV:
        m_popString1();
        m_popString2();
        LongDivide(m_string2, m_string1, m_string3, m_string4);
        m_pushString3();
        break;
      case SUBOP_iMOD:
        m_popString1();
        m_popString2();
        LongDivide(m_string2, m_string1, m_string3, m_string4);
        m_pushString4();
        break;
      case SUBOP_iTIMES:
        // n-digit multiplication.
        m_popString1();
        m_popString2();
        m_string3 = LongMultiply(m_string1, m_string2);
        m_pushString3();
        /*
        {
          int k, k1, carry;
          int i,j;
          unsigned char *t;  // Temporary space for all three strings.
          unsigned char *m, *n, *r;
          int len1, len2;
          bool error=false;
          m_string1.TrimLeft();
          m_string2.TrimLeft();
          m_string1.TrimRight();
          m_string2.TrimRight();
          len1=m_string1.GetLength();
          len2=m_string2.GetLength();
          t=(unsigned char *)alloca(len1+len2+(len1+len2));
          if (t==NULL) {
            MsgBoxError("$TIMES alloca failure");
            break;
          };
          m=t;
          n=t+len1;
          r=n+len2;
          for (i=0; i<len1; i++) {
            m[i]=(unsigned char)(m_string1.GetAt(i)-'0');
            if (m[i]>9) error=true;
          };
          for (i=0; i<len2; i++) {
            n[i]=(unsigned char)(m_string2.GetAt(i)-'0');
            if (n[i]>9) error=true;
          };
          if (error) {
            m_error_message("argument to $TIMES not decimal digits");
            break;
          };
          for (i=0; i<len1+len2; i++) r[i]=0;
          for (i=len1-1; i>=0; i--) { // each digit of multiplier
            for (j=len2-1; j>=0; j--) { // each digit of multiplicand
              k=i+j+1; // where answer goes.
              r[k]=(unsigned char)(r[k]+(m[i]*n[j]));
              k1=k;
              carry=0;
              while (r[k1]>=10) {
                carry=r[k1]/10;
                r[k1]=(unsigned char)(r[k1]%10);
                k1--;
                r[k1]=(unsigned char)(r[k1]+carry);
              };
            };
          };
          k=-1;
          for (i=0; i<len1+len2; i++) {
            r[i]+='0';
            if ((r[i]!='0')&&(k==-1))k=i; // first non-zero digit
          };
          if (k==-1) k=0;
          m_dataStack[m_SP]=CString((char *)r+k,len1+len2-k);
        };
        */
        break;
      case SUBOP_SetFriendly:
        {
          m_popInteger2();  // friendly adjustment
          m_popInteger1();  // combatant id
#ifdef UAFEngine
          COMBATANT *pCombatant;
          pCombatant = combatData.GetCombatant(m_Integer1);
          if (pCombatant == NULL)
          {
            m_Integer3 = 0;
          }
          else
          {
            m_Integer3 = pCombatant->m_adjFriendly;
            if ((m_Integer2 >= 0) && (m_Integer2 <= 3))
            {
              pCombatant->m_adjFriendly = m_Integer2;
            };
          };
#else
          m_Integer3 = 0;
#endif
          m_pushInteger3();
        };
        break;
      case SUBOP_GetFriendly:
        {
          m_popString2();  // data type (basic, adjustment, or adjusted)
          m_popInteger1();  // combatant id
#ifdef UAFEngine
          COMBATANT *pCombatant;
          pCombatant = combatData.GetCombatant(m_Integer1);
          if (pCombatant == NULL)
          {
            m_string3 = "Huh?";
            m_pushString3();
          }
          else
          {
            if      (m_string2 == "A") m_Integer3 = pCombatant->m_adjFriendly;
            else if (m_string2 == "B") m_Integer3 = pCombatant->friendly?1:0;
            else if (m_string2 == "F") m_Integer3 = pCombatant->GetIsFriendly()?1:0;
            else
            {
              m_string3 = "Huh?";
              m_pushString3();
              break;
            };
            m_pushInteger3();
          };
#else
          m_Integer3 = 0;
          m_pushInteger3();
#endif
        };
        break;
      case SUBOP_GetCombatRound:
        {
#ifdef UAFEngine
          m_Integer1 = GetCurrentRound();
#else
          m_Integer1 = 3;
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_TeleportCombatant:
        {
          m_popInteger3();              // y
          m_popInteger2();              // x
          m_popInteger1();              // id
#ifdef UAFEngine
          if (combatData.GetCombatant(m_Integer1) == NULL)
          {
            m_interpretError("Attempt to teleport illegal combatant");
          }
          else
          {
            combatData.GetCombatant(m_Integer1)->TeleportCombatant(m_Integer2, m_Integer3);
          };
#endif
        };
        m_string1 = emptyString;
        m_pushString1();
        break;
      case SUBOP_CombatantLocation:
        m_popString1();  // Axis
        m_popInteger1(); // Combatant ID
#ifdef UAFEngine
        if (    IsCombatActive()
             && (m_Integer1 < combatData.NumCombatants()))
        {
          if (m_string1 == "X")
          {
            m_Integer2 = combatData.GetCombatant(m_Integer1)->x;
          }
          else
          {
            m_Integer2 = combatData.GetCombatant(m_Integer1)->y;
          };
        }
        else
        {
          m_Integer2 = -1;
        };
#else
        m_Integer2 = 0;
#endif
        m_pushInteger2();
        break;
      case SUBOP_VisualDistance:
        m_popInteger1();
        m_popInteger2();
        m_Integer3 = 999999;
#ifdef UAFEngine
        if (IsCombatActive())
        {
          COMBATANT *pC1, *pC2;
          pC1 = combatData.GetCombatant(m_Integer1);
          pC2 = combatData.GetCombatant(m_Integer2);
          if ( (pC1 != NULL) && (pC2 != NULL) )
          {
            int x1, x2, y1, y2;
            x1 = pC1->x;
            x2 = pC2->x;
            y1 = pC1->y;
            y2 = pC2->y;
            if (HaveLineOfSight(x1, y1, x2, y2, NULL))
            {
              m_Integer3 = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
              m_Integer3 = sqrt((float)m_Integer3);
            };
          };
        };
#endif
        m_pushInteger3();
        break;
      case SUBOP_IsLineOfSight:
        m_popInteger5();
        m_popInteger4();
        m_popInteger3();
        m_popInteger2();
        m_popInteger1();
#ifdef UAFEngine
        if (IsLineOfSight(m_Integer1, m_Integer2, m_Integer3, m_Integer4))
        {
          m_string1 = "1";
        }
        else
        {
          m_string1 = "";
        };
#else
        m_string1 = "1";
#endif
        m_pushString1();
        break;
/*
      case SUBOP_DialogSelect:
        {
          for (i=0; i<MAX_DLG_CONTROLS; i++)
          {
            dialogControls[i] = "";
          };
          m_popInteger1();
          switch (m_Integer1)
          {
          case 0:
            gpdlDlg = 0;
            if (activeDlg != NULL) delete activeDlg;
            activeDlg = new GPDLDlg(IDD_GPDL, NULL);            
            break;
          };
        };
        m_pushInteger1();
        break;
      case SUBOP_DialogSet:
        {
          m_popString1();
          m_popUInteger1();
          switch(gpdlDlg)
          {
          default:
            {
              if (m_uInteger1 < MAX_DLG_CONTROLS)
              {
                dialogControls[m_uInteger1] = m_string1;
              };
            };
            break;
          case 999: break; // to shut up the compiler
          };
        };
        m_pushString1();
        break;
      case SUBOP_DialogDisplay:
        {
          ShowCursor(true);
          activeDlg->DoModal();
          ShowCursor(false);
        };
        m_string1 = "";
        m_pushString1();
        break;
      case SUBOP_DialogGet:
        {
          m_popUInteger1();
          if (m_uInteger1 <MAX_DLG_CONTROLS)
          {
            m_pushSP(dialogResults[m_uInteger1]);
          }
          else
          {
            m_pushSP(emptyString);
          };
        };
        break;
*/
      case SUBOP_ListAdjacentCombatants:
        m_popInteger1();  // The combatant in question
#ifdef UAFEngine
        m_string1 = combatData.ListAdjacentCombatants(m_Integer1, FALSE);
#else
        m_string1 = "|0";
#endif
        m_pushString1();
        break;
      case SUBOP_ComputeAttackDamage:
        {
          const COMBATANT *pAttacker;
          const COMBATANT *pTarget;
          int damage = 0;
          pTarget   = m_popCombatant("$ComputeAttackDamage()");
          pAttacker = m_popCombatant("$ComputeAttackDamage()");
          if ((pTarget!=NULL) && (pAttacker!=NULL))
          {
            damage = ComputeAttackDamage(pAttacker, pTarget);
          };
          m_Integer1 = damage;
          m_pushInteger1();
        };
        break;
      case SUBOP_ZERO:
        m_string1 = "0";
        m_pushString1();
        break;
      case SUBOP_UpCase:
        m_popString1();
        m_string1.MakeUpper();
        m_pushString1();
        break;
      case SUBOP_DownCase:
        m_popString1();
        m_string1.MakeLower();
        m_pushString1();
        break;
      case SUBOP_Capitalize:
        m_popString1();
        {
          bool ws = true;
          int len = m_string1.GetLength();
          m_string2 = m_string1;
          m_string2.MakeUpper();
          m_string1.MakeLower();
          for (int j=0; j<len; j++)
          {
            if (ws)
            {
              m_string1.SetAt(j, m_string2.GetAt(j)) ;
              ws = false;
            }
            if ( m_string1.GetAt(j) == ' ') ws = true;
          };
        };
        m_pushString1();
        break;
//   Readded 20160907 PRS /* Deprecated
      case SUBOP_GetSpellbook:
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          m_popString1();
          pCharacter = m_popCharacterActor();
          m_string1 = GetSpellbook(pCharacter, m_string1);
          m_pushString1();
#else
          m_popString1();
#endif
        };
        break;
//        */
      case SUBOP_SelectSpell:
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          SPELL_ID spellID;
          m_popString1();  // the spell name
          pCharacter = m_popCharacterActor();
          spellID = m_string1;
          m_string2 = SelectSpell(pCharacter, spellID);
          m_pushString2();
#else
          m_popString1();
#endif
        };
        break;
      case SUBOP_Memorize:
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          pCharacter = m_popCharacterActor();
          Memorize(pCharacter);
#else
          m_popString1();
#endif
          m_string2 = "OK";
          m_pushString2();
        };
        break;
/*  Replaced with version that uses uniqueID instead of party index
      case SUBOP_SetMemorizeCount:
        {
#ifdef UAFEngine
          m_popString1();
          m_popString3();
          m_popInteger1();
          m_Integer1 = SetMemorizeCount(m_Integer1, m_string3, m_string1, "$SetMemorizeCount()");
          m_pushInteger1();
#else
          m_popString1();
          m_popString2();
          m_popString3();
          m_string2 = "OK";
          m_pushString2();
#endif
        };
        break;
        */

      case SUBOP_KNOW_SPELL:
        {
#ifdef UAFEngine
          CHARACTER *pCharacter;
          SPELL_ID spellid;
          const SPELL_DATA *pSpell;
          m_popInteger1(); // Know or un-know
          m_popString1(); // the spellID
          pCharacter = m_popCharacterActor();
          spellid = m_string1;
          pSpell = spellData.PeekSpell(spellid);
          m_string2 = "1";
          if (pSpell == NULL)
          {
            m_string2.Empty();
          }
          else
          {
            if (!pCharacter->KnowSpellyyy(pSpell, m_Integer1 != 0))
            {
              m_string2.Empty();
            };
          };
#else
          m_popString1(); // 'know' flag
          m_popString1(); // spell name
          m_popString1(); // actor
          m_string2 = "1"; 
#endif
        };
        m_pushString2();
        break;

      case SUBOP_MonsterPlacement:
#ifdef UAFEngine
        m_popString1();
        m_string2 = MonsterPlacementCallback(m_string1);
        m_pushString2();
#endif
        break;


      case SUBOP_SetWall:
          m_popInts(5);
#ifdef UAFEngine
          SetMapOverride(WALL_OVERRIDE_USER, m_intArray);
#endif
          m_pushEmptyString();
          break;
      case SUBOP_SetBackground:
          m_popInts(5);
#ifdef UAFEngine
          SetMapOverride(BACKGROUND_OVERRIDE_USER, m_intArray);
#endif
          m_pushEmptyString();
          break;
      case SUBOP_SetOverlay:
          m_popInts(5);
#ifdef UAFEngine
          SetMapOverride(OVERLAY_OVERRIDE_USER, m_intArray);
#endif
          m_pushEmptyString();
          break;
      case SUBOP_SetBlockage:
          m_popInts(5);
#ifdef UAFEngine
          SetMapOverride(BLOCKAGE_OVERRIDE, m_intArray);
#endif
          m_pushEmptyString();
          break;
      case SUBOP_SetDoor:
          m_popInts(5);
#ifdef UAFEngine
          SetMapOverride(DOOR_OVERRIDE_USER, m_intArray);
#endif
          m_pushEmptyString();
          break;
      case SUBOP_GetWall:
          m_popInts(4);
#ifdef UAFEngine
          m_Integer1 = GetMapOverride(WALL_OVERRIDE_USER, m_intArray);
#endif
          m_pushInteger1();
          break;
      case SUBOP_GetBackground:
          m_popInts(4);
#ifdef UAFEngine
          m_Integer1 = GetMapOverride(BACKGROUND_OVERRIDE_USER, m_intArray);
#endif
          m_pushInteger1();
          break;
      case SUBOP_GetOverlay:
          m_popInts(4);
#ifdef UAFEngine
          m_Integer1 = GetMapOverride(OVERLAY_OVERRIDE_USER, m_intArray);
#endif
          m_pushInteger1();
          break;
      case SUBOP_GetBlockage:
          m_popInts(4);
#ifdef UAFEngine
          m_Integer1 = GetMapOverride(BLOCKAGE_OVERRIDE, m_intArray);
#endif
          m_pushInteger1();
          break;
      case SUBOP_GetDoor:
          m_popInts(5);
#ifdef UAFEngine
          m_Integer1 = GetMapOverride(DOOR_OVERRIDE_USER, m_intArray);
#endif
          m_pushInteger1();
          break;
      case SUBOP_COMBATANT_AVAILATTACKS:
#ifdef UAFEngine
        {
          COMBATANT *pCombatant;
          m_popInteger1(); // Function
          m_popInteger2(); // Value
          pCombatant = m_popCombatantActor();
          if (pCombatant != NULL)
          {
            switch (m_Integer1)
            {
            case 0:
              pCombatant->availAttacks =  m_Integer2; break;
            case 1:
              pCombatant->availAttacks += m_Integer2; break;
            };
            m_Integer1 = pCombatant->availAttacks;
            m_pushInteger1();
            break;
          }
          else
          {
            m_string1 = "";
          };
        };
#else
        m_string1 = "";
#endif
        m_pushString1();
        break;

      case SUBOP_IntegerTable:
        {
          m_popString1();  // The function
          m_popInteger2(); // The value
          m_popString3();  // The table name
          m_popString4();  // The Special Ability name
#ifdef UAFEngine
          m_Integer1 = IntegerTableLookup(m_string4, m_string3, m_Integer2, m_string1);
#else
          m_Integer1 = 1;
#endif
          m_pushInteger1();
          break;
        };
      case SUBOP_RollHitPointDice:
        {
          m_popInteger3(); // upper limit
          m_popInteger2(); // lower limit
          m_popString1(); // basclass name
#ifdef UAFEngine
          m_Integer1 = RollHitPointDice(m_string1, m_Integer2, m_Integer3);
#else
          m_Integer1 = 7;
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_AURA_Create: 
        {
#ifdef UAFEngine
          m_popString5();
          m_popString4();
          m_popString3();
          m_popString2();
          m_popString1();
          combatData.CreateAura(m_string1,m_string2,m_string3,m_string4,m_string5);
          break;
#endif
#ifdef UAFEDITOR
          m_popString1();
          m_popString1();
          m_popString1();
          m_popString1();
#endif
        };
        break;
      case SUBOP_AURA_Destroy:      AURA_FUNCTION($AURA_Destroy);   break;
      case SUBOP_AURA_AddSA:        AURA_FUNCTION($AURA_AddSA);     break;
      case SUBOP_AURA_GetSA:        AURA_FUNCTION($AURA_GetSA);     break;
      case SUBOP_AURA_RemoveSA:     AURA_FUNCTION($AURA_RemoveSA);  break;
      case SUBOP_AURA_Attach:       AURA_FUNCTION($AURA_Attach);    break;
      case SUBOP_AURA_Combatant:    AURA_FUNCTION($AURA_Combatant); break;
      case SUBOP_AURA_Wavelength:   AURA_FUNCTION($AURA_Wavelength);break;
      case SUBOP_AURA_Size:         AURA_FUNCTION($AURA_Size);      break;
      case SUBOP_AURA_Shape:        AURA_FUNCTION($AURA_Shape);     break;
      case SUBOP_AURA_Spell:        AURA_FUNCTION($AURA_Spell);     break;
      case SUBOP_AURA_Location:     AURA_FUNCTION($AURA_Location);  break;
      case SUBOP_AURA_GetData:      AURA_FUNCTION($AURA_GetData);   break;
      case SUBOP_AURA_SetData:      AURA_FUNCTION($AURA_SetData);   break;

      case SUBOP_GrSet:
        {
          m_popString3();
          m_popString2();
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrSet(m_string1, m_string2, m_string3);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrSetLinefeed:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrSetLinefeed(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrMoveTo:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrMoveTo(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrFormat:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrFormat(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrPrint:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrPrint(m_string1, 0, FALSE);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrPrtLF:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrPrtLF(m_string1, 0, FALSE);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrColor:
        {
          m_popString1();
          m_pushString1();
#ifdef UAFEngine
          m_popInteger1();
          m_string2 = GrColor(m_string1, m_Integer1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrMark:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrMark(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrMove:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrMove(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrTab:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrTab(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_GrPic:
        {
#ifdef UAFEngine
          m_popString1();
          m_string2 = GrPic(m_string1);
          m_pushString2();
#endif
        };
        break;
      case SUBOP_DAT_Race_Weight:
        {
          CHARACTER *pChar;
          pChar = m_popCharacterActor();
#ifdef UAFEngine
          m_Integer1 = DAT_Race_Weight(pChar);
#else
          m_Integer1 = 0;
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_DAT_Race_Height:
        {
          CHARACTER *pChar;
          pChar = m_popCharacterActor();
#ifdef UAFEngine
          m_Integer1 = DAT_Race_Height(pChar);
#else
          m_Integer1 = 0;
#endif
          m_pushInteger1();
        };
        break;
      case SUBOP_DAT_Item_CommonName:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2 = pItem->CommonName();
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_IDName:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2 = pItem->IdName();
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_Priority:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2.Format("%d", pItem->m_priorityAI);
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_MaxRange:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem == NULL)
          {
            m_string2.Empty();
          }
          else
          {
            m_string2.Format("%d", pItem->RangeMax);
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_MediumRange:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
#ifdef UAFEngine
          if (pItem != NULL)
          {
            m_string2.Format("%d", pItem->RangeMedium);
          };
#endif
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_ShortRange:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
#ifdef UAFEngine
          if (pItem != NULL)
          {
            m_string2.Format("%d", pItem->RangeShort);
          };
#endif
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_DamageSmall:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2.Format("$%d$%d$%d", pItem->Nbr_Dice_Sm, pItem->Dmg_Dice_Sm, pItem->Dmg_Bonus_Sm);
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_DamageLarge:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2.Format("$%d$%d$%d", pItem->Nbr_Dice_Lg, pItem->Dmg_Dice_Lg, pItem->Dmg_Bonus_Lg);
          };
          m_pushString2();
        };
        break;
      case SUBOP_DAT_Item_AttackBonus:
        {
          const ITEM_DATA *pItem;
          pItem = m_GetItemData();
          if (pItem != NULL)
          {
            m_string2.Format("%d", pItem->Attack_Bonus);
          };
          m_pushString2();
        };
        break;
      case SUBOP_GET_CONFIG:
        {
          m_popString1();
#ifdef UAFEngine
          FindConfigToken(m_string1, m_string2);
#else
          FindConfigToken(m_string1, m_string2);
#endif
          m_pushString2();
        };
        break;
      case SUBOP_SLEEP:
#ifdef UAFEngine
        Sleep(m_popInteger());
#endif
        m_pushEmptyString();
        break;
      case SUBOP_DRAWADVENTURESCREEN:
#ifdef UAFEngine
        UpdateAdventureScreen();
#endif
        m_pushEmptyString();
        break;
      default:
        {
          CString msg;
          msg.Format("Illegal subop code\n0x%08x",m_subop);
          m_interpretError(msg);
        };
        return BreakExecute();
      };
      break;
    default:
      {
        CString msg;
        msg.Format("Illegal opcode\n0x%02x",m_opcode);
        m_interpretError(msg);
      };
      return BreakExecute();
    };
  };
  // Some sort of error must have occurred.
  INTERP_RETURN(m_interpStatus);
}


bool GPDL::IsActive(void) {
  return (m_state==GPDL_WAIT_ACK)||(m_state==GPDL_WAIT_INPUT);
}

GPDL_STATE GPDL::State(void) {
  if (m_state==GPDL_WAIT_ACK) return m_state;
  if (m_state==GPDL_WAIT_INPUT) return m_state;
  if (m_state==GPDL_UNINITIALIZED) return m_state;
  if (m_state==GPDL_IDLE) return m_state;
  MsgBoxError("Internal GPDL::State error");
  return BreakExecute();

}

CString GPDL::indexName(unsigned int i) {
  return m_index.name(i);
}

GPDL_STATE GPDL::Display(void) {
  switch (m_state) {
  case GPDL_IDLE:
    return m_state;
  case GPDL_WAIT_INPUT:
  case GPDL_WAIT_ACK:
    m_discourseText.Display();
    return m_state;
  default:
    MsgBoxError("Internal GPDL::Display error");
  };
  return m_state;
}

unsigned int GPDL::numIndex(void) {
  return m_index.numEntry();
}

void GPDL::m_IllegalStateMessageAndQuit(void) {
  CString msg;
  CString PC;
  switch(m_state) {
  case GPDL_UNDER_RP:
    msg="Return stack underflow";
    break;
  case GPDL_OVER_RP:
    msg="Return stack overflow";
    break;
  case GPDL_UNDER_SP:
    msg="Data stack underflow";
    break;
  case GPDL_OVER_SP:
    msg="Data stack overflow";
    break;
  case GPDL_ILLPARAM:
    msg="Illegal parameter to system function";
    break;
  default:
    msg="Unknown illegal state";
  };
  PC.Format("\nPC = 0x%06x",m_PC);
  msg="GPDL interpreter\n"+msg+PC;
  MsgBoxError(msg);
  m_state=GPDL_IDLE;
  m_discourseText.Clear();
}


GPDL_STATE GPDL::Char(UINT c) {
  // Return GPDL_ACCEPTED if character accepted.
  switch (m_state) {
  case GPDL_UNINITIALIZED:
  case GPDL_IDLE:
    return GPDL_IDLE;
  case GPDL_WAIT_ACK:
    switch(c) {
    case CARRIAGE_RETURN:
      BreakExecute();
      break;
    case DOWN_ARROW:
      m_discourseText.downArrow();
      break;
    case UP_ARROW:
      m_discourseText.upArrow();
      break;
    case SPACE:
      m_status=m_interpret();
      if (  (m_status==GPDL_IDLE)
        ||(m_status==GPDL_WAIT_INPUT)
        ||(m_status==GPDL_WAIT_ACK) ) {
        m_state=m_status;
      } else {
        BreakExecute();
      };
      break;
    default:
      return GPDL_IGNORED;
    };
    return GPDL_ACCEPTED;
  case GPDL_WAIT_INPUT:
    switch(c) {
    case DOWN_ARROW:
      m_discourseText.downArrow();
      return GPDL_ACCEPTED;
    case UP_ARROW:
      m_discourseText.upArrow();
      return GPDL_ACCEPTED;
    case CARRIAGE_RETURN:
        m_listenText=m_discourseText.GetInput();
        m_pushSP(m_listenText);
        m_status=m_interpret();
        if (  (m_status==GPDL_IDLE)
          ||(m_status==GPDL_WAIT_INPUT)
          ||(m_status==GPDL_WAIT_ACK) ) {
          m_state=m_status;
        } else {
          BreakExecute();
        };
        return GPDL_ACCEPTED;
    default:
      return m_discourseText.Char(c);
    };
  case GPDL_UNDER_RP:
  case GPDL_OVER_RP:
  case GPDL_UNDER_SP:
  case GPDL_OVER_SP:
  case GPDL_ILLPARAM:
    m_IllegalStateMessageAndQuit();
    return GPDL_IGNORED;
//  case GPDL_WAIT_INPUT:
//    if (!m_discourseText.Char(c)) return GPDL_IGNORED;
//    switch(m_discourseText.State()) {
//    case GPDL_WAIT_INPUT:
//      m_state=GPDL_WAIT_INPUT;
//      return GPDL_ACCEPTED;
//    default:
//      MessageBox(NULL,"Internal GPDL::Char wait_input error",NULL,MB_OK|MB_TOPMOST);
//      return GPDL_IGNORED;
//    };
  default:
    MsgBoxError("Internal GPDL::Char error");
    return GPDL_IGNORED;
  };
}


GPDL_STATE GPDL::BreakExecute(void) {
  m_state=GPDL_IDLE;
  m_discourseText.Clear();
  return GPDL_IDLE;
}


GPDL::DISCOURSETEXT::DISCOURSETEXT(void) {
  m_MAXCHARS=500;
  m_firstLine=NULL;
  m_numLine=0;
  m_topLine=0;
  m_listening=false;
  m_pushStack = NULL;
}

GPDL::DISCOURSETEXT::~DISCOURSETEXT(void) {  
  LINE *FL;
  while (m_firstLine!=NULL) {
    FL=m_firstLine;
    m_firstLine=FL->m_nextLine;
    if (m_firstLine!=NULL) m_firstLine->m_prevLine=NULL;
    FL->m_nextLine=NULL;
    FL->m_prevLine=NULL;
    delete FL;
  };
  if (m_pushStack != NULL) NotImplemented(0xdca76, false);
}

void GPDL::DISCOURSETEXT::SetText(CString& text, unsigned int width) {
  m_format(text, width);
}

void GPDL::DISCOURSETEXT::StartInput(void) {
  char prompt[]="You say:   ";
  m_addLine(prompt);  // The line being input.
  m_promptLen=strlen(prompt);
  m_listening=true;
  m_topLine=m_numLine-1;
  Display();
}

void GPDL::DISCOURSETEXT::SetNewTop(void) {
  m_topLine=m_numLine;
}

void GPDL::DISCOURSETEXT::m_addLine(const char *line) {
  LINE *newline;
  LINE *LL; // LL=='L'ast  'L'ine
  newline=new LINE(line);
  m_numLine++;
  // Find last line in list.
  if (m_firstLine==NULL) 
  {
    m_firstLine=newline;
    if (pDiscourseLine->discourseLine.GetSize() == 0)
      pDiscourseLine->discourseLine.SetSize(TEXTBOX_LINES);
    return;
  };
  for (LL=m_firstLine; LL->m_nextLine!=NULL; LL=LL->m_nextLine) ;
  newline->m_prevLine=LL;
  LL->m_nextLine=newline;
  if (pDiscourseLine->discourseLine.GetSize() == 0)
    pDiscourseLine->discourseLine.SetSize(TEXTBOX_LINES);
}

CString GPDL::DISCOURSETEXT::GetInput(void) {
  LINE *LL;
  for (LL=m_firstLine; LL->m_nextLine!=NULL; LL=LL->m_nextLine);
  if (LL==NULL) return CString("");
  return LL->m_text.Right(LL->m_text.GetLength()-m_promptLen);
}

static bool whitespace(char c) {
  if (c==' ') return true;
  if (c=='\t') return true;
  if (c=='\n') return true;
  return false;
}

static bool controlChar(UINT c) {
  return c<0x20;
}

static bool systemChar(UINT c) {
  return c>0x7e;
}

class tempchar {
public:
  char *m_text;
  tempchar(unsigned int size) {m_text=(char *)My_malloc(size);};
  ~tempchar(void) {My_free(m_text);};
  char& operator[](int index) {return m_text[index];};
}; 

void GPDL::DISCOURSETEXT::m_format(CString& text, unsigned int width, bool colorTags)
{
// We want to format the 'text' so that it fits in a screen width of 'width'.
// We want to provide 'color-tag' capability ('/g' = green, etc.) in a scrolling
// environment.  So, for example, when there is a '/g/ in the first line but the
// scrolling area begins with the second line we want the second line to be green.
// Our solution is to remember the color as we reformat the text and to begin
// each line with a color-tag.
// We also need to accomocate text substitutions (^1= first character name, etc.).
// So we need to do the substitutions here in order to format the text correctly.
// All of this gets a bit complicated so we do it in two steps.
// 1) First we do any substitutions.  We make a copy of the text with the 
//    substitutions.
// 2) We go through the copy, deleting any (recursive) substitutions and measure
//    the characters and emit the result one line at a time.  We remember the
//    last color-tag in each emitted line and start the following line with that tag.
#ifdef UAFEngine
  char *textCopy = NULL, *outLine;
  const char *tagLast;
  const char *tagCur, *tagFit;
  const char *srcCur, *srcEnd, *srcFit= NULL;
  int outLineSize, charLen;
  unsigned int lineSize;
  int dstCur, dstFit;
  PreProcessText((LPCSTR)text, &textCopy);
  // There....that is step one.  char textCopy[] contains
  // the text with all substitutions completed.
  srcCur = textCopy;
  srcEnd = srcCur + strlen(srcCur);
  tagLast = "/w";
  outLineSize = 10;
  outLine = (char *) My_malloc(outLineSize);
  while (srcCur < srcEnd) // Fetch another line
  {
    dstCur = 0;
    dstFit = 0;
    outLine[dstCur++] = tagLast[0];
    outLine[dstCur++] = tagLast[1];
    tagFit = NULL;
    tagCur = NULL;
    lineSize = 0; //#pixels in current line
    while (srcCur < srcEnd)  // next character
    {
      if (outLineSize-dstCur < 3)
      {
        outLineSize = dstCur+100;
        outLine = (char *)My_realloc(outLine, outLineSize);
      };
      if (srcCur[0] == '/')
      {
        if (   (srcCur[1] == 'g')
            || (srcCur[1] == 'r')
            || (srcCur[1] == 'y')
            || (srcCur[1] == 'w')
            || (srcCur[1] == 'b')
            || (srcCur[1] == 'c'))
        {
          tagCur = srcCur;
          outLine[dstCur++] = *(srcCur++);
          outLine[dstCur++] = *(srcCur++);
          continue;
        };
      }
      else if (srcCur[0] == '^')
      {
        if (   ((srcCur[1] >= '1') && (srcCur[1] <= '8'))
            || ((srcCur[1] >= 'a') && (srcCur[1] <= 'z'))
            || (srcCur[1] == 'D')
            || (srcCur[1] == 'H')
            || (srcCur[1] == 'M') )
        {
          srcCur += 2;
          continue;
        };
      };
      if (srcCur[0] == '\n')
      {
        srcCur++;
        srcFit = srcCur;
        dstFit = dstCur;
        break;  // No more characters in this line
      };
      if (*srcCur == ' ')
      {
        srcFit = srcCur;
        dstFit = dstCur;
        tagFit = tagCur;
      };
#ifdef UAFEngine
      charLen=GraphicsMgr.GetTextWidth(srcCur,1);
#else
      charLen = 16;
#endif
      lineSize += charLen;
      if (lineSize > width)
      {
        break;
      };
      outLine[dstCur++] = *(srcCur++);
    };
    if (srcCur == srcEnd) 
    {
      srcFit = srcCur;
      dstFit = dstCur;
    };
    // Ready to output a line.
    if (dstFit > 0)
    {
      outLine[dstFit] = 0;
      srcCur = srcFit;
    }
    else
    {
      outLine[dstCur] = 0; // No breaks in the line
    };
    m_addLine(outLine);
    if (*srcCur == ' ') srcCur++; //If we broke at space then skip one space.
    if (tagFit != NULL) tagLast = tagFit;
  };
  if (outLine != NULL) My_free(outLine);
  if (textCopy != NULL) delete[] textCopy;
#else // if not UAFEngine
  m_addLine((LPCSTR)text);
#endif
}


/*
void GPDL::DISCOURSETEXT::m_format(CString& text, unsigned int width, bool colorTags)
{
  tempchar outline(m_MAXCHARS+2);
  unsigned int col, linelen, linePixelLen, line, end, linestart;
  unsigned int charLen, skipWhitespaceAtFrontOfLine=0;
  int wordstart;
  bool wordended;
  char c, c1;
  col=0;  // Where to start.
  linestart=col;
  line=0;               // Line we are assembling
  linelen=0;
  linePixelLen=0;
  wordstart=-1;    // No word started on this line.
  wordended=false; // No word has ended on this line.
  end=text.GetLength(); // Where we can quit
  skipWhitespaceAtFrontOfLine = 0;
  while (col < end) { 
    c = text.GetAt(col);
    if (colorTags && (c=='/'))
    {
      c1 = text.GetAt(col+1);
      if (   (c1 == 'g')
          || (c1 == 'r')
          || (c1 == 'y')
          || (c1 == 'w')
          || (c1 == 'b')
          || (c1 == 'c')
         )
      {
        outline[linelen++] = c;   //Do not include colorTags in length computation
        outline[linelen++] = c1;
        col += 2;
        continue; //while (col < end)
      };
    };
    if (c == '^')
    {
      c1 = text.GetAt(col+1);
      if (   ((c1 >= 'a') && (c1 <= 'z'))
          || ((c1 >= '1') && (c1 <= '8'))
          || (c1 == 'D')
          || (c1 == 'H')
          || (c1 == 'M')
          || (c1 == ' ')
         )
      {
        col += 2; // Delete any character or string substitutions
        continue; //while (col < end)
      };
    };
    if (c=='\n') 
    { // Force end-of-line
      // The whole line is a single word!
      // Nothing to do but break it up.
      skipWhitespaceAtFrontOfLine = 0;
      outline[linelen]=0;
      m_addLine(outline.m_text);
      wordstart=-1;
      wordended=false;
      linelen=0;
      linePixelLen=0;
      col++;
      continue;
    };
    if (whitespace(text.GetAt(col)))  
    {
      if (wordstart>=0) wordended=true;
      wordstart=-1;
      if ((linelen == 0) && (skipWhitespaceAtFrontOfLine > 0)) 
      {
        col++;
        skipWhitespaceAtFrontOfLine--;
        continue;  // Skip whitespace at front of line.
      };
#ifdef UAFEngine
      charLen=GraphicsMgr.GetTextWidth(" ",1);
#else
      charLen=16;
#endif
      if (linePixelLen+charLen<width && linelen<m_MAXCHARS) 
      {
        outline[linelen]=' ';
        linelen++;  // Whitespace in line.
        linePixelLen+=charLen;
        col++;
        continue;
      };
      wordstart=-1;
      wordended=false;
      outline[linelen]=0;
      m_addLine(outline.m_text);
      skipWhitespaceAtFrontOfLine = 1;
      line++;
      linelen=0;
      linePixelLen=0;
      continue;  // Whitespace at end of line.
    };
    // We have taken care of whitespace.  Now deal with non-blanks
    if (wordstart==-1) wordstart=col;
    outline[linelen]=text.GetAt(col); // Assume it will fit.
#ifdef UAFEngine
    charLen=GraphicsMgr.GetTextWidth(&outline[linelen],1);
#else
    charLen = 16;
#endif
    if (linePixelLen+charLen>=width && linelen<m_MAXCHARS) 
    {
      // Won't fit on this line.
      if (wordended==false) {
        // The whole line is a single word!
        // Nothing to do but break it up.
        outline[linelen]=0;
        m_addLine(outline.m_text);
        skipWhitespaceAtFrontOfLine = 1;
        wordstart=-1;
        wordended=false;
        linelen=0;
        linePixelLen=0;
        continue;
      };
      // So we can fall back to where the last word
      // started.
      linelen-=col-wordstart;
      col=wordstart;
      outline[linelen]=0;
      m_addLine(outline.m_text);
      skipWhitespaceAtFrontOfLine = 1;
      wordstart=-1;
      wordended=false;
      linelen=0;
      linePixelLen=0;
      continue;
    };
    if (linelen==0) linestart=col;
    outline[linelen++]=text.GetAt(col++);
    linePixelLen+=charLen;
  };
  if (linelen>0) 
  {
    outline[linelen]=0;
    m_addLine(outline.m_text);
  };
}
*/
void GPDL::DISCOURSETEXT::Display(void) {
  LINE *L;
  unsigned int i;
  L=m_firstLine;
//  if (m_numLine>m_MAXLINES) {
//    if (m_topLine>m_numLine-m_MAXLINES) {
//      m_topLine=m_numLine-m_MAXLINES;
//    };
//  };
  // First, we find the top line
  for (i=0; i < m_topLine; i++)  {
    if (L==NULL) break;
    L=L->m_nextLine;
  };
  for (i=0; i < (unsigned)pDiscourseLine->discourseLine.GetSize(); i++) {
    if (L==NULL) pDiscourseLine->discourseLine[i]=CString("");
    else {
      pDiscourseLine->discourseLine[i]=L->m_text;
      L=L->m_nextLine;
    };
  };
}

void GPDL::DISCOURSETEXT::Clear(void) {
  int i;
  LINE *FL;
  if (pDiscourseLine->discourseLine.GetSize() == 0)
    pDiscourseLine->discourseLine.SetSize(TEXTBOX_LINES);
  for (i=0; i<pDiscourseLine->discourseLine.GetSize(); i++) pDiscourseLine->discourseLine[i]="";
  while (m_firstLine!=NULL) {
    FL=m_firstLine;
    m_firstLine=FL->m_nextLine;
    FL->m_nextLine=NULL;
    FL->m_prevLine=NULL;
    delete FL;
  };
  m_topLine=0;
  m_numLine=0;
}

void GPDL::DISCOURSETEXT::Push(void)
{
  DISCOURSETEXT *pEntry;
  pEntry = new DISCOURSETEXT;
  pEntry->m_firstLine = m_firstLine;
  pEntry->m_listening = m_listening;
  pEntry->m_MAXCHARS  = m_MAXCHARS;
  pEntry->m_numLine   = m_numLine;
  pEntry->m_promptLen = m_promptLen;
  pEntry->m_pushStack = m_pushStack;
  pEntry->m_topLine   = m_topLine;
  m_pushStack = pEntry;
  pDiscourseLine->Push();
}

void GPDL::DISCOURSETEXT::Pop(void)
{
  DISCOURSETEXT *pEntry;
  pEntry      = m_pushStack;
  m_firstLine = pEntry->m_firstLine;
  m_listening = pEntry->m_listening;
  m_MAXCHARS  = pEntry->m_MAXCHARS;
  m_numLine   = pEntry->m_numLine;
  m_promptLen = pEntry->m_promptLen;
  m_pushStack = pEntry->m_pushStack;
  m_topLine   = pEntry->m_topLine;
  m_pushStack = pEntry->m_pushStack;
  delete pEntry;
  pDiscourseLine->Pop();
}

void GPDL::DISCOURSETEXT::downArrow(void) {
  if (m_topLine+pDiscourseLine->discourseLine.GetSize()>=m_numLine) return;
  m_topLine++;
  Display();
};

void GPDL::DISCOURSETEXT::upArrow(void) {
  if (m_topLine==0) return;
  m_topLine--;
  Display();
};

GPDL_STATE GPDL::DISCOURSETEXT::Char(UINT  c  ) {
  // Return GPDL_ACCEPTED if we gobble up the character
  LINE *LL;
  if (!m_listening) return GPDL_IGNORED;
  // Find the last line.
  for (LL=m_firstLine; LL->m_nextLine!=NULL; LL=LL->m_nextLine) ;
  if (LL==NULL) MsgBoxError("DISCOURSETEXT::Char internal error");
  if (c==BACKSPACE) LL->deleteLastChar(m_promptLen);
  else if (controlChar(c)) return GPDL_IGNORED;
  else if (systemChar(c)) return GPDL_IGNORED;
  else {
    LL->addOneChar(c);
  };
  Display();
  return GPDL_ACCEPTED;
}


GPDL::DISCOURSETEXT::LINE::LINE(const char *line) {
  m_text=CString(line);
  m_nextLine=NULL;
  m_prevLine=NULL;
}

GPDL::DISCOURSETEXT::LINE::~LINE(void) {
  // All work should have been done by ~DISCOURSETEXT
  if ((m_prevLine!=NULL) || (m_nextLine!=NULL)) {
    MsgBoxError("DISCOURSETEXT::LINE::~LINE internal error");
  };
}

void GPDL::DISCOURSETEXT::LINE::deleteLastChar(unsigned int backspaceLimit) {
  if (m_text.GetLength()>(int)backspaceLimit) {
    m_text=m_text.Left(m_text.GetLength()-1);
  };
}

void GPDL::DISCOURSETEXT::LINE::addOneChar(UINT c) {
  m_text+=(char)c;
}

GPDL::GLOBALS::GLOBALS (void) {
  m_value=NULL;
  m_allocatedSize=0;
}

GPDL::GLOBALS::~GLOBALS(void) {
  if (m_value!=NULL) delete []m_value;
  m_value=NULL;
  m_allocatedSize=0;
}

int GPDL::GLOBALS::read(CArchive& ar) {
  unsigned int i;
  if (m_value!=NULL) delete []m_value;
  m_value=NULL;
  ar >> m_allocatedSize;
  m_value=new CString[m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++) ar >> m_value[i];
  return 0;
}

int GPDL::GLOBALS::read(RAM_FILE& ar) {
  unsigned int i;
  if (m_value!=NULL) delete []m_value;
  m_value=NULL;
  ar >> m_allocatedSize;
  m_value=new CString[m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++) ar >> m_value[i];
  return 0;
}

#ifdef UAFEDITOR
int GPDL::GLOBALS::read(GPDLCOMP *pGPDL) {
  unsigned int i;
  if (m_value!=NULL) delete []m_value;
  m_value=NULL;
  m_allocatedSize = pGPDL->constantsSize(); 
  m_value=new CString[m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++)
  {
    m_value[i] = pGPDL->constantValue(i);
  };
  return 0;
}
#endif

GPDL::INDEX::INDEX(void) {
  m_name=NULL;
  m_value=NULL;
  m_allocatedSize=0;
}

GPDL::INDEX::~INDEX(void) {
  if (m_name!=NULL) delete []m_name;
  if (m_value!=NULL) delete []m_value;
  m_name=NULL;
  m_value=NULL;
  m_allocatedSize=0;
}

unsigned int GPDL::INDEX::lookup(const CString& funcName) {
  unsigned int i;
  for (i=0; i<m_allocatedSize; i++) {
    if (m_name[i]==funcName) return m_value[i];
  };
  return 0;
}

int GPDL::INDEX::read(CArchive& ar) {
  unsigned int i;
  if (m_name!=NULL) delete []m_name;
  if (m_value!=NULL) delete []m_value;
  m_name=NULL;
  m_value=NULL;
  ar >> m_allocatedSize;
  m_name=new CString [m_allocatedSize];
  m_value=new unsigned int [m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++) 
  {
    ar >> m_name[i]; m_name[i].TrimLeft(); m_name[i].TrimRight();
    ar >> m_value[i];
  };
  return 0;
}

int GPDL::INDEX::read(RAM_FILE& ar) {
  unsigned int i;
  if (m_name!=NULL) delete []m_name;
  if (m_value!=NULL) delete []m_value;
  m_name=NULL;
  m_value=NULL;
  ar >> m_allocatedSize;
  m_name=new CString [m_allocatedSize];
  m_value=new unsigned int [m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++) 
  {
    ar >> m_name[i]; m_name[i].TrimLeft(); m_name[i].TrimRight();
    ar >> m_value[i];
  };
  return 0;
}

#ifdef UAFEDITOR
int GPDL::INDEX::read(GPDLCOMP *pGPDL) {
  unsigned int i;
  if (m_name!=NULL) delete []m_name;
  if (m_value!=NULL) delete []m_value;
  m_name=NULL;
  m_value=NULL;
  m_allocatedSize = pGPDL->indexSize();
  m_name=new CString [m_allocatedSize];
  m_value=new unsigned int [m_allocatedSize];
  for (i=0; i<m_allocatedSize; i++) 
  {
    m_name[i] = pGPDL->indexName(i); 
    m_name[i].TrimLeft(); m_name[i].TrimRight();
    m_value[i] = pGPDL->indexValue(i);
  };
  return 0;
}
#endif

GPDL::ERRORLOG::ERRORLOG(void) {
  m_file=NULL;
  m_messagePrinted=false;
}

GPDL::ERRORLOG::~ERRORLOG(void) {
  if (m_file!=NULL) fclose(m_file);
}

void GPDL::ERRORLOG::log(const char *msg) 
{
  if (logDebuggingInfo)
  {
    WriteDebugString("GPDL LOG: %s\n", msg);
  };
  if (m_file==NULL) 
  {
    m_file=fopen("interp.log","w");

    if (m_file==NULL) 
    {
#ifdef UAFEDITOR
      if (m_messagePrinted==false) 
      {
        MsgBoxError("Cannot open interp.log");
        m_messagePrinted=true;
      };
#endif
      return;
    };
  };
  fprintf(m_file,"%s\n",msg);
}

void GPDL::ERRORLOG::log(CString& msg) {
  log((LPCTSTR)msg);
}

ActorType GPDL::m_StringToActor(const char *data)
{
  if (data==NULL)
  {
    return INVALID_ACTOR;
  };
  if (strlen(data)==0)
  {
    return INVALID_ACTOR;
  };
  ActorType actor = *((ActorType*)data);
  if (!actor.IsValidSrc()) 
  { 
    CString errmsg;
    errmsg.Format("Invalid source string \'%s\' in m_StringToActor()\n", data);
    errmsg += GetActorDebugText(actor);
    WriteDebugString(errmsg);
    //WriteStackTrace();
    return NULL_ACTOR; 
  }
  return actor;
}

CString GPDL::m_ActorToString(const ActorType *data)
{
  if (data==NULL) return m_false;
  m_string1 = CString((LPCTSTR)(data), sizeof (ActorType));
  return m_string1;
}

//CString GPDL::m_giIDToString(GLOBAL_ITEM_ID& giID)
CString GPDL::m_giIDToString(const ITEM_ID& itemID)
{
  //int id;
  //id = giID.IllegalConversion();
  //m_string1 = CString((LPCSTR)(&id), sizeof(int));
  //return m_string1;
  return itemID.UniqueName();
}

//CString GPDL::m_gsIDToString(GLOBAL_SPELL_ID& gsID)
CString GPDL::m_gsIDToString(const SPELL_ID& spellID)
{
  //int id;
  //id = gsID.IllegalConversion();
  //m_string1 = CString((LPCSTR)(&id), sizeof(int));
  //return m_string1;
  return spellID.UniqueName();
}

//GLOBAL_SPELL_ID GPDL::m_StringTogsID(const CString& str)
CString GPDL::m_StringTogsID(const CString& str)
{
  //int id;
  //GLOBAL_SPELL_ID gsID;
  //if (str.GetLength() < 4) return gsID;
  //id = *(int *)((LPCTSTR)str);
  //gsID.IllegalConversion(id);

  // A little shortcut to avoid a database lookup
  //if (itemContext.m_giID == giID) return itemContext.m_pItemData;
  //

  //return gsID;
  return str;
}



/*
ITEM_DATA *GPDL::m_StringToItemData(const CString& itemName)
{
  int id;
  GLOBAL_ITEM_ID giID;
  if (str.GetLength() < 4) return NULL;
  id = *(int *)((LPCTSTR)str);
  giID.IllegalConversion(id);

  // A little shortcut to avoid a database lookup
  //if (itemContext.m_giID == giID) return itemContext.m_pItemData;
  //

  return itemData.GetItemData(giID);
}
*/
CString GPDL::m_Myself(void)
{
  ActorType actor = GetCharContext();
  //  20171206 PRS Not needed because GetCharContext processes this error.  ASSERT(actor!=NULL_ACTOR);
  m_string1 = m_ActorToString(&actor);
  return m_string1;  
}
/*
CString GPDL::m_Target(void)
{
  static bool error = false;
  ActorType actor = GetTargetContext();
  if ((actor == NULL_ACTOR) && !error) 
  {
    WriteDebugString("GPDL Function Attempted $TargetIndex() but no target defined" );
    error = true;
  };
  m_string1 = m_ActorToString(&actor);
  return m_string1;  
}
*/
/*
void GPDL::m_Attacker(void)
{
#ifdef UAFEngine
  int i;
  CHARACTER *pCharacter = pScriptContext->GetAttackerContext();
  if (pCharacter != NULL)
  {
    if (IsCombatActive())
    {
      i = pCharacter->GetCombatant()->self;
    }
    else
    {
      i = pCharacter->uniquePartyID;
    };
    m_string1.Format("%i", i);
  }
  else
  {
    m_string1 = "";
  };
#else
  m_string1 = "";
#endif
}
*/
/*
void GPDL::m_Combatant(void)
{
#ifdef UAFEngine
  int i;
  COMBATANT *pCombatant = pScriptContext->GetCombatantContext();
  if (pCombatant != NULL)
  {
    if (IsCombatActive())
    {
      i = pCombatant->GetCombatant()->self;
      m_string1.Format("%i", i);
    }
    else
    {
      m_string1 = "";
    };
  }
  else
  {
    m_string1 = "";
  };
#else
  m_string1 = "";
#endif
}
*/
/*
CString GPDL::m_Item(void)
{
  return pScriptContext->GetItemContext()->Name;
}
*/
/*
CString GPDL::m_Spell(void)
{
  return pScriptContext->GetSpellContext()->Name;
}
*/
//GLOBAL_SPELL_ID GPDL::m_Spell(const CString& string)
//{
//  return m_StringTogsID(string);
//}

CString GPDL::m_Name(const char *name)
{
  ActorType actor;
  if (IsCombatActive())
  {
#ifdef UAFEngine
    for (int i=0; i<combatData.NumCombatants(); i++)
    {
      if (stricmp(name, combatData.GetCombatant(i)->GetName())==0)
      {
        combatData.GetContext(&actor, i);
        m_string1 = m_ActorToString(&actor);
        return m_string1;
      }
    }
#else
    m_string1 = m_ActorToString(&NULL_ACTOR);
    return m_string1;
#endif
  }
  else
  {
    for (int i=0;i<GetPartySize(party);i++)
    {
      if (stricmp(name, party.characters[i].GetName())==0)
      {
        party.characters[i].GetContext(&actor);
        m_string1 = m_ActorToString(&actor);
        return m_string1;
      }
    }
  }
  return m_false;
}

CString GPDL::m_Gender(CString &data)
{  
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor, "GPDL::m_Gender");
  if (dude==NULL) return m_false;
#ifdef UAFEngine
  return (CharGenderTypeText[dude->GetAdjGender()]);
#else
  return "Male";
#endif
}

CString GPDL::m_Class(CString &data)
{  
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_Class");
  if (dude==NULL) return m_false;
  //return (classData[dude->GetAdjClass()].m_name);
  return dude->GetAdjClass().UniqueName();
}

/*
CString GPDL::m_Race(CString &data)
{  
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor);
  if (dude==NULL) return m_false;
  return (raceData.GetRaceName(dude->race()));
}
*/
CString GPDL::m_Status(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_Status");
  if (dude==NULL) return m_false;
#ifdef UAFEngine
  return (CharStatusTypeText[dude->GetAdjStatus()]);
#else
  return "Okay";
#endif
}

CString GPDL::m_Alignment(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_Alignment");
  if (dude==NULL) return m_false;
#ifdef UAFEngine
  return (CharAlignmentTypeText[dude->GetAdjAlignment()]);
#else
  return "Chaotic Neutral";
#endif
}
CString GPDL::m_AlignmentGood(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_AlignmentGood");
  if (dude==NULL) return m_false;
  if (dude->IsAlignmentGood()) return m_true;
  return m_false;
}
CString GPDL::m_AlignmentEvil(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_AlignmentEvil");
  if (dude==NULL) return m_false;
  if (dude->IsAlignmentEvil()) return m_true;
  return m_false;
}
CString GPDL::m_AlignmentLawful(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_AlignmentLawful");
  if (dude==NULL) return m_false;
  switch (dude->GetAdjAlignment())
  {
  case LawfulEvil:
  case LawfulNeutral:
  case LawfulGood:
    return m_true;
  }
  return m_false;
}
CString GPDL::m_AlignmentNeutral(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_AlignmentNeutral");
  if (dude==NULL) return m_false;
  switch (dude->GetAdjAlignment())
  {
  case TrueNeutral:
  case LawfulNeutral:
  case ChaoticNeutral:
  case NeutralGood:
  case NeutralEvil:
    return m_true;
  }
  return m_false;
}
CString GPDL::m_AlignmentChaotic(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_AlignmentChaotic");
  if (dude==NULL) return m_false;
  switch (dude->GetAdjAlignment())
  {
  case ChaoticEvil:
  case ChaoticNeutral:
  case ChaoticGood:
    return m_true;
  }
  return m_false;
}
CString GPDL::m_HitPoints(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_HitPoints");
  if (dude==NULL) return CString("0");
  CString tmp;
  tmp.Format("%i", dude->GetAdjHitPoints());
  return tmp;
}
CString GPDL::m_InParty(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_InParty");
  if (dude==NULL) return m_false;
  return dude->IsPartyMember()?m_true:m_false;
}
CString GPDL::m_IsUndead(CString &data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_IsUndead");
  if (dude==NULL) return m_false;
  //return (dude->GetAdjUndeadType()!=NotUndead)?m_true:m_false;
  return (!dude->GetUndeadType().IsEmpty())?m_true:m_false;
}


CString GPDL::m_IndexToActor()
{
#ifdef UAFEngine
  if (IsCombatActive())
  {
    COMBATANT *pCombatant;
    int index;
    m_popInteger1();
    index = m_Integer1;
    if ((index < 0) || (index >= combatData.NumCombatants()))
    {
      if (!debugStrings.AlreadyNoted("FITAICI"))
      {
        MsgBoxInfo("Function IndexToActor(Illegal combatant index)", "Warning");
      };
    }
    else
    {
      ActorType actor;
      pCombatant = combatData.getCombatantPtr(index);
      pCombatant->GetContext(&actor);
      return m_ActorToString(&actor);
    };
    return "";
  }
  else
  {
    ActorType actor;
    CHARACTER *pChar;
    pChar = Dude("$IndexToActor()");
    pChar->GetContext(&actor);
    return m_ActorToString(&actor);
  };
#else
  return "";
#endif
}

CString GPDL::m_IndexOf(const CString& data)
{
  ActorType actor = m_StringToActor((LPCSTR)data);
  CString index("");
  // Instance is uniqueCharID for party, not 0..numCharacters!
  // Instance is 0..numCombatants during combat
//  if (actor == NULL_ACTOR)
//  {
  if (actor.instanceType == InstanceType_Invalid)
  {
    index = "Invalid Context";
  }
  else if (actor.instanceType == InstanceType_NewCombatant)
  {
    index.Format("%i", actor.Instance+NewCombatantInstanceOffset);
  }
  else
  {
    if (actor.Flags & FLAG_CREATED_CHARACTER)
    {
      return "-2";
    };
    index.Format("%i", actor.Instance); 
  };
//  }
//  else
//  {
//    index = -1;
//  };
  return index;
}

CString GPDL::m_IsCharAffectedBySpell(const CString &ActorString, const SPELL_ID &SpellID)
{
#ifdef UAFEngine
  ActorType actor = m_StringToActor((LPCSTR)ActorString);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_IsCharAffectedBySpell");
  if (dude==NULL) return m_false;
  return (dude->IsAffectedBySpell(SpellID)?m_true:m_false);
#else
  return m_false;
#endif
}
CString GPDL::m_IsCharAffectedBySpellAttribute(const CString &ActorString, const CString &SpellAttribName)
{
#ifdef UAFEngine
  ActorType actor = m_StringToActor((LPCSTR)ActorString);
  CHARACTER *dude = GetCurrentlyActiveContext(&actor,"GPDL::m_IsCharAffectedBySpellAttribute");
  if (dude==NULL) return m_false;
  return (dude->IsAffectedBySpellAttribute(SpellAttribName)?m_true:m_false);
#else
  return m_false;
#endif
}
int CleanNumber(const CString& num, char *digits)             //ARITHMETIC
{                                                             //ARITHMETIC
  // Get rid of leading zeroes, non-digits.                   //ARITHMETIC
  // Put a zero preceeded by a single sign character at front.//ARITHMETIC
  // Returns length of result.                                //ARITHMETIC
  // **Warning** you better provide a digits array at least   //ARITHMETIC
  // as long as the input string plus 2;                      //ARITHMETIC
  char c;                                                     //ARITHMETIC
  int col=2;                                                  //ARITHMETIC
  int len=num.GetLength();                                    //ARITHMETIC
  digits[0]='+';                                              //ARITHMETIC
  digits[1]=0;                                                //ARITHMETIC
  for (int i=0; i<len; i++)                                   //ARITHMETIC
  {                                                           //ARITHMETIC
    c=num[i];                                                 //ARITHMETIC
    if ((c>='0')&&(c<='9'))                                   //ARITHMETIC
      if ((c>='1') || (col!=2)) digits[col++]=c;              //ARITHMETIC
    if ((c=='-') && (col==2)) digits[0]='-';                  //ARITHMETIC
  };                                                          //ARITHMETIC
  return col;                                                 //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
static int LongPlus(const char *x,const char *y, char **rd)   //ARITHMETIC
{                                                             //ARITHMETIC
  // Add two positive numbers.                                //ARITHMETIC
  // return length of result.  Set *rd = sign of result;      //ARITHMETIC
  int len;                                                    //ARITHMETIC
  char *r=*rd;  // End+1 of result                            //ARITHMETIC
  char carry=0;                                               //ARITHMETIC
  while (*(x-1)!=0)                                           //ARITHMETIC
  {                                                           //ARITHMETIC
    carry += *(--x)-'0';                                      //ARITHMETIC
    if (*(y-1)!=0) carry += *(--y) - '0';                     //ARITHMETIC
    *(--r) = (carry%10) + '0';                                //ARITHMETIC
    carry /= 10;                                              //ARITHMETIC
  };                                                          //ARITHMETIC
  while (*(y-1)!=0)                                           //ARITHMETIC
  {                                                           //ARITHMETIC
    carry += *(--y) - '0';                                    //ARITHMETIC
    *(--r) = (carry%10) + '0';                                //ARITHMETIC
    carry /= 10;                                              //ARITHMETIC
  };                                                          //ARITHMETIC
  if (carry != 0) *(--r) = carry + '0';                       //ARITHMETIC
  if (r==*rd) *(--r)='0';                                     //ARITHMETIC
  *(--r)='+';                                                 //ARITHMETIC
  len=*rd-r;                                                  //ARITHMETIC
  *rd=r;                                                      //ARITHMETIC
  return len;                                                 //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
static int LongMinus(const char *x,const char *y, char **rd)  //ARITHMETIC
{                                                             //ARITHMETIC
  // Subtract two positive numbers.  *rd = x - y              //ARITHMETIC
  // return length of result.  Set *rd = sign of result;      //ARITHMETIC
  int len;                                                    //ARITHMETIC
  char *r=*rd;  // End+1 of result                            //ARITHMETIC
  char borrow=0;                                              //ARITHMETIC
  while (*(x-1)!=0)                                           //ARITHMETIC
  {                                                           //ARITHMETIC
    borrow += *(--x)-'0'+10;                                  //ARITHMETIC
    if (*(y-1)!=0) borrow -= *(--y) - '0';                    //ARITHMETIC
    *(--r) = (borrow%10) + '0';                               //ARITHMETIC
    borrow = borrow/10-1;                                     //ARITHMETIC
  };                                                          //ARITHMETIC
  while (*(y-1)!=0)                                           //ARITHMETIC
  {                                                           //ARITHMETIC
    borrow -= *(--y)-'0'-10;                                  //ARITHMETIC
    *(--r) = (borrow%10) + '0';                               //ARITHMETIC
    borrow = borrow/10-1;                                     //ARITHMETIC
  };                                                          //ARITHMETIC
  if (borrow<0)                                               //ARITHMETIC
  {  // We have a negative result.                            //ARITHMETIC
    char *d=*rd;                                              //ARITHMETIC
    borrow=0;                                                 //ARITHMETIC
    while (d!=r)                                              //ARITHMETIC
    {                                                         //ARITHMETIC
      borrow -= *(--d)-'0'-10;                                //ARITHMETIC
      *d=borrow%10+'0';                                       //ARITHMETIC
      borrow = borrow/10-1;                                   //ARITHMETIC
    };                                                        //ARITHMETIC
  };                                                          //ARITHMETIC
  // Now get rid of leading zeroes.                           //ARITHMETIC
  while (*r == '0') r++;                                      //ARITHMETIC
  if (r==*rd) *(--r)='0';  // Need one digit minimum          //ARITHMETIC
  *(--r) = borrow?'-':'+';                                    //ARITHMETIC
  len=*rd-r;                                                  //ARITHMETIC
  *rd=r;                                                      //ARITHMETIC
  return len;                                                 //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
CString LongAdd(const CString& x, const CString& y)           //ARITHMETIC
{                                                             //ARITHMETIC
  int xl, yl, rl;                                             //ARITHMETIC
  char *xd, *yd, *rd;                                         //ARITHMETIC
  xl=x.GetLength()+3;                                         //ARITHMETIC
  yl=y.GetLength()+3;                                         //ARITHMETIC
  rl=xl+yl+3;                                                 //ARITHMETIC
  xd=(char *)_alloca(xl+yl+rl+20);                            //ARITHMETIC
  yd=xd+xl;                                                   //ARITHMETIC
  rd=yd+yl+rl;                                                //ARITHMETIC
  xl=CleanNumber(x, xd);                                      //ARITHMETIC
  yl=CleanNumber(y, yd);                                      //ARITHMETIC
  if (*xd=='-')                                               //ARITHMETIC
  {                                                           //ARITHMETIC
    if (*yd=='-')                                             //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongPlus (xd+xl, yd+yl, &rd);                        //ARITHMETIC
      *rd='-';                                                //ARITHMETIC
    }                                                         //ARITHMETIC
    else                                                      //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongMinus (yd+yl, xd+xl, &rd);                       //ARITHMETIC
    };                                                        //ARITHMETIC
  }                                                           //ARITHMETIC
  else                                                        //ARITHMETIC
  {                                                           //ARITHMETIC
    if (*yd=='-')                                             //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongMinus(xd+xl, yd+yl, &rd);                        //ARITHMETIC
    }                                                         //ARITHMETIC
    else                                                      //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongPlus(xd+xl, yd+yl, &rd);                         //ARITHMETIC
    };                                                        //ARITHMETIC
  };                                                          //ARITHMETIC
  if (*rd=='+') return CString(rd+1,rl-1);                    //ARITHMETIC
  return CString(rd,rl);                                      //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
CString LongSubtract(const CString& x, const CString& y)      //ARITHMETIC
{                                                             //ARITHMETIC
  int xl, yl, rl;                                             //ARITHMETIC
  char *xd, *yd, *rd;                                         //ARITHMETIC
  xl=x.GetLength()+3;                                         //ARITHMETIC
  yl=y.GetLength()+3;                                         //ARITHMETIC
  rl=xl+yl+3;                                                 //ARITHMETIC
  xd=(char *)_alloca(xl+yl+rl+20);                            //ARITHMETIC
  yd=xd+xl;                                                   //ARITHMETIC
  rd=yd+yl+rl;                                                //ARITHMETIC
  xl=CleanNumber(x, xd);                                      //ARITHMETIC
  yl=CleanNumber(y, yd);                                      //ARITHMETIC
  if (*xd=='-')                                               //ARITHMETIC
  {                                                           //ARITHMETIC
    if (*yd=='-')                                             //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongMinus (yd+yl, xd+xl, &rd);                       //ARITHMETIC
    }                                                         //ARITHMETIC
    else                                                      //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongPlus (yd+yl, xd+xl, &rd);                        //ARITHMETIC
      *rd='-';                                                //ARITHMETIC
    };                                                        //ARITHMETIC
  }                                                           //ARITHMETIC
  else                                                        //ARITHMETIC
  {                                                           //ARITHMETIC
    if (*yd=='-')                                             //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongPlus(xd+xl, yd+yl, &rd);                         //ARITHMETIC
    }                                                         //ARITHMETIC
    else                                                      //ARITHMETIC
    {                                                         //ARITHMETIC
      rl=LongMinus(xd+xl, yd+yl, &rd);                        //ARITHMETIC
    };                                                        //ARITHMETIC
  };                                                          //ARITHMETIC
  if (*rd=='+') return CString(rd+1,rl-1);                    //ARITHMETIC
  return CString(rd,rl);                                      //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
int LongCompare(const CString& x, const CString& y)           //ARITHMETIC
{                                                             //ARITHMETIC
  CString result;                                             //ARITHMETIC
  result = LongSubtract(x, y);                                //ARITHMETIC
  if (result[0] == '-') return -1;                            //ARITHMETIC
  if (result[0]=='0') return 0;                               //ARITHMETIC
  return 1;                                                   //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
CString LongMultiply(const CString& s1, const CString& s2)    //ARITHMETIC
{                                                             //ARITHMETIC
/*  Test cases in the event that we need to fix something.    //ARITHMETIC
    We should check it out again.                             //ARITHMETIC
CString t1, t2;                                               //ARITHMETIC
                                                              //ARITHMETIC
LongDivide(CString("0"), CString("5"), t1, t2);               //ARITHMETIC
ASSERT(t1=="0");                                              //ARITHMETIC
ASSERT(t2=="0");                                              //ARITHMETIC
LongDivide(CString("5"), CString("0"), t1, t2);               //ARITHMETIC
ASSERT(t1=="999999");                                         //ARITHMETIC
ASSERT(t2=="0");                                              //ARITHMETIC
LongDivide(CString("9") , CString("5"), t1, t2);              //ARITHMETIC
ASSERT(t1=="1");                                              //ARITHMETIC
ASSERT(t2=="4");                                              //ARITHMETIC
LongDivide(CString("47"), CString("9"), t1, t2);              //ARITHMETIC
ASSERT(t1=="5");                                              //ARITHMETIC
ASSERT(t2=="2");                                              //ARITHMETIC
LongDivide(CString("123456"), CString("00987654321"), t1, t2);//ARITHMETIC
ASSERT(t1=="0");                                              //ARITHMETIC
ASSERT(t2=="123456");                                         //ARITHMETIC
LongDivide(CString("123456"), CString("00103"), t1, t2);      //ARITHMETIC
ASSERT(t1=="1198");                                           //ARITHMETIC
ASSERT(t2=="62");                                             //ARITHMETIC
LongDivide(CString("123456"), CString("000183"), t1, t2);     //ARITHMETIC
ASSERT(t1=="674");                                            //ARITHMETIC
ASSERT(t2="114");                                             //ARITHMETIC
LongDivide(CString("99"), CString("11"), t1, t2);             //ARITHMETIC
ASSERT(t1=="9");                                              //ARITHMETIC
ASSERT(t2=="0");                                              //ARITHMETIC
LongDivide(CString("613"), CString("614"), t1, t2);           //ARITHMETIC
ASSERT(t1=="0");                                              //ARITHMETIC
ASSERT(t2=="613");                                            //ARITHMETIC
LongDivide(CString("-5"),CString("3"), t1, t2);               //ARITHMETIC
ASSERT(t1=="-1");                                             //ARITHMETIC
ASSERT(t2=="-2");                                             //ARITHMETIC
LongDivide(CString("5"),CString("-3"), t1, t2);               //ARITHMETIC
ASSERT(t1=="-1");                                             //ARITHMETIC
ASSERT(t2=="2");                                              //ARITHMETIC
LongDivide(CString("5"),CString("-6"), t1, t2);               //ARITHMETIC
ASSERT(t1=="0");                                              //ARITHMETIC
ASSERT(t2=="5");                                              //ARITHMETIC
LongDivide(CString("-44"), CString("11"), t1, t2);            //ARITHMETIC
ASSERT(t1=="-4");                                             //ARITHMETIC
ASSERT(t2=="0");                                              //ARITHMETIC
LongDivide(CString("-678"), CString("-22"), t1, t2);          //ARITHMETIC
ASSERT(t1=="30");                                             //ARITHMETIC
ASSERT(t2=="-18");                                            //ARITHMETIC
*/                                                            //ARITHMETIC
                                                              //ARITHMETIC
  int xl, yl, rl;                                             //ARITHMETIC
  int i, j, c, p;                                             //ARITHMETIC
  char *x, *y, *r, *s, *t;                                    //ARITHMETIC
  char *xd, *yd, *rd;                                         //ARITHMETIC
  xl=s1.GetLength()+4; // 3 should suffice                    //ARITHMETIC
  yl=s2.GetLength()+4; // 3 should suffice                    //ARITHMETIC
  rl=xl+yl+3; //0 should suffice                              //ARITHMETIC
  xd=(char *)_alloca(xl+yl+rl+20);                            //ARITHMETIC
  yd=xd+xl;                                                   //ARITHMETIC
  rd=yd+yl;                                                   //ARITHMETIC
  xl=CleanNumber(s1, xd);                                     //ARITHMETIC
  yl=CleanNumber(s2, yd);                                     //ARITHMETIC
  rl=xl+yl-4;                                                 //ARITHMETIC
  memset(rd+1,'0',rl);                                        //ARITHMETIC
  r=rd+rl+1;                                                  //ARITHMETIC
  *r=0;                                                       //ARITHMETIC
  x=xd+xl;                                                    //ARITHMETIC
  for (i=2; i<xl; i++)                                        //ARITHMETIC
  {                                                           //ARITHMETIC
    x--;                                                      //ARITHMETIC
    y=yd+yl;                                                  //ARITHMETIC
    s=r--;                                                    //ARITHMETIC
    for (j=2; j<yl; j++)                                      //ARITHMETIC
    {                                                         //ARITHMETIC
      t=--s;                                                  //ARITHMETIC
      p=((*x)-'0') * ((*(--y))-'0');                          //ARITHMETIC
      c=p/10;                                                 //ARITHMETIC
      *t += p%10;                                             //ARITHMETIC
      if (*t > '9')                                           //ARITHMETIC
      {                                                       //ARITHMETIC
        *t -= 10;                                             //ARITHMETIC
        c++;                                                  //ARITHMETIC
      };                                                      //ARITHMETIC
      while (c)                                               //ARITHMETIC
      {                                                       //ARITHMETIC
        *(--t) += c;                                          //ARITHMETIC
        c = 0;                                                //ARITHMETIC
        if (*t > '9')                                         //ARITHMETIC
        {                                                     //ARITHMETIC
          *t -= 10;                                           //ARITHMETIC
          c++;                                                //ARITHMETIC
        };                                                    //ARITHMETIC
      };                                                      //ARITHMETIC
    };                                                        //ARITHMETIC
  };                                                          //ARITHMETIC
  if (rd[1] == '0')                                           //ARITHMETIC
  {                                                           //ARITHMETIC
    rd++; //skip leading zero.                                //ARITHMETIC
  };                                                          //ARITHMETIC
  if (rd[1] != 0)                                             //ARITHMETIC
  {                                                           //ARITHMETIC
    *rd='-';                                                  //ARITHMETIC
    if (!(((*xd)=='-') ^ ((*yd)=='-'))) rd++;                 //ARITHMETIC
  };                                                          //ARITHMETIC
  return CString(rd);                                         //ARITHMETIC
}                                                             //ARITHMETIC
                                                              //ARITHMETIC
void LongDivide(const CString& dividend,                      //ARITHMETIC
                const CString& divisor,                       //ARITHMETIC
                CString& quotient,                            //ARITHMETIC
                CString& remainder)                           //ARITHMETIC
{                                                             //ARITHMETIC
  int b, xl = dividend.GetLength(), yl = divisor.GetLength(); //ARITHMETIC
  char *xd = (char *)_alloca(xl+yl+20);                       //ARITHMETIC
  char *yd = xd+xl+10, *rc, *yb;                              //ARITHMETIC
  xl = CleanNumber(dividend, xd+2);                           //ARITHMETIC
  xd[xl+2]=0;                                                 //ARITHMETIC
  yl = CleanNumber(divisor, yd);                              //ARITHMETIC
  if (yl == 2) {quotient="999999"; remainder="0"; return;};   //ARITHMETIC
  xd[0] = xd[2]; xd[2] = 0; xd[1] = xd[3]; xd[3] = '0';       //ARITHMETIC
  if (yl > xl) {quotient="0"; remainder=dividend; return;};   //ARITHMETIC
  char *rb=xd+yl+1, *q=xd;                                    //ARITHMETIC
  while (*(rb++)!=0)                                          //ARITHMETIC
  {                                                           //ARITHMETIC
    *(++q)='0'-1;                                             //ARITHMETIC
    do                                                        //ARITHMETIC
    {                                                         //ARITHMETIC
      yb=yd+yl; b=0; rc=rb;                                   //ARITHMETIC
      while (*(--yb)!=0)                                      //ARITHMETIC
      {                                                       //ARITHMETIC
        *(--rc)-=*yb-'0'+b;                                   //ARITHMETIC
        if (*rc<'0') {*rc+=10; b=1;} else b=0;                //ARITHMETIC
      };                                                      //ARITHMETIC
      *(--rc)-=b; (*q)++;                                     //ARITHMETIC
    } while (*rc>='0');                                       //ARITHMETIC
    yb=yd+yl; b=0; rc=rb;                                     //ARITHMETIC
    while (*(--yb)!=0)                                        //ARITHMETIC
    {                                                         //ARITHMETIC
      *(--rc)+=*yb-'0'+b;                                     //ARITHMETIC
      if (*rc>'9') {*rc-=10; b=1;} else b=0;                  //ARITHMETIC
    };                                                        //ARITHMETIC
    *(--rc)+=b;                                               //ARITHMETIC
  };                                                          //ARITHMETIC
  for (b=3+xl-yl; b<=xl; b++) if (xd[b] != '0') break;        //ARITHMETIC
  xd[b-1] = xd[0];                                            //ARITHMETIC
  if ((xd[b-1]=='-')&&(xd[b]!='0'))b--;                       //ARITHMETIC
  remainder=xd+b;                                             //ARITHMETIC
  q[1]=0;                                                     //ARITHMETIC
  q=xd+(((xl!=yl)&&(xd[1]=='0'))?1:0);                        //ARITHMETIC
  *q = (*xd=='-')^(*yd=='-')?'-':'+';                         //ARITHMETIC
  if ((q[0]=='+')||(q[1]=='0'))q++;                           //ARITHMETIC
  quotient = q;                                               //ARITHMETIC
}                                                             //ARITHMETIC

RDREXEC::RDREXEC(void)                                                  //RDR
{                                                                       //RDR
  m_code = "";                                                          //RDR
}                                                                       //RDR
                                                                        //RDR
RDREXEC::~RDREXEC(void)                                                 //RDR
{                                                                       //RDR
}                                                                       //RDR
                                                                        //RDR
int RDREXEC::m_getChar(const char **str)                                //RDR
{                                                                       //RDR
  int result;                                                           //RDR
  result = **str;                                                       //RDR
  *str += 1;                                                            //RDR
  return result;                                                        //RDR
}                                                                       //RDR
                                                                        //RDR
int RDREXEC::m_getInteger(const char **str)                             //RDR
{                                                                       //RDR
  COPS op;                                                              //RDR
  int result;                                                           //RDR
  op = (COPS)(**str);                                                   //RDR
  *str += 1;                                                            //RDR
  if (op == COP_INTEGER1)                                               //RDR
  {                                                                     //RDR
    result = **str;                                                     //RDR
    *str += 1;                                                          //RDR
  }                                                                     //RDR
  else if (op == COP_INTEGER2)                                          //RDR
  {                                                                     //RDR
    result = *((*str)+1);  // fill with proper sign                     //RDR
    result = ((*(int *)(*str)) & 0xffff) | (result & 0xffff0000);       //RDR
    *str += 2;                                                          //RDR
  }                                                                     //RDR
  else if (op == COP_INTEGER3)                                          //RDR
  {                                                                     //RDR
    result = *((*str)+2);  // fill with proper sign                     //RDR
    result = ((*(int *)(*str)) & 0xffffff) | (result & 0xff000000);     //RDR
    *str += 3;                                                          //RDR
  }                                                                     //RDR
  else                                                                  //RDR
  {                                                                     //RDR
    ASSERT(op == COP_INTEGER4);                                         //RDR
    result = *((int *)(*str));                                          //RDR
    *str += 4;                                                          //RDR
  };                                                                    //RDR
  return result;                                                        //RDR
}                                                                       //RDR
                                                                        //RDR
#define C1 ASSERT(SP >= 1);                                             //RDR
#define C2 ASSERT(SP >= 2); SP--;                                       //RDR
#define IS ASSERT(SP < 38); SP++;                                       //RDR
#define OP2(op) C2 stk[SP] = stk[SP] op stk[SP+1]; break;               //RDR
                                                                        //RDR
int RDREXEC::InterpretExpression(const CString& expression,             //RDR
                              int (*interpretRDR)(int *array),          //RDR
                              int (*interpretRDRB)(const char *array))  //RDR
{                                                                       //RDR
  COPS op;                                                              //RDR
  int i, n, SP = 0, array[20], stk[40];                                 //RDR
  const char *expr = expression;                                        //RDR
  const char *end = expr + expression.GetLength();                      //RDR
  stk[SP] = -1;                                                         //RDR
  while (expr < end)                                                    //RDR
  {                                                                     //RDR
    op = (COPS)*(expr++);                                               //RDR
    switch (op)                                                         //RDR
    {                                                                   //RDR
    case COP_ILLEGAL: ASSERT(false); return -1;                         //RDR
    case COP_LOR:          OP2(||)                                      //RDR
    case COP_LAND:         OP2(&&)                                      //RDR
    case COP_ISEQUAL:      OP2(==)                                      //RDR
    case COP_ISNOTEQUAL:   OP2(!=)                                      //RDR
    case COP_LESS:         OP2(<)                                       //RDR
    case COP_LESSEQUAL:    OP2(<=)                                      //RDR
    case COP_GREATER:      OP2(>)                                       //RDR
    case COP_GREATEREQUAL: OP2(>=)                                      //RDR
    case COP_PLUS:         OP2(+)                                       //RDR
    case COP_TIMES:        OP2(*)                                       //RDR
    case COP_SLASH:                                                     //RDR
      C2                                                                //RDR
      if (stk[SP+1] == 0) stk[SP] = 0;                                  //RDR
      else stk[SP] = stk[SP]/stk[SP+1];                                 //RDR
      break;                                                            //RDR
    case COP_PERCENT:                                                   //RDR
      C2                                                                //RDR
      if (stk[SP+1] == 0) stk[SP] = 0;                                  //RDR
      else stk[SP] = stk[SP]%stk[SP+1];                                 //RDR
      break;                                                            //RDR
    case COP_BITOR:        OP2(|)                                       //RDR
    case COP_BITXOR:       OP2(^)                                       //RDR
    case COP_BITAND:       OP2(&)                                       //RDR
    case COP_MINUS:        OP2(-)                                       //RDR
    case COP_NEGATE: C1  stk[SP] = -stk[SP]; break;                     //RDR
    case COP_NOT:C1  stk[SP] = !stk[SP]; break;                         //RDR
    case COP_MIN:C2 stk[SP]=stk[SP]>stk[SP+1]?stk[SP]:stk[SP+1]; break; //RDR
    case COP_MAX:C2 stk[SP]=stk[SP]<stk[SP+1]?stk[SP]:stk[SP+1]; break; //RDR    
    case COP_RDR:                                                       //RDR
        array[0] = m_getInteger(&expr);                                 //RDR
        for (i=1; i<=array[0]; i++)                                     //RDR
        {                                                               //RDR
          array[i] = m_getInteger(&expr);                               //RDR
        };                                                              //RDR
        IS stk[SP] = (*interpretRDR)(array);                            //RDR
        break;                                                          //RDR
    case COP_RDR1:                                                      //RDR
        n = *expr;                                                      //RDR
        IS stk[SP] = (*interpretRDRB)(expr);                            //RDR
        expr += *expr+1;                                                //RDR
        break;                                                          //RDR
    case COP_INTEGER1:                                                  //RDR
    case COP_INTEGER2:                                                  //RDR
    case COP_INTEGER3:                                                  //RDR
    case COP_INTEGER4:                                                  //RDR
        expr--; IS  stk[SP] = m_getInteger(&expr); break;               //RDR
    case COP_LXOR:                                                      //RDR
        C2                                                              //RDR
        stk[SP] = stk[SP] && !stk[SP+1] || !stk[SP] && stk[SP+1];       //RDR
        break;                                                          //RDR
    default: ASSERT(false); // That should cause an error!              //RDR
             return -1;                                                 //RDR
    };                                                                  //RDR
  };                                                                    //RDR
  return stk[SP];                                                       //RDR
}                                                                       //RDR

//GPDL::VARIABLES::VARIABLES(void) {
//  m_firstVariable=NULL;
//}

//GPDL::VARIABLES::~VARIABLES(void) {
//  VARIABLE *v;
//  while (m_firstVariable!=NULL) {
//    v=m_firstVariable;
//    m_firstVariable=v->m_next;
//    v->m_next=NULL;
//    delete v;
//  };
//}


//CString GPDL::VARIABLES::Set(const CString& name, 
//                     const CString& value) {
//  VARIABLE *v;
//  for (v=m_firstVariable; v!=NULL; v=v->m_next) {
//    if (name==v->m_name) {
//      CString result;
//      result=v->m_value;
//      v->m_value=value;
//      return result;
//    };
//  };
//  v=new VARIABLE;
//  v->m_name=name;
//  v->m_value=value;
//  v->m_next=m_firstVariable;
//  m_firstVariable=v;
//  return m_false;
//}
//
//CString GPDL::VARIABLES::Get(const CString& name) {
//  VARIABLE *v;
//  for (v=m_firstVariable; v!=NULL; v=v->m_next) {
//    if (name==v->m_name) {
//      return v->m_value;
//    };
//  };
//
//  return m_false;
//}
//
//GPDL::VARIABLES::VARIABLE::VARIABLE(void) {
//}
//
//  
//GPDL::VARIABLES::VARIABLE::~VARIABLE(void) {
//  if (m_next!=NULL) {
//    MessageBox(NULL,
//             "GPDL::VARIABLES::VARIABLE::~VARIABLE error",
//           NULL,
//           MB_OK|MB_TOPMOST);
//    return;
//  };
//}