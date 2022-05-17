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
#include "TVisualAnalogScale.h"
//------------------------------------------------------------------------------
namespace mat
{
mxArray *mCreateStringArray(std::vector<AnsiString> array);

class TStruct
{
 public:

	TStruct(std::vector<const char*> names){
		 pStruct = mxCreateStructMatrix(1, 1, names.size(), names.data());
	}

	void Field(int index, double _value) {
		mxSetFieldByNumber(pStruct, 0, index, mxCreateDoubleScalar(_value));
	}

	void Field(int index, AnsiString _value) {
		mxSetFieldByNumber(pStruct, 0, index, mxCreateString(AnsiToUtf8(_value).c_str()));
	}

	void Field(int index, std::vector<AnsiString> _value) {
		mxSetFieldByNumber(pStruct, 0, index, mCreateStringArray(_value));
	}

	void Field(int index, mxArray* _value) {
		mxSetFieldByNumber(pStruct, 0, index, _value);
	}

    operator mxArray*() const
	{
		return pStruct;
	}

	void PutToMFile(MATFile *pmat, AnsiString name) {
	   matPutVariable(pmat, name.c_str(), pStruct);
	   mxDestroyArray(pStruct);
	}

 private:
	mxArray *pStruct;
};

class TDoubleMatrix
{
  public:
	TDoubleMatrix(const int _m, const int _n): m(_m), n(_n){
	  pMatrix = mxCreateDoubleMatrix(_m, _n, mxREAL);
	  matrix_ptr = mxGetPr(pMatrix);
	}

	double& operator() (int i, int j) const
	{
		return matrix_ptr[j*m+i];
	}

	double& operator() (int i) const
	{
		return matrix_ptr[i];
	}

	operator mxArray*() const
	{
		return pMatrix;
	}

  private:
	mxArray *pMatrix = nullptr;
	double *matrix_ptr = nullptr;
	int m = 0, n = 0;
};

class TCellMatrix
{
	public:
		TCellMatrix(const int _m, const int _n): m(_m), n(_n){
			pMatrix = mxCreateCellMatrix(m, n);
		}

		TCellMatrix& operator=(mxArray* _value) {
		  mxSetCell(pMatrix,ItemIndex, _value);
		  return *this;
		}

		TCellMatrix& operator=(double _value) {
		  mxSetCell(pMatrix,ItemIndex, mxCreateDoubleScalar(_value));
		  return *this;
		}

        TCellMatrix& operator=(AnsiString _value) {
		  mxSetCell(pMatrix,ItemIndex, mxCreateString(AnsiToUtf8(_value).c_str()));
		  return *this;
		}

		operator mxArray*() const {
			return pMatrix;
		}

		TCellMatrix& operator() (const int i, const int j) {
			ItemIndex = m*j+i;
			return *this;
		}

	private:
		mxArray *pMatrix = nullptr;
		int ItemIndex = 0;
		int m = 0, n = 0;
};

}
//------------------------------------------------------------------------------

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

    std::vector<VAS> pMental, pPhysical, pBoredom, pEffort;
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
    void SaveLearningTask(MATFile *pmat);

	void SetInstractionTime(unsigned int time){
		Data.back()->InstructionTime = time;
        instruction_count++;
	}
	int InstructionSize() {
        return instruction_count;
    }
	//mxArray *mCreateStringArray(std::vector<AnsiString> array);

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
