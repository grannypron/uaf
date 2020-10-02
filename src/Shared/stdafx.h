/******************************************************************************
* Filename: UAFWin\stdafx.h
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
#ifdef UAFEDITOR

#ifndef STRICT
#define STRICT
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>
#include <afxtempl.h>       // MFC template support
#pragma warning( disable : 4996 )
#include <atlbase.h>        // for CRegKey functions

#include <stdarg.h>
#include <stdlib.h>
#pragma warning (disable : 4201) // non-standard extension used
#include <mmsystem.h>
#pragma warning( disable : 4995 )
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <Process.h>
// for AVI video playback
#include <mmstream.h>
#include <amstream.h>
#include <ddstream.h>

#pragma warning (disable : 4100) // unused function parameters
#pragma warning (disable : 4711) // func chosen for inline expansion
#pragma warning (disable : 4244) // converting int down to char
#pragma warning (disable : 4201) // non-standard extension used
#pragma warning (disable : 4786) // var name too long (it's those STL templates)

#include "..\Shared\externs.h"
#include "..\Shared\ASL.h"
#include "..\UAFWinEd\UAFWinEd.h"
#include "..\Shared\SharedQueue.h"


#else // Engine versus editor


#ifndef STRICT
#define STRICT
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <afxmt.h>
#include <afxtempl.h>       // MFC template support
#pragma warning( disable : 4996 )
#include <atlbase.h>        // for CRegKey functions

#include <stdarg.h>
#include <stdlib.h>
#pragma warning (disable : 4201) // non-standard extension used
#include <mmsystem.h>
#pragma warning( disable : 4995 )
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <Process.h>
// for AVI video playback
#include <mmstream.h>
#include <amstream.h>
#include <ddstream.h>

#pragma warning (disable : 4100) // unused function parameters
#pragma warning (disable : 4711) // func chosen for inline expansion
#pragma warning (disable : 4244) // converting int down to char
#pragma warning (disable : 4201) // non-standard extension used
#pragma warning (disable : 4786) // var name too long (it's those STL templates)

//#include "..\Shared\ASL.h"
//#include "..\Shared\externs.h"

#endif // Engine versus editor