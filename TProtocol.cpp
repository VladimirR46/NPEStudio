//---------------------------------------------------------------------------
#pragma hdrstop
#include "TProtocol.h"
#include "TElementaryCognitiveFunctions.h"
#include "TCubeTask.h"
#include "TFingerTest.h"
#include "TLearningTask.h"
#include <filesystem>
#include "SettingsWin.h"
#include <variant>
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

	mat::TStruct Subject({"Name", "age", "gender", "hand"});
	Subject.Field(0, subject.LastName+" "+subject.Name+" "+subject.Patronymic);
	Subject.Field(1, StrToInt(subject.Age));
	Subject.Field(2, subject.Gender);
	Subject.Field(3, subject.ActiveHand);
	Subject.PutToMFile(pmat, "Subject");

	if(TaskName == "Элементарные когнитивные функции") {
       SaveECFTask(pmat);
    }

	if(TaskName == "Кубы Неккера") {
	   SaveCubeTask(pmat);
	}

	if(TaskName == "Finger Test") {
	   SaveFingerTask(pmat);
	}

    if(TaskName == "Обучение") {
	   SaveLearningTask(pmat);
	}

	if(matClose(pmat) != 0) {
		ShowMessage("Ошибка: Невозможно закрыть файл протокола");
		return;
	}

	if(Form3->CheckBox3->IsChecked) {
		UnicodeString pt = "/select, "+GetHomePath()+"\\NPEStudio\\"+FilePath;
		ShellExecuteW(NULL, NULL, L"explorer.exe", pt.w_str(), NULL, SW_SHOWNORMAL);
	}
}
__finally{
	  std::filesystem::current_path(path);
}

}
//------------------------------------------------------------------------------

