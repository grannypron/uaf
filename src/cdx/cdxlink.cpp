//////////////////////////////////////////////////////////////////////////////////
// $Source: /cvsroot/uaf/UAF/src/cdx/cdxlink.cpp,v $
// $Author: cocoaspud $
//
// $Log: cdxlink.cpp,v $
// Revision 1.4  2001/10/13 15:19:59  cocoaspud
// Updates for MSVC ver 7 and other misc
//
// Revision 1.1.1.1  2000/04/22 16:10:25  hebertjo
// Initial checkin of v3.0 to SourceForge CVS.
//
// Revision 2.1  1999/05/20 15:29:02  pietro
// Multiple changes:
// * fixed #include bugs in all .cpp and various .h files
// * fixed all rcsid[] bugs
// * added conditional compile variable CDXINCLUDEALL - when defined,
//   all #include files are included in cdx.h to keep backward compatibility
// * All the libraries are created in ..\..\lib\vc\ directory, library names are
//   cdx.lib/cdxd.lib/cdxdx3.lib/cdxdx3d.lib/cdxadx3.lib/cdxadx3d.lib
//
// Revision 2.0  1999/05/01 13:51:16  bsimser
// Updated revision number to 2.0
//
// Revision 1.1.1.1  1999/05/01 04:10:56  bsimser
// Initial revision to cvs
//
// $Revision: 1.4 $
//////////////////////////////////////////////////////////////////////////////////
#ifdef SAVE_RCSID
static char rcsid[] = "@(#) $Id: cdxlink.cpp,v 1.4 2001/10/13 15:19:59 cocoaspud Exp $";
#endif

#include "CDX.h"
#include "cdxlink.h"
#include <string.h>

static int SessionCount = 0;
static int PlayerCount = 0;

