/******************************************************************************
* Filename: SelectPicDlg.cpp
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

#include "UAFWinEd.h"
#include "SmallPicDlg.h"
#include "ScrollPicDlg.h"

#include "SelectPicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CSelectPicDlg dialog


CSelectPicDlg::CSelectPicDlg(PIC_DATA& data, DWORD flags, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectPicDlg)
	m_PicType = 0;
	m_Filename = _T("");
	//}}AFX_DATA_INIT

   m_pic = data;
   m_flags = flags;
}

void CSelectPicDlg::GetData(PIC_DATA& data)
{
   data = m_pic;
}

void CSelectPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPicDlg)
	DDX_Control(pDX, IDC_LOADPIC, m_LoadPic);
	DDX_Radio(pDX, IDC_NOPIC, m_PicType);
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectPicDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectPicDlg)
	ON_BN_CLICKED(IDC_BIGPIC, OnBigpic)
	ON_BN_CLICKED(IDC_LOADPIC, OnLoadpic)
	ON_BN_CLICKED(IDC_NOPIC, OnNopic)
	ON_BN_CLICKED(IDC_SMALLPIC, OnSmallpic)
	ON_BN_CLICKED(IDC_SPRITEPIC, OnSpritepic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPicDlg message handlers

void CSelectPicDlg::LoadSmallPic(void)
{   
   SurfaceType tmp = m_pic.picType;
   m_pic.picType = SmallPicDib;
   CSmallPicDlg dlg(m_pic);
   
   if (dlg.DoModal() == IDOK)
      m_pic = dlg.m_SmallPic;
   else
     m_pic.picType = tmp;
}

void CSelectPicDlg::LoadSpritePic(void)
{
   SurfaceType tmp = m_pic.picType;
   m_pic.picType = SpriteDib;
   int nf = m_pic.NumFrames;
   m_pic.NumFrames = 3;
   CSmallPicDlg dlg(m_pic);
   if (dlg.DoModal() == IDOK)
      m_pic = dlg.m_SmallPic;
   else
   {
     m_pic.picType = tmp;
     m_pic.NumFrames = nf;
   }
}

void CSelectPicDlg::LoadBigPic(void)
{
   SurfaceType tmp = m_pic.picType;
   m_pic.picType = BigPicDib;
   CScrollPicDlg dlg(m_pic.filename, rte.PicArtDir(), "");
   if (dlg.DoModal() == IDOK)
      m_pic.filename = dlg.m_Filename;
   else
     m_pic.picType = tmp;
}

void CSelectPicDlg::OnLoadpic() 
{
   switch (m_pic.picType)
   {
   case SmallPicDib:
      LoadSmallPic();
      break;

   case BigPicDib:
      LoadBigPic();
      break;

   case SpriteDib:
      LoadSpritePic();
      break;

   default:
      WriteDebugString("Unhandled pic type in CSelectPicDlg::OnLoadpic()\n");
      break;
   }

   m_Filename = m_pic.filename;
   UpdateData(FALSE);
}

void CSelectPicDlg::OnNopic() 
{
   m_LoadPic.EnableWindow(FALSE);	
   m_pic.picType = BogusDib;
   SetPicType(BogusDib, TRUE);
   m_Filename="";
   UpdateData(FALSE);
}

void CSelectPicDlg::OnBigpic() 
{
   if (m_flags & BigPicDib)
   {
      m_LoadPic.EnableWindow(TRUE);
      m_pic.picType = BigPicDib;   
      SetPicType(BigPicDib, TRUE);
      m_Filename = m_pic.filename;
      UpdateData(FALSE);
   }
}

void CSelectPicDlg::OnSmallpic() 
{
   if (m_flags & SmallPicDib)
   {
	   m_LoadPic.EnableWindow(TRUE);	
      m_pic.picType = SmallPicDib;
      SetPicType(SmallPicDib, TRUE);
      m_Filename = m_pic.filename;
      UpdateData(FALSE);
   }
}

void CSelectPicDlg::OnSpritepic() 
{
   if (m_flags & SpriteDib)
   {
	   m_LoadPic.EnableWindow(TRUE);
      m_pic.picType = SpriteDib;   
      SetPicType(SpriteDib, TRUE);
      m_Filename = m_pic.filename;
      UpdateData(FALSE);
   }
}

void CSelectPicDlg::OnOK() 
{
   UpdateData(TRUE);

   if (m_Filename.IsEmpty())
      m_pic.Clear();
   else if ((m_pic.picType & m_flags) == 0)
      m_pic.Clear();

	CDialog::OnOK();
}

void CSelectPicDlg::SetPicType(DWORD type, BOOL SetDefaults)
{
   switch (type)
   {
   case BogusDib:
      m_PicType = 0;
      break;

   case SmallPicDib:      
      m_pic.picType = SmallPicDib;         
      if (SetDefaults) m_pic.SetDefaults();
      m_PicType = 1;
      break;
   case BigPicDib:
      m_pic.picType = BigPicDib;   
      if (SetDefaults) m_pic.SetDefaults();
      m_PicType = 2;
      break;
   case SpriteDib:
      m_pic.picType = SpriteDib;   
      if (SetDefaults) m_pic.SetDefaults();
      m_PicType = 3;
      break;
   }
}

BOOL CSelectPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   
   m_Filename = m_pic.filename;
	
   if (m_Filename.IsEmpty())
   {
      m_pic.Clear();
      SetPicType(BogusDib, TRUE);
   }
   else
      SetPicType(m_pic.picType, FALSE);

   BOOL enable;

   enable = m_flags & SmallPicDib;
   GetDlgItem(IDC_SMALLPIC)->EnableWindow(enable);

   enable = m_flags & BigPicDib;
   GetDlgItem(IDC_BIGPIC)->EnableWindow(enable);

   enable = m_flags & SpriteDib;
   GetDlgItem(IDC_SPRITEPIC)->EnableWindow(enable);

   if (m_pic.picType == BogusDib)
      m_LoadPic.EnableWindow(FALSE);

   UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
