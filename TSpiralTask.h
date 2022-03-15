//---------------------------------------------------------------------------

#ifndef TSpiralTaskH
#define TSpiralTaskH

#include "TBaseTask.h"
#include "windows.h"
//#include "mmsystem.h"
#include <memory>
#include <vector>

enum Steps {
	SPIRAL = 0,
	BLANK,
	RESULT
};

enum MotionDirection {
	INSIDE = 0,
	OUTSIDE
};

enum DrawDirection : int{
	FORWARD = -1,
	REVERSE = 1
};

struct TSpiral {
    int Type;
	float ExternalRadius;
	float ExternalAngle;
	float InternalAngle;
	MotionDirection MoveDir;
	DrawDirection Dir;
	TPointF CenterPoint;
	TPointF StartPoint;
	TPointF FinalPoint;
};

enum SettingsName : int {
	empty = 0,
	LineThick,
	TrialMaxCount,
	ExternalSpiralRadius,
	LineColor,
	StartPointColor,
	EndPointColor,
	SpiralColor,
	MonitorDiagonal,
	SpiralThick,
	ShowResults,
	TurnsCount,
	InternalSpiralRadius
};

struct TrialData {
	TrialData(unsigned int _time, int _x, int _y): time(_time), X(_x), Y(_y)
	{}

	unsigned int time;
	int X;
	int Y;

	float phi;
	float ro;
};

struct PolarCoord {
	float phi;
	float ro;
};

struct CartCoord {
	float x;
	float y;
};
// --------------------------------------------------------------------------

class TSpiralTask : public TBaseTask
{
   typedef std::vector<TrialData> trial_data;

public:
    TSpiralTask(AnsiString _name);

	bool Finished();
	void UserClick();
	void DrawUserLine(TPointF point);
	void UserMouseMove(int X, int Y) override;
	void __fastcall TimerEvent(TObject *Sender);
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void UserMouseUp(int X, int Y) override;
	void UserTouch(const TTouches Touches, const TTouchAction Action) override;
	void DrawCircle(TAlphaColor color, int r, TPointF point);
	void DrawPoints();
	void CalcSpiral();
	TPointF GetSpiral(int dir, float a, TPointF p1, float r);
	void InitTrialSequence();
	void InitTask(AnsiString Path);
	void StateManager();
	void Draw();
    void DrawSpiral();
	void CloseTask();

    void InitLogFiles();
	void ClearLogFiles();
	void SaveLogFiles();
	void AddLog(unsigned int time, int X, int Y);

    void DrawHelpSpiral(float angle);

	// Match function
	PolarCoord car2pol(float x, float y);
	float calc_relative_angle(float x1, float y1, float x0, float y0, float base_angle);
	~TSpiralTask();

private:
    Steps steps;
	bool isDrawLine;
	bool isTrialRun;
	float pixelSize;
	float MonitorH;
	float MonitorW;
    bool isTrialFinish;

	TSpiral Spiral;
	TPointF TargetPoint; // Точка куда нужно попасть мышкой
    TPointF CurrentPoint;

    int CurrentTrial;
    std::vector<int> TrialSequence;

	TTimer* StartTimer;

	std::vector<TPointF> spiral_points;

	// Logger data
    TPointF HelpCurrentPoint;
    AnsiString LogPath;
    std::vector<TStringList*> log_files;
	std::vector<trial_data> trials;
};

#endif
