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
        Settings->Add(TShowResult, "Показа результата", GuiType::TCheckBox, 1);
		Settings->Add(TShowResultTime, "Время показа результата", GuiType::TEdit, "1500");
        Settings->Add(QTRest, "Отдых", GuiType::TEdit, "4000");
		Settings->Save(_name);
	}

	ButtonYes = new TButtonFigure(bitmap.get());
	ButtonNo = new TButtonFigure(bitmap.get());
	ButtonYes->SetPosition(int(Screen->Size().Width) / 5, (int(Screen->Size().Height) / 4) * 3);
	ButtonYes->SetSize(140,60);
	ButtonYes->SetText("Да", 55);
	ButtonNo->SetPosition((int(Screen->Size().Width) / 5)*4,(int(Screen->Size().Height) / 4) * 3);
	ButtonNo->SetSize(140,60);
	ButtonNo->SetText("Нет", 55);

}
//------------------------------------------------------------------------------
bool TLearningTask::Questions()
{
	if(QTrialCount == QList.size() || Settings->getInt(LearnEnable) == 0) return true;

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

            /*
			if(QList[QTrialCount].Type == "звук" || QList[QTrialCount].Type == "текст+звук") {
			   if(FileExists(Settings->get(PathToQuestions)+"\\sound\\"+QList[QTrialCount].SoundName)){
				   unsigned int t1= millis();
				   MediaPlayer->FileName = Settings->get(PathToQuestions)+"\\sound\\"+QList[QTrialCount].SoundName;
				   unsigned int dt = millis() - t1;
				   int duration = (MediaPlayer->Duration / MediaTimeScale)*1000.0;
				   MediaPlayer->Play();
			   }
			}
			*/


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
	if(TTrialCount == QList.size() || Settings->getInt(TestEnable) == 0) return true;

	switch(tstate)
	{
		case TestingState::PLUS:
		{
			ClearCanva();
            DrawPlus();
			Timer->Interval = Settings->getRandFromRange(TPlus);
			tstate = TestingState::QUESTION;
			break;
		}
		case TestingState::QUESTION:
		{
			ClearCanva();

			if(QList[TTrialCount].Sample == "целевая")
				DrawText(QList[TTrialCount].Question+" "+QList[TTrialCount].Goal,48);
			if(QList[TTrialCount].Sample == "нецелевая")
				DrawText(QList[TTrialCount].Question+" "+QList[TTrialCount].Ungoal,48);

			ButtonYes->SetVisible(true);
            ButtonNo->SetVisible(true);
            Timer->Interval = 0;
			break;
		}
		case TestingState::REST:
		{
			ButtonYes->SetVisible(false);
            ButtonNo->SetVisible(false);
			ClearCanva();
            TTrialCount++;
			Timer->Interval = 2000;
			tstate = TestingState::PLUS;
			break;
		}
		default:
			break;
	}
	return false;
}
//------------------------------------------------------------------------------
void TLearningTask::ExternalTrigger(int trigger)
{
   if((ButtonYes->isVisible() || ButtonNo->isVisible()) && tstate == TestingState::QUESTION)
   {
	   TAlphaColor color = TAlphaColorRec::Lightskyblue;
	   if(Settings->getInt(TShowResult))
	   {
		   if((trigger == 0 && QList[TTrialCount].Sample == "целевая") || (trigger == 1 && QList[TTrialCount].Sample == "нецелевая"))
			color = TAlphaColorRec::Lightgreen;
		   else
			color = TAlphaColorRec::Lightcoral;
       }

	  if(trigger == 0) {
		if(ButtonYes->isSelect()){
			ButtonYes->Press(color);
			tstate = TestingState::REST;
			Timer->Interval = Settings->getInt(TShowResultTime);
		}
		else {
			ButtonYes->Select();
			ButtonNo->UnSelect();
		}
	  }

      if(trigger == 1) {
		if(ButtonNo->isSelect()){
			ButtonNo->Press(color);
			tstate = TestingState::REST;
			Timer->Interval = Settings->getInt(TShowResultTime);
		}
		else {
			ButtonNo->Select();
            ButtonYes->UnSelect();
		}
	  }
   }
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
		for(int i = 2 ; i < 32; i++)
		{
			TQuestion question;
			question.Question = exl.OlePropertyGet("Cells", i, 2);
			question.Goal = exl.OlePropertyGet("Cells", i, 3);
			question.Ungoal = exl.OlePropertyGet("Cells", i, 4);
			question.Type = exl.OlePropertyGet("Cells", i, 5);
			question.Sample = exl.OlePropertyGet("Cells", i, 6);
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
	ButtonYes->SetVisible(false);
	ButtonNo->SetVisible(false);
	state = LEARNING;
	qstate = QuestionState::PLUS;
    tstate = TestingState::PLUS;
	QTrialCount = 0;
    TTrialCount = 0;
    Timer->Interval = 100;
	Timer->Enabled = true;
}
//------------------------------------------------------------------------------
void TLearningTask::StateManager()
{
    switch(state) {
		case LEARNING:
		{
			if(Questions()){
			 if(Settings->getInt(TestEnable) && !Settings->getInt(LearnEnable)) state = TESTING;
             else if(Settings->getInt(TestEnable) && Settings->getInt(LearnEnable)) state = REST;
			 else state = FINISHED;
            }
			break;
		}
		case TESTING:
		{
			if(Testing()) state = FINISHED;
			break;
		}
        case REST:
		{
			ClearCanva();
			DrawText("Отдых",66);
            state = TESTING;
            Timer->Interval = Settings->getInt(QTRest);
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
void TLearningTask::UserMouseDown(int X, int Y, TMouseButton Button)
{
	if(Button == TMouseButton::mbLeft)
		ExternalTrigger(0);
	if(Button == TMouseButton::mbRight)
    	ExternalTrigger(1);
}
//------------------------------------------------------------------------------
void TLearningTask::Draw()
{
	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 ButtonYes->Draw();
	 ButtonNo->Draw();
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
//------------------------------------------------------------------------------
void TLearningTask::CloseTask()
{
   Timer->Enabled = false;
   ClearCanva();
   ButtonYes->SetVisible(false);
   ButtonNo->SetVisible(false);
}
//------------------------------------------------------------------------------
TLearningTask::~TLearningTask()
{
   delete ButtonYes;
   delete ButtonNo;
}
