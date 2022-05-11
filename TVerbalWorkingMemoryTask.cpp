//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVerbalWorkingMemoryTask.h"

#include <algorithm>
#include <random>
//---------------------------------------------------------------------------
#pragma package(smart_init)



TVerbalWorkingMemory::TVerbalWorkingMemory(AnsiString _name)
	: TBaseTask(_name)
{
	for(int i = u'А'; i <= u'Я'; i++){
		wchar_t buf[2] = {(wchar_t)i, '\0'};
		AnsiString str(buf);
		alphabet.push_back(str.c_str());
	}

	if(!Settings->Load(_name)){
		Settings->Add(MaxBlockCount, "Количество блоков", GuiType::TEdit, "1");
		Settings->Add(MaxTrialPerCmplexity, "Триалов на сложность", GuiType::TEdit, "1");
		Settings->Add(MaxSymbolCount, "Кол. символов", GuiType::TEdit, "8");
		Settings->Add(Complexity, "Уровни сложности", GuiType::TEdit, "2,3,4,5,6,7,8");

		Settings->Add(BeginTime, "Начало", GuiType::TEdit, "1000");
		Settings->Add(CrossTimeRange, "Крест", GuiType::TRange, "1000:2000");
		Settings->Add(SymbolsTimeRange, "Символы", GuiType::TRange, "1000:2000");
		Settings->Add(RestTimeRange, "Отдых 1", GuiType::TRange, "1000:2000");
		Settings->Add(SymbolTimeRange, "Стимул", GuiType::TRange, "1000:2000");
		Settings->Add(Rest2TimeRange, "Отдых 2", GuiType::TRange, "1000:2000");
		Settings->Add(EndTime, "Конец", GuiType::TEdit, "1000");

		Settings->Save(_name);
	}

    std::srand(time(NULL));
}

std::string vec_to_str(std::vector<std::string>& vec)
{
	std::string s;
	std::for_each(vec.begin(), vec.end(), [&](const std::string &piece) {s += piece+" ";});
	return s;
}

std::vector<int> str_to_vecint(const char* str, std::string delim)
{
	TStringDynArray data = SplitString(str,delim.c_str());
	std::vector<int> vec(data.size());
	for(int i = 0; i < data.size(); i++) vec[i] = data[i].ToInt();
    return vec;
}

void TVerbalWorkingMemory::samples_to_lsl(std::vector<Sample>& samples, samples_t &lsl_sample)
{
	for(int i = 0; i < sample_list.size(); i++)
	{
		 std::map<std::string, std::string> sample;

		 sample["symbols"] = vec_to_str(sample_list[i].symbol_list);
		 sample["symbol"] = sample_list[i].symbol;
		 sample["targeted"] = std::to_string(sample_list[i].targeted);
		 sample["complexity"] = std::to_string(sample_list[i].complexity);
		 lsl_sample.push_back(sample);
	}
}

void TVerbalWorkingMemory::InitTask(AnsiString Path)
{
     BlockCount = 0;
	 TrialCount = 0;
	 state = State::BEGIN;
	 Timer->Enabled = true;

	 gen_sample();

	 samples_t lsl_samples;
	 samples_to_lsl(sample_list,lsl_samples);

	 lsl->start(lsl_samples);
     lsl->start_rec(Path);
}

void get_rand_symbols(const std::vector<std::string>& _alphabet, std::vector<std::string>& ret, int number)
{
	std::vector<std::string> alphabet = _alphabet;
	ret.resize(number);

	std::random_shuffle(alphabet.begin(), alphabet.end());
	std::copy(alphabet.begin(),alphabet.begin()+number, ret.begin());
}

std::string get_goal_symbol(const std::vector<std::string>& list)
{
	int item =  std::rand() % list.size();
	return list[item];
}

std::string get_ungoal_symbol(const std::vector<std::string>& alphabet, const std::vector<std::string>& list)
{
	 for(int i = 0; i < alphabet.size(); i++)
	 {
		 std::string symbol =  alphabet[std::rand() % alphabet.size()];
		 if(std::find(list.begin(), list.end(), symbol) != list.end())
			 return symbol;
	 }
	 return "";
}

std::vector<int> gen_sequence(int size, int max_number)
{
	std::vector<int> ret(size);
	for(int i = 0; i < size; i++)
		ret[i] = i % max_number;
	return ret;
}

std::vector<std::string> set_complexity(const std::vector<std::string>& list, int complexity)
{
	std::vector<std::string> ret = list;
	std::vector<int> indexes = gen_sequence(list.size(), list.size());
	std::random_shuffle(indexes.begin(), indexes.end());

	for(int i = 0; i < list.size() - complexity; i++)
		ret[indexes[i]] = "*";

	return ret;
}

