//---------------------------------------------------------------------------
#include <fmx.h>
#pragma hdrstop
#include "MainWin.h"
#include "DrawWin.h"
#include "SettingsWin.h"
#include <filesystem>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm1* Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	AnsiString patch = GetHomePath()+"\\NPEStudio";
	CreateDir(patch);
	std::filesystem::current_path(patch.c_str());

    actiCHamp = new TActiCHamp(true);
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
void __fastcall TForm1::Button4Click(TObject *Sender)
{
    Form2->Show();
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

