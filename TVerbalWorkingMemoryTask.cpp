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
	for(int i = u'а'; i <= u'€'; i++){
		wchar_t buf[2] = {(wchar_t)i, '\0'};
		AnsiString str(buf);
		alphabet.push_back(str.c_str());
	}



	if(!Settings->Load(_name)){
		//Settings->Save(_name);
	}

    std::srand(time(NULL));
}

std::string vec_to_str(std::vector<std::string>& vec)
{
	std::string s;
	std::for_each(vec.begin(), vec.end(), [&](const std::string &piece)
	{
		s += piece+" ";
	});

	return s;
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
	 TrialCount = 0;
	 state = TVerbalWorkingMemory::BEGIN;
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
	int block_count = 1;
	int trial_per_comp = 1;
	int symbol_count = 8;
	std::vector<int> complexity_list = {2,3,4,5,7,8};

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
	 }
}

void TVerbalWorkingMemory::StateManager()
{
	if(TrialCount == sample_list.size() && state != TVerbalWorkingMemory::THANKS) state = TVerbalWorkingMemory::END;

	switch(state)
	{
		case TVerbalWorkingMemory::BEGIN:
		{
			 ClearCanva();
			 DrawText("ќтдых\n ќставл€йте глаза открытыми", 66, 100);
             Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::CROSS;
			 lsl->sendSample("BEGIN");
			 break;
		}
		case TVerbalWorkingMemory::CROSS:
		{
             ClearCanva();
			 DrawPlus();
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::CHARSET;
			 lsl->sendSample("CROSS");
			 break;
		}
		case TVerbalWorkingMemory::CHARSET:
		{
			 ClearCanva();
			 DrawText(vec_to_str(sample_list[TrialCount].symbol_list).c_str(), 166, 100);
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::REST1;
			 lsl->sendSample("CHARSET");
			 break;
		}
		case TVerbalWorkingMemory::REST1:
		{
             ClearCanva();
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::CHAR;
			 lsl->sendSample("REST1");
			 break;
		}
		case TVerbalWorkingMemory::CHAR:
		{
			 ClearCanva();
			 DrawText(sample_list[TrialCount].symbol.c_str(), 166, 100);
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::REST2;
			 lsl->sendSample("CHAR");
			 break;
		}
		case TVerbalWorkingMemory::REST2:
		{
             ClearCanva();
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::CROSS;
			 TrialCount++;
			 lsl->sendSample("REST2");
			 break;
		}
		case TVerbalWorkingMemory::END:
		{
			 ClearCanva();
			 DrawText("ќтдых\n ќставл€йте глаза открытыми", 66, 100);
			 Timer->Interval = 1000;
			 state = TVerbalWorkingMemory::THANKS;
			 lsl->sendSample("END");
			 break;
		}
		case TVerbalWorkingMemory::THANKS:
		{
			 ClearCanva();
			 DrawText("—пасибо за участие!", 66, 100);
			 Timer->Interval = 3000;
			 state = TVerbalWorkingMemory::FINISHED;
			 lsl->sendSample("THANKS");
			 break;
		}

        default:
			break;
    }
}

bool TVerbalWorkingMemory::Finished()
{
	if(state ==  TVerbalWorkingMemory::FINISHED) return true;
    else return false;

}

void TVerbalWorkingMemory::UserMouseDown(int X, int Y, TMouseButton Button)
{

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
