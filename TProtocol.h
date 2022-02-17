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
   int id;
   ClickInfo click;
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

	void CurrentBlock(AnsiString name)
	{
		 NameBlocks[block_index] = name.c_str();
    }

	void NextBlock(AnsiString name)
	{
		block_index++;
		NameBlocks[block_index] = name.c_str();
	}

	void ResetBlockCounter()
	{
        block_index = 0;
	}

	void Init(UnicodeString path, SubjectInfo _sub);

	void SaveTrial(std::shared_ptr<TrialBase> _trial)
	{
	   Data[block_index].push_back(_trial);
    }

	void Save();



    ~TProtocol();
private:
    UnicodeString TaskName;
	TStringList* ProtocolFile;
	TStringList* DescriptionFile;
	UnicodeString FilePath = "";
    SubjectInfo subject;

	std::vector<std::vector<std::shared_ptr<TrialBase>>> Data;
	std::map<int, AnsiString> NameBlocks;
	int block_index = 0;
};

#endif
