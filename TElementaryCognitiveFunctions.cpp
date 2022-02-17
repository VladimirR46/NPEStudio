//---------------------------------------------------------------------------

#pragma hdrstop

#include "TElementaryCognitiveFunctions.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)


TElementaryCognitiveFunctions::TElementaryCognitiveFunctions(AnsiString _name)
	: TBaseTask(_name)
{
	cur_block = 0;

	if(!Settings->Load(_name)){
		Settings->Add(PartsCount, "Частей", GuiType::TEdit, "3");
		Settings->Add(RangeBackground, "Фон", GuiType::TEdit, "1000");
		Settings->Add(RangeBreak, "Перерыв", GuiType::TEdit, "2500");
		Settings->Save(_name);
	}

}
//------------------------------------------------------------------------------
void TElementaryCognitiveFunctions::InitTask(AnsiString Path)
{
	   state = BACKGROUND;
	   PartCount = 0;
       cur_block = 0;
	   BlockSequence.clear();
       Protocol->ResetBlockCounter();

	   std::vector<int> sequence{0, 1, 2, 3, 3, 3};
       srand(time(NULL));
	   for(int i = 0; i < Settings->getInt(PartsCount); i++) {
        std::random_shuffle(sequence.begin(), sequence.end());
		BlockSequence.push_back(sequence);
	   }

	   Protocol->AddDescription("Описание протокола "+ TaskName);
	   Protocol->AddDescription("0 - фон\n2 - перерыв\n");
}

void TElementaryCognitiveFunctions::StateManager()
{
	switch(state) {
		case BACKGROUND:
		{
			ClearCanva();
			DrawText("Запись фоновой активности.", 80);
			state = RUN_BLOCK;
			Timer->Interval = Settings->getInt(RangeBackground);  // 60000
            Timer->Enabled = true;
			#ifdef PROTOCOL_LOGGER
				Protocol->CurrentBlock(GetTaskName());
				Trial = TrialProtocol::CreateTrial(Protocol);
				Trial->BACKGROUND_TIME = millis();
				Protocol->NextBlock(Blocks[BlockSequence[PartCount][cur_block]]->GetTaskName());
			#endif
			break;
		}
		case RUN_BLOCK:
		{
			if(Blocks[BlockSequence[PartCount][cur_block]]->Finished()){
				cur_block++;
                Protocol->NextBlock(Blocks[BlockSequence[PartCount][cur_block]]->GetTaskName());
				if(cur_block >= BlockSequence[PartCount].size())
				{
				   state = PAUSE;
				   cur_block = 0;
				}
				StateManager();
			}
			else{
				Blocks[BlockSequence[PartCount][cur_block]]->StateManager();
			}
			break;
		}
		case PAUSE:
		{
			ClearCanva();
			Timer->Interval = Settings->get(RangeBreak).ToInt();
			DrawText("Перерыв", 80);
            state = BACKGROUND;
			PartCount++;
			#ifdef PROTOCOL_LOGGER
				Trial->PAUSE_TIME = millis();
				Protocol->NextBlock(GetTaskName());
			#endif
			break;
		}
		default:
				break;
	}
}

bool TElementaryCognitiveFunctions::Finished()
{
    if(PartCount == Settings->getInt(PartsCount)) return true;
	return false;
}

void TElementaryCognitiveFunctions::UserMouseDown(int X, int Y)
{
   if(state == RUN_BLOCK) Blocks[BlockSequence[PartCount][cur_block]]->UserMouseDown(X,Y);
}

void TElementaryCognitiveFunctions::Draw()
{
   	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}

void TElementaryCognitiveFunctions::CloseTask()
{
    Protocol->Save();
}
