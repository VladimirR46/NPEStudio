//---------------------------------------------------------------------------

#pragma hdrstop

#include "TFingerTest.h"
#include <System.Types.hpp>
#include <System.IOUtils.hpp>

#include <algorithm>
#include <random>
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
		Settings->Add(BACKGROUND1, "Фон начало", GuiType::TEdit, "2000");
		Settings->Add(BACKGROUND2, "Фон конец", GuiType::TEdit, "2000");

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

	images.clear();
	srand(time(NULL));
	for(int i = 0; i < Settings->getInt(TrialMax); i++)
	{
		std::vector<TrialImage> images_in_block;
		for(int j = 0; j < list.size(); j++)
		{
			TrialImage image(std::make_shared<TBitmap>(), ExtractFileName(list[j]));
			image.Image->LoadFromFile(list[j]);
			images_in_block.push_back(image);
		}
		std::random_shuffle(begin(images_in_block), end(images_in_block));
		images.insert(images.end(), images_in_block.begin(), images_in_block.end());
	}


    return true;
}
//----------------------------------------------------------------------------
void TFingerTest::InitTask(AnsiString Path)
{
	ImageNoise->SetSize(int(bitmap->Width),int(bitmap->Height));
	DrawNoise(ImageNoise);

	TrialCount = 0;
	state = BEGIN;
	isFinished = false;

	LoadImages();

    // Protocol
	pBlock = Protocol->AddBlock<ProtocolBlock>();
}
//----------------------------------------------------------------------------
void TFingerTest::StateManager()
{
	switch(state) {
		case BEGIN:
		{
			ClearCanva(TAlphaColorRec::White);
			DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100, TAlphaColorRec::Black);
			Timer->Interval = Settings->getInt(BACKGROUND1);
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
			// Protocol
			pTrial = pBlock->AddTrial<ProtocolBlock::Trial>();
			pTrial->StateTime[NOISE] = millis();
			break;
		}
		case IMAGE:
		{
            ClearCanva(TAlphaColorRec::White);
			DrawBitmap(images[TrialCount].Image.get());
			Timer->Interval = Settings->getRandFromRange(RangeImage);
            state = NOISE;

			// Protocol
			pTrial->StateTime[IMAGE] = millis();
			pTrial->ImageName = images[TrialCount].ImageName;
            //
			TrialCount++;
			break;
		}
		case END:
		{
			ClearCanva(TAlphaColorRec::White);
			DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100, TAlphaColorRec::Black);
			Timer->Interval = Settings->getInt(BACKGROUND2);
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
	if(TrialCount == images.size()) {
        state = END;
	}

	return isFinished;
}
//-----------------------------------------------------------------------------
void TFingerTest::UserMouseDown(int X, int Y, TMouseButton Button)
{

}
//-----------------------------------------------------------------------------
void TFingerTest::KeyDown(int Key)
{
	if(pTrial)
	{
	   pTrial->Key = Key;
	   pTrial->KeyTime = millis();
	}
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
    Protocol->Save(GetTaskName());
	pBlock = nullptr;
	pTrial = nullptr;
}
//----------------------------------------------------------------------------
TFingerTest::~TFingerTest()
{
   delete ImageNoise;
}
