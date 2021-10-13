//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit2.h"
#include "MainWin.h"

#include "TaskSpiral.hpp"
#include "TaskTable1.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm2* Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormKeyDown(
    TObject* Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift)
{
    if (Key == VK_ESCAPE) {
        Tasks[CurrentTask]->CloseTask();
        Close();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormActivate(TObject* Sender)
{
    Image1->Bitmap->SetSize(int(Image1->Width), int(Image1->Height));
    Image1->Bitmap->Clear(TAlphaColorRec::Black);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormCreate(TObject* Sender)
{
    Randomize();
	Tasks.push_back(Task_ptr(new TaskSpiral(Form2, Timer1, Image1->Bitmap, "Моторные реакции")));
    Tasks.push_back(Task_ptr(new Task1(Form2, Timer1, Image1->Bitmap, "Таблица")));

    for (int i = 0; i < Tasks.size(); i++) {
        Form1->ComboBox1->Items->Add(Tasks[i]->GetTaskName());
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormShow(TObject* Sender)
{
    AnsiString Path =
        Form1->ComboBox1->Items->Strings[Form1->ComboBox1->ItemIndex];
    Path = StringReplace(Path, "\r\n", "", TReplaceFlags() << rfReplaceAll);
    CreateDir(Path);

    Path = Path + "\\" + Form1->Edit1->Text + " " + Form1->Edit2->Text;
    AnsiString timName = Now().FormatString("DD.MM.YYYY hh.nn.ss");

    Path = Path + " " + timName;
	CreateDir(Path);

	Tasks[CurrentTask]->InitTask(Path);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject* Sender)
{
    if (Tasks[CurrentTask]->Finished()) {
        Timer1->Enabled = false;
        Close();
    } else {
        Tasks[CurrentTask]->StateManager();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Image1MouseMove(
    TObject* Sender, TShiftState Shift, float X, float Y)
{
    Tasks[CurrentTask]->UserMouseMove(X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Image1MouseDown(
    TObject* Sender, TMouseButton Button, TShiftState Shift, float X, float Y)
{
    Tasks[CurrentTask]->UserMouseDown(X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Image1MouseUp(
    TObject* Sender, TMouseButton Button, TShiftState Shift, float X, float Y)
{
    Tasks[CurrentTask]->UserMouseUp(X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormTouch(
    TObject* Sender, const TTouches Touches, const TTouchAction Action)

{
    Tasks[CurrentTask]->UserTouch(Touches, Action);
}
//---------------------------------------------------------------------------

