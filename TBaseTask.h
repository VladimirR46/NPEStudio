//---------------------------------------------------------------------------

#ifndef TBaseTaskH
#define TBaseTaskH

#include <fmx.h>
#include "math.hpp"
#include <chrono>
#include <vector>
#include "TProtocol.h"
#include "SettingsBase.h"
#include "TVisualAnalogScale.h"
#include "LabStreamingLayer.h"
//---------------------------------------------------------------------------
class TBaseTask;
typedef std::shared_ptr<TBaseTask> task_ptr;
typedef std::shared_ptr<TBitmap> bitmap_ptr;
typedef std::shared_ptr<SettingsBase> settings_ptr;
typedef std::shared_ptr<TProtocol> protocol_ptr;

class TBaseTask
{
  typedef std::chrono::time_point<std::chrono::system_clock> chrono_time;

  public:
	static TCanvas* Canvas;
	static TTimer* Timer;
	static TForm* Form;
	static TVisualAnalogScale* VAS;
	static lsl_ptr lsl;

	TBaseTask(AnsiString _name);
	TBaseTask(TBaseTask* _parent, AnsiString _name);

	void Init(AnsiString Path, SubjectInfo _sub);
	virtual void InitTask(AnsiString Path)= 0;
    virtual bool isEnable() { return true; }
	virtual void StateManager() = 0;
    virtual bool Finished() = 0;
    virtual void UserClick() {};
    virtual void UserMouseMove(int X, int Y) {};
    virtual void UserMouseDown(int X, int Y, TMouseButton Button) = 0;
	virtual void UserMouseUp(int X, int Y) {};
	virtual void UserTouch(const TTouches Touches, const TTouchAction Action) {};
	virtual void Draw() {};
	virtual void CloseTask() = 0;
	virtual std::shared_ptr<ProtocolBase> CreateProtocol() { return nullptr; };
	virtual void ExternalTrigger(int trigger) {};
	virtual void KeyDown(int Key) {};
    virtual void VasFinished(TObject *Sender) {};

	void DrawPlus();
	void DrawOneNumber(int number);
	void DrawTable(int numbers[], int size);
	void DrawText(AnsiString text, int size, int horShift = 0, TAlphaColor color = TAlphaColorRec::White);
	void DrawSymbols(int array[], int FontSize);
	void DrawNoise(TBitmap *_bitmap = nullptr);
	void DrawPoint(int size);
    void DrawPoint(int CenterX, int CenterY, int size, TAlphaColor color);
    void DrawBitmap(TBitmap *_bitmap);
	void ClearCanva();
	void ClearCanva(TAlphaColor color);

    chrono_time midnight_time(chrono_time& now);
	unsigned int millis();
    unsigned int micros();
	AnsiString GetTaskName();
	settings_ptr GetSettings();
	bitmap_ptr GetBitmap();
    protocol_ptr GetProtocol();
	void AddBlock(task_ptr block);
    void StartTask();
	std::vector<task_ptr> &GetBlocks();
	void SaveSettings();
    void LoadSettings();

	~TBaseTask();
  protected:
	TBaseTask* Parent = nullptr;
	bitmap_ptr bitmap;
	settings_ptr Settings;

	std::vector<task_ptr> Blocks;
	AnsiString TaskName;
	protocol_ptr Protocol;
};

#endif
