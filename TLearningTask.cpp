//---------------------------------------------------------------------------

#pragma hdrstop

#include "TLearningTask.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


//------------------------------------------------------------------------------
TLearningTask::TLearningTask(AnsiString _name)
	: TBaseTask(_name)
{
	if(!Settings->Load(_name)){

		Settings->Add(PathToQuestions, "Путь к вопросам", GuiType::TDirectoryPath, "");
		Settings->Add(isLearning, "Обучение", GuiType::TCheckBox, 1);
		Settings->Add(isTesting, "Тестирование", GuiType::TCheckBox, 1);
		Settings->Add(QPlus, "Обучение крест", GuiType::TRange, "1000:2000");
		Settings->Add(TPlus, "Тестирование крест", GuiType::TRange, "1000:2000");
		Settings->Save(_name);
	}

}
//------------------------------------------------------------------------------
bool TLearningTask::Questions()
{
	switch(qstate) {
		case QuestionState::PLUS:
		{
			ClearCanva();
			DrawPlus();
            Timer->Interval = Settings->getRandFromRange(QPlus);
			break;
		}
		default:
			break;
	}
	return false;
}
//------------------------------------------------------------------------------
bool TLearningTask::Testing()
{
	switch(tstate) {
		case TestingState::PLUS:
		{
			ClearCanva();
            DrawPlus();
            Timer->Interval = Settings->getRandFromRange(TPlus);
			break;
		}
		default:
			break;
	}
	return false;
}
//------------------------------------------------------------------------------
void TLearningTask::InitTask(AnsiString Path)
{
	state = LEARNING;
}
//------------------------------------------------------------------------------
void TLearningTask::StateManager()
{
    switch(state) {
		case LEARNING:
		{
			if(Questions()) state = TESTING;
			break;
		}
		case TESTING:
		{
			if(Testing()) state = FINISHED;
			break;
		}
		default:
			break;
	}
}
//------------------------------------------------------------------------------
bool TLearningTask::Finished()
{
	if(state == FINISHED) return true;
	return false;
}
//------------------------------------------------------------------------------
void TLearningTask::UserMouseDown(int X, int Y)
{

}
//------------------------------------------------------------------------------
void TLearningTask::Draw()
{
   	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
//------------------------------------------------------------------------------
void TLearningTask::CloseTask()
{

}
