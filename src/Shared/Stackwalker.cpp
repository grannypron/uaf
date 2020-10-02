#include "..\shared\stdafx.h"
#include "Stackwalker.h"

void *My_malloc(int size);
void My_free(void *addr);


#ifdef _IMAGEHLP_
#error "'imagehlp.h' should only included here, not before this point! Otherwise there are some problems!"
#endif

#pragma pack( push, before_imagehlp, 8 )
#pragma warning (disable:4091)
#include <imagehlp.h>

#pragma pack( pop, before_imagehlp )


// Global data:
static BOOL g_bInitialized = FALSE;
static HINSTANCE g_hImagehlpDll = NULL;
static IMAGEHLP_SYMBOL *g_pSym = NULL;

//static DWORD g_dwShowCount = 0;  // increase at every ShowStack-Call
//static CRITICAL_SECTION g_csFileOpenClose = {0};

#define gle (GetLastError())
#define lenof(a) (sizeof(a) / sizeof((a)[0]))
#define MAXNAMELEN 1024 // max name length for found symbols
#define IMGSYMLEN ( sizeof IMAGEHLP_SYMBOL )
#define TTBUFLEN 8096 // for a temp buffer (2^13)

static CHAR tt[TTBUFLEN+1];

// SymCleanup()
typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );
tSC pSC = NULL;

// SymFunctionTableAccess()
typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD AddrBase );
tSFTA pSFTA = NULL;

// SymGetLineFromAddr()
typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD dwAddr,
  OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );
tSGLFA pSGLFA = NULL;

// SymGetModuleBase()
typedef DWORD (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD dwAddr );
tSGMB pSGMB = NULL;

// SymGetModuleInfo()
typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );
tSGMI pSGMI = NULL;

// SymGetOptions()
typedef DWORD (__stdcall *tSGO)( VOID );
tSGO pSGO = NULL;

// SymGetSymFromAddr()
typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD dwAddr,
  OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );
tSGSFA pSGSFA = NULL;

// SymInitialize()
typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
tSI pSI = NULL;

// SymLoadModule()
typedef DWORD (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
  IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll );
tSLM pSLM = NULL;

// SymSetOptions()
typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );
tSSO pSSO = NULL;

// StackWalk()
typedef BOOL (__stdcall *tSW)( DWORD MachineType, HANDLE hProcess,
  HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
  PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
  PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
  PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
  PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );
tSW pSW = NULL;

// UnDecorateSymbolName()
typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
  DWORD UndecoratedLength, DWORD Flags );
tUDSN pUDSN = NULL;



struct ModuleEntry
{
  CString imageName;
  CString moduleName;
  DWORD baseAddress;
  DWORD size;
};

typedef CArray< ModuleEntry, ModuleEntry& > ModuleList;

///////////////////////////////////////////////////////////////////////////////

static int InitStackWalk(void)
{
  if (g_bInitialized != FALSE)
    return 0;  // already initialized
  
  // old:  we load imagehlp.dll dynamically because the NT4-version does not
  // old: offer all the functions that are in the NT5 lib
  // 02-12-19: Now we only support dbghelp.dll!
  //           To use it on NT you have to install the redistributable for DBGHELP.DLL
  g_hImagehlpDll = LoadLibrary( _T("dbghelp.dll") );
  if ( g_hImagehlpDll == NULL )
  {
    TRACE( "LoadLibrary( \"dbghelp.dll\" ): GetLastError = %lu\n", gle );
    g_bInitialized = FALSE;
    return 1;
  }
  
  pSC = (tSC) GetProcAddress( g_hImagehlpDll, "SymCleanup" );
  pSFTA = (tSFTA) GetProcAddress( g_hImagehlpDll, "SymFunctionTableAccess" );
  pSGLFA = (tSGLFA) GetProcAddress( g_hImagehlpDll, "SymGetLineFromAddr" );
  pSGMB = (tSGMB) GetProcAddress( g_hImagehlpDll, "SymGetModuleBase" );
  pSGMI = (tSGMI) GetProcAddress( g_hImagehlpDll, "SymGetModuleInfo" );
  pSGO = (tSGO) GetProcAddress( g_hImagehlpDll, "SymGetOptions" );
  pSGSFA = (tSGSFA) GetProcAddress( g_hImagehlpDll, "SymGetSymFromAddr" );
  pSI = (tSI) GetProcAddress( g_hImagehlpDll, "SymInitialize" );
  pSSO = (tSSO) GetProcAddress( g_hImagehlpDll, "SymSetOptions" );
  pSW = (tSW) GetProcAddress( g_hImagehlpDll, "StackWalk" );
  pUDSN = (tUDSN) GetProcAddress( g_hImagehlpDll, "UnDecorateSymbolName" );
  pSLM = (tSLM) GetProcAddress( g_hImagehlpDll, "SymLoadModule" );
  
  if ( pSC == NULL  || pSFTA == NULL  || pSGMB == NULL || pSGMI == NULL ||
       pSGO == NULL || pSGSFA == NULL || pSI == NULL   || pSSO == NULL  ||
       pSW == NULL  || pUDSN == NULL  || pSLM == NULL )
  {
    TRACE( "GetProcAddress(): some required function not found.\n" );
    FreeLibrary( g_hImagehlpDll );
    g_bInitialized = FALSE;
    return 1;
  }
  
  g_bInitialized = TRUE;
  return 0;
}