void TProtocol::SaveLearningTask(MATFile *pmat)
{
	for(int idx = 0; idx < Data.size(); idx++)
	{
		if(Data[idx]->BlockName == "Questions")
		{
			typedef TLearningTask::QProtocolBlock::Trial Trial;
			TLearningTask::QProtocolBlock* qBlock = static_cast<TLearningTask::QProtocolBlock*>(Data[idx].get());

			mat::TDoubleMatrix TimeLine(qBlock->trial_list.size(), 3);
			mat::TCellMatrix Stimuls(qBlock->trial_list.size(), 5);
			for(int i = 0; i < qBlock->trial_list.size(); i++)
			{
			   Trial* trial = static_cast<Trial*>(qBlock->trial_list[i].get());

			   TimeLine(i,0) = trial->StateTime[0];
			   TimeLine(i,1) = trial->StateTime[1];
			   TimeLine(i,2) = trial->StateTime[2];

			   Stimuls(i, 0) = trial->Number;
			   Stimuls(i, 1) = trial->Question;
			   Stimuls(i, 2) = trial->ModalityType;
			   Stimuls(i, 3) = trial->Topic;
			   Stimuls(i, 4) = trial->Category;
			}


			mat::TStruct vas({"Mental", "Physical", "Boredom", "Effort"});

			mat::TDoubleMatrix MentalMatrix(qBlock->pMental.size(), 2);
			for(int i = 0; i < qBlock->pMental.size(); i++){
				MentalMatrix(i,0) = qBlock->pMental[i].value;
				MentalMatrix(i,1) = qBlock->pMental[i].global_counter;
			}

			mat::TDoubleMatrix PhysicalMatrix(qBlock->pPhysical.size(), 2);
			for(int i = 0; i < qBlock->pPhysical.size(); i++){
				PhysicalMatrix(i,0) = qBlock->pPhysical[i].value;
				PhysicalMatrix(i,1) = qBlock->pPhysical[i].global_counter;
			}

			mat::TDoubleMatrix BoredomMatrix(qBlock->pBoredom.size(), 2);
			for(int i = 0; i < qBlock->pBoredom.size(); i++){
				BoredomMatrix(i,0) = qBlock->pBoredom[i].value;
				BoredomMatrix(i,1) = qBlock->pBoredom[i].global_counter;
			}

			mat::TDoubleMatrix EffortMatrix(qBlock->pEffort.size(), 2);
			for(int i = 0; i < qBlock->pEffort.size(); i++){
				EffortMatrix(i,0) = qBlock->pEffort[i].value;
				EffortMatrix(i,1) = qBlock->pEffort[i].global_counter;
			}

			vas.Field(0, MentalMatrix);
			vas.Field(1, PhysicalMatrix);
			vas.Field(2, BoredomMatrix);
			vas.Field(3, EffortMatrix);

			mat::TDoubleMatrix Backgrounds(qBlock->Backgrounds.size(), 2);
			for(int i = 0; i < qBlock->Backgrounds.size(); i++){
			   Backgrounds(i,0) = qBlock->Backgrounds[i].begin;
			   Backgrounds(i,1) = qBlock->Backgrounds[i].end;
			}

			mat::TStruct Protocol({"TimeLineName", "TimeLine", "StimulsName", "Stimuls", "VAS", "Backgrounds"});
			Protocol.Field(0, {"Cross","Image", "Rest"});
			Protocol.Field(1, TimeLine);
			Protocol.Field(2, {"Number","Question", "Modality", "Topic", "Category"});
			Protocol.Field(3, Stimuls);
			Protocol.Field(4, vas);
			Protocol.Field(5, Backgrounds);
			Protocol.PutToMFile(pmat, "QProtocol");
		}

		if(Data[idx]->BlockName == "Testing")
		{
			typedef TLearningTask::TProtocolBlock::Trial Trial;
			TLearningTask::TProtocolBlock* qBlock = static_cast<TLearningTask::TProtocolBlock*>(Data[idx].get());

			mat::TDoubleMatrix TimeLine(qBlock->trial_list.size(), 3);
			mat::TCellMatrix Stimuls(qBlock->trial_list.size(), 6);
            for(int i = 0; i < qBlock->trial_list.size(); i++)
			{
			   Trial* trial = static_cast<Trial*>(qBlock->trial_list[i].get());

			   TimeLine(i,0) = trial->StateTime[0];
			   TimeLine(i,1) = trial->StateTime[1];
			   TimeLine(i,2) = trial->StateTime[2];

			   Stimuls(i, 0) = trial->Number;
			   Stimuls(i, 1) = trial->Question;
			   Stimuls(i, 2) = trial->Topic;
			   Stimuls(i, 3) = trial->Category;
               Stimuls(i, 4) = trial->TestType;

			   mat::TDoubleMatrix Keys(trial->key_list.size(), 2);
			   for(int j = 0; j < trial->key_list.size(); j++){
				 Keys(j, 0) = trial->key_list[j].key;
				 Keys(j, 1) = trial->key_list[j].time;
			   }
			   Stimuls(i, 5) = Keys;
			}

            mat::TStruct vas({"Mental", "Physical", "Boredom", "Effort"});

			mat::TDoubleMatrix MentalMatrix(qBlock->pMental.size(), 2);
			for(int i = 0; i < qBlock->pMental.size(); i++){
				MentalMatrix(i,0) = qBlock->pMental[i].value;
				MentalMatrix(i,1) = qBlock->pMental[i].global_counter;
			}

			mat::TDoubleMatrix PhysicalMatrix(qBlock->pPhysical.size(), 2);
			for(int i = 0; i < qBlock->pPhysical.size(); i++){
				PhysicalMatrix(i,0) = qBlock->pPhysical[i].value;
				PhysicalMatrix(i,1) = qBlock->pPhysical[i].global_counter;
			}

			mat::TDoubleMatrix BoredomMatrix(qBlock->pBoredom.size(), 2);
			for(int i = 0; i < qBlock->pBoredom.size(); i++){
				BoredomMatrix(i,0) = qBlock->pBoredom[i].value;
				BoredomMatrix(i,1) = qBlock->pBoredom[i].global_counter;
			}

			mat::TDoubleMatrix EffortMatrix(qBlock->pEffort.size(), 2);
			for(int i = 0; i < qBlock->pEffort.size(); i++){
				EffortMatrix(i,0) = qBlock->pEffort[i].value;
				EffortMatrix(i,1) = qBlock->pEffort[i].global_counter;
			}

			vas.Field(0, MentalMatrix);
			vas.Field(1, PhysicalMatrix);
			vas.Field(2, BoredomMatrix);
			vas.Field(3, EffortMatrix);

            mat::TDoubleMatrix Backgrounds(qBlock->Backgrounds.size(), 2);
			for(int i = 0; i < qBlock->Backgrounds.size(); i++){
			   Backgrounds(i,0) = qBlock->Backgrounds[i].begin;
			   Backgrounds(i,1) = qBlock->Backgrounds[i].end;
			}

			mat::TStruct Protocol({"TimeLineName", "TimeLine", "StimulsName", "Stimuls", "VAS", "Backgrounds"});
			Protocol.Field(0, {"Cross","Image", "Rest"});
			Protocol.Field(1, TimeLine);
			Protocol.Field(2, {"Number","Question", "Topic", "Category", "Test", "Keys"});
			Protocol.Field(3, Stimuls);
			Protocol.Field(4, vas);
            Protocol.Field(5, Backgrounds);
			Protocol.PutToMFile(pmat, "TProtocol");
		}

		if(Data[idx]->BlockName == "Main")
		{
			TLearningTask::MProtocolBlock* qBlock = static_cast<TLearningTask::MProtocolBlock*>(Data[idx].get());

			mat::TDoubleMatrix Backgrounds(qBlock->GlobalRest.size(), 2);
			for(int i = 0; i < qBlock->GlobalRest.size(); i++){
			   Backgrounds(i,0) = qBlock->GlobalRest[i].begin;
			   Backgrounds(i,1) = qBlock->GlobalRest[i].end;
			}

			matPutVariable(pmat, "GlobalRest", Backgrounds);
			mxDestroyArray(Backgrounds);
        }
	}
}
//------------------------------------------------------------------------------
void TProtocol::SaveFingerTask(MATFile *pmat)
{
	const char *fieldnames[4] = {"TimeLineName", "TimeLine", "StimulsName", "Stimuls"};
	mxArray *block = mxCreateStructMatrix(1, 1, 4, fieldnames);

    typedef TFingerTest::ProtocolBlock::Trial Trial;
	TFingerTest::ProtocolBlock* proto_ptr = static_cast<TFingerTest::ProtocolBlock*>(Data[0].get());

	mxArray* timelineName = mat::mCreateStringArray({"Noise", "Image"});
	mxSetField(block, 0, fieldnames[0], timelineName);

	mxArray* stimulsName = mat::mCreateStringArray({"Key","ImageName"});
	mxSetField(block, 0, fieldnames[2], stimulsName);

	mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->trial_list.size(), mxGetN(timelineName), mxREAL);
	double *timeline_ptr = mxGetPr(timeline);
	mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->trial_list.size(), mxGetN(stimulsName));

   	for(int j = 0; j < proto_ptr->trial_list.size(); j++)
	{
		Trial* trial = static_cast<Trial*>(proto_ptr->trial_list[j].get());

        int size = proto_ptr->trial_list.size();
		for(int k = 0; k < 2; k++){
		  timeline_ptr[size*k+j] = trial->StateTime[k];
		}

		mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, mxCreateDoubleScalar(trial->Key));
		mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mxCreateString(AnsiString(trial->ImageName).c_str()));
	}

    mxSetField(block, 0, fieldnames[1], timeline);
	mxSetField(block, 0, fieldnames[3], stimuls_cell);

	matPutVariable(pmat, "protocol", block);
	mxDestroyArray(block);
}
//------------------------------------------------------------------------------
void TProtocol::SaveCubeTask(MATFile *pmat)
{
	const char *fieldnames[4] = {"TimeLineName", "TimeLine", "StimulsName", "Stimuls"};
	mxArray *block = mxCreateStructMatrix(1, 1, 4, fieldnames);

	typedef TCubeTask::DProtocol::Trial Trial;
	TCubeTask::DProtocol* proto_ptr = static_cast<TCubeTask::DProtocol*>(Data[0].get());

	mxArray* timelineName = mat::mCreateStringArray({"Noise", "Cube"});
	mxSetField(block, 0, fieldnames[0], timelineName);

	mxArray* stimulsName = mat::mCreateStringArray({"CubeType","Intensity"});
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

		mxArray* timelineName = mat::mCreateStringArray({"Cross", "Matrix", "UserClick"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mat::mCreateStringArray({"MatrixShown","UserSelection"});
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

		mxArray* timelineName = mat::mCreateStringArray({"Cross", "Equation", "UserClick", "BlackScreen"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mat::mCreateStringArray({"X","N","R","Result"});
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
    ////////////////"Рабочая память"//////////////////
	else if(Data[i]->BlockName == "Рабочая память")
	{
		typedef TWorkingMemoryBlock::DProtocol::Trial Trial;
		TWorkingMemoryBlock::DProtocol* proto_ptr = static_cast<TWorkingMemoryBlock::DProtocol*>(Data[i].get());

        mxArray* timelineName = mat::mCreateStringArray({"Cross", "Numbers", "BlackScreen1", "Cross2", "Stimul", "UserClick", "BlackScreen2"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mat::mCreateStringArray({"Numbers","Stimul","Result", "ResponseTimeOut"});
		mxSetField(block, 0, fieldnames[2], stimulsName);

        mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->Trials.size(), mxGetN(timelineName), mxREAL);
		double *timeline_ptr = mxGetPr(timeline);
		mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->Trials.size(), mxGetN(stimulsName));

        for(int j = 0; j < proto_ptr->Trials.size(); j++)
		{
			Trial* trial = static_cast<Trial*>(proto_ptr->Trials[j].get());
			if(j == 0 ) BlockStartTime = trial->StateTime[TWorkingMemoryBlock::State::PLUS];
            int size = proto_ptr->Trials.size();

			for(int k = 0; k < 7; k++){
			  timeline_ptr[size*k+j] = trial->StateTime[k];
			}

			std::vector<AnsiString> array_lable;
			for(int k = 0; k < trial->varray.size(); k++){
				if(trial->varray[k] == 0) array_lable.push_back("*");
                else array_lable.push_back(IntToStr(trial->varray[k]));
            }
            mxArray* array = mat::mCreateStringArray(array_lable);

			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, array);
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mxCreateDoubleScalar(trial->Stimul));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*2+j, mxCreateDoubleScalar(0));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*3+j, mxCreateDoubleScalar(trial->ResponseTimeOut));
		}

		mxSetField(block, 0, fieldnames[1], timeline);
		mxSetField(block, 0, fieldnames[3], stimuls_cell);
	}
    ////////////////"Визуальный поиск"//////////////////
	else if(Data[i]->BlockName == "Визуальный поиск")
	{
		typedef TVisualSearchBlock::DProtocol::Trial Trial;
		TVisualSearchBlock::DProtocol* proto_ptr = static_cast<TVisualSearchBlock::DProtocol*>(Data[i].get());

		mxArray* timelineName = mat::mCreateStringArray({"Cross", "Number", "Table", "UserClick", "BlackScreen"});
		mxSetField(block, 0, fieldnames[0], timelineName);

		mxArray* stimulsName = mat::mCreateStringArray({"Numbers", "Table", "Result" });
		mxSetField(block, 0, fieldnames[2], stimulsName);

        mxArray *timeline = mxCreateDoubleMatrix(proto_ptr->Trials.size(), mxGetN(timelineName), mxREAL);
		double *timeline_ptr = mxGetPr(timeline);
		mxArray *stimuls_cell = mxCreateCellMatrix(proto_ptr->Trials.size(), mxGetN(stimulsName));

		for(int j = 0; j < proto_ptr->Trials.size(); j++)
		{
			Trial* trial = static_cast<Trial*>(proto_ptr->Trials[j].get());
			if(j == 0 ) BlockStartTime = trial->StateTime[TVisualSearchBlock::State::PLUS];
            int size = proto_ptr->Trials.size();

			for(int k = 0; k < 5; k++){
			  timeline_ptr[size*k+j] = trial->StateTime[k];
			}

			mxArray *mat = mxCreateDoubleMatrix(1, trial->array.size(), mxREAL);
			for(int k = 0; k < trial->array.size(); k++) mxGetPr(mat)[k] = trial->array[k];

			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*0+j, mxCreateDoubleScalar(trial->number));
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*1+j, mat);
			mxSetCell(stimuls_cell,mxGetM(stimuls_cell)*2+j, mxCreateDoubleScalar(0));
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
mxArray *mat::mCreateStringArray(std::vector<AnsiString> array)
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
