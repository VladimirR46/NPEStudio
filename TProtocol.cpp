//---------------------------------------------------------------------------
#pragma hdrstop
#include "TProtocol.h"
#include "TElementaryCognitiveFunctions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TProtocol::TProtocol(UnicodeString _TaskName) : TaskName(_TaskName)
{
	ProtocolFile = new TStringList();
	DescriptionFile = new TStringList();
}
//------------------------------------------------------------------------------
void TProtocol::AddDescription(UnicodeString str)
{
	 DescriptionFile->Add(str);
}
//------------------------------------------------------------------------------
void TProtocol::AddData(int time, UnicodeString str)
{
	 ProtocolFile->Add(IntToStr(time) + " " + str);
}
//------------------------------------------------------------------------------
void TProtocol::AddData(int time, int state)
{
	 ProtocolFile->Add(IntToStr(time) + " " + IntToStr(state));
}
//------------------------------------------------------------------------------
void TProtocol::NextBlock(std::shared_ptr<ProtocolBase> block)
{
	Data.push_back(block);
}
//------------------------------------------------------------------------------
void TProtocol::Save()
{
	 DescriptionFile->SaveToFile(FilePath+"\\Description.txt", TEncoding::Unicode);
	 ProtocolFile->SaveToFile(FilePath+"\\Protocol.txt", TEncoding::Unicode);

	 MATFile *pmat = matOpen("Protocol.mat", "w");

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
			mxSetCell(protocol_cell,sz*idx,   mxCreateString(u8"����������"));
			mxSetCell(protocol_cell,sz*idx+1, mxCreateDoubleScalar(Data[i]->InstructionTime));
			mxSetCell(protocol_cell,sz*idx+2, mxCreateDoubleScalar(0));
			mxSetCell(protocol_cell,sz*idx+3, mxCreateDoubleScalar(0));
			idx++;
		}

		const char *fieldnames[4] = {"TimeLineName", "TimeLine", "StimulsName", "Stimuls"};
		mxArray *block = mxCreateStructMatrix(1, 1, 4, fieldnames);

		AnsiString BlockName = Data[i]->BlockName;
		double BlockStartTime = 0;

		////////////////��������������� �������//////////////////
		if(Data[i]->BlockName == "��������������� �������")
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
		////////////////������������ ����������� �������//////////////////
		else if(Data[i]->BlockName == "������������ ����������� �������")
		{
			typedef TElementaryCognitiveFunctions::DProtocol Protocol;
			Protocol* protocol = static_cast<Protocol*>(Data[i].get());

			if(protocol->BACKGROUND_TIME != 0) {
			 BlockName = "���";
			 BlockStartTime = protocol->BACKGROUND_TIME;
			}
			else if(protocol->PAUSE_TIME != 0)
			{
			 BlockName = "�����";
			 BlockStartTime = protocol->PAUSE_TIME;
			}
            mxDestroyArray(block);
			block = mxCreateDoubleScalar(0);
		}
		////////////////"���������� ����������"//////////////////
		else if(Data[i]->BlockName == "���������� ����������")
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
		///////////////������////////////////////////////////////////////
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
	 matClose(pmat);
}
//------------------------------------------------------------------------------
void TProtocol::Init(UnicodeString path, SubjectInfo _sub)
{
	FilePath = path;
	subject = _sub;
	DescriptionFile->Clear();
	ProtocolFile->Clear();
	AddDescription("����������������� ������: "+ TaskName+"\n");
	AddDescription("����������: "+_sub.LastName+" "+_sub.Name+" "+_sub.Patronymic+", "+_sub.Age+
					" ���, "+"�������� ����: "+_sub.ActiveHand+", ���: "+_sub.Gender);
	AddDescription("\n____________________________________________________________________________________\n");
	Data.clear();
    instruction_count = 0;
	//NameBlocks.clear();
	//Data.assign(24,std::vector<std::shared_ptr<TrialBase>>());
}
//------------------------------------------------------------------------------
TProtocol::~TProtocol()
{
	delete ProtocolFile;
    delete DescriptionFile;
}
