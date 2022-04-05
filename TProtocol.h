//---------------------------------------------------------------------------
#ifndef TProtocolH
#define TProtocolH
//---------------------------------------------------------------------------
#include <fmx.h>
#include <vector>
#include <map>
#include "mat.h"
#pragma comment (lib,"borland/libmat.lib")
#pragma comment (lib,"borland/libmx.lib")

struct SubjectInfo
{
   AnsiString LastName;
   AnsiString Name;
   AnsiString Patronymic;
   AnsiString Gender;
   AnsiString ActiveHand;
   AnsiString Age;
};
struct ClickInfo
{
	int X;
	int Y;
	unsigned int time;
	int numbers;
};

struct TrialBase
{
    int count;
};

struct ProtocolBase
{
	AnsiString BlockName = "";
	unsigned int InstructionTime = 0;

	std::vector<std::shared_ptr<TrialBase>> trial_list;

	template <typename T>
	T* AddTrial() {
		std::shared_ptr<T> trial = std::make_shared<T>();
		trial_list.push_back(trial);
		return trial.get();
	}
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class TProtocol
{
public:
	TProtocol(UnicodeString TaskName);

	void AddData(int time, UnicodeString str);
	void NextBlock(std::shared_ptr<ProtocolBase> block);
	void Init(UnicodeString path, SubjectInfo _sub);
	void Save(AnsiString TaskName);
	void SaveCubeTask(MATFile *pmat);
	void SaveECFTask(MATFile *pmat);
	void SaveFingerTask(MATFile *pmat);

	void SetInstractionTime(unsigned int time){
		Data.back()->InstructionTime = time;
        instruction_count++;
	}
	int InstructionSize() {
        return instruction_count;
    }
	mxArray *mCreateStringArray(std::vector<AnsiString> array);

	template <typename T>
	T* AddBlock() {
		std::shared_ptr<T> block = std::make_shared<T>();
		Data.push_back(block);
		return block.get();
	}



    ~TProtocol();
private:
    UnicodeString TaskName;
	TStringList* ProtocolFile;
	UnicodeString FilePath = "";
    SubjectInfo subject;

	std::vector<std::shared_ptr<ProtocolBase>> Data;
    int instruction_count = 0;
};
#endif