void TVerbalWorkingMemory::gen_sample()
{
	sample_list.clear();
    block_index.clear();
	//Complexity,
	int block_count = Settings->getInt(MaxBlockCount);
	int trial_per_comp = Settings->getInt(MaxTrialPerCmplexity);
	int symbol_count = Settings->getInt(MaxSymbolCount);
	std::vector<int> complexity_list = str_to_vecint(AnsiString(Settings->get(Complexity)).c_str(), ",");

	int trial_in_block = trial_per_comp*complexity_list.size();
	std::vector<std::string> symbols;

	std::vector<int> targeted_in_block = gen_sequence(trial_in_block, 2);
	std::vector<int> complexity_in_block(trial_in_block);

	for(int i = 0; i < trial_in_block; i++) complexity_in_block[i] = complexity_list[i%complexity_list.size()];

	/* Generate block*/
	for (int idx = 0; idx < block_count; idx++)
	{
		std::random_shuffle(targeted_in_block.begin(), targeted_in_block.end());
		std::random_shuffle(complexity_in_block.begin(), complexity_in_block.end());

		for(int i = 0; i < trial_in_block; i++)
		{
            get_rand_symbols(alphabet, symbols, symbol_count);

			Sample sample;
			sample.targeted = targeted_in_block[i];
			sample.symbol = (sample.targeted == 0) ? get_goal_symbol(symbols) : get_ungoal_symbol(alphabet, symbols);
			sample.symbol_list = set_complexity(symbols, complexity_in_block[i]);
			sample.complexity = complexity_in_block[i];
			sample_list.push_back(sample);
		}
		block_index.push_back(sample_list.size());
	 }
}

void TVerbalWorkingMemory::StateManager()
{

	if(BlockCount < block_index.size()){
		if(TrialCount == block_index[BlockCount]){
			state = State::VAS;
			BlockCount++;
		}
	}

	switch(state)
	{
		case State::BEGIN:
		{
			 ClearCanva();
			 DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100);
			 Timer->Interval = Settings->getInt(BeginTime);
			 state = State::CROSS;
			 lsl->sendSample("BEGIN");
			 break;
		}
		case State::CROSS:
		{
             ClearCanva();
			 DrawPlus();
			 Timer->Interval = Settings->getRandFromRange(CrossTimeRange);
			 state = State::CHARSET;
			 lsl->sendSample("CROSS");
			 break;
		}
		case State::CHARSET:
		{
			 ClearCanva();
			 //std::vector<std::string> list = {"ц","X","p","m","i","L","k","g"};
			 DrawSymArray(sample_list[TrialCount].symbol_list);
			 Timer->Interval = Settings->getRandFromRange(SymbolsTimeRange);
			 state = State::REST1;
			 lsl->sendSample("CHARSET");
			 break;
		}
		case State::REST1:
		{
             ClearCanva();
			 Timer->Interval = Settings->getRandFromRange(RestTimeRange);
			 state = State::CHAR;
			 lsl->sendSample("REST1");
			 break;
		}
		case State::CHAR:
		{
			 ClearCanva();
			 DrawText(sample_list[TrialCount].symbol.c_str(), 166, 1);
			 Timer->Interval = Settings->getRandFromRange(SymbolTimeRange);
			 state = State::REST2;
			 lsl->sendSample("CHAR");
			 break;
		}
		case State::REST2:
		{
             ClearCanva();
			 Timer->Interval = Settings->getRandFromRange(Rest2TimeRange);
			 state = State::CROSS;
			 TrialCount++;
			 lsl->sendSample("REST2");
			 break;
		}
		case State::VAS:
		{
             ClearCanva();
			 Timer->Interval = 0;
			 VAS->Init({vasMental});
			 VAS->Run();
			 break;
		}
		case State::END:
		{
			 ClearCanva();
			 DrawText("Отдых\n Оставляйте глаза открытыми", 66, 100);
			 Timer->Interval = Settings->getInt(EndTime);
			 state = State::THANKS;
			 lsl->sendSample("END");
			 break;
		}
		case State::THANKS:
		{
			 ClearCanva();
			 DrawText("Спасибо за участие!", 66, 100);
			 Timer->Interval = 3000;
			 state = State::FINISHED;
			 lsl->sendSample("THANKS");
			 break;
		}

        default:
			break;
    }
}

bool TVerbalWorkingMemory::Finished()
{
	if(state == State::FINISHED) return true;
    else return false;

}

void TVerbalWorkingMemory::UserMouseDown(int X, int Y, TMouseButton Button)
{

}

void TVerbalWorkingMemory::VasFinished(TObject *Sender)
{
	 ClearCanva();
	 state = State::CROSS;
	 Timer->Interval = 300;
	 if(TrialCount == sample_list.size()) state = State::END;
}

void TVerbalWorkingMemory::Draw()
{
   TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
   Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);
}

void TVerbalWorkingMemory::CloseTask()
{
    Timer->Enabled = false;
	lsl->stop_rec();
	lsl->stop();
}
