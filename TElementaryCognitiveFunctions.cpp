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
	   std::vector<int> sequence;
		for(int i = 0; i < Blocks.size(); i++){
			if(Blocks[i]->isEnable()) {
			   sequence.push_back(i);
			   if(Blocks[i]->GetTaskName() == "Комбинированные функции") {
				  sequence.push_back(i);
                  sequence.push_back(i);
               }
            }
		}
	   srand(time(NULL));
	   for(int i = 0; i < Settings->getInt(PartsCount); i++) {
		std::random_shuffle(sequence.begin(), sequence.end());
		BlockSequence.push_back(sequence);
	   }
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
				Protocol->NextBlock(CreateProtocol());
				OwnProtocol->BACKGROUND_TIME = millis();
				Protocol->NextBlock(Blocks[BlockSequence[PartCount][cur_block]]->CreateProtocol());
			#endif
			break;
		}
		case RUN_BLOCK:
		{
			if(Blocks[BlockSequence[PartCount][cur_block]]->Finished()){
				cur_block++;
				if(cur_block >= BlockSequence[PartCount].size())
				{
				   state = PAUSE;
				   cur_block = 0;
				   Protocol->NextBlock(CreateProtocol());
				} else {
                    Protocol->NextBlock(Blocks[BlockSequence[PartCount][cur_block]]->CreateProtocol());
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
                OwnProtocol->PAUSE_TIME = millis();
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
