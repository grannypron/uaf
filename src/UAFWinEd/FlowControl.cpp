// FlowControl.cpp : implementation file
//

#include "stdafx.h"
#include "UAFWinEd.h"
#include "GameEvent.h"
#include "FlowControl.h"
#include "afxdialogex.h"
#include "resource.h"


// CFlowControl dialog

IMPLEMENT_DYNAMIC(CFlowControl, CDialog)

CFlowControl::CFlowControl(const FLOW_CONTROL_EVENT_DATA& event, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FlowControl, pParent)
  , m_destinationMarkerName(_T(""))
  , m_entryMarkerName(_T(""))
  , m_exitMarkerName(_T(""))
  , m_value(_T(""))
  , m_variableName(_T(""))
  , m_action(0)
  , m_condition(0)
  , m_manipulate(0)
  , m_localChainOnly(TRUE)
{
  m_pEvent = &event;
}

CFlowControl::~CFlowControl()
{
}

void CFlowControl::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //  DDX_Text(pDX, IDC_MarkerName, m_markerName);
  //  DDX_Radio(pDX, IDC_Action, m_action);
  //  DDX_Radio(pDX, IDC_Condition, m_condition);
  DDX_Text(pDX, IDC_DestinationMarkerName, m_destinationMarkerName);
  DDX_Text(pDX, IDC_EntryMarkerName, m_entryMarkerName);
  DDX_Text(pDX, IDC_ExitMarkerName, m_exitMarkerName);
  //  DDX_Radio(pDX, IDC_Manipulate, m_manupulate);
  DDX_Text(pDX, IDC_VALUE, m_value);
  DDX_Text(pDX, IDC_VariableName, m_variableName);
  DDX_Radio(pDX, IDC_Action, m_action);
  DDX_Radio(pDX, IDC_Condition, m_condition);
  DDX_Radio(pDX, IDC_Manipulate, m_manipulate);
  DDX_Check(pDX, IDC_Local, m_localChainOnly);
}


BEGIN_MESSAGE_MAP(CFlowControl, CDialog)
END_MESSAGE_MAP()


// CFlowControl message handlers


BOOL CFlowControl::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_action = 0;
  switch (m_pEvent->action)
  {
    case ACTION_NONE:   m_action = 0; break;
    case ACTION_GOTO:   m_action = 1; break;
    case ACTION_CALL:   m_action = 2; break;
    case ACTION_RETURN: m_action = 3; break;
    case ACTION_POP:    m_action = 4; break;
  }
  m_condition = 0;
  switch (m_pEvent->actionCondition)
  {
    case CONDITION_ALWAYS:    m_condition = 0; break;
    case CONDITION_EQUALS:    m_condition = 1; break;
    case CONDITION_NOTEQUALS: m_condition = 2; break;
  };
  m_manipulate = 0;
  switch (m_pEvent->valueModification)
  {
    case NO_CHANGE:          m_manipulate = 0; break;
    case SET_VARIABLE:       m_manipulate = 1; break;
    case INCREMENT_VARIABLE: m_manipulate = 2; break;
    case DECREMENT_VARIABLE: m_manipulate = 3; break;
  };
  m_destinationMarkerName = m_pEvent->destinationMarkerName;
  m_entryMarkerName       = m_pEvent->entryMarkerName;
  m_exitMarkerName        = m_pEvent->exitMarkerName;
  m_value                 = m_pEvent->value;
  m_variableName          = m_pEvent->globalVariableName;

  m_localChainOnly = (m_pEvent->flags & FCF_LocalChainOnly) != 0;

  UpdateData(false);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CFlowControl::OnOK()
{
  // TODO: Add your specialized code here and/or call the base class
  UpdateData(true);
  CDialog::OnOK();
}


void CFlowControl::GetData(FLOW_CONTROL_EVENT_DATA &event)
{
  switch (m_action)
  {
  case 0: event.action = ACTION_NONE;   break;
  case 1: event.action = ACTION_GOTO;   break;
  case 2: event.action = ACTION_CALL;   break;
  case 3: event.action = ACTION_RETURN; break;
  case 4: event.action = ACTION_POP;    break;
  }
  switch (m_condition)
  {
  case 0: event.actionCondition = CONDITION_ALWAYS;    break;
  case 1: event.actionCondition = CONDITION_EQUALS;    break;
  case 2: event.actionCondition = CONDITION_NOTEQUALS; break;
  };
  switch (m_manipulate)
  {
  case  0: event.valueModification = NO_CHANGE;          break;
  case  1: event.valueModification = SET_VARIABLE;       break;
  case  2: event.valueModification = INCREMENT_VARIABLE; break;
  case  3: event.valueModification = DECREMENT_VARIABLE; break;
  };
  event.destinationMarkerName = m_destinationMarkerName;
  event.entryMarkerName = m_entryMarkerName;
  event.exitMarkerName = m_exitMarkerName;
  event.value = m_value;
  event.globalVariableName = m_variableName;

  event.flags = 0;
  if (m_localChainOnly) event.flags |= FCF_LocalChainOnly;
}
