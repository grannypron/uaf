/******************************************************************************
* Filename: ArrowHeaderCtrl.cpp
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

#include "ArrowHeaderCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrowHeaderCtrl


//*****************************************************************************
// NAME: CArrowHeaderCtrl::CArrowHeaderCtrl
//
// PURPOSE: Constructor
//*****************************************************************************
CArrowHeaderCtrl::CArrowHeaderCtrl() : CHeaderCtrl(), m_sortCol(-1), m_sortAsc(TRUE)
{
}

//*****************************************************************************
// NAME: CArrowHeaderCtrl::~CArrowHeaderCtrl
//
// PURPOSE: Destructor
//*****************************************************************************
CArrowHeaderCtrl::~CArrowHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CArrowHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CArrowHeaderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowHeaderCtrl message handlers


//*****************************************************************************
// NAME: CArrowHeaderCtrl::SetSortImage
//
// PURPOSE: This is the function that will be used to set the sort indicator. 
//
// PARMETERS:
//    int nCol		: the column number
//   BOOL bAsc		: indicates the sort order
//                    true = ascending, false = descending.
//
// RETURNS: int - the previous sorted column number
//*****************************************************************************
int CArrowHeaderCtrl::SetSortImage( int nCol, BOOL bAsc )
{
	int nPrevCol = m_sortCol;

	m_sortCol = nCol;
	m_sortAsc = bAsc;

	// Change the item to ownder drawn
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	GetItem( nCol, &hditem );
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem( nCol, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();
	return nPrevCol;
}

//*****************************************************************************
// NAME: CArrowHeaderCtrl::DrawItem
//
// PURPOSE: Overrides the CHeaderCtrl::DrawItem method.
//*****************************************************************************
void CArrowHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    CDC dc;

    dc.Attach( lpDrawItemStruct->hDC );

    // Get the column rect
    CRect rcLabel( lpDrawItemStruct->rcItem );

    // Save DC
    int nSavedDC = dc.SaveDC();

    // Set clipping region to limit drawing within column
    CRgn rgn;
    rgn.CreateRectRgnIndirect( &rcLabel );
    dc.SelectObject( &rgn );
    rgn.DeleteObject();

    // Draw the background
    CBrush FillBrush(::GetSysColor(COLOR_3DFACE));
    dc.FillRect(rcLabel, &FillBrush);
    
    // Labels are offset by a certain amount  
    // This offset is related to the width of a space character
    int offset = dc.GetTextExtent(_T(" "), 1 ).cx*2;


    // Get the column text and format
    TCHAR buf[256];
    HD_ITEM hditem;
    
    hditem.mask = HDI_TEXT | HDI_FORMAT;
    hditem.pszText = buf;
    hditem.cchTextMax = 255;

    GetItem( lpDrawItemStruct->itemID, &hditem );

    // Determine format for drawing column label
    UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
                                            | DT_VCENTER | DT_END_ELLIPSIS ;

    if( hditem.fmt & HDF_CENTER)
	{
        uFormat |= DT_CENTER;
	}
    else if( hditem.fmt & HDF_RIGHT)
	{
        uFormat |= DT_RIGHT;
	}
    else
	{
        uFormat |= DT_LEFT;
	}

    // Adjust the rect if the mouse button is pressed on it
    if( lpDrawItemStruct->itemState == ODS_SELECTED )
    {
        rcLabel.left++;
        rcLabel.top += 2;
        rcLabel.right++;
    }

    // Adjust the rect further if Sort arrow is to be displayed
    if( lpDrawItemStruct->itemID == (UINT)m_sortCol )
    {
        rcLabel.right -= 3 * offset;
    }

    rcLabel.left += offset;
    rcLabel.right -= offset;

    // Draw column label
    if( rcLabel.left < rcLabel.right )
	{
        dc.DrawText(buf,-1,rcLabel, uFormat);
	}

    // Draw the Sort arrow
    if( lpDrawItemStruct->itemID == (UINT)m_sortCol )
    {
        CRect rcIcon( lpDrawItemStruct->rcItem );

        // Set up pens to use for drawing the triangle
        CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
        CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
        CPen *pOldPen = dc.SelectObject( &penLight );


		offset = (rcIcon.bottom - rcIcon.top) / 4;
		if (m_sortAsc) 
		{
			// Draw triangle pointing upwards
			dc.MoveTo( rcIcon.right - 2*offset, offset);
			dc.LineTo( rcIcon.right - offset, rcIcon.bottom - offset-1 );
			dc.LineTo( rcIcon.right - 3*offset-2, rcIcon.bottom - offset-1 );
			dc.MoveTo( rcIcon.right - 3*offset-1, rcIcon.bottom - offset-1 );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 2*offset, offset-1);                
		}               
		else 
		{
			// Draw triangle pointing downwards
			dc.MoveTo( rcIcon.right - offset-1, offset);
			dc.LineTo( rcIcon.right - 2*offset-1, rcIcon.bottom - offset );
			dc.MoveTo( rcIcon.right - 2*offset-2, rcIcon.bottom - offset );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 3*offset-1, offset );
			dc.LineTo( rcIcon.right - offset-1, offset);          
		} 
        // Restore the pen
        dc.SelectObject( pOldPen );
    }

    // Restore dc
    dc.RestoreDC( nSavedDC );

    // Detach the dc before returning
    dc.Detach();	
}