void CleanupStackWalk()
{
  if ( g_hImagehlpDll != NULL )
    FreeLibrary(g_hImagehlpDll);
  if (g_pSym != NULL)
    My_free(g_pSym);
}

// **************************************** ToolHelp32 ************************
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE   0x00000008
#pragma pack( push, 8 )
typedef struct tagMODULEENTRY32
{
  DWORD   dwSize;
  DWORD   th32ModuleID;       // This module
  DWORD   th32ProcessID;      // owning process
  DWORD   GlblcntUsage;       // Global usage count on the module
  DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
  BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
  DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
  HMODULE hModule;            // The hModule of this module in th32ProcessID's context
  char    szModule[MAX_MODULE_NAME32 + 1];
  char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;
#pragma pack( pop )


static bool GetModuleListTH32(ModuleList& modules, DWORD pid, FILE *fLogFile)
{
  // CreateToolhelp32Snapshot()
  typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
  // Module32First()
  typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
  // Module32Next()
  typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
  
  // try both dlls...
  const TCHAR *dllname[] = { _T("kernel32.dll"), _T("tlhelp32.dll") };
  HINSTANCE hToolhelp=NULL;
  tCT32S pCT32S=NULL;
  tM32F pM32F=NULL;
  tM32N pM32N=NULL;
  
  HANDLE hSnap;
  MODULEENTRY32 me;
  me.dwSize = sizeof(me);
  bool keepGoing;
  ModuleEntry e;
  int i;
  
  for (i = 0; i<lenof(dllname); i++ )
  {
    hToolhelp = LoadLibrary( dllname[i] );
    if (hToolhelp == NULL)
      continue;
    pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
    pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
    pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
    if ( pCT32S != 0 && pM32F != 0 && pM32N != 0 )
      break; // found the functions!
    FreeLibrary(hToolhelp);
    hToolhelp = NULL;
  }
  
  if (hToolhelp == NULL)
    return false;
  
  hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
  if (hSnap == (HANDLE) -1)
  {
    if (hToolhelp != NULL)
      FreeLibrary(hToolhelp);
    return false;
  }
  
  keepGoing = !!pM32F( hSnap, &me );
  while (keepGoing)
  {
    e.imageName = me.szExePath;
    e.moduleName = me.szModule;
    e.baseAddress = (DWORD) me.modBaseAddr;
    e.size = me.modBaseSize;
    modules.Add( e );
    keepGoing = !!pM32N( hSnap, &me );
  }
  
  CloseHandle(hSnap);
  FreeLibrary(hToolhelp);
  
  return (modules.GetSize() != 0);
}  // GetModuleListTH32


// **************************************** PSAPI ************************
typedef struct _MODULEINFO {
  LPVOID lpBaseOfDll;
  DWORD SizeOfImage;
  LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;

static bool GetModuleListPSAPI(ModuleList &modules, DWORD pid, HANDLE hProcess, FILE *fLogFile)
{
  // EnumProcessModules()
  typedef BOOL (__stdcall *tEPM)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
  // GetModuleFileNameEx()
  typedef DWORD (__stdcall *tGMFNE)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
  // GetModuleBaseName()
  typedef DWORD (__stdcall *tGMBN)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
  // GetModuleInformation()
  typedef BOOL (__stdcall *tGMI)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );
  
  HINSTANCE hPsapi=NULL;
  tEPM pEPM;
  tGMFNE pGMFNE;
  tGMBN pGMBN;
  tGMI pGMI;
  
  DWORD i;
  ModuleEntry e;
  DWORD cbNeeded;
  MODULEINFO mi;
  HMODULE *hMods = 0;
  //char *tt = 0;
  
  hPsapi = LoadLibrary( _T("psapi.dll") );
  if ( hPsapi == 0 )
    goto cleanup;
  
  modules.RemoveAll();
  
  pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );
  pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );
  pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );
  pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );
  if ( pEPM == 0 || pGMFNE == 0 || pGMBN == 0 || pGMI == 0 )
  {
    // we couldn´t find all functions
    //FreeLibrary( hPsapi );
    //return false;
    goto cleanup;
  }
  
  hMods = (HMODULE*) My_malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof HMODULE));
  //tt = (char*) malloc(sizeof(char) * TTBUFLEN);
  
  if ( ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) )
  {
    _ftprintf(fLogFile, _T("EPM failed, GetLastError = %lu\n"), gle );
    goto cleanup;
  }
  
  if ( cbNeeded > TTBUFLEN )
  {
    _ftprintf(fLogFile, _T("More than %lu module handles. Huh?\n"), lenof( hMods ) );
    goto cleanup;
  }
  
  for ( i = 0; i < cbNeeded / sizeof hMods[0]; i++ )
  {
    // base address, size
    pGMI(hProcess, hMods[i], &mi, sizeof mi );
    e.baseAddress = (DWORD) mi.lpBaseOfDll;
    e.size = mi.SizeOfImage;
    // image file name
    tt[0] = 0;
    pGMFNE(hProcess, hMods[i], tt, TTBUFLEN );
    e.imageName = tt;
    // module name
    tt[0] = 0;
    pGMBN(hProcess, hMods[i], tt, TTBUFLEN );
    e.moduleName = tt;
    
    modules.Add(e);
  }
  
