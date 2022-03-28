//---------------------------------------------------------------------------

#pragma hdrstop

#include "TTriggerBox.h"
#include "MainWin.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TTriggerBox::TTriggerBox()
{
	timer = new TTimer(NULL);
	timer->OnTimer = &TimerEvent;
	timer->Interval = 2000;
}
//---------------------------------------------------------------------------
void TTriggerBox::AutoConnect(bool value)
{
   timer->Enabled = value;
}
//---------------------------------------------------------------------------
bool TTriggerBox::OpenComPort(std::string com)
{
	com = "\\\\.\\"+com;
	hPort = CreateFileA(com.c_str(),
		GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (hPort == INVALID_HANDLE_VALUE)
	{
		std::cout << "failed to open COM port" << std::endl;
		return false;
	}

	ReadThread = new TReadThread(false, hPort);
    isConnected = true;

	return true;
}
//---------------------------------------------------------------------------
bool TTriggerBox::Connected()
{
    return isConnected;
}
//---------------------------------------------------------------------------
void TTriggerBox::SendData(BYTE value)
{
	OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(0, true, 0, 0);
	WriteFile(hPort, &value, 1, NULL, &ov);
	WaitForSingleObject(ov.hEvent, 20);
	Sleep(10);
}
void TTriggerBox::SendTrigger(unsigned char _pin)
{
    BYTE data = 0x00;
	data = data | (1 << _pin);
	SendData(data);
    SendData(0x00);
}
//---------------------------------------------------------------------------
void TTriggerBox::CloseComPort()
{
    isConnected = false;
	ReadThread->Terminate();
    Sleep(100);
	CloseHandle(hPort); // Close the serial port
}
//---------------------------------------------------------------------------
void __fastcall TTriggerBox::TimerEvent(TObject *Sender)
{
	std::vector<SerialPortInfo> ports;
	if(SerialPortInfo::availablePorts(ports))
	{
		for(size_t i=0; i<ports.size(); ++i)
		{
		  AnsiString disc = ports[i].description().c_str();
		  if(disc.Pos("STLink"))
		  {
			  AnsiString name = ports[i].portName().c_str();
			  if(OpenComPort(name.c_str()))
			  {
				 timer->Enabled = false;
				 Form1->AniIndicator1->Enabled = false;
                 Form1->AniIndicator1->Visible = false;
                 Form1->Button7->Enabled = true;
				 Form1->Button7->ImageIndex = 4;
			  }
		  }
		}
	}
}
//---------------------------------------------------------------------------
TTriggerBox::~TTriggerBox()
{
   timer->Free();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

__fastcall TReadThread::TReadThread(bool CreateSuspended, HANDLE _hPort)
	: TThread(CreateSuspended) , hPort(_hPort)
{

}

void __fastcall TReadThread::Execute()
{
    FreeOnTerminate = true;
/*
	BYTE buffer[10000];
	DWORD bytesRead;
	BOOL ok;
	BOOL WaitingOnRead;
	DWORD dwRes;

	try
	{
		OVERLAPPED ovFile = { 0 };
		ovFile.hEvent = CreateEvent(0, true, 0, 0);

		OVERLAPPED ov = { 0 };
		DWORD dwEventMask;
		ov.hEvent = CreateEvent(0, true, 0, 0);
		ok = SetCommMask(hPort, EV_RXCHAR);
		WaitingOnRead = FALSE;

		while (!Terminated)
		{
			if (!WaitingOnRead)
			{
				WaitCommEvent(hPort, &dwEventMask, &ov);
				dwRes = WaitForSingleObject(ov.hEvent, 20);

				// issue a new read request
				ok = ReadFile(hPort, buffer, sizeof(buffer), &bytesRead, &ovFile);
				if (!ok)
				{
					DWORD lastError = ::GetLastError();
					if (lastError != ERROR_IO_PENDING)
					{
						std::cout << "error " << lastError << " in read thread" << std::endl;
                        Terminate();
					}
					else
						// read operation delayed
						WaitingOnRead = TRUE;
				}
				else
				{
					// read completed immediately
					if (bytesRead)
					{
						for (DWORD b = 0; b < bytesRead; b++)
							std::cout << (int)buffer[b] << std::endl;
					}
				}
			}
			else
			{
				// delayed read operation
				dwRes = WaitForSingleObject(ovFile.hEvent, 20);

				switch (dwRes)
				{
				case WAIT_OBJECT_0:
					// Read completed.
					if (!GetOverlappedResult(hPort, &ovFile, &bytesRead, FALSE))
					{
						// Error in communications; report it.
						DWORD lastError = ::GetLastError();
						std::cout << "error " << lastError << " in read thread" << std::endl;
						Terminate();
					}
					else
					{
						// Read completed successfully.
						if (bytesRead)
						{
							for (DWORD b = 0; b < bytesRead; b++)
								std::cout << (int)buffer[b] << std::endl;
						}
						// Reset flag so that another opertion can be issued.
						WaitingOnRead = FALSE;
					}
				break;
				case WAIT_TIMEOUT:
				{
					// Operation isn't complete yet.
					// This is a good time to do some background work.

					break;
				}
				default:
					// Error in the WaitForSingleObject; abort.
					// This indicates a problem with the OVERLAPPED structure's
					// event handle.
					DWORD lastError = ::GetLastError();
					std::cout << "error " << lastError << " in read thread" << std::endl;
					Terminate();
				break;
				}
			}
		}

		Synchronize([this](){
			Form1->Button7->ImageIndex = 5;
		});

		// clean up
		CloseHandle(ovFile.hEvent);
	}
	catch (...)
	{
		std::cout << "exception in read thread" << std::endl;
	}
*/

	try
	{
         AnsiString error = "";
		 DWORD dwEventMask, signal, temp, btr;
		 COMSTAT curstat;
		 OVERLAPPED over=  { 0 };
		 over.hEvent = CreateEvent(NULL, true, true, NULL);
		 SetCommMask(hPort, EV_RXCHAR);

		 while (!Terminated)
		 {
			WaitCommEvent(hPort, &dwEventMask, &over);
			signal = WaitForSingleObject(over.hEvent, 20);

			if(signal == WAIT_OBJECT_0)
			{
				if(GetOverlappedResult(hPort, &over, &temp, true))
				{
				   if((dwEventMask & EV_RXCHAR) != 0)
				   {
					   ClearCommError(hPort, &temp, &curstat);
					   btr = curstat.cbInQue;
					   if(btr)
					   {

                       }
                   }
				}
				else
				{
					DWORD lastError = ::GetLastError();
					error =  "error " + AnsiString(lastError);
					Terminate();
                }
			}
			else if(signal == WAIT_TIMEOUT)	{
                // TimeOut
			}
			else {
                DWORD lastError = ::GetLastError();
				error = "error2 " + AnsiString(lastError);
				Terminate();
            }
		 }

		 Synchronize([&]()
		 {
			if(error != "") Form1->Memo1->Lines->Add(error);
			Form1->Button7->ImageIndex = 5;
		 });

		 CloseHandle(over.hEvent);
	}
	catch (...)
	{
		 Synchronize([this]() {
			Form1->Memo1->Lines->Add("exception in read thread");
		 });
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

tstring SerialPortInfo::devicePortName(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData)
{
  static const tchar portKeyName[]= TEXT("PortName");

  const HKEY key= ::SetupDiOpenDevRegKey(deviceInfoSet, deviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

  if(key== INVALID_HANDLE_VALUE)
    return tstring();

  DWORD dataSize;
  if(::RegQueryValueEx(key,portKeyName, NULL, NULL, NULL,&dataSize)!= ERROR_SUCCESS)
  {
    ::RegCloseKey(key);
    return tstring();
  }

  tstring data(dataSize/sizeof(tchar),0);
  if(::RegQueryValueEx( key, portKeyName, NULL, NULL, reinterpret_cast<unsigned char*>(&data[0]), &dataSize)!= ERROR_SUCCESS)
  {
    ::RegCloseKey(key);
    return tstring();
  }
  ::RegCloseKey(key);

  data.resize((dataSize/sizeof(tchar))?(dataSize/sizeof(tchar)-1):0);
  return data;
}
// ---------------------------------------------------------------------------
tstring deviceRegistryProperty(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData, DWORD property)
{
  DWORD dataType= 0;
  DWORD dataSize= 0;

  ::SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, & dataType, NULL, 0, &dataSize);

  tstring data(dataSize/sizeof(tchar),0);
  if (!::SetupDiGetDeviceRegistryProperty(
        deviceInfoSet,
        deviceInfoData,
        property,
        NULL,
        reinterpret_cast<unsigned char*>(&data[0]),
        dataSize,
        NULL) || !dataSize)
  {
    return tstring();
  }

  switch (dataType)
  {
    case REG_EXPAND_SZ:
    case REG_SZ:
    {
      data.resize((dataSize/sizeof(tchar))?(dataSize/sizeof(tchar)-1):0);
      return data;
    }
    case REG_MULTI_SZ:
    {
      std::replace(data.begin(),data.end(),TEXT('\0'),TEXT(';'));
      return data;
    }
    default:
      break;
  }
  return tstring();
}
//---------------------------------------------------------------------------
std::size_t SerialPortInfo::availablePorts(std::vector<SerialPortInfo>& ports)
{
  static const int guidCount= sizeof(guidsArray)/ sizeof(guidsArray[0]);

  for (int i= 0; i< guidCount; ++i)
  {
    const HDEVINFO deviceInfoSet = ::SetupDiGetClassDevs(&guidsArray[i], NULL, 0, DIGCF_PRESENT);

    if (deviceInfoSet== INVALID_HANDLE_VALUE)
	  return ports.size();


    SP_DEVINFO_DATA deviceInfoData= {0};
    deviceInfoData.cbSize= sizeof(deviceInfoData);

	DWORD index= 0;
    while(::SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData))
	{
      SerialPortInfo serialPortInfo;

	  tstring portName= devicePortName(deviceInfoSet, &deviceInfoData);
      if (portName.empty()|| (portName.find(TEXT("LPT"))!= tstring::npos))
		continue;

	  serialPortInfo.portName_= portName;
	  serialPortInfo.description_= deviceRegistryProperty(deviceInfoSet,&deviceInfoData, SPDRP_DEVICEDESC);

      ports.push_back(serialPortInfo);
    }
    ::SetupDiDestroyDeviceInfoList(deviceInfoSet);
  }
  return ports.size();
}
//---------------------------------------------------------------------------
tstring SerialPortInfo::portName() const
{
  return portName_;
}
// ---------------------------------------------------------------------------
tstring SerialPortInfo::description() const
{
  return description_;
}
