//---------------------------------------------------------------------------
#include <fmx.h>
#pragma hdrstop
#include "MainWin.h"
#include "DrawWin.h"
#include "SettingsWin.h"
#include <filesystem>
#include <FMX.FontGlyphs.hpp>
#include "TestsWin.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm1* Form1;

HHOOK hook;

//---------------------------------------------------------------------------
LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode == HC_ACTION)
    {
		DWORD vk = ((LPKBDLLHOOKSTRUCT)lParam)->vkCode;
        if(wParam == WM_KEYDOWN) // кнопка отпущена?
		{
			if(Form1->triggerbox->Connected())
			{
                // Hotkey List
				TEdit *eHotKeyList[] = {Form3->eHotKey1, Form3->eHotKey2, Form3->eHotKey3, Form3->eHotKey4, Form3->eHotKey5};
				TSpinBox *sbTrBxInList[] = {Form3->sbTrBxIn1, Form3->sbTrBxIn2, Form3->sbTrBxIn3, Form3->sbTrBxIn4, Form3->sbTrBxIn5};

				for(int i = 0; i < 5; i++)
				{
					if(eHotKeyList[i]->Tag == vk) {
						Form1->triggerbox->SendTrigger(sbTrBxInList[i]->Value);
                    }
                }

			}

			WORD Key = vk;
            System::WideChar KeyChar = '-';
			Form2->FormKeyDown(NULL, Key , KeyChar, TShiftState());
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	AnsiString patch = GetHomePath()+"\\NPEStudio";
	CreateDir(patch);
	std::filesystem::current_path(patch.c_str());

	actiCHamp = new TActiCHamp(true); // Актичамп
	triggerbox = new TTriggerBox();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox1Change(TObject* Sender)
{
	Form2->CurrentTask = Form1->ComboBox1->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MenuItem3Click(TObject *Sender)
{
    Form3->Show();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MenuItem5Click(TObject *Sender)
{
    Form1->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnStartClick(TObject *Sender)
{
     /*
	if(ComboBox1->ItemIndex == 0 || ComboBox1->ItemIndex == 1 ||
	   ComboBox1->ItemIndex == 3)
	{
        ShowMessage("Данный тест заблокирован");
		return;
	}
	*/



	if(btnStart->ImageIndex == 0) {
		Screen->UpdateDisplayInformation();
		//System::Types::TRectF r1 = Screen->Displays[0].BoundsRect();
		Form2->Top = Screen->Displays[cbMonitors->ItemIndex].BoundsRect().Top;
		Form2->Left = Screen->Displays[cbMonitors->ItemIndex].BoundsRect().Left;
		//Form2->Height = Screen->Displays[ComboBox4->ItemIndex].BoundsRect().Bottom;
		//Form2->Width = Screen->Displays[cbMonitors->ItemIndex].BoundsRect().Right;
		Form2->Show();
        btnStart->ImageIndex = 7;
	}
	else {
		Form2->Close();
        btnStart->ImageIndex = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
	if(!DirectoryExists(GetHomePath()+"\\NPEStudio\\"+ComboBox1->Selected->Text))
	{
        ShowMessage("Папка ещё не создана");
	} else {
        UnicodeString pt = "/select, "+GetHomePath()+"\\NPEStudio\\"+ComboBox1->Selected->Text;
		ShellExecuteW(NULL, NULL, L"explorer.exe", pt.w_str(), NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	actiCHamp->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
	if(actiCHamp->InitializeSockets())
	{
		Button5->ImageIndex = 4;
        actiCHamp->Resume();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
	for(int i = 0; i <  Screen->DisplayCount; i++)
	{
	   TListBoxItem* item = new TListBoxItem(cbMonitors);
	   item->Parent = cbMonitors;
	   item->Text = "Монитор "+ IntToStr(i+1);
	   item->StyleLookup = "listboxitemstyle";
	   item->ImageIndex = 6;
	}
	cbMonitors->ItemIndex = 0;

	triggerbox->AutoConnect(Form3->cbTBAutoConnect->IsChecked);
	AniIndicator1->Enabled = Form3->cbTBAutoConnect->IsChecked;
	AniIndicator1->Visible = Form3->cbTBAutoConnect->IsChecked;
	Button7->Enabled = !Form3->cbTBAutoConnect->IsChecked;

	if(Form3->cbGlobalKeyHook->IsChecked)
		SetKeyHook();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift)
{
	if (Key == VK_ESCAPE) {
        Form2->Close();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button7Click(TObject *Sender)
{
	if(Button7->ImageIndex == 4)
	{
	   if(MessageBox(NULL, L"Вы действительно хотите отключиться от TriggerBox?",L"Подтверждение",MB_YESNO) == IDYES)
	   {
		  Button7->ImageIndex = 5;
		  triggerbox->CloseComPort();
       }
	   return;
	}

	if(Form3->eComName->Text == ""){
		ShowMessage("Ошибка: укажите имя COM порта в настройках");
        return;
	}

	if(triggerbox->OpenComPort(AnsiString(Form3->eComName->Text).c_str())) {
		Button7->ImageIndex = 4;
	} else {
		ShowMessage("Ошибка: Невозможно подключиться к "+ Form3->eComName->Text);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SetKeyHook()
{
	if(!hook)
		hook = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyProc, GetModuleHandle(NULL), 0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::UnSetKeyHook()
{
	if(hook){
		UnhookWindowsHookEx(hook);
        hook = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	UnSetKeyHook();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RunTest(UnicodeString test_name)
{
	BOOL ok = TRUE;
	PROCESS_INFORMATION pi = { };
	STARTUPINFO si = { };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;

	UnicodeString lrec = ExtractFilePath(ParamStr(0))+test_name;
	UnicodeString lpCommandLine = L"";
    DWORD dwCreationFlags = 0;

	/* Start Proccess*/
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

	WaitForSingleObject(pi.hProcess,INFINITE);

	AnsiString Path = "Tests";
	CreateDir(Path);

	Path = Path + "\\" + Form1->Edit3->Text + " " + Form1->Edit4->Text;
	AnsiString timName = Now().FormatString("DD.MM.YYYY hh.nn.ss");
	Path = Path + " " + timName;
	CreateDir(Path);

	UnicodeString fileName = "";
	if(test_name == "NASA_TLI_PC.exe")
		fileName = "Log_TLI.txt";
	if(test_name == "MFI_PC.exe")
		fileName = "MFI.txt";

	CopyFileW(fileName.w_str(), UnicodeString(Path+"\\"+fileName).w_str(),true);
	DeleteFile(fileName.w_str());

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button8Click(TObject *Sender)
{
	if(ComboBox4->ItemIndex == 0) Form4->DDPM();
	if(ComboBox4->ItemIndex == 1) Form4->IPAQ();
	if(ComboBox4->ItemIndex == 2) Form4->FSS();
	if(ComboBox4->ItemIndex == 3) Form4->EHI();

	if(ComboBox4->ItemIndex == 4) { RunTest("NASA_TLI_PC.exe"); return; }
	if(ComboBox4->ItemIndex == 5) { RunTest("MFI_PC.exe");  return; }

	if(ComboBox4->ItemIndex == 6) Form4->Poll();

	Form4->ShowModal();
}
//---------------------------------------------------------------------------

