//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "TActiCHamp.h"
#include "SettingsWin.h"
#include "MainWin.h"
#include "DrawWin.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TActiCHamp::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TActiCHamp::TActiCHamp(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall TActiCHamp::Execute()
{
    FreeOnTerminate = true;

	ULONG nLastBlock, nDataOffset, nMarkerCount;
	while(!Terminated)
	{
        RDA_MessageHeader* pHeader = NULL;
		int nResult = GetServerMessage(hSocket, &pHeader);
		if (nResult > 0)	// New message?
		{
			RDA_MessageStart* pMsgStart = NULL;
			RDA_MessageData32* pMsgData = NULL;
			switch(pHeader->nType)
			{
			case 1:			// Start / setup
				printf("Start\n");
				pMsgStart = (RDA_MessageStart*)pHeader;
                // Retrieve number of channels.
				nChannels = pMsgStart->nChannels;
				// Open EEG files
				//if (!OpenNewFiles()) { printf("File error!\n");	exit(-1); }
				// Write header file
				//if (!WriteHeaderFile(pMsgStart)) { printf("File error!\n");	exit(-1); }
				nLastBlock = -1;	// Last received block
				nDataOffset = 0;	// Data offset in points
				nMarkerCount = 0;  	// Number of markers in the marker file
				break;

			case 4:			// 32 bit Data
				pMsgData = (RDA_MessageData32*)pHeader;
				// Simple overflow test
				if (nLastBlock != -1 && pMsgData->nBlock > nLastBlock + 1)
					printf("******* Overflow ******\n");
				nLastBlock = pMsgData->nBlock;

				// Write data
				//if (!WriteDataBlock(pMsgData)) { printf("File error!\n");	exit(-1); }

				// Write markers
				if (pMsgData->nMarkers > 0)
				{
					WriteMarkers(pMsgData, nDataOffset, nMarkerCount);
					nMarkerCount += pMsgData->nMarkers;
				}
				// printf("%d\n", pMsgData->nBlock);
				// Adjust data offset for marker file.
				nDataOffset += pMsgData->nPoints;
				break;

			case 3:			// Stop
				printf("Stop\n");
				break;

			default:
				// Ignore all unknown types!
				break;
			}
			if (pHeader) free(pHeader);
		}
        else if (nResult == -2)
		{
			closesocket(hSocket);
			Synchronize([this](){
				ShowMessage("Server has closed the connection.");
                Form1->Button5->ImageIndex = 5;
			});
            break;
		}
		else if (nResult < 0)
		{
			closesocket(hSocket);
			Synchronize([this](){
				ShowMessage("An error occured during message receiving.");
				Form1->Button5->ImageIndex = 5;
			});
			break;
		}
	}

    closesocket(hSocket);
}
//---------------------------------------------------------------------------
bool __fastcall TActiCHamp::InitializeSockets()
{
    WSAStartup(2, &wsaData);

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == SOCKET_ERROR)
	{
		ShowMessage("Error: can't create socket!");
		return false;
	}

    memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);

	AnsiString hst = Form3->EditClientTP->Text;
	char* pszHost = hst.c_str();

    char buff[200];
	if (!stricmp(pszHost, "localhost"))
	{
		gethostname(buff, sizeof(buff));
		pszHost = buff;
	}

	hostent* pHost = gethostbyname(pszHost);
	if (pHost == NULL)
	{
		closesocket(hSocket);
		ShowMessage("Can't find host "+ Form3->EditClientTP->Text);
		return false;
	}

	if (pHost->h_addr_list == 0)
	{
		closesocket(hSocket);
		ShowMessage("Can't initialize Remote Data Access.");
		return false;
	}

	addr.sin_addr = **((in_addr**)(pHost->h_addr_list));

	if (connect(hSocket, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(hSocket);
		ShowMessage("Can't connect to server.\n");
		return false;
	}
	else {
		//ShowMessage("Connected to server "+ Form3->EditClientTP->Text);
    }

    return true;
}
//---------------------------------------------------------------------------
int __fastcall TActiCHamp::GetServerMessage(SOCKET socket, RDA_MessageHeader** ppHeader)
// Get message from server, if available
// returns 0 if no data, -1 if error, -2 if server closed,  > 0 if ok.
{
	// Use select() to avoid blocking.
	timeval tv; tv.tv_sec = 0; tv.tv_usec = 5000;	// 50 ms.
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(socket, &readfds);
	int nResult = select(1, &readfds, NULL, NULL, &tv);
	if (nResult != 1) return nResult;

	RDA_MessageHeader header;
	char* pData = (char*)&header;
	int nLength = 0;
	bool bFirstRecv = true;
	// Retrieve header.
	while(nLength < sizeof(header))
	{
		int nReqLength = sizeof(header) - nLength;
		nResult = recv(socket, (char*)pData, nReqLength, 0);

		// When select() succeeds and recv() returns 0 the server has closed the connection.
		if (nResult == 0 && bFirstRecv)	return -2;
		bFirstRecv = false;
		if (nResult < 0) return nResult;
		nLength += nResult;
		pData += nResult;
	}

	// Check for correct header GUID.
	if (header.guid != GUID_RDAHeader) return -1;
	*ppHeader = (RDA_MessageHeader*)malloc(header.nSize);
	if (!*ppHeader) return -1;

	memcpy(*ppHeader, &header, sizeof(header));
	pData = (char*)*ppHeader + sizeof(header);

	nLength = 0;
	int nDatasize = header.nSize - sizeof(header);

	// Retrieve rest of block.
	while(nLength < nDatasize)
	{
		int nReqLength = nDatasize - nLength;
		nResult = recv(socket, (char*)pData, nReqLength, 0);
		if (nResult < 0) return nResult;
		nLength += nResult;
		pData += nResult;
	}

	return 1;
}
//---------------------------------------------------------------------------
bool __fastcall TActiCHamp::WriteMarkers(RDA_MessageData32* pMsg, ULONG nOffset, ULONG nExistingMarkers)
// Write the markers of the given data message to the marker file.
// The marker position is defined relativ to the current data block,
// so we need to add the offset in data points.
{
	if (pMsg->nMarkers == 0) return true;	// Nothing to do
	// Position of first marker, immediately after the data block.
	RDA_Marker* pMarker = (RDA_Marker*)((char*)pMsg->fData + pMsg->nPoints * nChannels * sizeof(pMsg->fData[0]));

	for (ULONG i = 0; i < pMsg->nMarkers; i++)
	{
		char* pszType = pMarker->sTypeDesc;
		char* pszDescription = pszType + strlen(pszType) + 1;
		//if (fprintf(fpMarkers, "Mk%u=%s,%s,%u,%u,%d\n", nExistingMarkers + i + 1, pszType, pszDescription, nOffset + pMarker->nPosition + 1, pMarker->nPoints, pMarker->nChannel + 1) <= 0)
		//	return false;

		if (!strcmp(pszType, "Left")) trigger = t_Left;
		if(!strcmp(pszType, "Right")) trigger = t_Right;
        Synchronize(&SendTrigger);

		pMarker = (RDA_Marker*)((char*)pMarker + pMarker->nSize);
	}

	return true;
}
//----------------------------------------------------------------------------
void __fastcall TActiCHamp::SendTrigger()
{
    Form2->ExternalTrigger(trigger);
}
//----------------------------------------------------------------------------