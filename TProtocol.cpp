//---------------------------------------------------------------------------

#pragma hdrstop

#include "TProtocol.h"
#include "TElementaryCognitiveFunctions.h"

#include <codecvt>
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

std::string UTF8_to_CP1251(std::string const & utf8)
{
    if(!utf8.empty())
    {
        int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
        if(wchlen > 0 && wchlen != 0xFFFD)
        {
            std::vector<wchar_t> wbuf(wchlen);
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], wchlen);
            std::vector<char> buf(wchlen);
            WideCharToMultiByte(1251, 0, &wbuf[0], wchlen, &buf[0], wchlen, 0, 0);

            return std::string(&buf[0], wchlen);
        }
    }
    return std::string();
}

void TProtocol::Save()
{
	 DescriptionFile->SaveToFile(FilePath+"\\Description.txt", TEncoding::Unicode);
	 ProtocolFile->SaveToFile(FilePath+"\\Protocol.txt", TEncoding::Unicode);

     MATFile *pmat;
     // Создаю mat файл
	 pmat = matOpen("Protocol.mat", "w");

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
		AnsiString Name = AnsiString(NameBlocks[i].c_str());

		////////////////Комбинированные функции//////////////////
		if(NameBlocks[i] == "Комбинированные функции")
		{
			const char *fieldnames[8] = {"TrialsName", "Trials", "Stimuls", "StimulsName", "GuideTime", "CrossTime", "Goal", "Complexity"};
			block = mxCreateStructMatrix(1, 1, 8, fieldnames);

			const char *trialsname[2] = {"Matrix", "Response"};
			mxArray *trialsname_cell = mxCreateCellMatrix(1,2);
			mxSetCell(trialsname_cell,0, mxCreateString(trialsname[0]));
            mxSetCell(trialsname_cell,1, mxCreateString(trialsname[1]));
			mxSetField(block, 0, fieldnames[0], trialsname_cell);

			const char *stimulsname[1] = {"Matrix"};
			mxArray *stimulsname_cell = mxCreateCellMatrix(1,1);
			mxSetCell(stimulsname_cell,0, mxCreateString(stimulsname[0]));
			mxSetField(block, 0, fieldnames[3], stimulsname_cell);

			int size = Data[i].size()-1;
			mxArray *Trials = mxCreateDoubleMatrix(size, 2, mxREAL);
			double *trials_ptr = mxGetPr(Trials);

			mxArray *stimul_cell = mxCreateCellMatrix(size, 1);
			for(int j = 0; j < Data[i].size(); j++)
			{
				TFunctionCombination::TrialProtocol* Trial = static_cast<TFunctionCombination::TrialProtocol*>(Data[i][j].get());

				if(j == 0) {
					 mxSetField(block, 0, fieldnames[4], mxCreateDoubleScalar(Trial->INFO_TIME));
					 mxSetField(block, 0, fieldnames[5], mxCreateDoubleScalar(Trial->PLUS_TIME));
					 mxSetField(block, 0, fieldnames[6], mxCreateDoubleScalar(Trial->number));
                     mxSetField(block, 0, fieldnames[7], mxCreateDoubleScalar(Trial->complexity));
				} else{
					 trials_ptr[size*0+j-1] = Trial->MATRIX_TIME;
					 trials_ptr[size*1+j-1] = Trial->MATRIX_CLICK_TIME;

					 mxArray *mat = mxCreateDoubleMatrix(1, Trial->varray.size(), mxREAL);
					 for(int k = 0; k < Trial->varray.size(); k++) mxGetPr(mat)[k] = Trial->varray[k];
					 mxSetCell(stimul_cell,j-1, mat);
				}
			}
			mxSetField(block, 0, fieldnames[1], Trials);
			mxSetField(block, 0, fieldnames[2], stimul_cell);
		} else {
			block = mxCreateString("Error");
        }
        //////////////////////////////////////////////////////////////
		std::string utf8 =  u8"Мыва"; // or u8"zß水𝄋"
							// or "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b";
		std::cout << "original UTF-8 string size: " << utf8.size() << '\n';

		// the UTF-8 - UTF-32 standard conversion facet
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

		// UTF-8 to UTF-32
		//std::u32string utf32 = cvt.from_bytes(utf8);
		mxSetCell(protocol_cell,3*0, mxCreateString("Вова"));
		mxSetCell(protocol_cell,3*1, mxCreateString(UTF8_to_CP1251("Вовавыаыв").c_str()));
        //std::u8string dd = "ВАлыв";
		mxSetCell(protocol_cell,3*2, mxCreateString(UTF8_to_CP1251(utf8).c_str()));


		mxSetCell(protocol_cell,3*i+1, mxCreateDoubleScalar(234234234));
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
	Data.assign(24,std::vector<std::shared_ptr<TrialBase>>());
}

TProtocol::~TProtocol()
{
	delete ProtocolFile;
    delete DescriptionFile;
}
