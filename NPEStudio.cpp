//---------------------------------------------------------------------------
#include <fmx.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#pragma hdrstop
#include <System.StartUpCopy.hpp>
//---------------------------------------------------------------------------
USEFORM("MainWin.cpp", Form1);
USEFORM("DrawWin.cpp", Form2);
USEFORM("SettingsWin.cpp", Form3);
USEFORM("TestsWin.cpp", Form4);
//---------------------------------------------------------------------------
extern "C" int FMXmain()
{
	try
	{
		Application->Initialize();
		//GlobalUseDX = true;
		//GlobalUseDXInDX9Mode = true;
		GlobalUseDXSoftware = true;
		//GlobalUseDirect2D = true;
		//GlobalUseGPUCanvas = true;
		//GlobalUseHWEffects = true;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TForm2), &Form2);
		Application->CreateForm(__classid(TForm3), &Form3);
		Application->CreateForm(__classid(TForm4), &Form4);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
