//---------------------------------------------------------------------------

#ifndef TFingerTestH
#define TFingerTestH
//---------------------------------------------------------------------------
#include "TBaseTask.h"

//---------------------------------------------------------------------------

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
       CONCLUSION
	} state;


	TFingerTest(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void Draw() override;
	void CloseTask() override;

    bool LoadImages();


    ~TFingerTest();

private:
	int TrialCount = 0;
	bool isFinished = false;

    TBitmap *ImageNoise;
	std::vector<bitmap_ptr> images;

};


#endif
