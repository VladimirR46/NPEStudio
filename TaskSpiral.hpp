#ifndef TASKSPIRAL
#define TASKSPIRAL

#include "TaskBase.hpp"
#include "windows.h"
//#include "mmsystem.h"
#include <memory>

class TaskSpiral : public TaskBase {
public:
    TaskSpiral(
		TForm* _form, TTimer* _timer, TBitmap* _bitmap, AnsiString _name)
        : TaskBase(_form, _timer, _bitmap, _name)
    {
        MousePress = false;
        isLineDraw = false;

		if(!FileExists(_name+".cfg"))
		{
			Settings.Add(LineThick, "Толщина линии", GuiType::TEdit, 10);
			Settings.Add(TrialMaxCount, "Кол. триалов", GuiType::TEdit, 2);
			Settings.Add(SparalRadius, "Радиус спирали", GuiType::TEdit, 18);
			Settings.Add(LineColor, "Цвет линии", GuiType::TComboColorBox, (int)0xFF008000);
			Settings.save(_name+".cfg");
		}
		else
		{
			Settings.load(_name+".cfg");
        }

    }

    enum Steps {
        SPIRAL = 0,
        BLANK
    };

    enum MotionDirection {
        INSIDE = 0,
        OUTSIDE
    };

    enum DrawDirection {
        FORWARD = 0,
        REVERSE
    };

    struct TSpiral {
        int Radius;
        MotionDirection MoveDir;
        DrawDirection Dir;
        int x1, x2, y1, y2;
        TPoint StartPoint;
        TPoint FinalPoint;
    };

    enum SettingsName : int {
        empty = 0,
        LineThick,
        TrialMaxCount,
        SparalRadius,
		LineColor
    };
    // --------------------------------------------------------------------------
    bool Finished()
    {
		if (Settings.get<int>(TrialMaxCount) * 4 == CurrentTrial + 1) {
            SaveLogFiles();
            return true;
        }
        return false;
    }

    // --------------------------------------------------------------------------
    void UserClick() { }

    // --------------------------------------------------------------------------
    void DrawLine(TAlphaColor color, TPointF point)
    {
        Bitmap->Canvas->BeginScene();
        Bitmap->Canvas->Stroke->Color = color;
        // Bitmap->Canvas->Stroke->Kind = TBrushKind::Solid;
        // Bitmap->Canvas->Stroke->Dash = TStrokeDash::Solid;
        Bitmap->Canvas->Stroke->Thickness = Settings.get<int>(LineThick);
        Bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;
        Bitmap->Canvas->DrawLine(PointClick, point, 1);
        Bitmap->Canvas->EndScene();
        PointClick = point;
    }

    // --------------------------------------------------------------------------
    void UserMouseMove(int X, int Y)
    {
        if (MousePress && isLineDraw) {
			DrawLine(Settings.get<int>(LineColor)/*TAlphaColorRec::Lime*/, TPointF(X, Y));

            if (abs(Spiral.FinalPoint.x - X) < 8 && abs(Spiral.FinalPoint.y - Y) < 8) {
                MousePress = false;
                PlaySound(L"beep-09.wav", 0, SND_ASYNC);

                DrawCircle(TAlphaColorRec::Lime, 8, Spiral.FinalPoint);

                Timer->Interval = 1000;
                Timer->Enabled = true;
            }

            unsigned __int64 tm = millis();
            log_files[CurrentTrial]->Add(
                UIntToStr(tm) + " " + FloatToStr(X) + " " + FloatToStr(Y));
        }
    }

    // --------------------------------------------------------------------------
    void UserMouseDown(int X, int Y)
    {
        if (abs(Spiral.StartPoint.x - X) < 8 && abs(Spiral.StartPoint.y - Y) < 8 && !isLineDraw) {
            PlaySound(L"beep-07a.wav", 0, SND_ASYNC);
            isLineDraw = true;
        }

        PointClick = TPointF(X, Y);
        MousePress = true;
    }

    // --------------------------------------------------------------------------
    void UserMouseUp(int X, int Y)
    {
        MousePress = false;

        unsigned __int64 tm = millis();
        log_files[CurrentTrial]->Add(
            UIntToStr(tm) + " " + FloatToStr(-1) + " " + FloatToStr(-1));
    }
    //--------------------------------------------------------------------------
    void UserTouch(const TTouches Touches)
    {
        if (Touches.Length == 1 && !isLineDraw) {
            UserMouseDown(Touches[0].Location.X, Touches[0].Location.Y);
        }
    }

    // --------------------------------------------------------------------------
    void DrawCircle(TAlphaColor color, int r, TPoint point)
    {
        TRectF MyRect(point.x - r, point.y - r, point.x + r, point.y + r);
        Bitmap->Canvas->BeginScene();
        Bitmap->Canvas->Stroke->Color = color;
        // Bitmap->Canvas->Stroke->Kind = TBrushKind::Solid;
        // Bitmap->Canvas->Stroke->Dash = TStrokeDash::Solid;
        Bitmap->Canvas->Stroke->Thickness = 4;
        Bitmap->Canvas->Fill->Color = color;

        Bitmap->Canvas->FillEllipse(MyRect, 1);
        Bitmap->Canvas->DrawEllipse(MyRect, 1);
        Bitmap->Canvas->EndScene();
    }

