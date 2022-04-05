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

            // Clear bitmap
            bitmap->Canvas->Fill->Color = TAlphaColorRec::Black;
			bitmap->Canvas->FillRect(TRectF(X-sizeX-Thickness/2,Y-sizeY-Thickness/2,X+sizeX+Thickness/2,Y+sizeY+Thickness/2), 1);

			// Draw
			switch(state)
			{
				case NORMAL:
                bitmap->Canvas->Stroke->Color = TAlphaColorRec::Black;
				break;
				case SELECTED:
                bitmap->Canvas->Stroke->Color = TAlphaColorRec::White;
				break;
				case PRESSED:
				bitmap->Canvas->Stroke->Color = TAlphaColorRec::White;
				bitmap->Canvas->Fill->Color = PressColor;
				break;
			}
			bitmap->Canvas->FillRect(rect, 40, 40, AllCorners, 1, TCornerType::Round);
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
	void Press(TAlphaColor color = TAlphaColorRec::Lightskyblue) {PressColor = color; state = PRESSED; }
	bool isSelect() { return (state == SELECTED) ? true : false; }
	void SetText(AnsiString str, int text_size) { Text = str; TextSize = text_size;	}
	void SetVisible(bool value) { state = NORMAL; Visible = value; }
	bool isVisible() {return Visible; }

private:
	State state;
	int TextSize = 0;
    AnsiString Text = "";
	int X = 0, Y = 0;
	int sizeX = 0, sizeY = 0;
	TBitmap *bitmap;
    bool Visible = false;
	int Thickness = 8;
    TAlphaColor PressColor;
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
		int Topic;
        int Category;
		int ModalityType;
        int TestType;
        int Number;
    };

	enum SettingsName : int
	{
	   PathToQuestions,
	   LearnEnable,
	   TestEnable,
	   QPlus,
	   QQuastion,
       QRest,
	   TPlus,
       TShowResult,
	   TShowResultTime,
       QTRest
    };

	enum State : int
	{
	   LEARNING,
	   TESTING,
       REST,
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

	enum QuestionType : int
	{
		TEXT,
		SOUND,
        ALL
    };

	TLearningTask(AnsiString _name, TMediaPlayer *_player);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void Draw() override;
	void CloseTask() override;
	void ExternalTrigger(int trigger) override;

	void LoadQuestions();
	bool Questions();
	bool Testing();

	void get_modality_type();
    void get_test_type();

    ~TLearningTask();

private:
	TMediaPlayer *MediaPlayer;

	std::vector<TQuestion> QList;
	int QTrialCount = 0;

    int TTrialCount = 0;

	TButtonFigure *ButtonYes;
	TButtonFigure *ButtonNo;
};

#endif
