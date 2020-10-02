/******************************************************************************
* Filename: ListSort.cpp
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
*
* Code downloaded from http://www.codeguru.com
******************************************************************************/
#include "..\Shared\stdafx.h"

#ifdef UAFEDITOR
#endif
#include "ListSort.h"

#include <afxdisp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//*****************************************************************************
// NAME: CListSort::CListSort
//
// PURPOSE:  Constructor
//
// PARMETERS:
//   CListCtrl * pWnd	: pointer to the CListCtrl
//   const int nCol		: the column number to be sorted
//
// RETURNS: N/A
//*****************************************************************************
CListSort::CListSort(CListCtrl * pWnd, const int nCol)
{
	m_pWnd = pWnd;

	ASSERT(m_pWnd);
	int max = m_pWnd->GetItemCount();
	DWORD dw;
	CString txt;

	// replace Item data with pointer to CSortItem structure
	for (int t = 0; t < max; t++)
	{
        dw = m_pWnd->GetItemData(t); // save current data to restore it later
        txt = m_pWnd->GetItemText(t, nCol); 
        m_pWnd->SetItemData(t, (DWORD) new CSortItem(dw, txt));
	}
}

//*****************************************************************************
// NAME: CListSort::~CListSort
//
// PURPOSE: Destructor
//*****************************************************************************
CListSort::~CListSort()
{
	ASSERT(m_pWnd);
	int max = m_pWnd->GetItemCount();
	CSortItem * pItem;
	for (int t = 0; t < max; t++)
	{
		pItem = (CSortItem *) m_pWnd->GetItemData(t);
		ASSERT(pItem);
		m_pWnd->SetItemData(t, pItem->dw);
		delete pItem;
	}
}


//*****************************************************************************
// NAME: CListSort::Sort
//
// PURPOSE:  To call CListCtrl::SortItems with the dwData argument properly 
//           encoded
//
// PARMETERS:
//   bool bAsc		: Sort order: true = ascending, false = descending
//   EDataType dtype: The type of data to be sorted and the sort type
//
// RETURNS: void 
//*****************************************************************************
void CListSort::Sort(bool bAsc, EDataType dtype)
{
	long lParamSort = dtype;

	// if lParamSort positive - ascending sort order, negative - descending
	if (!bAsc)
	{
		lParamSort *= -1; 
	}
	// Call sortItems on the CListCtrl which calls Compare to 
	// do the sort
	// If lParamSort is negative then the sort is descending
	// If lParamSort is positive then the sort is ascending
	// abs(lParamSort) indicates the data type, will be one of values from
	// the enumeration EDataType
	m_pWnd->SortItems(Compare, lParamSort);
}


//*****************************************************************************
// NAME: CListSort::Compare
//
// PURPOSE:  Compares lParam1 to lParam2
//
// PARMETERS:
//   LPARAM lParam1		: data for item 1
//   LPARAM lParam2		: data for item 2
//   LPARAM lParamSort	: data passed through the dwData argument from the 
//                        CListCtrl::SortItems method
//
// RETURNS: int - The comparison function must return a negative value if the 
//                first item should precede the second, a positive value if 
//                the first item should follow the second, or zero if the two 
//                items are equivalent.
//*****************************************************************************
int CALLBACK CListSort::Compare(LPARAM lParam1, LPARAM lParam2, 
								LPARAM lParamSort)
{
	CSortItem * item1 = (CSortItem *) lParam1;
	CSortItem * item2 = (CSortItem *) lParam2;
	ASSERT(item1 && item2);

	// restore data type and sort order from lParamSort
	// if lParamSort positive - ascending sort order, negative - descending
	short   sOrder = lParamSort < 0 ? -1 : 1; 
	EDataType dType  = (EDataType) (lParamSort * sOrder); // get rid of sign

	// declare typed buffers
	COleDateTime t1, t2;
  int result = FALSE;

	switch (dType)
	{
		case  dtINT:
			result = ((atol(item1->txt) - atol(item2->txt))*sOrder);
      break;
		case  dtDEC:
			result = ((atof(item1->txt) < atof(item2->txt) ? -1 : 1)*sOrder);
      break;
		case  dtDATETIME:
			if (t1.ParseDateTime(item1->txt) && t2.ParseDateTime(item2->txt))
				result = ((t1 < t2 ? -1 : 1 )*sOrder);
      break;
		case  dtSTRING:
			result = (item1->txt.CompareNoCase(item2->txt)*sOrder);
      break;
		default:
			ASSERT("Error: attempt to sort a column without type.");
      break;
	}
  return result;
}


//*****************************************************************************
// NAME: CListSort::GetItemPosition
//
// PURPOSE:  Return the position in the list where pItem should be inserted
//			 base upon the lParamSort value.
//
// PARMETERS:
//   LVITEM *pItem		: The item to be inserted
//   LONG lParamSort	: If lParamSort is negative then the sort is descending
//						  If lParamSort is positive then the sort is ascending
//						  abs(lParamSort) indicates the data type, will be one 
//						  of values from the enumeration EDataType
//
// RETURNS: int 
//*****************************************************************************
int CListSort::GetItemPosition(LVITEM *pItem, LONG lParamSort)
{
	CSortItem sortItem(0, pItem->pszText);

	int max = m_pWnd->GetItemCount();

	int i;
	for(i = 0; i < max; i++)
	{
		int x = Compare(reinterpret_cast<LONG>(&sortItem), m_pWnd->GetItemData(i), lParamSort);
		if(x <= 0)
		{
			break;
		}
	}
	return i;
}

//*****************************************************************************
// NAME: CListSort::CSortItem::CSortItem
//
// PURPOSE:  Constructor
//
// PARMETERS:
//   const DWORD _dw		: Place to store the result from GetItemData 
//   const CString & _txt	: Place to store the result from GetItemText
//
// RETURNS: CSortItem - Is used as a temporary holding place while Sort is
//						executing.
//*****************************************************************************
CListSort::CSortItem::CSortItem(const DWORD _dw, const CString & _txt)
{
    dw  = _dw;
    txt = _txt;
}
