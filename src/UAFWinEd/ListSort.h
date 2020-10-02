/******************************************************************************
* Filename: ListSort.h
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
#if !defined(AFX_LISTSORT_H__3381E8C6_1360_11D3_97B9_005004055F30__INCLUDED_)
#define AFX_LISTSORT_H__3381E8C6_1360_11D3_97B9_005004055F30__INCLUDED_
class CListSort  
{

public:
    enum EDataType {dtNULL, dtINT, dtSTRING, dtDATETIME, dtDEC};
    
    CListSort(CListCtrl * _pWnd, const int _iCol);
    virtual ~CListSort();
    void Sort(bool _bAsc, EDataType _dtype);
	int GetItemPosition(LVITEM *pItem, LONG lParamSort);
        
protected:
    CListCtrl * m_pWnd;
    
    static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM 
            lParamSort);
   
	// The CSortItem class is used to hold data while Sort is executing.
    struct CSortItem
    {
            CSortItem(const DWORD _dw, const CString &_txt);
            DWORD dw; 
            CString txt;
    };
};

#endif // !defined(AFX_LISTSORT_H__3381E8C6_1360_11D3_97B9_005004055F30__INCLUDED_)