    // --------------------------------------------------------------------------
    void DrawPoints()
    {
        DrawCircle(TAlphaColorRec::Red, 12, Spiral.StartPoint);
        DrawCircle(TAlphaColorRec::White, 12, Spiral.FinalPoint);
    }

    // --------------------------------------------------------------------------
    void DrawSpiral()
    {
        float a = 0;

        TPointF p1(Spiral.x1, Spiral.y1);

        do {
            if (Spiral.Dir == FORWARD) {
                Spiral.x2 = Spiral.x1 + (Spiral.Radius * a * sin(a));
                Spiral.y2 = Spiral.y1 - (Spiral.Radius * a * cos(a));
            }

            if (Spiral.Dir == REVERSE) {
                Spiral.x2 = Spiral.x1 - (Spiral.Radius * a * sin(a));
                Spiral.y2 = Spiral.y1 + (Spiral.Radius * a * cos(a));
            }

            TPointF p2(Spiral.x2, Spiral.y2);
            Bitmap->Canvas->BeginScene();
            Bitmap->Canvas->Stroke->Thickness = 10;
            Bitmap->Canvas->Stroke->Color = claWhite;
            Bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;
            // Bitmap->Canvas->Stroke->Kind = TBrushKind::Solid;
            // Bitmap->Canvas->Stroke->Dash = TStrokeDash::Solid;
            Bitmap->Canvas->DrawLine(p1, p2, 1);
            Bitmap->Canvas->EndScene();
            p1 = p2;

            a += 0.1;
        } while (a <= 20);

        if (Spiral.MoveDir == OUTSIDE) {
            Spiral.StartPoint = Point(Spiral.x2, Spiral.y2);
            Spiral.FinalPoint = Point(Spiral.x1, Spiral.y1);
        }

        if (Spiral.MoveDir == INSIDE) {
            Spiral.StartPoint = Point(Spiral.x1, Spiral.y1);
            Spiral.FinalPoint = Point(Spiral.x2, Spiral.y2);
        }
    }
    //--------------------------------------------------------------------------
    void InitTrialSequence()
    {
        TrialSequence.clear();

        do {
            int trial = RandomRange(0, 4);
            int count = std::count(TrialSequence.begin(), TrialSequence.end(), trial);

            if (count < Settings.get<int>(TrialMaxCount))
                TrialSequence.push_back(trial);
        } while (TrialSequence.size() < Settings.get<int>(TrialMaxCount) * 4);
    }
    // -------------------------------------------------------------------------
    void ClearLogFiles()
    {
        for (int i = 0; i < log_files.size(); i++)
            delete log_files[i];

        log_files.clear();
    }
    //--------------------------------------------------------------------------
    void InitLogFiles()
    {
        ClearLogFiles();

        CreateDir(LogPath + "\\Type 1");
        CreateDir(LogPath + "\\Type 2");
        CreateDir(LogPath + "\\Type 3");
        CreateDir(LogPath + "\\Type 4");

        int ff = TrialSequence.size();
        for (int i = 0; i < TrialSequence.size(); i++) {
            TStringList* list = new TStringList;
            log_files.push_back(list);
        }
    }
    //--------------------------------------------------------------------------
    void SaveLogFiles()
    {
        AnsiString patch = "";
        for (int i = 0; i < TrialSequence.size(); i++) {
            patch = LogPath + "\\Type " + IntToStr(TrialSequence[i] + 1) + "\\";
            log_files[i]->SaveToFile(patch + "G" + IntToStr(i + 1) + ".txt");
        }
    }
    //--------------------------------------------------------------------------
    void InitTask(AnsiString Path)
    {
        LogPath = Path;
        steps = BLANK;

        InitTrialSequence();
        InitLogFiles();
        CurrentTrial = -1;

        Timer->Interval = 2000;
        Timer->Enabled = true;
    }

    // --------------------------------------------------------------------------
    void StateManager()
    {
        if (steps == SPIRAL) {
            ClearCanva();
            steps = BLANK;
            Timer->Interval = RandomRange(1500, 2500);
        } else if (steps == BLANK) {
            steps = SPIRAL;

            CurrentTrial++;

            Spiral.Radius = Settings.get<int>(SparalRadius);
            Spiral.x1 = Form->Width / 2;
            Spiral.y1 = Form->Height / 2;
            Spiral.Dir = static_cast<DrawDirection>(
                TrialSequence[CurrentTrial] & 1u); //static_cast<DrawDirection>(RandomRange(0, 2));
            // REVERSE;
            Spiral.MoveDir = static_cast<MotionDirection>(
                (TrialSequence[CurrentTrial] >> 1) & 1u); //static_cast<MotionDirection>(RandomRange(0, 2));
            // OUTSIDE;

            DrawSpiral();
            DrawPoints();
            Timer->Enabled = false;
        }

        isLineDraw = false;
    }
    // -------------------------------------------------------------------------
    void CloseTask()
    {
        SaveLogFiles();
    }
    //--------------------------------------------------------------------------
    ~TaskSpiral()
    {
        ClearLogFiles();
    }

private:
    Steps steps;
    bool MousePress;
    bool isLineDraw;

    TSpiral Spiral;
    TPointF PointClick;

    int CurrentTrial;
    std::vector<int> TrialSequence;

    std::vector<TStringList*> log_files;
};

#endif

