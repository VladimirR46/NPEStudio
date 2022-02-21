//---------------------------------------------------------------------------

#ifndef TCubeTaskH
#define TCubeTaskH

#include "TBaseTask.h"
//---------------------------------------------------------------------------

class TCubeTask : public TBaseTask
{
public:
	enum SettingsName : int
	{
		RangeBackground = 0,
		RangeBreak = 1,
        PartsCount = 2
    };
	enum State : int
	{
		 INSTRUCTION = -1,
		 NOISE,
		 POINT,
		 CUBE,
         Size
	} state;

	// Описание для протокола --------------------------------------------------
	/*
	struct DProtocol : ProtocolBase
	{
		unsigned int BACKGROUND_TIME = 0;
        unsigned int PAUSE_TIME = 0;
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
        protocol->BlockName = GetTaskName();
        OwnProtocol = protocol.get();
		return protocol;
	}
	*/
    //--------------------------------------------------------------------------
	TCubeTask(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;
private:

	//DProtocol* OwnProtocol;
};




#endif
