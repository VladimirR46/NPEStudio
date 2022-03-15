//---------------------------------------------------------------------------

#pragma hdrstop

#include "TCubeTask.h"
#include <algorithm>
#include <random>
//---------------------------------------------------------------------------
#pragma package(smart_init)

const int lines[12][2] = {
	 {0, 1}, {1, 2}, {2, 3}, {3, 0},
	 {4, 5}, {5, 6}, {6, 7}, {7, 4},
	 {0, 4}, {1, 5}, {2, 6}, {3, 7}};

TCubeF cube1 = {TPointF(-103,-405), TPointF(405,-405),TPointF(405,103),TPointF(-103,103),
				TPointF(-405,-103),TPointF(103,-103),TPointF(103,405),TPointF(-405,405)};

TCubeF cube2 = {TPointF(-103,405), TPointF(405,405),TPointF(405,-103),TPointF(-103,-103),
				TPointF(-405,103),TPointF(103,103),TPointF(103,-405),TPointF(-405,-405)};


void TCubeTask::DrawCube(TCubeF cube, float target_intensity)
{
    TPointF center(bitmap->Width/2.0, bitmap->Height/2.0);

	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Thickness = 16;
	bitmap->Canvas->Stroke->Color = TAlphaColorRec::Black;
	bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;

	float Intensity = 1.0;
	for(int i = 0; i < 12; i++)
	{
	   if(i == 2 || i == 3 || i == 11) Intensity = target_intensity;
	   if(i == 4 || i == 5 || i == 9) Intensity = 1-target_intensity;

	   bitmap->Canvas->DrawLine(cube.point[lines[i][0]]+center, cube.point[lines[i][1]]+center, Intensity);
       Intensity = 1.0;
	}
	bitmap->Canvas->EndScene();


	// Fix Overlay
	TAlphaColor col1, col2;
	TBitmapData data;
	bitmap->Map(TMapAccess::ReadWrite, data);
	col1 = data.GetPixel((cube.point[3]+center).x+20,(cube.point[3]+center).y);
	col2 = data.GetPixel((cube.point[5]+center).x-20,(cube.point[5]+center).y);
	bitmap->Unmap(data);

	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Fill->Color = col1;
	bitmap->Canvas->FillRect(TRectF(cube.point[3]+center-TPointF(8,8), cube.point[3]+center+TPointF(8,8)), 1);
	bitmap->Canvas->Fill->Color = col2;
	bitmap->Canvas->FillRect(TRectF(cube.point[5]+center-TPointF(8,8), cube.point[5]+center+TPointF(8,8)), 1);
	bitmap->Canvas->EndScene();
}

TCubeTask::TCubeTask(AnsiString _name)
	: TBaseTask(_name)
{

	if(!Settings->Load(_name)){

		Settings->Add(TrialMax, "Количество триалов", GuiType::TEdit, 2);
		Settings->Add(RangeInstruction, "Приветствие", GuiType::TRange, "3000:3000");
		Settings->Add(RangeNoise, "Шум", GuiType::TRange, "3000:5000");
		Settings->Add(RangeCoube, "Куб", GuiType::TRange, "1000:1500");
		Settings->Save(_name);
	}

}
//------------------------------------------------------------------------------
void TCubeTask::InitTask(AnsiString Path)
{
    TrialCount = 0;
	state = INSTRUCTION;
    isFinished = false;
    complexity.clear();

	std::vector<float> _complexity = {0.4, 0.6, 0.15, 0.25, 0.45, 0.55, 0.75, 0.85};

	for(int i = 0; i < _complexity.size()*2*Settings->getInt(TrialMax); i++)
	{
	   TComplexity complex;
	   complex.intensity = _complexity[i%_complexity.size()];
	   complex.cube_type = int(i/_complexity.size())%2;
	   complex.cube = (complex.cube_type == 0)?&cube1:&cube2;
	   complexity.push_back(complex);
    }

    srand(time(NULL));
	std::random_shuffle(begin(complexity), end(complexity));

	Protocol->NextBlock(CreateProtocol());
}
//------------------------------------------------------------------------------
void TCubeTask::StateManager()
{
	switch(state) {
		case INSTRUCTION:
		{
			ClearCanva(TAlphaColorRec::White);
			DrawText("Приготовьтесь.", 80, TAlphaColorRec::Black);
			Timer->Interval = Settings->getRandFromRange(RangeInstruction);
			Timer->Enabled = true;
			state = NOISE;
			Protocol->SetInstractionTime(millis());
			break;
		}
		case NOISE:
		{
			DrawNoise();
			DrawPoint(bitmap->Width-60, bitmap->Height-60, 20, TAlphaColorRec::Black);
			Timer->Interval = Settings->getRandFromRange(RangeNoise);
			state = CUBE;
			Trial = OwnProtocol->CreateTrial(); // Protocol
			Trial->StateTime[NOISE] = millis();
			break;
		}
		case CUBE:
		{
            ClearCanva(TAlphaColorRec::White);
			DrawPoint(5);
			DrawCube(*complexity[TrialCount].cube, complexity[TrialCount].intensity);
			Timer->Interval = Settings->getRandFromRange(RangeCoube);
			state = NOISE;
            Trial->StateTime[CUBE] = millis();
			Trial->cube_type = complexity[TrialCount].cube_type;
			Trial->intensity = complexity[TrialCount].intensity;
			TrialCount++;

			//bitmap->SaveToFile("cube"+IntToStr(TrialCount)+".bmp");

			break;
		}
		case CONCLUSION:
		{
			ClearCanva(TAlphaColorRec::White);
			DrawText("Спасибо за участие!", 80, TAlphaColorRec::Black);
			Timer->Interval = 2000;
            isFinished = true;
			break;
        }
		default:
			break;
	}
}
bool TCubeTask::Finished()
{
	if(TrialCount == complexity.size()) {
        state = CONCLUSION;
	}

	return isFinished;
}
void TCubeTask::UserMouseDown(int X, int Y, TMouseButton Button)
{

}
void TCubeTask::Draw()
{
   	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
void TCubeTask::CloseTask()
{
	Timer->Interval = 0;
    Protocol->Save(GetTaskName());
}