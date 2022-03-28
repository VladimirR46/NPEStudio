//---------------------------------------------------------------------------

#ifndef TTriggerBoxH
#define TTriggerBoxH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Types.hpp>
#include <windows.h>
#include <iostream>
#include <string>
#include <setupapi.h>
#include <algorithm>
#include <vector>

//---------------------------------------------------------------------------

static const GUID guidsArray[]=
{
  // Windows Ports Class GUID
  {0x4D36E978, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03,	0x18}},
  // Virtual Ports Class GUID (i.e. com0com and etc)
  {0xDF799E12, 0x3C56, 0x421B, {0xB2, 0x98, 0xB6, 0xD3, 0x64, 0x2B, 0xC8,	0x78}},
  // Windows Modems Class GUID
  {0x4D36E96D, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}},
  // Eltima Virtual Serial Port Driver v4 GUID
  {0xCC0EF009, 0xB820, 0x42F4, {0x95, 0xA9, 0x9B, 0xFA, 0x6A, 0x5A, 0xB7,  0xAB}},
  // Advanced Virtual COM Port GUID
  {0x9341CD95, 0x4371, 0x4A37, {0xA5, 0xAF, 0xFD, 0xB0, 0xA9, 0xD1, 0x96,	0x31}},
};

typedef wchar_t tchar;
typedef std::basic_string<tchar> tstring;

class SerialPortInfo
{
public:
	static std::size_t availablePorts(std::vector<SerialPortInfo> & ports);

	tstring portName() const;
	tstring description() const;

private:
	static tstring devicePortName(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData);

  void parseIdentifiers();

private:
  tstring 	portName_;
  tstring  description_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class TReadThread : public TThread
{
public:
	__fastcall TReadThread(bool CreateSuspended, HANDLE _hPort);

protected:
	void __fastcall Execute();

private:
   HANDLE hPort;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class TTriggerBox
{
public:
	TTriggerBox();

	bool OpenComPort(std::string com);
	void CloseComPort();
	void SendData(BYTE value);
    void SendTrigger(unsigned char _pin);
	void AutoConnect(bool value);
    bool Connected();

    void __fastcall TimerEvent(TObject *Sender);

	~TTriggerBox();

private:
	HANDLE hPort;
	TTimer* timer;
	TReadThread *ReadThread;
    bool isConnected = false;
};
//---------------------------------------------------------------------------
#endif
