#include "pch.h" // For Visual Studio 2017 and earlier, use "stdafx.h"
#include <afx.h>
extern "C" { int _afxForceUSRDLL; }
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//static AFX_EXTENSION_MODULE PROJNAMEDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE0("PROJNAME.DLL Initializing!\n");

        // MFC extension DLL one-time initialization
        //AfxInitExtensionModule(PROJNAMEDLL,
         //   hInstance);

        // Insert this DLL into the resource chain
        //new CDynLinkLibrary(Dll3DLL);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        TRACE0("PROJNAME.DLL Terminating!\n");
    }
    return 1;   // ok
}