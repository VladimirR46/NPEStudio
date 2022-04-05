//---------------------------------------------------------------------------

#ifndef TFingerTestH
#define TFingerTestH
//---------------------------------------------------------------------------
#include "TBaseTask.h"

//---------------------------------------------------------------------------

struct TrialImage
{
	TrialImage(bitmap_ptr _image, UnicodeString _name) : Image(_image), ImageName(_name)
	{ }

	bitmap_ptr Image;
	UnicodeString ImageName;
};


class TFingerTest : public TBaseTask
{
public:
	enum SettingsName : int
	{
	   PathToImage,
       TrialMax,
	   RangeNoise,
	   RangeImage
    };

	enum State : int
	{
       INSTRUCTION = -1,
	   NOISE,
	   IMAGE,
       Size,
       CONCLUSION
	} state;

	struct ProtocolBlock : ProtocolBase
	{
		struct Trial : TrialBase
		{
			unsigned int StateTime[State::Size] = {0};
			int Key = 0;
			unsigned int KeyTime = 0;
            UnicodeString ImageName = "";
		};


    };


	TFingerTest(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void Draw() override;
	void CloseTask() override;
	void KeyDown(int Key) override;

    bool LoadImages();


    ~TFingerTest();

private:
	int TrialCount = 0;
	bool isFinished = false;

    TBitmap *ImageNoise;
	std::vector<TrialImage> images;

	ProtocolBlock* pBlock = nullptr;
	ProtocolBlock::Trial* pTrial = nullptr;
};


#endif
