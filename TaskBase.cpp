//---------------------------------------------------------------------------

#pragma hdrstop

#include "TaskBase.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

TaskBase::TaskBase(TForm* _form, TTimer* _timer, TBitmap* _bitmap, AnsiString _name)
{
	Bitmap = _bitmap;
	Timer = _timer;
	Form = _form;
	TaskName = _name;
}
//--------------------------------------------------------------------------
void TaskBase::DrawPlus()
{
	Bitmap->Canvas->BeginScene();
	Bitmap->Canvas->Stroke->Thickness = 18;
	Bitmap->Canvas->Stroke->Color = claWhite;
	Bitmap->Canvas->DrawLine(
		TPointF(Form->Width / 2 - 360, Form->Height / 2),
		TPointF(Form->Width / 2 + 360, Form->Height / 2), 1);
	Bitmap->Canvas->DrawLine(
		TPointF(Form->Width / 2, Form->Height / 2 - 360),
		TPointF(Form->Width / 2, Form->Height / 2 + 360), 1);
	Bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TaskBase::DrawOneNumber(int number)
{
	TRectF MyRect(0, 0, Form->Width, Form->Height);
	Bitmap->Canvas->BeginScene();
	Bitmap->Canvas->Font->Size = 360;
	Bitmap->Canvas->FillText(MyRect, IntToStr(number), false, 100,
		TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
	Bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TaskBase::DrawText(AnsiString text, int size)
{
	TRectF MyRect(0, 0, Form->Width, Form->Height);
	Bitmap->Canvas->BeginScene();
	Bitmap->Canvas->Font->Size = size;
	Bitmap->Canvas->Fill->Color = claBlack;
	Bitmap->Canvas->FillText(MyRect, text, false, 100,
		TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
	Bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TaskBase::DrawTable(int numbers[], int size)
{
	Bitmap->Canvas->BeginScene();
	Bitmap->Canvas->Stroke->Thickness = 8;

	int RectSize = Form->Height * 0.8 / 5;
	int x = Form->Width / 2 - 2.5 * RectSize;
	int y = Form->Height / 2 - 2.5 * RectSize;

	int FontSize = RectSize * 0.6;
	Bitmap->Canvas->Font->Size = FontSize;

	for (int i = 0; i < sqrt(float(size)); i++) {
		for (int j = 0; j < sqrt(float(size)); j++) {
			int x_begin = i * RectSize + x;
			int y_begin = j * RectSize + y;
			int x_end = i * RectSize + RectSize + x;
			int y_end = j * RectSize + RectSize + y;
			Bitmap->Canvas->DrawRect(TRectF(x_begin, y_begin, x_end, y_end),
				0, 0, AllCorners, 100);
			Bitmap->Canvas->FillText(TRectF(x_begin, y_begin, x_end, y_end),
				IntToStr(numbers[5 * i + j]), false, 100, TFillTextFlags(),
				TTextAlign::Center, TTextAlign::Center);
		}
	}

	Bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------

void TaskBase::ClearCanva()
{
	Bitmap->Clear(TAlphaColorRec::Black);
}
//--------------------------------------------------------------------------
void TaskBase::ClearCanva(TAlphaColor color)
{
	Bitmap->Clear(color);
}
//--------------------------------------------------------------------------
/*
long micros()
{
	boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	return current_date_microseconds.time_of_day().total_microseconds();
}
//--------------------------------------------------------------------------

long millis()
{
	boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	return current_date_microseconds.time_of_day().total_milliseconds();
}
*/

unsigned int TaskBase::millis()
{
	std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();

	time_t tnow = std::chrono::system_clock::to_time_t(now);
	tm* date = std::localtime(&tnow);
	date->tm_hour = 0;
	date->tm_min = 0;
	date->tm_sec = 0;
	auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));

	return std::chrono::duration_cast<std::chrono::milliseconds>(now - midnight).count();
}

AnsiString TaskBase::GetTaskName()
{
	return TaskName;
}

SettingsBase* TaskBase::GetSettings()
{
	return &Settings;
}

TaskBase::~TaskBase() {}

