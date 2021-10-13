//---------------------------------------------------------------------------

#ifndef TaskBaseH
#define TaskBaseH

#include <fmx.h>
#include "math.hpp"
#include <chrono>
#include "SettingsBase.h"
//---------------------------------------------------------------------------
class TaskBase
{
  public:
    TaskBase(TForm* _form, TTimer* _timer, TBitmap* _bitmap, AnsiString _name);

	virtual void InitTask(AnsiString Path) = 0;
    virtual void StateManager() = 0;
    virtual bool Finished() = 0;
    virtual void UserClick() = 0;
    virtual void UserMouseMove(int X, int Y) = 0;
    virtual void UserMouseDown(int X, int Y) = 0;
	virtual void UserMouseUp(int X, int Y) = 0;
	virtual void UserTouch(const TTouches Touches, const TTouchAction Action) = 0;
	virtual void CloseTask() = 0;

	void DrawPlus();
	void DrawOneNumber(int number);
	void DrawTable(int numbers[], int size);
    void DrawText(AnsiString text, int size);

	void ClearCanva();
    void ClearCanva(TAlphaColor color);
	unsigned int millis();
	AnsiString GetTaskName();
	SettingsBase* GetSettings();

    ~TaskBase();
  protected:
    TBitmap* Bitmap;
    TTimer* Timer;
    TForm* Form;

    //private:
    AnsiString TaskName;
    AnsiString LogPath;

    SettingsBase Settings;
};

#endif
