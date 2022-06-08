//---------------------------------------------------------------------------
#pragma hdrstop
#include "TElementaryCognitiveFunctions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


/* TDrawHint */
TDrawHint::TDrawHint(TBitmap *_bitmap) : bitmap(_bitmap)
{
	timer = new TTimer(NULL);
    timer->Enabled = false;
	timer->OnTimer = TimerEvent;
}

TDrawHint::~TDrawHint()
{
	delete timer;
}

void __fastcall TDrawHint::TimerEvent(TObject* Sender)
{
	Hide();
	timer->Enabled = false;
}

void TDrawHint::Draw()
{
	rect = System::Types::TRectF(X-sizeX,Y-sizeY,X+sizeX,Y+sizeY);

	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Thickness = 3;
	bitmap->Canvas->Font->Size = 55;
	bitmap->Canvas->Stroke->Color = 0xFF252525;//0x17191B;
	bitmap->Canvas->Fill->Color = 0xFF101010; /*TAlphaColors::Lightgrey*/;
	bitmap->Canvas->FillRect(rect, 1);
	bitmap->Canvas->DrawRect(rect, 1);
	bitmap->Canvas->Fill->Color = TAlphaColors::White;
	if(isShow) bitmap->Canvas->FillText(rect, text, false, 100, TFillTextFlags(),TTextAlign::Center, TTextAlign::Center);
	bitmap->Canvas->EndScene();
}

bool TDrawHint::Contains(int _x, int _y)
{
	if(rect.Contains(System::Types::TPoint(_x, _y))) return true;
	else return false;
}
bool TDrawHint::Visible()
{
     return isShow;
}

void TDrawHint::Hide()
{
   isShow = false;
   Draw();
   timer->Enabled = false;
}

void TDrawHint::SetPosition(int _x, int _y)
{
	X = _x;
	Y = _y;
}

void TDrawHint::Show(UnicodeString _text)
{
   text = _text;
   isShow = true;
   Draw();
   timer->Enabled = true;
   timer->Interval = 2000;
}

/* TElementaryCognitiveFunctions */
TElementaryCognitiveFunctions::TElementaryCognitiveFunctions(AnsiString _name)
	: TBaseTask(_name)
{
	cur_block = 0;
	if(!Settings->Load(_name)){
		Settings->Add(PartsCount, "Частей (максимум 3)", GuiType::TEdit, "3");
		Settings->Add(RangeBackground, "Фон", GuiType::TEdit, "1000");
		Settings->Add(RangeBreak, "Перерыв", GuiType::TEdit, "2500");
		Settings->Save(_name);
	}
}
//------------------------------------------------------------------------------
void TElementaryCognitiveFunctions::InitTask(AnsiString Path)
{
       randomize();
	   std::srand(time(NULL));

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
			Timer->Interval = Settings->getInt(RangeBackground);  // 60000
			Timer->Enabled = true;

            if(PartCount == Settings->getInt(PartsCount)){ state = END; break; }
			else state = RUN_BLOCK;
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
			PartCount++;

			if(PartCount == Settings->getInt(PartsCount))
			{
			   state = BACKGROUND;
			   StateManager();
			   break;
            }


			ClearCanva();
			Timer->Interval = Settings->get(RangeBreak).ToInt();
			DrawText("Перерыв", 80);
			state = BACKGROUND;

			#ifdef PROTOCOL_LOGGER
                OwnProtocol->PAUSE_TIME = millis();
			#endif
			break;
		}
		case END:
		{
			ClearCanva();
			DrawText("Спасибо за участие!", 80);
			state = FINISHED;
			Timer->Interval = 2000;
			break;
		}
		default:
				break;
	}
}
bool TElementaryCognitiveFunctions::Finished()
{
	if(state == State::FINISHED){ return true; Timer->Enabled = false; }
	return false;
}
void TElementaryCognitiveFunctions::UserMouseDown(int X, int Y, TMouseButton Button)
{
   if(state == RUN_BLOCK) Blocks[BlockSequence[PartCount][cur_block]]->UserMouseDown(X,Y,Button);
}
void TElementaryCognitiveFunctions::Draw()
{
	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
void TElementaryCognitiveFunctions::CloseTask()
{
    Protocol->Save(GetTaskName());
}
