//---------------------------------------------------------------------------

#ifndef TCubeTaskH
#define TCubeTaskH

#include "TBaseTask.h"
//---------------------------------------------------------------------------

struct TCubeF {
	TPointF point[8];
};

struct TComplexity
{
	float intensity = 0.0;
	int cube_type = -1;
    TCubeF* cube;
};

class TCubeTask : public TBaseTask
{
public:
	enum SettingsName : int
	{
		TrialMax = 0,
		RangeInstruction,
		RangeNoise,
        RangeCoube
    };
	enum State : int
	{
		 INSTRUCTION = -1,
		 NOISE,
		 CUBE,
		 Size,
         CONCLUSION
	} state;

	// Описание для протокола --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			int cube_type = -1;
            float intensity = 0;
		};
		std::vector<std::shared_ptr<Trial>> Trials;

		Trial* CreateTrial() {
			std::shared_ptr<Trial> trial = std::make_shared<Trial>();
            Trials.push_back(trial);
			return trial.get();
        }
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
        protocol->BlockName = GetTaskName();
        OwnProtocol = protocol.get();
		return protocol;
	}
    //--------------------------------------------------------------------------
	TCubeTask(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;

	void DrawCube(TCubeF cube, float target_intensity);
private:
	int TrialCount = 0;
    bool isFinished = false;

	std::vector<TComplexity> complexity;
	DProtocol* OwnProtocol;
    DProtocol::Trial* Trial;
};




#endif
