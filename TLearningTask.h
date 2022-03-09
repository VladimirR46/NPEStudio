//---------------------------------------------------------------------------

#ifndef TLearningTaskH
#define TLearningTaskH

#include "TBaseTask.h"
#include <FMX.Media.hpp>
//---------------------------------------------------------------------------

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
        p2
	} tstate;

	TLearningTask(AnsiString _name, TMediaPlayer *_player);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;

	void LoadQuestions();
	bool Questions();
	bool Testing();
private:
	TMediaPlayer *MediaPlayer;

	std::vector<TQuestion> QList;
    int QTrialCount = 0;
};

#endif
