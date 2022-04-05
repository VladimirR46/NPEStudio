//---------------------------------------------------------------------------

#pragma hdrstop

#include "TLearningTask.h"
#include "ComObj.hpp"
#include <algorithm>
#include <random>
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
	ButtonYes->SetSize(140,60);
	ButtonYes->SetText("Да", 55);
	ButtonNo->SetSize(140,60);
	ButtonNo->SetText("Нет", 55);

}
//------------------------------------------------------------------------------
bool TLearningTask::Questions()
{
	if(QTrialCount == QList.size() || Settings->getInt(LearnEnable) == 0) return true;

	switch(qstate)
	{
		case QuestionState::PLUS:
		{
			ClearCanva();
			DrawPlus();

			int range = Settings->getRandFromRange(QPlus);
			UnicodeString FileName = Settings->get(PathToQuestions)+"\\sounds\\"+IntToStr(QList[QTrialCount].Number)+".wav";
			if(FileExists(FileName)) {
				unsigned int t1= millis();
				MediaPlayer->FileName = FileName;
				unsigned int dt = millis() - t1;

				if(dt >= Settings->getRandFromRange(QPlus)) range = 1;
				else range = Settings->getRandFromRange(QPlus) - dt;
			}

			Timer->Interval =  range;
			qstate = QuestionState::QUESTION;
			break;
		}
		case QuestionState::QUESTION:
		{
			ClearCanva();

			int duration = (MediaPlayer->Duration / MediaTimeScale)*1000.0;

			if(QList[QTrialCount].ModalityType == QuestionType::TEXT || QList[QTrialCount].ModalityType == QuestionType::ALL){
				DrawText(QList[QTrialCount].Question+" "+QList[QTrialCount].Goal, 66);
			}

			if(QList[QTrialCount].ModalityType == QuestionType::SOUND || QList[QTrialCount].ModalityType == QuestionType::ALL) {
			   MediaPlayer->Play();
			}

			Timer->Interval = duration+500;//Settings->getRandFromRange(QQuastion);
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

			if(QList[TTrialCount].TestType == 0)  // Целевая
				DrawText(QList[TTrialCount].Question+" "+QList[TTrialCount].Goal,48);
			if(QList[TTrialCount].TestType == 1)
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
		   if((trigger == 0 && QList[TTrialCount].TestType == 0) || (trigger == 1 && QList[TTrialCount].TestType == 1))
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
void TLearningTask::get_test_type()
{
	srand(time(NULL));
	std::vector<int> type1;
	std::vector<int> type2;
	for(int i = 0; i < QList.size()/2; i++) {
		type1.push_back(i%2);
		type2.push_back(i%2);
	}

	std::random_shuffle(begin(type1), end(type1));
	std::random_shuffle(begin(type2), end(type2));

    for(int i = 0; i < QList.size(); i++){
		if(QList[i].Category == 1)
			QList[i].TestType = type1[i];
		if(QList[i].Category == 2)
			QList[i].TestType = type2[i];
	}
}
//------------------------------------------------------------------------------
void TLearningTask::get_modality_type()
{
	srand(time(NULL));
	std::vector<int> qtypes;
	for(int i = 0; i < QList.size(); i++) qtypes.push_back(i%3);

	std::random_shuffle(begin(qtypes), end(qtypes));

	for(int i = 0; i < QList.size(); i++){
        QList[i].ModalityType = qtypes[i];
	}
}
//------------------------------------------------------------------------------
void TLearningTask::LoadQuestions()
{
 	QList.clear();

	UnicodeString excel_filename = Settings->get(PathToQuestions)+"\\Questions.xlsx";
    /*
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
	*/

	TStringList *list = new TStringList();
	list->LoadFromFile(Settings->get(PathToQuestions)+"\\Questions.csv", TEncoding::UTF8);

	for(int i = 1; i < list->Count; i++)
	{
        TQuestion question;
		TStringDynArray data = SplitString(list->Strings[i],";");
		question.Question = data[1];
		question.Goal = data[2];
		question.Ungoal = data[3];
		question.Topic = StrToInt(data[4]);
		question.Category = StrToInt(data[5]);
		question.Number = i;
        QList.push_back(question);
	}
	delete list;

    get_modality_type();
    get_test_type();

	srand(time(NULL));
	std::random_shuffle(begin(QList), end(QList));
}
//------------------------------------------------------------------------------
void TLearningTask::InitTask(AnsiString Path)
{
	LoadQuestions();
	ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
	ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);
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
            std::random_shuffle(begin(QList), end(QList));
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
    // For Debugging
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
