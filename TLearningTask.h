//---------------------------------------------------------------------------

#ifndef TLearningTaskH
#define TLearningTaskH

#include "TBaseTask.h"
//---------------------------------------------------------------------------

class TLearningTask : public TBaseTask
{
public:
	enum SettingsName : int
	{
	   PathToQuestions,
	   isLearning,
	   isTesting,
	   QPlus,
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
        p2
	} qstate;

	enum class TestingState : int
	{
		PLUS,
        p2
	} tstate;

	TLearningTask(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;

	bool Questions();
	bool Testing();
private:

};

#endif
