//---------------------------------------------------------------------------

#ifndef TLearningTaskH
#define TLearningTaskH

#include "TBaseTask.h"
#include <FMX.Media.hpp>
//---------------------------------------------------------------------------

class TButtonFigure
{
	enum State
	{
		NORMAL,
		SELECTED,
        PRESSED
	};

public:
	TButtonFigure(TBitmap *_bitmap)
	{
		bitmap = _bitmap;
		state = NORMAL;
	}

	void Draw()
	{
		if(Visible)
		{
			TRectF rect(X-sizeX,Y-sizeY,X+sizeX,Y+sizeY);
			bitmap->Canvas->BeginScene();
			bitmap->Canvas->Font->Size = TextSize;
			bitmap->Canvas->Stroke->Thickness = Thickness;

			if(state == NORMAL) bitmap->Canvas->Stroke->Color = TAlphaColorRec::White;
			else if(state == SELECTED) bitmap->Canvas->Stroke->Color = TAlphaColorRec::Green;
			else if(state == PRESSED) bitmap->Canvas->Stroke->Color = TAlphaColorRec::Blue;

			// Clear bitmap
			bitmap->Canvas->Fill->Color = TAlphaColorRec::Black;
			bitmap->Canvas->FillRect(TRectF(X-sizeX-Thickness/2,Y-sizeY-Thickness/2,X+sizeX+Thickness/2,Y+sizeY+Thickness/2), 1);
			// Draw
			bitmap->Canvas->Fill->Color = TAlphaColorRec::White;
			bitmap->Canvas->DrawRect(rect, 40, 40, AllCorners, 1, TCornerType::Round);
			bitmap->Canvas->FillText(rect, Text, false, 100, TFillTextFlags(),TTextAlign::Center, TTextAlign::Center);
			bitmap->Canvas->EndScene();
		}
	}

	void SetPosition(int _x, int _y) { X = _x; Y = _y; }
	void SetSize(int _sizeX, int _sizeY) { sizeX = _sizeX; sizeY = _sizeY; }
	void Select() { state = SELECTED; }
	void UnSelect() {state = NORMAL; }
	void Press() {state = PRESSED; }
	bool isSelect() { return (state == SELECTED) ? true : false; }
	void SetText(AnsiString str, int text_size) { Text = str; TextSize = text_size;	}
	void SetVisible(bool value) { state = NORMAL; Visible = value; }

private:
	State state;
	int TextSize = 0;
    AnsiString Text = "";
	int X = 0, Y = 0;
	int sizeX = 0, sizeY = 0;
	TBitmap *bitmap;
    bool Visible = false;
    int Thickness = 8;
};
////////////////////////////////////////////////////////////////////////////////

class TLearningTask : public TBaseTask
{
public:
	struct TQuestion
	{
		AnsiString Question;
		AnsiString Goal;
		AnsiString Ungoal;
		AnsiString Type;
        AnsiString SoundName;
    };

	enum SettingsName : int
	{
	   PathToQuestions,
	   LearnEnable,
	   TestEnable,
	   QPlus,
	   QQuastion,
       QRest,
       TPlus
    };

	enum State : int
	{
	   LEARNING,
	   TESTING,
       FINISHED
	} state;

	enum class QuestionState : int
	{
		PLUS,
		QUESTION,
        REST
	} qstate;

	enum class TestingState : int
	{
		PLUS,
		QUESTION,
        REST
	} tstate;

	TLearningTask(AnsiString _name, TMediaPlayer *_player);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;
	void ExternalTrigger(int trigger) override;

	void LoadQuestions();
	bool Questions();
	bool Testing();

    ~TLearningTask();

private:
	TMediaPlayer *MediaPlayer;

	std::vector<TQuestion> QList;
	int QTrialCount = 0;

	TButtonFigure *ButtonYes;
	TButtonFigure *ButtonNo;
};

#endif