cleanup:
  if (hPsapi)
    FreeLibrary(hPsapi);
  //free(tt);
  if (hMods)
    My_free(hMods);
  
  return (modules.GetSize() != 0);
}  // GetModuleListPSAPI

static bool GetModuleList(ModuleList& modules, DWORD pid, HANDLE hProcess, FILE *fLogFile)
{
  // first try toolhelp32
  if (GetModuleListTH32(modules, pid, fLogFile) )
    return true;
  // then try psapi
  return GetModuleListPSAPI(modules, pid, hProcess, fLogFile);
}  // GetModuleList

static void EnumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid, FILE *fLogFile )
{
  static ModuleList modules;
  char *img, *mod;
  
  // fill in module list
  GetModuleList(modules, pid, hProcess, fLogFile);
  
  for ( int i=0; i < modules.GetSize(); i++ )
  {
    // SymLoadModule() wants writeable strings
    img = strdup(modules[i].imageName);
    mod = strdup(modules[i].moduleName);
    
    pSLM( hProcess, 0, img, mod, modules[i].baseAddress, modules[i].size );
    
    My_free(img);
    My_free(mod);
    //std::string s;
  }
}  // EnumAndLoadModuleSymbols

void ShowStack(FILE *fLogFile) 
{  
  CONTEXT c;
  memset( &c, '\0', sizeof c );
  c.ContextFlags = CONTEXT_FULL;
  
  HANDLE hThread = NULL;
  if (DuplicateHandle( GetCurrentProcess(), 
                       GetCurrentThread(),
                       GetCurrentProcess(), 
                       &hThread, 
                       0, 
                       false, 
                       DUPLICATE_SAME_ACCESS ) == 0) 
  {
    return;
  }
  
  // init CONTEXT record so we know where to start the stackwalk
  if ( GetThreadContext( hThread, &c )  == 0) 
  {
    CloseHandle(hThread);
    return;
  }

  HANDLE hSWProcess = GetCurrentProcess();
        
  // normally, call ImageNtHeader() and use machine info from PE header
  DWORD imageType = IMAGE_FILE_MACHINE_I386;
  HANDLE hProcess = GetCurrentProcess(); // hProcess normally comes from outside
  int frameNum; // counts walked frames
  DWORD offsetFromSymbol; // tells us how far from the symbol we were
  DWORD offsetFromLine; // tells us how far from the line we were
  DWORD symOptions; // symbol handler settings
  
  char undName[MAXNAMELEN]; // undecorated name
  char undFullName[MAXNAMELEN]; // undecorated name with all shenanigans
  IMAGEHLP_MODULE Module;
  IMAGEHLP_LINE Line;
  BOOL bXMLTagWrote;

  CString symSearchPath;

  static BOOL bFirstTime = TRUE;

  // If no logfile is present, outpur to "stdout"
  if (fLogFile == NULL) {
    fLogFile = stdout;
  }

  fprintf(fLogFile, "STACK TRACE:\n");

  STACKFRAME s; // in/out stackframe
  memset( &s, '\0', sizeof s );

  if ( (g_bInitialized == FALSE) && (bFirstTime == TRUE) ) 
  {
    InitStackWalk();
  }

  if (g_bInitialized == FALSE)
  {
    // Could not init!!!!
    bFirstTime = FALSE;
    _ftprintf(fLogFile, _T("Stackwalker not initialized (or was not able to initialize)!\n"));
    CloseHandle(hThread);
    return;
  }

  // NOTE: normally, the exe directory and the current directory should be taken
  // from the target process. The current dir would be gotten through injection
  // of a remote thread; the exe fir through either ToolHelp32 or PSAPI.

  if (g_pSym == NULL) 
  {
    g_pSym = (IMAGEHLP_SYMBOL *) My_malloc( IMGSYMLEN + MAXNAMELEN );
    if (!g_pSym) goto cleanup;  // not enough memory...
  }

  if (bFirstTime) 
  {    
    CHAR *p;//,tt[TTBUFLEN+1];

    //tt = (CHAR*) malloc(sizeof(CHAR) * TTBUFLEN); // Get the temporary buffer
    //if (!tt) goto cleanup;  // not enough memory...

    // build symbol search path from:
    symSearchPath = "";
    // current directory
    if ( GetCurrentDirectoryA( TTBUFLEN, tt ) )
    {
      CString tts(tt);
      symSearchPath += tts + ";" ;
    }
    // dir with executable
    if ( GetModuleFileNameA( 0, tt, TTBUFLEN ) )
    {
      for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )
      {
        // locate the rightmost path separator
        if ( *p == '\\' || *p == '/' || *p == ':' )
          break;
      }
      // if we found one, p is pointing at it; if not, tt only contains
      // an exe name (no path), and p points before its first byte
      if ( p != tt ) // path sep found?
      {
        if ( *p == ':' ) // we leave colons in place
          ++ p;
        *p = '\0'; // eliminate the exe name and last path sep
        CString tts(tt);
        symSearchPath += tts + ";";
      }
    }
    // environment variable _NT_SYMBOL_PATH
    if ( GetEnvironmentVariableA( "_NT_SYMBOL_PATH", tt, TTBUFLEN ) )
    {
      CString tts(tt);
      symSearchPath += tts + ";" ;
    }
    // environment variable _NT_ALTERNATE_SYMBOL_PATH
    if ( GetEnvironmentVariableA( "_NT_ALTERNATE_SYMBOL_PATH", tt, TTBUFLEN ) )
    {
      CString tts(tt);
      symSearchPath += tts + ";" ;
    }
    // environment variable SYSTEMROOT
    if ( GetEnvironmentVariableA( "SYSTEMROOT", tt, TTBUFLEN ) )
    {
      CString tts(tt);
      symSearchPath += tts +  ";" ;
    }

    if ( symSearchPath.GetLength() > 0 ) // if we added anything, we have a trailing semicolon
      symSearchPath = symSearchPath.Left( symSearchPath.GetLength() - 1 );

    // why oh why does SymInitialize() want a writeable string?
    strncpy( tt, symSearchPath, TTBUFLEN );
    tt[TTBUFLEN - 1] = '\0'; // if strncpy() overruns, it doesn't add the null terminator

    // init symbol handler stuff (SymInitialize())
    if ( ! pSI( hProcess, tt, false ) )
    {
      _ftprintf(fLogFile, _T("SymInitialize(): GetLastError = %lu\n"), gle );
      //if (tt) free( tt );
      goto cleanup;
    }

    // SymGetOptions()
    symOptions = pSGO();
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions &= ~SYMOPT_UNDNAME;
    symOptions &= ~SYMOPT_DEFERRED_LOADS;
    pSSO( symOptions ); // SymSetOptions()

    // Enumerate modules and tell imagehlp.dll about them.
    // On NT, this is not necessary, but it won't hurt.
    EnumAndLoadModuleSymbols( hProcess, GetCurrentProcessId(), fLogFile );

    //if (tt) free( tt );
  }  // bFirstTime = TRUE
  
  bFirstTime = FALSE;

  // init STACKFRAME for first call
  // Notes: AddrModeFlat is just an assumption. I hate VDM debugging.
  // Notes: will have to be #ifdef-ed for Alphas; MIPSes are dead anyway,
  // and good riddance.
  s.AddrPC.Offset = c.Eip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Ebp;
  s.AddrFrame.Mode = AddrModeFlat;

  memset( g_pSym, '\0', IMGSYMLEN + MAXNAMELEN );
  g_pSym->SizeOfStruct = IMGSYMLEN;
  g_pSym->MaxNameLength = MAXNAMELEN;

  memset( &Line, '\0', sizeof Line );
  Line.SizeOfStruct = sizeof Line;

  memset( &Module, '\0', sizeof Module );
  Module.SizeOfStruct = sizeof Module;

  for ( frameNum=0; ; ++frameNum )
  {
    // get next stack frame (StackWalk(), SymFunctionTableAccess(), SymGetModuleBase())
    // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
    // assume that either you are done, or that the stack is so hosed that the next
    // deeper frame could not be found.
    // CONTEXT need not to be supplied if imageTyp is IMAGE_FILE_MACHINE_I386!
    if ( ! pSW( imageType, hSWProcess, hThread, &s, NULL, NULL, pSFTA, pSGMB, NULL ) )
      break;

    if ( s.AddrPC.Offset == 0 )
    {
      // Special case: If we are here, we have no valid callstack entry!
      _ftprintf(fLogFile, _T("(-nosymbols- PC == 0)\n"));
    }
    else
    {
      // we seem to have a valid PC
      bXMLTagWrote = FALSE;
      undName[0] = 0;
      undFullName[0] = 0;
      offsetFromSymbol = 0;
      // show procedure info (SymGetSymFromAddr())
      if ( ! pSGSFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, g_pSym ) )
      {

      }
      else
      {
        // UnDecorateSymbolName()
        pUDSN( g_pSym->Name, undName, MAXNAMELEN, UNDNAME_NAME_ONLY );
        pUDSN( g_pSym->Name, undFullName, MAXNAMELEN, UNDNAME_COMPLETE );
      }

      // show line number info, NT5.0-method (SymGetLineFromAddr())
      offsetFromLine = 0;
      if ( pSGLFA != NULL )
      { 
        // yes, we have SymGetLineFromAddr()
        if ( ! pSGLFA( hProcess, s.AddrPC.Offset, &offsetFromLine, &Line ) )
        {
          if ( (gle != 487) && (frameNum > 0) )  // ignore error for first frame
          {
            _ftprintf(fLogFile, _T("SymGetLineFromAddr(): GetLastError = %lu\n"), gle );
          }
        }
        else
        {
          if (frameNum > 1)
          {
            fprintf(fLogFile, "\t%s(%lu) %s + %ld bytes\n",
                    Line.FileName, Line.LineNumber, undName, offsetFromLine);
          }
        }
      } // yes, we have SymGetLineFromAddr()
    } // we seem to have a valid PC

    // no return address means no deeper stackframe
    if ( s.AddrReturn.Offset == 0 )
    {
      // avoid misunderstandings in the printf() following the loop
      SetLastError( 0 );
      break;
    }
  } // for ( frameNum )

  if ( gle != 0 )
    _ftprintf(fLogFile, _T("\nStackWalk(): GetLastError = %lu\n"), gle );

cleanup:
  if (fLogFile) 
  {
    _ftprintf(fLogFile, _T("\n"));
  }

  if (hThread)
    CloseHandle(hThread);
}  // ShowStack
