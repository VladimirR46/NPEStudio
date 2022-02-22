//---------------------------------------------------------------------------
#pragma hdrstop
#include "TProtocol.h"
#include "TElementaryCognitiveFunctions.h"
#include "TCubeTask.h"
#include <filesystem>
//---------------------------------------------------------------------------
#pragma package(smart_init)

TProtocol::TProtocol(UnicodeString _TaskName) : TaskName(_TaskName)
{
	ProtocolFile = new TStringList();
}
//------------------------------------------------------------------------------
void TProtocol::AddData(int time, UnicodeString str)
{
	 ProtocolFile->Add(IntToStr(time) + " " + str);
}
//------------------------------------------------------------------------------
void TProtocol::NextBlock(std::shared_ptr<ProtocolBase> block)
{
	Data.push_back(block);
}
//------------------------------------------------------------------------------
void TProtocol::Save(AnsiString TaskName)
{
	std::wstring path = std::filesystem::current_path(); //getting path
    std::filesystem::current_path(FilePath.c_str()); //setting path
__try {
	/*FilePath*/
	MATFile *pmat = matOpen("Protocol.mat", "w");
	if (pmat == NULL) {
		ShowMessage("Ошибка: Невозможно сохранить протокол");
        return;
	}


	if(TaskName == "Элементарные когнитивные функции") {
       SaveECFTask(pmat);
    }

   	if(TaskName == "Кубы Неккера") {
	   SaveCubeTask(pmat);
    }

	if(matClose(pmat) != 0) {
		ShowMessage("Ошибка: Невозможно закрыть файл протокола");
		return;
	}
}
__finally{
	  std::filesystem::current_path(path);
}

}
//------------------------------------------------------------------------------
void TProtocol::SaveCubeTask(MATFile *pmat)
{
	// Subject
	const char *SubFieldNames[5] = {"id", "age", "gender", "hand", "edinburgh"};
	mxArray *sub = mxCreateStructMatrix(1, 1, 5, SubFieldNames);
	mxSetField(sub, 0, SubFieldNames[0], mxCreateDoubleScalar(0));
	mxSetField(sub, 0, SubFieldNames[1], mxCreateDoubleScalar(subject.Age.ToDouble()));
	mxSetField(sub, 0, SubFieldNames[2], mxCreateString(subject.Gender.c_str()));
	mxSetField(sub, 0, SubFieldNames[3], mxCreateString(subject.ActiveHand.c_str()));
	mxArray *edinburgh = mxCreateDoubleMatrix(10, 1, mxREAL);
	double *x = mxGetPr(edinburgh);
	std::fill(x, x+10, 22);
	mxSetField(sub, 0, SubFieldNames[4], edinburgh);
	matPutVariable(pmat, "subject", sub);
	mxDestroyArray(sub);
	//////////////////////////////////////

	const char *fieldnames[4] = {"TimeLineName", "TimeLine", "StimulsName", "Stimuls"};
	mxArray *block = mxCreateStructMatrix(1, 1, 4, fieldnames);

	typedef TCubeTask::DProtocol::Trial Trial;
	TCubeTask::DProtocol* proto_ptr = static_cast<TCubeTask::DProtocol*>(Data[0].get());

	mxArray* timelineName = mCreateStringArray({"Noise", "Cube"});
	mxSetField(block, 0, fieldnames[0], timelineName);

	mxArray* stimulsName = mCreateStringArray({"CubeType","Intensity"});
	mxSetField(block, 0, fieldnames[2], stimulsName);

	mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->Trials.size(), mxGetN(timelineName), mxREAL);
	double *timeline_ptr = mxGetPr(timeline);
	mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->Trials.size(), mxGetN(stimulsName));

	for(int j = 0; j < proto_ptr->Trials.size(); j++)
	{
		Trial* trial = static_cast<Trial*>(proto_ptr->Trials[j].get());

        int size = proto_ptr->Trials.size();
		for(int k = 0; k < 2; k++){
		  timeline_ptr[size*k+j] = trial->StateTime[k];
		}

        mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, mxCreateDoubleScalar(trial->cube_type));
		mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mxCreateDoubleScalar(trial->intensity));
	}

    mxSetField(block, 0, fieldnames[1], timeline);
	mxSetField(block, 0, fieldnames[3], stimuls_cell);

    matPutVariable(pmat, "protocol", block);
	mxDestroyArray(block);
}
//------------------------------------------------------------------------------
void TProtocol::SaveECFTask(MATFile *pmat)
{
	// Subject
	const char *fieldnames[5] = {"id", "age", "gender", "hand", "edinburgh"};
	mxArray *sub = mxCreateStructMatrix(1, 1, 5, fieldnames);
	mxSetField(sub, 0, fieldnames[0], mxCreateDoubleScalar(0));
	mxSetField(sub, 0, fieldnames[1], mxCreateDoubleScalar(subject.Age.ToDouble()));
	mxSetField(sub, 0, fieldnames[2], mxCreateString(subject.Gender.c_str()));
	mxSetField(sub, 0, fieldnames[3], mxCreateString(subject.ActiveHand.c_str()));
	mxArray *edinburgh = mxCreateDoubleMatrix(10, 1, mxREAL);
	double *x = mxGetPr(edinburgh);
	std::fill(x, x+10, 22);
	mxSetField(sub, 0, fieldnames[4], edinburgh);
	matPutVariable(pmat, "subject", sub);
	mxDestroyArray(sub);
	//////////////////////////////////////

	int colsize = Data.size()+InstructionSize();

	mxArray *protocol_cell = mxCreateCellMatrix(4, colsize);
	for(int i = 0, idx = 0; i < Data.size(); i++, idx++)
	{
	// Instructions
	if(Data[i]->InstructionTime){
		int sz = mxGetM(protocol_cell);
		mxSetCell(protocol_cell,sz*idx,   mxCreateString(u8"Инструкция"));
		mxSetCell(protocol_cell,sz*idx+1, mxCreateDoubleScalar(Data[i]->InstructionTime));
		mxSetCell(protocol_cell,sz*idx+2, mxCreateDoubleScalar(0));
		mxSetCell(protocol_cell,sz*idx+3, mxCreateDoubleScalar(0));
		idx++;
	}

	const char *fieldnames[4] = {"TimeLineName", "TimeLine", "StimulsName", "Stimuls"};
	mxArray *block = mxCreateStructMatrix(1, 1, 4, fieldnames);

	AnsiString BlockName = Data[i]->BlockName;
	double BlockStartTime = 0;

	////////////////Комбинированные функции//////////////////
	if(Data[i]->BlockName == "Комбинированные функции")
	{
		typedef TFunctionCombination::DProtocol::Trial fcTrial;
		TFunctionCombination::DProtocol* proto_ptr = static_cast<TFunctionCombination::DProtocol*>(Data[i].get());

		BlockStartTime = proto_ptr->PLUS_TIME;

		mxArray* timelineName = mCreateStringArray({"Cross", "Matrix", "UserClick"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mCreateStringArray({"MatrixShown","UserSelection"});
		mxSetField(block, 0, fieldnames[2], stimulsName);

		mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->Trials.size(), mxGetN(timelineName), mxREAL);
		double *timeline_ptr = mxGetPr(timeline);
		mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->Trials.size(), mxGetN(stimulsName));

		for(int j = 0; j < proto_ptr->Trials.size(); j++)
		{
			fcTrial* Trial = static_cast<fcTrial*>(proto_ptr->Trials[j].get());
			int size = proto_ptr->Trials.size();

			timeline_ptr[size*0+j] = proto_ptr->PLUS_TIME;
			timeline_ptr[size*1+j] = Trial->StateTime[TFunctionCombination::State::MATRIX];
			timeline_ptr[size*2+j] = Trial->StateTime[TFunctionCombination::State::CLICK];

			mxArray *mat = mxCreateDoubleMatrix(1, Trial->varray.size(), mxREAL);
			for(int k = 0; k < Trial->varray.size(); k++) mxGetPr(mat)[k] = Trial->varray[k];
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, mat);
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mxCreateDoubleScalar(Trial->Click.numbers));
		}

		mxSetField(block, 0, fieldnames[1], timeline);
		mxSetField(block, 0, fieldnames[3], stimuls_cell);
	}
	////////////////Элементарные когнитивные функции//////////////////
	else if(Data[i]->BlockName == "Элементарные когнитивные функции")
	{
		typedef TElementaryCognitiveFunctions::DProtocol Protocol;
		Protocol* protocol = static_cast<Protocol*>(Data[i].get());

		if(protocol->BACKGROUND_TIME != 0) {
		 BlockName = "Фон";
		 BlockStartTime = protocol->BACKGROUND_TIME;
		}
		else if(protocol->PAUSE_TIME != 0)
		{
		 BlockName = "Пауза";
		 BlockStartTime = protocol->PAUSE_TIME;
		}
		mxDestroyArray(block);
		block = mxCreateDoubleScalar(0);
	}
	////////////////"Ментальная арифметика"//////////////////
	else if(Data[i]->BlockName == "Ментальная арифметика")
	{
		typedef TMentalArithmeticBlock::DProtocol::Trial Trial;
		TMentalArithmeticBlock::DProtocol* proto_ptr = static_cast<TMentalArithmeticBlock::DProtocol*>(Data[i].get());

		mxArray* timelineName = mCreateStringArray({"Cross", "Equation", "UserClick", "BlackScreen"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mCreateStringArray({"X","N","R","Result"});
		mxSetField(block, 0, fieldnames[2], stimulsName);

		mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->Trials.size(), mxGetN(timelineName), mxREAL);
		double *timeline_ptr = mxGetPr(timeline);
		mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->Trials.size(), mxGetN(stimulsName));

		for(int j = 0; j < proto_ptr->Trials.size(); j++)
		{
			Trial* trial = static_cast<Trial*>(proto_ptr->Trials[j].get());
			if(j == 0 ) BlockStartTime = trial->StateTime[TMentalArithmeticBlock::State::PLUS];
			int size = proto_ptr->Trials.size();

			for(int k = 0; k < 4; k++){
			  timeline_ptr[size*k+j] = trial->StateTime[k];
			}

			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, mxCreateDoubleScalar(trial->X));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mxCreateDoubleScalar(trial->N));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*2+j, mxCreateDoubleScalar(trial->R));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*3+j, mxCreateDoubleScalar(trial->Click.numbers));

		}
		mxSetField(block, 0, fieldnames[1], timeline);
		mxSetField(block, 0, fieldnames[3], stimuls_cell);
	}
	///////////////Ошибка////////////////////////////////////////////
	else {
		mxDestroyArray(block);
		block = mxCreateString("Error");
	}
	//////////////////////////////////////////////////////////////
	mxSetCell(protocol_cell,mxGetM(protocol_cell)*idx,   mxCreateString(AnsiToUtf8(BlockName).c_str()));
	mxSetCell(protocol_cell,mxGetM(protocol_cell)*idx+1, mxCreateDoubleScalar(BlockStartTime));
	mxSetCell(protocol_cell,mxGetM(protocol_cell)*idx+2, block);
	mxSetCell(protocol_cell,mxGetM(protocol_cell)*idx+3, mxCreateDoubleScalar(0));
	}

	matPutVariable(pmat, "protocol", protocol_cell);
	mxDestroyArray(protocol_cell);
}
//------------------------------------------------------------------------------
void TProtocol::Init(UnicodeString path, SubjectInfo _sub)
{
	FilePath = path;
	subject = _sub;
	ProtocolFile->Clear();
	Data.clear();
    instruction_count = 0;
}
//------------------------------------------------------------------------------
mxArray *TProtocol::mCreateStringArray(std::vector<AnsiString> array)
{
	mxArray *cell = mxCreateCellMatrix(1,array.size());
	for(int i = 0; i < array.size(); i++) {
	   mxSetCell(cell,i, mxCreateString(AnsiToUtf8(array[i]).c_str()));
	}
	return cell;
}
//------------------------------------------------------------------------------
TProtocol::~TProtocol()
{
	delete ProtocolFile;
}
