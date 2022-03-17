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
void __fastcall TForm1::btnStartClick(TObject *Sender)
{
	if(btnStart->ImageIndex == 0) {
		Screen->UpdateDisplayInformation();
		//System::Types::TRectF r1 = Screen->Displays[0].BoundsRect();

		Form2->Top = Screen->Displays[cbMonitors->ItemIndex].BoundsRect().Top;
		Form2->Left = Screen->Displays[cbMonitors->ItemIndex].BoundsRect().Left;
		//Form2->Height = Screen->Displays[ComboBox4->ItemIndex].BoundsRect().Bottom;
		//Form2->Width = Screen->Displays[ComboBox4->ItemIndex].BoundsRect().Right;

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
        ShowMessage("����� ��� �� �������");
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
	   item->Text = "������� "+ IntToStr(i+1);
	   item->StyleLookup = "listboxitemstyle";
	   item->ImageIndex = 6;
	}
	cbMonitors->ItemIndex = 0;
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


