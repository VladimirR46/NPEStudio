//---------------------------------------------------------------------------

#pragma hdrstop

#include "TLearningTask.h"
#include "ComObj.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)


//------------------------------------------------------------------------------
TLearningTask::TLearningTask(AnsiString _name, TMediaPlayer *_player)
	: TBaseTask(_name)
{
	MediaPlayer = _player;

	if(!Settings->Load(_name)){
		Settings->Add(PathToQuestions, "Путь к вопросам", GuiType::TDirectoryPath, "");
		Settings->Add(LearnEnable, "Обучение", GuiType::TCheckBox, 1);
		Settings->Add(TestEnable, "Тестирование", GuiType::TCheckBox, 1);
		Settings->Add(QPlus, "Обучение: крест", GuiType::TRange, "1000:2000");
		Settings->Add(QQuastion, "Обучение: утверждение", GuiType::TRange, "1000:2000");
        Settings->Add(QRest, "Обучение: отдых", GuiType::TRange, "1000:2000");
		Settings->Add(TPlus, "Тестирование: крест", GuiType::TRange, "1000:2000");
		Settings->Save(_name);
	}

}
//------------------------------------------------------------------------------
bool TLearningTask::Questions()
{
	if(QTrialCount == QList.size() || Settings->getInt(LearnEnable) == 0)
	{
		return true;
	}

	switch(qstate) {
		case QuestionState::PLUS:
		{
			ClearCanva();
			DrawPlus();
            Timer->Interval = Settings->getRandFromRange(QPlus);
			qstate = QuestionState::QUESTION;
			break;
		}
		case QuestionState::QUESTION:
		{
			ClearCanva();

			if(QList[QTrialCount].Type == "текст" || QList[QTrialCount].Type == "текст+звук"){
                DrawText(QList[QTrialCount].Question+" "+QList[QTrialCount].Goal, 66);
			}

			if(QList[QTrialCount].Type == "звук" || QList[QTrialCount].Type == "текст+звук") {
			   if(FileExists(Settings->get(PathToQuestions)+"\\sound\\"+QList[QTrialCount].SoundName)){
				   MediaPlayer->FileName = Settings->get(PathToQuestions)+"\\sound\\"+QList[QTrialCount].SoundName;
				   MediaPlayer->Play();
			   }
            }


			Timer->Interval = Settings->getRandFromRange(QQuastion);
			qstate = QuestionState::REST;
			break;
		}
		case QuestionState::REST:
		{
			ClearCanva();
            Timer->Interval = Settings->getRandFromRange(QRest);
			qstate = QuestionState::PLUS;
			QTrialCount++;
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
	if(Settings->getInt(TestEnable) == 0)
	{
		return true;
	}

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
void TLearningTask::LoadQuestions()
{
 	QList.clear();

	UnicodeString excel_filename = Settings->get(PathToQuestions)+"\\Questions.xlsx";

    try
	{
		Variant exl = CreateOleObject("Excel.Application");
		exl.OlePropertyGet("Workbooks").OleProcedure("Open", static_cast<WideString>(excel_filename));
		for(int i = 2 ; i < 5; i++)
		{
			TQuestion question;
			question.Question = exl.OlePropertyGet("Cells", i, 2);
			question.Goal = exl.OlePropertyGet("Cells", i, 3);
			question.Ungoal = exl.OlePropertyGet("Cells", i, 4);
			question.Type = exl.OlePropertyGet("Cells", i, 5);
			question.SoundName = exl.OlePropertyGet("Cells", i, 6);
			QList.push_back(question);
		}
        exl.OleProcedure("Quit");
    }
	catch(...){
		ShowMessage("Ошибка при загрузке xls файла");
	}

    /*
	TStringList *list = new TStringList();
	list->LoadFromFile(Settings->get(PathToQuestions)+"\\Questions.txt", TEncoding::UTF8);
	for(int i = 0; i < list->Count; i++)
	{
		if(list->Strings[i] == "") break;
		TStringDynArray data = SplitString(list->Strings[i],"!");
		question_list.push_back(data[0]);
		goal_list.push_back(data[1]);
		ungoal_list.push_back(data[2]);
    }
	delete list;
	*/
}
//------------------------------------------------------------------------------
void TLearningTask::InitTask(AnsiString Path)
{
    LoadQuestions();
	state = LEARNING;
	qstate = QuestionState::PLUS;
	QTrialCount = 0;
   	Timer->Enabled = true;
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
	if(state == FINISHED) {
        Timer->Enabled = false;
		return true;
	}
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
