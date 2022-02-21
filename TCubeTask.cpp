//---------------------------------------------------------------------------

#pragma hdrstop

#include "TCubeTask.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


TCubeTask::TCubeTask(AnsiString _name)
	: TBaseTask(_name)
{

    /*
	if(!Settings->Load(_name)){
		Settings->Add(PartsCount, "Частей", GuiType::TEdit, "3");
		Settings->Add(RangeBackground, "Фон", GuiType::TEdit, "1000");
		Settings->Add(RangeBreak, "Перерыв", GuiType::TEdit, "2500");
		Settings->Save(_name);
	}
	*/
}
//------------------------------------------------------------------------------
void TCubeTask::InitTask(AnsiString Path)
{
	   state = INSTRUCTION;
}
void TCubeTask::StateManager()
{
	switch(state) {
		case INSTRUCTION:
		{
			ClearCanva();
			DrawText("Приготовьтесь.", 80);
			Timer->Interval = 3000;
			Timer->Enabled = true;
			state = NOISE;
			break;
		}
		case NOISE:
		{
			ClearCanva();
			DrawText("Запись фоновой активности.", 80);
			Timer->Interval = 1000;
			state = POINT;
			break;
		}
		case POINT:
		{
			Timer->Interval = 1000;
			state = CUBE;
			break;
		}
        case CUBE:
		{
            Timer->Interval = 1000;
			state = NOISE;
			break;
		}
		default:
			break;
	}
}
bool TCubeTask::Finished()
{
	return false;
}
void TCubeTask::UserMouseDown(int X, int Y)
{

}
void TCubeTask::Draw()
{
   	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
void TCubeTask::CloseTask()
{

}