//---------------------------------------------------------------------------

#pragma hdrstop

#include "LabStreamingLayer.h"
#include "SettingsWin.h"
#include <filesystem>
//#include <System.IOUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)


TLabStreamingLayer::TLabStreamingLayer()
{

}

void TLabStreamingLayer::start(samples_t& lsl_samples)
{
    lsl::stream_info info("NPES_Samples", "Samples", 1, lsl::IRREGULAR_RATE, lsl::cf_string, "ID1234");

	lsl::xml_element x_samples = info.desc().append_child("samples");
	for(int i = 0; i < lsl_samples.size(); i++)
	{
	   lsl::xml_element x_sample = x_samples.append_child("samples"+std::to_string(i));

	   for(auto it = lsl_samples[i].begin(); it != lsl_samples[i].end(); it++)
		   x_sample.append_child_value(it->first, it->second);
	}

	init_meta(info.desc());

    /* Run Outlet*/
	outletSample.reset(new lsl::stream_outlet(info));
}

void TLabStreamingLayer::sendSample(std::string data)
{
	if(outletSample)
	{
		outletSample->push_sample(&data);
    }
}

void TLabStreamingLayer::stop()
{
	outletSample.reset();
	outletFeedback.reset();
}

void TLabStreamingLayer::init_meta(lsl::xml_element e)
{
	/* Subject*/
	lsl::xml_element sub = e.append_child("subject");
	sub.append_child_value("id", "1234")
		.append_child_value("age","26")
		.append_child_value("gender", "Male")
		.append_child_value("handedness", "Right")
		.append_child_value("Name", "Антипов Владимир Михайлович");

	/* Facility*/
	lsl::xml_element fcy = e.append_child("facility");
	fcy.append_child_value("lab", "Nurolab and Robotics")
		.append_child_value("department", "Neurology")
		.append_child_value("organization", "Innopolis");
}

void TLabStreamingLayer::start_rec(AnsiString Path)
{
	BOOL ok = TRUE;
    hStdInPipeWrite = NULL;
	// Create the process.
	STARTUPINFO si = { };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi = { };

	if(Form3->cbStopLslRec->IsChecked)
	{
        // Create two pipes.
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
		ok = CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0);
		if (ok == FALSE) return;
		//ok = CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0);
		//if (ok == FALSE) return;

        //si.hStdError = hStdOutPipeWrite;
		//si.hStdOutput = hStdOutPipeWrite;
		si.hStdInput = hStdInPipeRead;
    }

	DWORD dwCreationFlags = 0;
	if(Form3->cbHideLslWin->IsChecked) dwCreationFlags = CREATE_NO_WINDOW;

	UnicodeString dir = System::Sysutils::GetCurrentDir();
	UnicodeString lrec = ExtractFilePath(ParamStr(0))+L"LabRecorderCLI.exe";
	UnicodeString lpCommandLine = L" \""+dir+"\\"+UnicodeString(Path)+L"\\Protocol.xdf\"";

	if(Form3->lbLslThreadsName->Count == 0) {ShowMessage("Ошибка: укажите список потоков в настройках"); return; }

	for(int i = 0; i < Form3->lbLslThreadsName->Count; i++)
	{
		TListBoxItem* item = Form3->lbLslThreadsName->ItemByIndex(i);
		TFmxObject* obj = item->Children->Items[1];
		if(obj->ClassType() == __classid(TEdit))
		{
		  TEdit *edit = static_cast<TEdit*>(obj);
          lpCommandLine += " 'name=\""+edit->Text+"\"'";
		}
	}

	/* Start LabRecorder*/
	ok = CreateProcess(
		lrec.w_str(),
		lpCommandLine.w_str(),
		NULL,
		NULL,
		TRUE,
		dwCreationFlags,
		NULL,
		NULL,
		&si,
		&pi);

	if (ok == FALSE){
		ShowMessage("Не удалось запустить запись");
		return;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if(Form3->cbStopLslRec->IsChecked){
		// Close pipes we do not need.
		//CloseHandle(hStdOutPipeWrite);
		CloseHandle(hStdInPipeRead);
	}
}

void TLabStreamingLayer::stop_rec()
{
	if(hStdInPipeWrite)
	{
		DWORD dwWritten;
		std::string str = "\n";
		BOOL bSuccess = false;

		bSuccess = WriteFile(hStdInPipeWrite, str.c_str(), str.length(), &dwWritten, NULL);
		if ( ! bSuccess ){
		  ShowMessage("Не удалось остановить запись");
		}
        hStdInPipeWrite = NULL;
	}
}

TLabStreamingLayer::~TLabStreamingLayer()
{

}
