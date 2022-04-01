//---------------------------------------------------------------------------

#pragma hdrstop

#include "TFingerTest.h"
#include <System.Types.hpp>
#include <System.IOUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)

TFingerTest::TFingerTest(AnsiString _name)
	: TBaseTask(_name)
{
	if(!Settings->Load(_name)){
		Settings->Add(PathToImage, "Путь к картинкам", GuiType::TDirectoryPath, "");
        Settings->Add(TrialMax, "Количество триалов", GuiType::TEdit, 1);
		Settings->Add(RangeNoise, "Шум", GuiType::TRange, "3000:5000");
		Settings->Add(RangeImage, "Картинка", GuiType::TRange, "1000:1500");
		Settings->Save(_name);
	}

	ImageNoise = new TBitmap();
}
//----------------------------------------------------------------------------
bool TFingerTest::LoadImages()
{
	if(Settings->get(PathToImage) == "") {
		ShowMessage("Ошибка: укажите путь к папке с картинками в настройках.");
		return false;
	}

	TStringDynArray list = TDirectory::GetFiles(Settings->get(PathToImage), "*.png", TSearchOption::soAllDirectories);

	if(list.size() == 0) {
    	ShowMessage("Ошибка: картинки в папке не найдены.");
		return false;
    }

	for(int i = 0; i < list.size(); i++)
	{
		bitmap_ptr Image(new TBitmap());
		Image->LoadFromFile(list[i]);
        images.push_back(Image);
	}




    return true;
}
//----------------------------------------------------------------------------
void TFingerTest::InitTask(AnsiString Path)
{
	ImageNoise->SetSize(int(bitmap->Width),int(bitmap->Height));
	DrawNoise(ImageNoise);

	TrialCount = 0;
	state = INSTRUCTION;
	isFinished = false;

    LoadImages();
}
//----------------------------------------------------------------------------
void TFingerTest::StateManager()
{
	switch(state) {
		case INSTRUCTION:
		{
			ClearCanva(TAlphaColorRec::White);
			DrawText("Приготовьтесь.", 80, TAlphaColorRec::Black);
			Timer->Interval = 2000;
			Timer->Enabled = true;
			state = NOISE;
			break;
		}
		case NOISE:
		{
            DrawBitmap(ImageNoise);
			DrawPoint(bitmap->Width-60, bitmap->Height-60, 20, TAlphaColorRec::Black);
			Timer->Interval = Settings->getRandFromRange(RangeNoise);
			state = IMAGE;
			break;
		}
		case IMAGE:
		{
            ClearCanva(TAlphaColorRec::White);
			DrawBitmap(images[TrialCount].get());
			Timer->Interval = Settings->getRandFromRange(RangeImage);
            state = NOISE;
			TrialCount++;
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
//-----------------------------------------------------------------------------
bool TFingerTest::Finished()
{
	if(TrialCount == images.size()*Settings->getInt(TrialMax)) {
        state = CONCLUSION;
	}

	return isFinished;
}
//-----------------------------------------------------------------------------
void TFingerTest::UserMouseDown(int X, int Y, TMouseButton Button)
{

}
//-----------------------------------------------------------------------------
void TFingerTest::Draw()
{
   	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}
//-----------------------------------------------------------------------------
void TFingerTest::CloseTask()
{
	Timer->Interval = 0;

}


//----------------------------------------------------------------------------
TFingerTest::~TFingerTest()
{
   delete ImageNoise;
}
