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

	void AddDescription(UnicodeString str);
	void AddData(int time, UnicodeString str);
	void AddData(int time, int state);
	void NextBlock(std::shared_ptr<ProtocolBase> block);
	void Init(UnicodeString path, SubjectInfo _sub);
    void Save();
	void SetInstractionTime(unsigned int time){
		Data.back()->InstructionTime = time;
        instruction_count++;
	}
	int InstructionSize() {
        return instruction_count;
    }

    mxArray *mCreateStringArray(std::vector<AnsiString> array)
	{
		mxArray *cell = mxCreateCellMatrix(1,array.size());
		for(int i = 0; i < array.size(); i++) {
		   mxSetCell(cell,i, mxCreateString(AnsiToUtf8(array[i]).c_str()));
		}
		return cell;
	}
    ~TProtocol();
private:
    UnicodeString TaskName;
	TStringList* ProtocolFile;
	TStringList* DescriptionFile;
	UnicodeString FilePath = "";
    SubjectInfo subject;

	std::vector<std::shared_ptr<ProtocolBase>> Data;
    int instruction_count = 0;
};
#endif
