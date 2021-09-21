#ifndef TASKBASE
#define TASKBASE

#include <fmx.h>
#include "math.hpp"
//#include "boost/date_time/posix_time/posix_time.hpp"

#include <chrono>
#include "SettingsBase.hpp"

//------------------------------------------------------------------------------
class TaskBase
{
  public:
    TaskBase(TForm* _form, TTimer* _timer, TBitmap* _bitmap, AnsiString _name)
    {
        Bitmap = _bitmap;
        Timer = _timer;
        Form = _form;
        TaskName = _name;
    }

    virtual void InitTask(AnsiString Path) = 0;
    virtual void StateManager() = 0;
    virtual bool Finished() = 0;
    virtual void UserClick() = 0;
    virtual void UserMouseMove(int X, int Y) = 0;
    virtual void UserMouseDown(int X, int Y) = 0;
    virtual void UserMouseUp(int X, int Y) = 0;
    virtual void UserTouch(const TTouches Touches) = 0;
    virtual void CloseTask() = 0;

    //-------------------------------------------------------------------------
    void DrawPlus()
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
    void DrawOneNumber(int number)
    {
        TRectF MyRect(0, 0, Form->Width, Form->Height);
        Bitmap->Canvas->BeginScene();
        Bitmap->Canvas->Font->Size = 360;
        Bitmap->Canvas->FillText(MyRect, IntToStr(number), false, 100,
            TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
        Bitmap->Canvas->EndScene();
    }
    //--------------------------------------------------------------------------
    void DrawTable(int numbers[], int size)
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

    void ClearCanva()
    {
        Bitmap->Clear(TAlphaColorRec::Black);
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

    unsigned int millis()
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

    AnsiString GetTaskName()
    {
        return TaskName;
    }

    SettingsBase* GetSettings()
    {
        return &Settings;
    }

    ~TaskBase() {}
  protected:
    TBitmap* Bitmap;
    TTimer* Timer;
    TForm* Form;

    //private:
    AnsiString TaskName;
    AnsiString LogPath;

    SettingsBase Settings;
};
//------------------------------------------------------------------------------

#endif

