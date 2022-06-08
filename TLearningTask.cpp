//---------------------------------------------------------------------------

#pragma hdrstop

#include "TLearningTask.h"
#include "ComObj.hpp"
#include <algorithm>
#include <random>

#define PROTOCOL_LOGGER 1

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
		Settings->Add(QBackgroundActivity, "Обучение: Фоновая активность", GuiType::TEdit, "120000");
		Settings->Add(QPlus, "Обучение: крест", GuiType::TRange, "1000:1500");
		Settings->Add(QQuastion, "Обучение: утверждение", GuiType::TRange, "1000:2000"); //!!
		Settings->Add(QRest, "Обучение: отдых", GuiType::TRange, "3000:5000");
		Settings->Add(TPlus, "Тест: крест", GuiType::TRange, "1000:1500");
		Settings->Add(TRest, "Тест: отдых", GuiType::TRange, "1500:2000");
        Settings->Add(TShowResult, "Показа результата", GuiType::TCheckBox, 1);
		Settings->Add(TBackgroundActivity, "Тест: Фоновая активность", GuiType::TEdit, "120000");
		Settings->Add(TShowResultTime, "Время показа результата", GuiType::TEdit, "1500");
		Settings->Add(QTRest, "Отдых", GuiType::TEdit, "300000");
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
	if(qstate == QuestionState::END || Settings->getInt(LearnEnable) == 0) return true;

	#ifdef PROTOCOL_LOGGER
		if(!QInit){
			qpBlock = Protocol->AddBlock<QProtocolBlock>();
			qpBlock->BlockName = "Questions";
            QInit = true;
		}
	#endif

	switch(qstate)
	{
		case QuestionState::BEGIN:
		{
			ClearCanva();
			DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100);

			if(QTrialCount == 0) qstate = QuestionState::VAS;
			else qstate = QuestionState::READY;

			Timer->Interval = Settings->getInt(QBackgroundActivity);

			qpBlock->SetBackgroundTime(millis(), millis()+Timer->Interval);

            break;
		}
		case QuestionState::PLUS:
		{
			ClearCanva();
			DrawPlus();
			DrawPoint(bitmap->Width-60, bitmap->Height-60, 20, TAlphaColorRec::White);

			#ifdef PROTOCOL_LOGGER
				qpTrial = qpBlock->AddTrial<QProtocolBlock::Trial>();
				qpTrial->StateTime[(int)QuestionState::PLUS] = millis();
			#endif

			int range = Settings->getRandFromRange(QPlus);
			UnicodeString FileName = Settings->get(PathToQuestions)+"\\sounds\\"+IntToStr(QList[QTrialCount].Number)+".wav";
			if(FileExists(FileName)) {
				unsigned int t1= millis();
				MediaPlayer->FileName = FileName;
				unsigned int dt = millis() - t1;

				if(dt >= range) range = 1;
				else range = range - dt;
			}

			Timer->Interval =  range;
			qstate = QuestionState::QUESTION;

			break;
		}
		case QuestionState::QUESTION:
		{
			ClearCanva();


			int duration = (MediaPlayer->Duration / MediaTimeScale)*1000.0;

            #ifdef PROTOCOL_LOGGER
				qpTrial->StateTime[(int)QuestionState::QUESTION] = millis();
				qpTrial->Number = QList[QTrialCount].Number;
				qpTrial->ModalityType = QList[QTrialCount].ModalityType;
				qpTrial->Question = QList[QTrialCount].Question+" "+QList[QTrialCount].Goal;
				qpTrial->Topic = QList[QTrialCount].Topic;
				qpTrial->Category = QList[QTrialCount].Category;
			#endif

			if(QList[QTrialCount].ModalityType == QuestionType::TEXT || QList[QTrialCount].ModalityType == QuestionType::ALL){
				DrawText(QList[QTrialCount].Question+" "+QList[QTrialCount].Goal, 66, 100);
			}

			if(QList[QTrialCount].ModalityType == QuestionType::SOUND || QList[QTrialCount].ModalityType == QuestionType::ALL) {
			   MediaPlayer->Play();
			}

			Timer->Interval = duration+500; //Settings->getRandFromRange(QQuastion);
			qstate = QuestionState::REST;
			break;
		}
		case QuestionState::REST:
		{
			ClearCanva();
			#ifdef PROTOCOL_LOGGER
				qpTrial->StateTime[(int)QuestionState::REST] = millis();
			#endif

            Timer->Interval = Settings->getRandFromRange(QRest);
			qstate = QuestionState::PLUS;
			QTrialCount++;

			if(QTrialCount == QVASInterval) {
				qstate = QuestionState::VAS;
				QVASInterval += BLOCK_SIZE;
			}

			break;
		}
		case QuestionState::VAS:
		{
			ClearCanva(TAlphaColorRec::Black);
			if(QTrialCount == 0) VAS->Init({vasMental,vasPhysical},"Ментальная усталость");
			if(QTrialCount > 0) VAS->Init({vasMental,vasPhysical,vasBoredom,vasEffort},"Ментальная усталость");
			VAS->Run();

            Timer->Interval = 0;
			break;
		}
		case QuestionState::READY:
		{
			ClearCanva(TAlphaColorRec::Black);
            DrawText("Приготовьтесь", 66, 100);
            qstate = QuestionState::PLUS;
			Timer->Interval = 5000;
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
	if(tstate == TestingState::END || Settings->getInt(TestEnable) == 0) return true;

    #ifdef PROTOCOL_LOGGER
		if(!TInit){
            shuffle_by_blocks();
			tpBlock = Protocol->AddBlock<TProtocolBlock>();
			tpBlock->BlockName = "Testing";
            TInit = true;
		}
	#endif

	switch(tstate)
	{
        case TestingState::BEGIN:
		{
			ClearCanva();
			DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100);

			if(TTrialCount == 0) tstate = TestingState::VAS;
			else tstate = TestingState::END;

			Timer->Interval = Settings->getInt(TBackgroundActivity);

			tpBlock->SetBackgroundTime(millis(), millis()+Timer->Interval);
			break;
		}
		case TestingState::PLUS:
		{
			ClearCanva();
			DrawPlus();
			DrawPoint(bitmap->Width-60, bitmap->Height-60, 20, TAlphaColorRec::White);

			#ifdef PROTOCOL_LOGGER
				tpTrial = tpBlock->AddTrial<TProtocolBlock::Trial>();
				tpTrial->StateTime[(int)TestingState::PLUS] = millis();
			#endif

			Timer->Interval = Settings->getRandFromRange(TPlus);
			tstate = TestingState::QUESTION;
			break;
		}
		case TestingState::QUESTION:
		{
			ClearCanva();

			AnsiString Text = "";
			if(QList[TTrialCount].TestType == 0)  // Целевая
				Text = QList[TTrialCount].Question+" "+QList[TTrialCount].Goal;
			if(QList[TTrialCount].TestType == 1)
				Text = QList[TTrialCount].Question+" "+QList[TTrialCount].Ungoal;

			DrawText(Text, 48, 100);

            #ifdef PROTOCOL_LOGGER
				tpTrial->StateTime[(int)TestingState::QUESTION] = millis();
                tpTrial->Number = QList[TTrialCount].Number;
				tpTrial->ModalityType = QList[TTrialCount].ModalityType;
				tpTrial->Question = Text;
				tpTrial->Topic = QList[TTrialCount].Topic;
				tpTrial->Category = QList[TTrialCount].Category;
				tpTrial->TestType = QList[TTrialCount].TestType;
			#endif

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

			#ifdef PROTOCOL_LOGGER
				tpTrial->StateTime[(int)TestingState::REST] = millis();
			#endif

			TTrialCount++;
			Timer->Interval = Settings->getRandFromRange(TRest);
			tstate = TestingState::PLUS;

			if(TTrialCount == TVASInterval) {
				tstate = TestingState::VAS;
				TVASInterval += BLOCK_SIZE;
			}

			break;
		}
        case TestingState::VAS:
		{
			ClearCanva(TAlphaColorRec::Black);
			if(TTrialCount == 0) VAS->Init({vasMental,vasPhysical},"Ментальная усталость");
			if(TTrialCount > 0) VAS->Init({vasMental,vasPhysical,vasBoredom,vasEffort},"Ментальная усталость");
			VAS->Run();

            Timer->Interval = 0;
			break;
		}
		case TestingState::READY:
		{
			ClearCanva(TAlphaColorRec::Black);
			DrawText("Приготовьтесь", 66, 100);
			tstate = TestingState::PLUS;
			Timer->Interval = 5000;
			break;
		}
		default:
			break;
	}
	return false;
}
//------------------------------------------------------------------------------
void TLearningTask::VasFinished(TObject *Sender)
{
	 ClearCanva();

	 if(state == LEARNING){

		if(QTrialCount == 0 ) qstate = QuestionState::READY;
		else if(QTrialCount == QList.size()) qstate = QuestionState::END;
		else qstate = QuestionState::BEGIN;

        #ifdef PROTOCOL_LOGGER
			for(int i = 0; i < VAS->vasQueue.size(); i++)
			{
				if(VAS->vasQueue[i].type == vasMental) qpBlock->pMental.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasPhysical) qpBlock->pPhysical.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasBoredom) qpBlock->pBoredom.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasEffort) qpBlock->pEffort.push_back(VAS->vasQueue[i]);
			}
		#endif
     }
	 if(state == TESTING){

		if(TTrialCount == QList.size()) tstate = TestingState::BEGIN;
		else tstate = TestingState::READY;

        #ifdef PROTOCOL_LOGGER
			for(int i = 0; i < VAS->vasQueue.size(); i++)
			{
				if(VAS->vasQueue[i].type == vasMental) tpBlock->pMental.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasPhysical) tpBlock->pPhysical.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasBoredom) tpBlock->pBoredom.push_back(VAS->vasQueue[i]);
				else if(VAS->vasQueue[i].type == vasEffort) tpBlock->pEffort.push_back(VAS->vasQueue[i]);
			}
		#endif
	 }

	 Timer->Interval = 300;
}
//------------------------------------------------------------------------------
void TLearningTask::ExternalTrigger(int trigger)
{
   if((ButtonYes->isVisible() || ButtonNo->isVisible()) && tstate == TestingState::QUESTION)
   {
	   #ifdef PROTOCOL_LOGGER
		TProtocolBlock::Trial::KeyInfo info;
		info.key = trigger;
        info.time = millis();
		tpTrial->key_list.push_back(info);
	   #endif

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
void TLearningTask::shuffle_by_blocks()
{
	std::vector<TQuestion> Category[2];

	for(int i = 0; i < QList.size(); i++) Category[QList[i].Category-1].push_back(QList[i]);


	srand(time(NULL));
	for(int i = 0; i < 2; i++) std::random_shuffle(begin(Category[i]), end(Category[i]));

	std::vector<int> modality[3];
    std::vector<int> test[3];
	for(int i = 0; i < QList.size()/3; i++){
		 for(int j = 0; j < 3; j++){
			modality[j].push_back(i%3);
			test[j].push_back(i%2);
         }
	}

    srand(time(NULL));
	for(int i = 0; i < 3; i++) {
		std::random_shuffle(begin(modality[i]), end(modality[i]));
		std::random_shuffle(begin(test[i]), end(test[i]));
	}


	std::vector<TQuestion> blocks[3];
	for(int i = 0, i_block = 0; i < QList.size()/2; i++){
	   blocks[i_block].push_back(Category[0][i]);
	   blocks[i_block].push_back(Category[1][i]);

	   if(blocks[i_block].size() == QList.size()/3) i_block++;
	}

    for(int i = 0; i < 3; i++) {
	   std::random_shuffle(begin(blocks[i]), end(blocks[i]));
	}

	int size = QList.size();
	QList.clear();
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < blocks[i].size(); j++){
			blocks[i][j].ModalityType = modality[i][j];
			blocks[i][j].TestType = test[i][j];
			QList.push_back(blocks[i][j]);
		}
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

	if(Settings->get(PathToQuestions) == "") {
		ShowMessage("Ошибка: укажите путь к папке с вопросами в настройках.");
        Form->Close();
		return;
	}

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

	shuffle_by_blocks();

	BLOCK_SIZE = QList.size()/3;
}
//------------------------------------------------------------------------------
void TLearningTask::InitTask(AnsiString Path)
{
    HideCursor();
	LoadQuestions();
	ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
	ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);
	ButtonYes->SetVisible(false);
	ButtonNo->SetVisible(false);
	state = LEARNING;
	qstate = QuestionState::BEGIN;

	if(Settings->getInt(TestEnable) && Settings->getInt(LearnEnable)) tstate = TestingState::VAS;
	else tstate = TestingState::BEGIN;

	QTrialCount = 0;
    TTrialCount = 0;
    Timer->Interval = 100;
	Timer->Enabled = true;

	QInit = false;
	TInit = false;

	QVASInterval = BLOCK_SIZE;
	TVASInterval = BLOCK_SIZE;

	mpBlock = Protocol->AddBlock<MProtocolBlock>();
    mpBlock->BlockName = "Main";
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
			 else state = END;
            }
			break;
		}
		case TESTING:
		{
			if(Testing()) state = END;
			break;
		}
        case REST:
		{
			ClearCanva();
			DrawText("Отдых\n Оставляйте глаза открытыми",66);

			mpBlock->SetGlobalRestTime(millis(), millis()+Settings->getInt(QTRest));

			std::random_shuffle(begin(QList), end(QList));
			state = TESTING;
			Timer->Interval = Settings->getInt(QTRest);
			break;
		}
		case END:
		{
			ClearCanva();
			DrawText("Спасибо за участие!",66);
			state = FINISHED;
			Timer->Interval = 5000;
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

   Protocol->Save(GetTaskName());
}
//------------------------------------------------------------------------------
TLearningTask::~TLearningTask()
{
   delete ButtonYes;
   delete ButtonNo;
}
