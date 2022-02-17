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

void TProtocol::AddDescription(UnicodeString str)
{
	 DescriptionFile->Add(str);
}

void TProtocol::AddData(int time, UnicodeString str)
{
	 ProtocolFile->Add(IntToStr(time) + " " + str);
}

void TProtocol::AddData(int time, int state)
{
	 ProtocolFile->Add(IntToStr(time) + " " + IntToStr(state));
}

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

	 // edinburgh
	 mxArray *edinburgh = mxCreateDoubleMatrix(10, 1, mxREAL);
	 double *x = mxGetPr(edinburgh);
	 std::fill(x, x+10, 22);
     mxSetField(sub, 0, fieldnames[4], edinburgh);

	 matPutVariable(pmat, "subject", sub);
	 mxDestroyArray(sub);
     //////////////////////////////////////

	 mxArray *protocol_cell = mxCreateCellMatrix(3, Data.size());
	 for(int i = 0; i < Data.size(); i++)
	 {
		mxArray *block;
		AnsiString BlockName = NameBlocks[i];
        double BlockStartTime = 0;

		////////////////Комбинированные функции//////////////////
		if(NameBlocks[i] == "Комбинированные функции")
		{
			const char *fieldnames[8] = {"TimeLineName", "TimeLine", "Stimuls", "StimulsName", "GuideTime", "CrossTime", "Goal", "Complexity"};
			block = mxCreateStructMatrix(1, 1, 8, fieldnames);

			const char *trialsname[2] = {"Matrix", "Response"};
			mxArray *trialsname_cell = mxCreateCellMatrix(1,2);
			mxSetCell(trialsname_cell,0, mxCreateString(trialsname[0]));
            mxSetCell(trialsname_cell,1, mxCreateString(trialsname[1]));
			mxSetField(block, 0, fieldnames[0], trialsname_cell);

			const char *stimulsname[2] = {"Matrix","Result"};
			mxArray *stimulsname_cell = mxCreateCellMatrix(1,2);
			mxSetCell(stimulsname_cell,0, mxCreateString(stimulsname[0]));
            mxSetCell(stimulsname_cell,1, mxCreateString(stimulsname[1]));
			mxSetField(block, 0, fieldnames[3], stimulsname_cell);

			int size = Data[i].size()-1;
			mxArray *Trials = mxCreateDoubleMatrix(size, 2, mxREAL);
			double *trials_ptr = mxGetPr(Trials);

			mxArray *stimul_cell = mxCreateCellMatrix(size, 2);
			for(int j = 0; j < Data[i].size(); j++)
			{
				TFunctionCombination::TrialProtocol* Trial = static_cast<TFunctionCombination::TrialProtocol*>(Data[i][j].get());

				if(j == 0) {
					 BlockStartTime = Trial->INFO_TIME;
					 mxSetField(block, 0, fieldnames[4], mxCreateDoubleScalar(Trial->INFO_TIME));
					 mxSetField(block, 0, fieldnames[5], mxCreateDoubleScalar(Trial->PLUS_TIME));
					 mxSetField(block, 0, fieldnames[6], mxCreateDoubleScalar(Trial->number));
                     mxSetField(block, 0, fieldnames[7], mxCreateDoubleScalar(Trial->complexity));
				} else{
					 trials_ptr[size*0+j-1] = Trial->MATRIX_TIME;
					 trials_ptr[size*1+j-1] = Trial->MATRIX_CLICK_TIME;

					 mxArray *mat = mxCreateDoubleMatrix(1, Trial->varray.size(), mxREAL);
					 for(int k = 0; k < Trial->varray.size(); k++) mxGetPr(mat)[k] = Trial->varray[k];

					 mxSetCell(stimul_cell,mxGetM(stimul_cell)*0+j-1, mat);
					 mxSetCell(stimul_cell,mxGetM(stimul_cell)*1+j-1, mxCreateDoubleScalar(Trial->click.numbers));
				}
			}
			mxSetField(block, 0, fieldnames[1], Trials);
			mxSetField(block, 0, fieldnames[2], stimul_cell);
		}
		////////////////Элементарные когнитивные функции//////////////////
		else if(NameBlocks[i] == "Элементарные когнитивные функции")
		{
			TElementaryCognitiveFunctions::TrialProtocol* Trial = static_cast<TElementaryCognitiveFunctions::TrialProtocol*>(Data[i][0].get());
			if(Trial->BACKGROUND_TIME != 0) {
			 BlockName = "Фон";
			 BlockStartTime = Trial->BACKGROUND_TIME;
			}
			else if(Trial->PAUSE_TIME != 0)
			{
			 BlockName = "Пауза";
			 BlockStartTime = Trial->PAUSE_TIME;
			}
			block = mxCreateDoubleScalar(0);
		}
		////////////////"Ментальная арифметика"//////////////////
		else if(NameBlocks[i] == "Ментальная арифметика")
		{
			const char *fieldnames[4] = {"TimeLineName", "TimeLine", "Stimuls", "StimulsName"};
			block = mxCreateStructMatrix(1, 1, 4, fieldnames);

			const char *timelinename[4] = {"Cross", "Equation", "Response", "Blank"};
			mxArray *timelinename_cell = mxCreateCellMatrix(1,4);
			for(int j = 0; j < 4; j++) mxSetCell(timelinename_cell,j, mxCreateString(timelinename[j]));
			mxSetField(block, 0, fieldnames[0], timelinename_cell);

            const int stimul_count = 4;
			const char *stimulsname[stimul_count] = {"X","N","R","Result"};
			mxArray *stimulsname_cell = mxCreateCellMatrix(1,stimul_count);
			for(int j = 0; j < stimul_count; j++) mxSetCell(stimulsname_cell,j, mxCreateString(stimulsname[j]));
			mxSetField(block, 0, fieldnames[3], stimulsname_cell);

			mxArray *timeline = mxCreateDoubleMatrix(Data[i].size(), 4, mxREAL);
			double *timeline_ptr = mxGetPr(timeline);
			mxArray *stimul_cell = mxCreateCellMatrix(Data[i].size(), stimul_count);

			size_t size =  Data[i].size();
			for(int j = 0; j < Data[i].size(); j++)
			{
			   TMentalArithmeticBlock::TrialProtocol* Trial = static_cast<TMentalArithmeticBlock::TrialProtocol*>(Data[i][j].get());
			   if(j == 0) BlockStartTime = Trial->PLUS_TIME;

			   timeline_ptr[size*0+j] = Trial->PLUS_TIME;
			   timeline_ptr[size*1+j] = Trial->EQUATION_TIME;
			   timeline_ptr[size*2+j] = Trial->click.time;
			   timeline_ptr[size*3+j] = Trial->BLANK_TIME;

			   mxSetCell(stimul_cell,mxGetM(stimul_cell)*0+j, mxCreateDoubleScalar(Trial->X));
			   mxSetCell(stimul_cell,mxGetM(stimul_cell)*1+j, mxCreateDoubleScalar(Trial->N));
			   mxSetCell(stimul_cell,mxGetM(stimul_cell)*2+j, mxCreateDoubleScalar(Trial->R));
               mxSetCell(stimul_cell,mxGetM(stimul_cell)*3+j, mxCreateDoubleScalar(Trial->click.numbers));
			}

			mxSetField(block, 0, fieldnames[1], timeline);
			mxSetField(block, 0, fieldnames[2], stimul_cell);
		}
		///////////////Ошибка////////////////////////////////////////////
		else {
            block = mxCreateString("Error");
        }
		//////////////////////////////////////////////////////////////
		mxSetCell(protocol_cell,3*i, mxCreateString(AnsiToUtf8(BlockName).c_str()));
		mxSetCell(protocol_cell,3*i+1, mxCreateDoubleScalar(BlockStartTime));
		mxSetCell(protocol_cell,3*i+2, block);
	 }

	 matPutVariable(pmat, "protocol", protocol_cell);
	 mxDestroyArray(protocol_cell);
     matClose(pmat);
}

void TProtocol::Init(UnicodeString path, SubjectInfo _sub)
{
	FilePath = path;
	subject = _sub;
	DescriptionFile->Clear();
	ProtocolFile->Clear();

	AddDescription("Экспериментальная сессия: "+ TaskName+"\n");
	AddDescription("Испытуемый: "+_sub.LastName+" "+_sub.Name+" "+_sub.Patronymic+", "+_sub.Age+
					" лет, "+"Активная рука: "+_sub.ActiveHand+", Пол: "+_sub.Gender);
	AddDescription("\n____________________________________________________________________________________\n");

	Data.clear();
	NameBlocks.clear();
	//Data.assign(24,std::vector<std::shared_ptr<TrialBase>>());
}

TProtocol::~TProtocol()
{
	delete ProtocolFile;
    delete DescriptionFile;
}
