//---------------------------------------------------------------------------

#ifndef TActiCHampH
#define TActiCHampH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <winsock2.h>
#include <initguid.h>
#include "RecorderRDA.h"

#define PORT_NUMBER		51244		// 32 bit port
//---------------------------------------------------------------------------

enum TriggerType : int
{
	t_Left,
	t_Right
};


class TActiCHamp : public TThread
{
private:
	// Sockets.
	WSADATA wsaData;
    SOCKET hSocket;
	sockaddr_in addr;

	ULONG nChannels;	// Number of channels

	// Trigger
    TriggerType trigger;


protected:
	void __fastcall Execute();
public:
	__fastcall TActiCHamp(bool CreateSuspended);
	bool __fastcall InitializeSockets();
	int __fastcall GetServerMessage(SOCKET socket, RDA_MessageHeader** ppHeader);
	bool __fastcall WriteMarkers(RDA_MessageData32* pMsg, ULONG nOffset, ULONG nExistingMarkers);
    void __fastcall SendTrigger();
};
//---------------------------------------------------------------------------
#endif
