//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "DrawWin.h"
#include "MainWin.h"

#include <Diagnostics.hpp>
#include "TSpiralTask.h"
#include "TElementaryCognitiveFunctions.h"
#include "TCubeTask.h"
#include "SettingsWin.h"
#include "TLearningTask.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm2* Form2;

TStopwatch sw = TStopwatch::Create();
__int64 freq = sw.Frequency;
__int64 curtime = 0;
int fps = 0;
int frames = 0;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormKeyDown(
    TObject* Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift)
{
	if (Key == VK_ESCAPE) {
		Close();
        return;
	}

	if(KeyChar == L'1') Form1->triggerbox->SendTrigger(0);
	if(KeyChar == L'2') Form1->triggerbox->SendTrigger(1);
	if(KeyChar == L'3') Form1->triggerbox->SendTrigger(2);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormCreate(TObject* Sender)
{
	Randomize();
	TBaseTask::Canvas = PaintBox1->Canvas;
	TBaseTask::Timer = Timer1;
	TBaseTask::Form = Form2;

	// Элементарные когнитивные функции
	task_ptr ECFTask(new TElementaryCognitiveFunctions("Элементарные когнитивные функции"));

	task_ptr WMBlock(new TWorkingMemoryBlock(ECFTask.get(), "Рабочая память"));
	task_ptr VSBlock(new TVisualSearchBlock(ECFTask.get(), "Визуальный поиск"));
	task_ptr MABlock(new TMentalArithmeticBlock(ECFTask.get(), "Ментальная арифметика"));
	task_ptr FCBlock(new TFunctionCombination(ECFTask.get(), "Комбинированные функции"));

	ECFTask->AddBlock(WMBlock);
	ECFTask->AddBlock(VSBlock);
	ECFTask->AddBlock(MABlock);
	ECFTask->AddBlock(FCBlock);
	Tasks.push_back(ECFTask);

	// Спираль
	task_ptr STask(new TSpiralTask("Моторные реакции (Спираль)"));
	Tasks.push_back(STask);

	// Кубы
	task_ptr CubeTask(new TCubeTask("Кубы Неккера"));
	Tasks.push_back(CubeTask);

	// Обучение
	task_ptr LearningTask(new TLearningTask("Обучение", MediaPlayer1));
	Tasks.push_back(LearningTask);

    for (int i = 0; i < Tasks.size(); i++) {
        Form1->ComboBox1->Items->Add(Tasks[i]->GetTaskName());
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormShow(TObject* Sender)
{
    Label1->Visible = Form3->CheckBox1->IsChecked;
    Label2->Visible = Form3->CheckBox2->IsChecked;
    //---------------------------------
	Label2->Text = Canvas->ClassName();

	SubjectInfo subject;
	subject.LastName = Form1->Edit3->Text;
	subject.Name = Form1->Edit4->Text;
	subject.Patronymic = Form1->Edit5->Text;
	subject.Gender = Form1->ComboBox2->Items->Strings[Form1->ComboBox2->ItemIndex];
	subject.ActiveHand = Form1->ComboBox3->Items->Strings[Form1->ComboBox3->ItemIndex];
	subject.Age = Form1->Edit1->Text;

	AnsiString Path =
		Form1->ComboBox1->Items->Strings[Form1->ComboBox1->ItemIndex];
	Path = StringReplace(Path, "\r\n", "", TReplaceFlags() << rfReplaceAll);
	CreateDir(Path);

	Path = Path + "\\" + Form1->Edit3->Text + " " + Form1->Edit4->Text;
    AnsiString timName = Now().FormatString("DD.MM.YYYY hh.nn.ss");

    Path = Path + " " + timName;
	CreateDir(Path);

	Tasks[CurrentTask]->Init(Path, subject);
	Tasks[CurrentTask]->StartTask();

    if(Form1->triggerbox->Connected()) Form1->triggerbox->SendData(0x00);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Timer1Timer(TObject* Sender)
{
    if (Tasks[CurrentTask]->Finished()) {
        Tasks[CurrentTask]->CloseTask();
		Timer1->Enabled = false;
        Close();
    } else {
        Tasks[CurrentTask]->StateManager();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormTouch(
    TObject* Sender, const TTouches Touches, const TTouchAction Action)

{
    Tasks[CurrentTask]->UserTouch(Touches, Action);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          float X, float Y)
{
    Tasks[CurrentTask]->UserMouseDown(X, Y, Button);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::PaintBox1MouseMove(TObject *Sender, TShiftState Shift, float X,
          float Y)
{
	Tasks[CurrentTask]->UserMouseMove(X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::PaintBox1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          float X, float Y)
{
	Tasks[CurrentTask]->UserMouseUp(X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Timer2Timer(TObject *Sender)
{
  PaintBox1->Repaint();
  //Invalidate();
  Label1->Text = IntToStr(fps);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::ExternalTrigger(int trigger)
{
	Tasks[CurrentTask]->ExternalTrigger(trigger);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::PaintBox1Paint(TObject *Sender, TCanvas *Canvas)
{
	if((sw.GetTimeStamp() - curtime) > freq)
	{
	   fps = round(frames*freq/(sw.GetTimeStamp() - curtime));
	   curtime = sw.GetTimeStamp();
	   frames = 0;
    }
	frames++;


	if(Canvas->BeginScene())
	{
		__try
		{
            Canvas->Fill->Color = TAlphaColorRec::Black;
			TRectF rect = PaintBox1->BoundsRect;
			Canvas->FillRect(rect, 0, 0, TCorners(), 100);

			Tasks[CurrentTask]->Draw();

		}
		__finally
		{
		   Canvas->EndScene();
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormClose(TObject *Sender, TCloseAction &Action)
{
	Tasks[CurrentTask]->CloseTask();
	Form1->btnStart->ImageIndex = 0;

    if(Form1->triggerbox->Connected()) Form1->triggerbox->SendData(0xFF);
}
//---------------------------------------------------------------------------

