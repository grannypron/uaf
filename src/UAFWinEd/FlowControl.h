#pragma once


// CFlowControl dialog

class CFlowControl : public CDialog
{
	DECLARE_DYNAMIC(CFlowControl)

public:
	CFlowControl(const FLOW_CONTROL_EVENT_DATA& event, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlowControl();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FlowControl };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
//  CString m_markerName;
  virtual void OnOK();
  const FLOW_CONTROL_EVENT_DATA *m_pEvent;
  void GetData(FLOW_CONTROL_EVENT_DATA &event);
//  BOOL m_action;
//  BOOL m_condition;
  CString m_destinationMarkerName;
  CString m_entryMarkerName;
  CString m_exitMarkerName;
//  BOOL m_manupulate;
  CString m_value;
  CString m_variableName;
  int m_action;
  int m_condition;
  int m_manipulate;
  BOOL m_localChainOnly;
};
