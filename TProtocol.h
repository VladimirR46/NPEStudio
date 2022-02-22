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

struct ProtocolBase
{
	AnsiString BlockName = "";
	unsigned int InstructionTime = 0;

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
	void SetInstractionTime(unsigned int time){
		Data.back()->InstructionTime = time;
        instruction_count++;
	}
	int InstructionSize() {
        return instruction_count;
    }
	mxArray *mCreateStringArray(std::vector<AnsiString> array);

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
