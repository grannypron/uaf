/******************************************************************************
* Filename: Chooseit.cpp
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

#include "..\Shared\stdafx.h"
#include "resource.h"

//#include "UAFWinEd.h"
#include "ChooseIt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseItems dialog


CChooseItems::CChooseItems(ITEM_LIST& items, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseItems::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseItems)
	m_Id1 = FALSE;
	m_Id2 = FALSE;
	m_Id3 = FALSE;
	m_Id4 = FALSE;
	m_Id5 = FALSE;
	m_Id6 = FALSE;
	m_Id7 = FALSE;
	m_Id8 = FALSE;
	//}}AFX_DATA_INIT

	m_items = items;
}


void CChooseItems::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseItems)
	DDX_Control(pDX, IDC_ITEMLIST, m_ItemList);
	DDX_Control(pDX, IDC_ITEM8, m_Item8);
	DDX_Control(pDX, IDC_ITEM7, m_Item7);
	DDX_Control(pDX, IDC_ITEM6, m_Item6);
	DDX_Control(pDX, IDC_ITEM5, m_Item5);
	DDX_Control(pDX, IDC_ITEM4, m_Item4);
	DDX_Control(pDX, IDC_ITEM3, m_Item3);
	DDX_Control(pDX, IDC_ITEM2, m_Item2);
	DDX_Control(pDX, IDC_ITEM1, m_Item1);
	DDX_Control(pDX, IDC_DELETEITEM, m_DeleteItem);
	DDX_Control(pDX, IDC_ADDITEM, m_AddItem);
	DDX_Check(pDX, IDC_IDENTIFIED1, m_Id1);
	DDX_Check(pDX, IDC_IDENTIFIED2, m_Id2);
	DDX_Check(pDX, IDC_IDENTIFIED3, m_Id3);
	DDX_Check(pDX, IDC_IDENTIFIED4, m_Id4);
	DDX_Check(pDX, IDC_IDENTIFIED5, m_Id5);
	DDX_Check(pDX, IDC_IDENTIFIED6, m_Id6);
	DDX_Check(pDX, IDC_IDENTIFIED7, m_Id7);
	DDX_Check(pDX, IDC_IDENTIFIED8, m_Id8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseItems, CDialog)
	//{{AFX_MSG_MAP(CChooseItems)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_LBN_SELCHANGE(IDC_ITEM1, OnSelchangeItem1)
	ON_LBN_SELCHANGE(IDC_ITEM2, OnSelchangeItem2)
	ON_LBN_SELCHANGE(IDC_ITEM3, OnSelchangeItem3)
	ON_LBN_SELCHANGE(IDC_ITEM4, OnSelchangeItem4)
	ON_LBN_SELCHANGE(IDC_ITEM5, OnSelchangeItem5)
	ON_LBN_SELCHANGE(IDC_ITEM6, OnSelchangeItem6)
	ON_LBN_SELCHANGE(IDC_ITEM7, OnSelchangeItem7)
	ON_LBN_SELCHANGE(IDC_ITEM8, OnSelchangeItem8)
	ON_LBN_SETFOCUS(IDC_ITEM1, OnSetfocusItem1)
	ON_LBN_SETFOCUS(IDC_ITEM2, OnSetfocusItem2)
	ON_LBN_SETFOCUS(IDC_ITEM3, OnSetfocusItem3)
	ON_LBN_SETFOCUS(IDC_ITEM4, OnSetfocusItem4)
	ON_LBN_SETFOCUS(IDC_ITEM5, OnSetfocusItem5)
	ON_LBN_SETFOCUS(IDC_ITEM6, OnSetfocusItem6)
	ON_LBN_SETFOCUS(IDC_ITEM7, OnSetfocusItem7)
	ON_LBN_SETFOCUS(IDC_ITEM8, OnSetfocusItem8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseItems message handlers

void CChooseItems::OnAdditem() 
{
  UpdateData(TRUE);
	CString txt;
	int add = m_ItemList.GetCurSel();

	if (add >= 0)
	{
		m_ItemList.GetText(add, txt);

		if (m_Item1.GetCount() == 0)
    {
			m_Item1.AddString(txt);
      m_Item1.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item2.GetCount() == 0)
    {
			m_Item2.AddString(txt);
      m_Item2.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item3.GetCount() == 0)
    {
			m_Item3.AddString(txt);
      m_Item3.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item4.GetCount() == 0)
    {
			m_Item4.AddString(txt);
      m_Item4.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item5.GetCount() == 0)
    {
			m_Item5.AddString(txt);
      m_Item5.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item6.GetCount() == 0)
    {
			m_Item6.AddString(txt);
      m_Item6.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item7.GetCount() == 0)
    {
			m_Item7.AddString(txt);
      m_Item7.SetItemData(0, m_ItemList.GetItemData(add));
    }
		else if (m_Item8.GetCount() == 0)
    {
			m_Item8.AddString(txt);
      m_Item8.SetItemData(0, m_ItemList.GetItemData(add));
    }
	}
}


void CChooseItems::OnDeleteitem() 
{
  UpdateData(TRUE);
	int num;
	
	if ((num = m_Item1.GetCurSel()) >= 0)
	{
		m_Item1.ResetContent();
		m_Id1 = FALSE;
	}
	else if ((num = m_Item2.GetCurSel()) >= 0)
	{
		m_Item2.ResetContent();
		m_Id2 = FALSE;
	}
	else if ((num = m_Item3.GetCurSel()) >= 0)
	{
		m_Item3.ResetContent();
		m_Id3 = FALSE;
	}
	else if ((num = m_Item4.GetCurSel()) >= 0)
	{
		m_Item4.ResetContent();
		m_Id4 = FALSE;
	}
	else if ((num = m_Item5.GetCurSel()) >= 0)
	{
		m_Item5.ResetContent();
		m_Id5 = FALSE;
	}
	else if ((num = m_Item6.GetCurSel()) >= 0)
	{
		m_Item6.ResetContent();
		m_Id6 = FALSE;
	}
	else if ((num = m_Item7.GetCurSel()) >= 0)
	{
		m_Item7.ResetContent();
		m_Id7 = FALSE;
	}
	else if ((num = m_Item8.GetCurSel()) >= 0)
	{
		m_Item8.ResetContent();
		m_Id8 = FALSE;
	}

	UpdateData(FALSE);
}

void CChooseItems::OnOK() 
{
	ITEM temp;
	CString txt;

	UpdateData(TRUE);
  m_items.Clear();

  temp.ready = FALSE;
  ITEM_DATA *psrc;

	if (m_Item1.GetCount() > 0)
	{
    temp.item = m_Item1.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id1 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	if (m_Item2.GetCount() > 0)
	{
    temp.item = m_Item2.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id2 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
      m_items.AddItem(temp, FALSE); 
    }
  }

	if (m_Item3.GetCount() > 0)
	{
    temp.item = m_Item3.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id3 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp, FALSE); 
    }
	}

	if (m_Item4.GetCount() > 0)
	{
    temp.item = m_Item4.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id4 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	if (m_Item5.GetCount() > 0)
	{
    temp.item = m_Item5.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id5 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	if (m_Item6.GetCount() > 0)
	{
    temp.item = m_Item6.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id6 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	if (m_Item7.GetCount() > 0)
	{
    temp.item = m_Item7.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id7 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	if (m_Item8.GetCount() > 0)
	{
    temp.item = m_Item8.GetItemData(0);
    psrc = itemData.GetItemData(temp.item);
    if (psrc!=NULL)
    {
      temp.qty = psrc->Bundle_Qty;
      temp.identified  = (m_Id8 && (temp.item >= 0));
      temp.charges = psrc->Num_Charges;
      temp.cursed = psrc->Cursed;
	    m_items.AddItem(temp,FALSE); 
    }
	}

	CDialog::OnOK();
}

BOOL CChooseItems::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ItemList.ResetContent();
  
  CString name;
  CString qty;
  int index;

  POSITION pos = itemData.GetHeadPosition();
  while (pos != NULL)
  {
    if (itemData.GetAt(pos).Bundle_Qty > 1)
    {      
      qty.Format(" (%u)", itemData.GetAt(pos).Bundle_Qty);
      name = itemData.GetAt(pos).GetItemName();
      name += qty;      
    }
    else
      name = itemData.GetAt(pos).GetItemName();

	  index = m_ItemList.AddString(name);
    m_ItemList.SetItemData(index, itemData.GetKeyAt(pos));
    itemData.GetNext(pos);
  }

	m_Item1.ResetContent();
	m_Item2.ResetContent();
	m_Item3.ResetContent();
	m_Item4.ResetContent();
	m_Item5.ResetContent();
	m_Item6.ResetContent();
	m_Item7.ResetContent();
	m_Item8.ResetContent();

  m_Id1 = FALSE;
  m_Id2 = FALSE;
  m_Id3 = FALSE;
  m_Id4 = FALSE;
  m_Id5 = FALSE;
  m_Id6 = FALSE;
  m_Id7 = FALSE;
  m_Id8 = FALSE;

  BOOL id;
  int count = 0;
  pos = m_items.GetHeadPosition();

  while ((pos != NULL) && (count < 8))
  {
    ITEM_DATA *data = itemData.GetItemData(m_items.GetAt(pos).item);

    if (data != NULL)
    {
      if (data->Bundle_Qty > 1)
      {      
        qty.Format(" (%u)", data->Bundle_Qty);
        name = data->GetItemName();
        name += qty;      
      }
      else
        name = data->GetItemName();

      id = m_items.GetAt(pos).identified; 

      switch (count)
      {
      case 0:
        m_Item1.AddString(name);
        m_Item1.SetItemData(0, data->key);
        m_Id1 = id;
        break;
      case 1:
        m_Item2.AddString(name);
        m_Item2.SetItemData(0, data->key);
        m_Id2 = id;
        break;
      case 2:
        m_Item3.AddString(name);
        m_Item3.SetItemData(0, data->key);
        m_Id3 = id;
        break;
      case 3:
        m_Item4.AddString(name);
        m_Item4.SetItemData(0, data->key);
        m_Id4 = id;
        break;
      case 4:
        m_Item5.AddString(name);
        m_Item5.SetItemData(0, data->key);
        m_Id5 = id;
        break;
      case 5:
        m_Item6.AddString(name);
        m_Item6.SetItemData(0, data->key);
        m_Id6 = id;
        break;
      case 6:
        m_Item7.AddString(name);
        m_Item7.SetItemData(0, data->key);
        m_Id7 = id;
        break;
      case 7:
        m_Item8.AddString(name);
        m_Item8.SetItemData(0, data->key);
        m_Id8 = id;
        break;
      }
      count++;
    }    
    m_items.GetNext(pos);
  }

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseItems::GetItemData(ITEM_LIST& items)
{
	items  = m_items;
}

void CChooseItems::OnSelchangeItem1() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);
}

void CChooseItems::OnSelchangeItem2() 
{
  UpdateData(TRUE);
	m_Item1.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem3() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem4() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem5() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem6() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem7() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSelchangeItem8() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item1.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem1() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem2() 
{
  UpdateData(TRUE);
	m_Item1.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem3() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem4() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem5() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem6() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem7() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
	m_Item8.SetCurSel(-1);	
}

void CChooseItems::OnSetfocusItem8() 
{
  UpdateData(TRUE);
	m_Item2.SetCurSel(-1);
	m_Item3.SetCurSel(-1);
	m_Item4.SetCurSel(-1);
	m_Item5.SetCurSel(-1);
	m_Item6.SetCurSel(-1);
	m_Item7.SetCurSel(-1);
	m_Item1.SetCurSel(-1);
}
