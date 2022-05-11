//---------------------------------------------------------------------------

#ifndef TVerbalWorkingMemoryTaskH
#define TVerbalWorkingMemoryTaskH
//---------------------------------------------------------------------------
#include "TBaseTask.h"
#include <map>
//---------------------------------------------------------------------------

struct Sample
{
   std::vector<std::string> symbol_list;
   std::string symbol;
   int targeted; // проба
   int complexity;
};


class TVerbalWorkingMemory : public TBaseTask
{
public:

	enum SettingsName : int
	{
	   MaxBlockCount,
	   MaxTrialPerCmplexity,
	   MaxSymbolCount,
	   Complexity,
	   BeginTime,
	   CrossTimeRange,
	   SymbolsTimeRange,
	   RestTimeRange,
	   SymbolTimeRange,
	   Rest2TimeRange,
       EndTime,
	   RangeCross
	};

	enum class State : int
	{
	   BEGIN,
	   CROSS,
	   CHARSET,
	   REST1,
       CHAR,
       REST2,
       VAS,
	   END,
	   THANKS,
	   FINISHED,
	} state;


    //--------------------------------------------------------------------------
	TVerbalWorkingMemory(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void Draw() override;
	void CloseTask() override;
    void VasFinished(TObject *Sender) override;
	//--------------------------------------------------------------------------
	void gen_sample();
    void samples_to_lsl(std::vector<Sample>& samples, samples_t &xml_sample);

private:
	std::vector<std::string> alphabet;

	int TrialCount = 0;
    int BlockCount = 0;
	bool isFinished = false;

	std::vector<Sample> sample_list;
    std::vector<int> block_index;
};








#endif