//////////////////////////////////////////////////////////////////////////////////
// Callback function for enumerating all CDXLinkSessions.
//////////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc,
									 LPDWORD lpdwTimeOut, DWORD dwFlags,
									 LPVOID lpContext)
{
	CDXLinkSession* Session = (CDXLinkSession*) lpContext;

	// Determine if the enumeration has timed out
	if(dwFlags & DPESC_TIMEDOUT) return FALSE;

	// Store the session description in the list
	strcpy(Session[SessionCount].m_Name, lpSessionDesc->lpszSessionNameA);
	Session[SessionCount].m_Guid = lpSessionDesc->guidInstance;
	Session[SessionCount].m_Flags = lpSessionDesc->dwFlags;
	Session[SessionCount].m_MaxPlayers = lpSessionDesc->dwMaxPlayers;
	Session[SessionCount].m_CurrentPlayers = lpSessionDesc->dwCurrentPlayers;

	SessionCount++;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Callback function for enumerating all CDXLinkPlayers.
//////////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL EnumPlayersCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName,
									DWORD dwFlags, LPVOID lpContext)
{
	CDXLinkPlayer* Player = (CDXLinkPlayer*) lpContext;

	if(dwPlayerType == DPPLAYERTYPE_PLAYER)
	{
		Player[PlayerCount].m_ID = dpId;
		strcpy(Player[PlayerCount].m_Name, lpName->lpszShortNameA);
		Player[PlayerCount].m_Flags = dwFlags;
		PlayerCount++;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Enumerates all modems in a system.
//////////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL EnumModemAddress(REFGUID lpguidDataType, DWORD dwDataSize,
								 LPCVOID lpData, LPVOID lpContext)
{
	LPSTR Name = (LPSTR) lpContext;
	LPSTR	Data = (LPSTR) lpData;

	if(IsEqualGUID(lpguidDataType, (REFGUID)DPAID_Modem))
	{
		while(lstrlen(Data))
		{
			// Store modem name
			Name = Data;

			// Skip to next string
			Data += lstrlen(Data) + 1;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Default constructor. Just initializes COM.
//////////////////////////////////////////////////////////////////////////////////
CDXLink::CDXLink(void)
{
	m_DP = NULL;
	CoInitialize(NULL);
}

//////////////////////////////////////////////////////////////////////////////////
// Default destructor. Uninitializes COM and releases any objects.
//////////////////////////////////////////////////////////////////////////////////
CDXLink::~CDXLink(void)
{
	RELEASE(m_DP);
	RELEASE(m_DPLobby);
	CoUninitialize();
}

//////////////////////////////////////////////////////////////////////////////////
// Creates an IPX session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CreateIPX(void* hWnd)
{
	// Create the DirectPlay object
	rval = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
													IID_IDirectPlay3A, (LPVOID*)&m_DP);
	if(rval != DP_OK) return FALSE;

	LPDIRECTPLAYLOBBYA DPLobbyA = NULL;
	
	// Create ANSI DirectPlayLobby interface
	rval = DirectPlayLobbyCreate(NULL, &DPLobbyA, NULL, NULL, 0);
	if(rval != DP_OK) return FALSE;

	// Get ANSI DirectPlayLobby2 interface
	rval = DPLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *)&m_DPLobby);
	if(rval != DP_OK) return FALSE;

	// Release the DirectPlayLobby interface
	DPLobbyA->Release();
	DPLobbyA = NULL;

	DPCOMPOUNDADDRESSELEMENT Address[1];
	DWORD AddressSize = 0;
	LPVOID Connection;

	// IPX just needs a service provider
	Address[0].guidDataType = DPAID_ServiceProvider;
	Address[0].dwDataSize = sizeof(GUID);
	Address[0].lpData = (LPVOID)&DPSPGUID_IPX;

	// See how much room is needed to store this address
	rval = m_DPLobby->CreateCompoundAddress(Address, 1, NULL, &AddressSize);
	if(rval != DPERR_BUFFERTOOSMALL) return FALSE;

	Connection = GlobalAllocPtr(GHND, AddressSize);
	if(Connection == NULL) return FALSE;

	// Create the address
	rval = m_DPLobby->CreateCompoundAddress(Address, 1,	Connection, &AddressSize);
	if(rval != DP_OK) return FALSE;

	// Create the connection
	rval = m_DP->InitializeConnection(Connection, 0);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a TCP/IP session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CreateTCPIP(void* hWnd, char* IPAddress)
{
	// Create the DirectPlay object
	rval = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
													IID_IDirectPlay3A, (LPVOID*)&m_DP);
	if(rval != DP_OK) return FALSE;

	LPDIRECTPLAYLOBBYA DPLobbyA = NULL;
	
	// Create ANSI DirectPlayLobby interface
	rval = DirectPlayLobbyCreate(NULL, &DPLobbyA, NULL, NULL, 0);
	if(rval != DP_OK) return FALSE;

	// Get ANSI DirectPlayLobby2 interface
	rval = DPLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *)&m_DPLobby);
	if(rval != DP_OK) return FALSE;

	// Release the DirectPlayLobby interface
	DPLobbyA->Release();
	DPLobbyA = NULL;

	DPCOMPOUNDADDRESSELEMENT Address[2];
	DWORD AddressSize = 0;
	LPVOID Connection;

	// TCP/IP needs a service provider and an IP address
	Address[0].guidDataType = DPAID_ServiceProvider;
	Address[0].dwDataSize = sizeof(GUID);
	Address[0].lpData = (LPVOID)&DPSPGUID_TCPIP;

	Address[1].guidDataType = DPAID_INet;
	Address[1].dwDataSize = lstrlen(IPAddress) + 1;
	Address[1].lpData = IPAddress;

	// See how much room is needed to store this address
	rval = m_DPLobby->CreateCompoundAddress(Address, 2, NULL, &AddressSize);
	if(rval != DPERR_BUFFERTOOSMALL) return FALSE;

	Connection = GlobalAllocPtr(GHND, AddressSize);
	if(Connection == NULL) return FALSE;

	// Create the address
	rval = m_DPLobby->CreateCompoundAddress(Address, 2,	Connection, &AddressSize);
	if(rval != DP_OK) return FALSE;

	// Create the connection
	rval = m_DP->InitializeConnection(Connection, 0);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a modem (dial-up) session using the passed PhoneNo.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CreateModem(void* hWnd, char* PhoneNo)
{
	// Create the DirectPlay object
	rval = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
													IID_IDirectPlay3A, (LPVOID*)&m_DP);
	if(rval != DP_OK) return FALSE;

	LPDIRECTPLAYLOBBYA DPLobbyA = NULL;
	
	// Create ANSI DirectPlayLobby interface
	rval = DirectPlayLobbyCreate(NULL, &DPLobbyA, NULL, NULL, 0);
	if(rval != DP_OK) return FALSE;

	// Get ANSI DirectPlayLobby2 interface
	rval = DPLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *)&m_DPLobby);
	if(rval != DP_OK) return FALSE;

	// Release the DirectPlayLobby interface
	DPLobbyA->Release();
	DPLobbyA = NULL;

	DPCOMPOUNDADDRESSELEMENT Address[3];
	DWORD AddressSize = 0;
	LPVOID ModemAddress;
	LPVOID Connection;
	LPSTR ModemName = "";

	// Modem needs a service provider, a phone number string and a modem string
	Address[0].guidDataType = DPAID_ServiceProvider;
	Address[0].dwDataSize = sizeof(GUID);
	Address[0].lpData = (LPVOID)&DPSPGUID_MODEM;

	// Get size of player address for player zero
	rval = m_DP->GetPlayerAddress(DPID_ALLPLAYERS, NULL, &AddressSize);
	if(rval != DP_OK) return FALSE;

	// Make room for it
	ModemAddress = GlobalAllocPtr(GHND, AddressSize);
	if(ModemAddress == NULL) return FALSE;

	// Get the address
	rval = m_DP->GetPlayerAddress(DPID_ALLPLAYERS, ModemAddress, &AddressSize);
	if(rval != DP_OK) return FALSE;
	
	// Get modem strings from address and select the first one
	// BUGBUG! What if the user has more than one modem?
	rval = m_DPLobby->EnumAddress(EnumModemAddress, ModemAddress, AddressSize, ModemName);
	if(rval != DP_OK) return FALSE;

	Address[1].guidDataType = DPAID_Modem;
	Address[1].dwDataSize = lstrlen(ModemName) + 1;
	Address[1].lpData = ModemName;

	Address[2].guidDataType = DPAID_Phone;
	Address[2].dwDataSize = lstrlen(PhoneNo) + 1;
	Address[2].lpData = PhoneNo;

	// See how much room is needed to store this address
	rval = m_DPLobby->CreateCompoundAddress(Address, 3, NULL, &AddressSize);
	if(rval != DPERR_BUFFERTOOSMALL) return FALSE;

	Connection = GlobalAllocPtr(GHND, AddressSize);
	if(Connection == NULL) return FALSE;

	// Create the address
	rval = m_DPLobby->CreateCompoundAddress(Address, 3,	Connection, &AddressSize);
	if(rval != DP_OK) return FALSE;

	// Create the connection
	rval = m_DP->InitializeConnection(Connection, 0);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates a serial session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CreateSerial(void* hWnd)
{
	// Create the DirectPlay object
	rval = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
													IID_IDirectPlay3A, (LPVOID*)&m_DP);
	if(rval != DP_OK) return FALSE;

	LPDIRECTPLAYLOBBYA DPLobbyA = NULL;
	
	// Create ANSI DirectPlayLobby interface
	rval = DirectPlayLobbyCreate(NULL, &DPLobbyA, NULL, NULL, 0);
	if(rval != DP_OK) return FALSE;

	// Get ANSI DirectPlayLobby2 interface
	rval = DPLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *)&m_DPLobby);
	if(rval != DP_OK) return FALSE;

	// Release the DirectPlayLobby interface
	DPLobbyA->Release();
	DPLobbyA = NULL;

	DPCOMPOUNDADDRESSELEMENT Address[1];
	DWORD AddressSize = 0;
	LPVOID Connection;

	// BUGBUG! Nasty dialog needed to get COM port info
	Address[0].guidDataType = DPAID_ServiceProvider;
	Address[0].dwDataSize = sizeof(GUID);
	Address[0].lpData = (LPVOID)&DPSPGUID_SERIAL;

	// See how much room is needed to store this address
	rval = m_DPLobby->CreateCompoundAddress(Address, 1, NULL, &AddressSize);
	if(rval != DPERR_BUFFERTOOSMALL) return FALSE;

	Connection = GlobalAllocPtr(GHND, AddressSize);
	if(Connection == NULL) return FALSE;

	// Create the address
	rval = m_DPLobby->CreateCompoundAddress(Address, 1,	Connection, &AddressSize);
	if(rval != DP_OK) return FALSE;

	// Create the connection
	rval = m_DP->InitializeConnection(Connection, 0);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Enumerates all sessions for a given GUID.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::EnumSessions(CDXLinkSession* Sessions, GUID APP_GUID)
{
	DPSESSIONDESC2 SessionDesc;

	// Search for this kind of session
	ZeroMemory(&SessionDesc, sizeof(DPSESSIONDESC2));
	SessionDesc.dwSize = sizeof(DPSESSIONDESC2);
	SessionDesc.guidApplication = APP_GUID;

	rval = m_DP->EnumSessions(&SessionDesc, 0, EnumSessionsCallback, Sessions,
														DPENUMSESSIONS_AVAILABLE);
	if(rval != DP_OK) return FALSE;

	SessionCount = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Host a DirectPlay session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::HostSession(CDXLinkSession* Session)
{
	DPSESSIONDESC2 SessionDesc;

	// Host a new session
	ZeroMemory(&SessionDesc, sizeof(DPSESSIONDESC2));
	SessionDesc.dwSize = sizeof(DPSESSIONDESC2);
	SessionDesc.dwFlags = Session->m_Flags;
	SessionDesc.guidApplication = Session->m_AppGuid;
	SessionDesc.dwMaxPlayers = Session->m_MaxPlayers;
	SessionDesc.lpszSessionNameA = Session->m_Name;

	rval = m_DP->Open(&SessionDesc, DPOPEN_CREATE);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Join a DirectPlay session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::JoinSession(CDXLinkSession* Session)
{
	DPSESSIONDESC2 SessionDesc;

	// Join an existing session
	ZeroMemory(&SessionDesc, sizeof(DPSESSIONDESC2));
	SessionDesc.dwSize = sizeof(DPSESSIONDESC2);
	SessionDesc.guidInstance = Session->m_Guid;

	rval = m_DP->Open(&SessionDesc, DPOPEN_JOIN);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Close a session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CloseSession(void)
{
	rval = m_DP->Close();
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Enumerate all players in a session.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::EnumPlayers(CDXLinkPlayer* Players, DWORD Flags)
{
	rval = m_DP->EnumPlayers(NULL, EnumPlayersCallback, Players, Flags);
	if(rval != DP_OK) return FALSE;

	PlayerCount = 0;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Create a new player.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::CreatePlayer(CDXLinkPlayer* Player)
{
	DPNAME Name;

	// Fill out the name structure
	ZeroMemory(&Name, sizeof(DPNAME));
	Name.dwSize = sizeof(DPNAME);
	Name.lpszShortNameA = Player->m_Name;
	Name.lpszLongNameA = NULL;

	// Create a player with this name
	rval = m_DP->CreatePlayer(&Player->m_ID, &Name, Player->m_Event,
														NULL, 0, Player->m_Flags);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Destroy and free a given player.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDXLink::DestroyPlayer(CDXLinkPlayer* Player)
{
	rval = m_DP->DestroyPlayer(Player->m_ID);
	if(rval != DP_OK) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Send a message from one id to another.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLink::Send(DPID idFrom, DPID idTo, DWORD dwFlags,
											LPVOID lpData, DWORD dwDataSize)
{
	return m_DP->Send(idFrom, idTo, dwFlags, lpData, dwDataSize);
}

//////////////////////////////////////////////////////////////////////////////////
// Receive a message for a session.
//////////////////////////////////////////////////////////////////////////////////
HRESULT CDXLink::Receive(LPDPID lpidFrom, LPDPID lpidTo,	DWORD dwFlags,
												 LPVOID lpData, LPDWORD lpdwDataSize)
{
	return m_DP->Receive(lpidFrom, lpidTo, dwFlags, lpData, lpdwDataSize);
}
