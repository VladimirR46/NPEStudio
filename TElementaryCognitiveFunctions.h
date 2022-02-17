//---------------------------------------------------------------------------

#ifndef TElementaryCognitiveFunctionsH
#define TElementaryCognitiveFunctionsH
#include "TBaseTask.h"
#include <vector>
#include <algorithm>
#include <random>

#define PROTOCOL_LOGGER 1
//---------------------------------------------------------------------------
typedef unsigned int uint32_t;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class TVisualSearchBlock : public TBaseTask
{
	enum SettingsName : int
	{
		Enable = 0,
		TrialMax = 1,
		RangeBackground = 2,
        RangePlus = 3
	};

	enum State : int
	{
		PLUS = 200,
		NUMBER,
		TABLE,
		BLANK,
		CLICK
	};

public:
	TVisualSearchBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "Триалы", GuiType::TEdit, 8);
			Settings->Add(RangeBackground, "Фон", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}
	}

	void InitTask(AnsiString Path) override {
        state = PLUS;
		TrialCount = 0;
	}

	bool Finished() override {
		if(Settings->get(Enable).ToInt() == 0) return true;

		if(TrialCount >= Settings->getInt(TrialMax)*2) return true;

		return false;
	}

    void CloseTask() override {}

	//--------------------------------------------------------------------------
	void GetRandomNumber()
	{
		int number  = RandomRange(10,100);

		while(int(number/10) == int(number%10))
			number  = RandomRange(10,100);

		CurrentNumber = number;
	}
	//--------------------------------------------------------------------------
	int GetTableType()
	{
		int TableType = RandomRange(0,2);

		if(type_count[TableType] < Settings->getInt(TrialMax))
		{
		  type_count[TableType]++;
		  return TableType;
		}

		TableType = int((bool)!TableType);
        type_count[TableType]++;
		return TableType;
    }
	//--------------------------------------------------------------------------
	void GetNumberArray(int number, int array[], int size, int TableType)
	{
		int n = 0;
		int index = 0;
		while(index < size)
		{
		   if(TableType == 0) n = GetNumberType1(number);
		   if(TableType == 1) n = GetNumberType2(number);

		   bool isNext = true;
		   for(int j = 0; j < index; j++)
		   {
			   if(n == array[j] || n == number)
			   {
				  isNext = false;
				  break;
			   }
		   }

		   if(isNext) array[index++] = n;
		}

		array[RandomRange(0,size)] = number;
	}
	//--------------------------------------------------------------------------

	int GetNumberType1(int number)
	{
       int n1 = 0;
	   int digit[2];
	   digit[0] = number / 10;
	   digit[1] = number % 10;

	   do
	   {
		n1 = RandomRange(10,100);
	   }
	   while(n1%10 != digit[0] && n1%10 != digit[1] && n1/10 != digit[0] && n1/10 != digit[1]);
       return n1;
	}
	//--------------------------------------------------------------------------

	int GetNumberType2(int number)
	{
       int n1 = 0;
	   int digit[2];
	   digit[0] = number / 10;
	   digit[1] = number % 10;

	   do
	   {
		n1 = RandomRange(10,100);
	   }
	   while(n1%10 == digit[0] || n1%10 == digit[1] || n1/10 == digit[0] || n1/10 == digit[1]);

	   return n1;
	}
	//--------------------------------------------------------------------------
	void UserMouseDown(int X, int Y) override
	{
		if(state == CLICK)
		{
			state = BLANK;
            StateManager();
		}
	}
    //--------------------------------------------------------------------------
	void StateManager() override
	{
		range_t rge;

		switch(state) {
			case PLUS:
                ClearCanva();
				DrawPlus();
				rge = Settings->getRange(RangePlus);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = NUMBER;
				break;
			case NUMBER:
				ClearCanva();
				GetRandomNumber();
				DrawOneNumber(CurrentNumber);
				Timer->Interval = RandomRange(1500,2000);
				state = TABLE;
				break;
			case TABLE:
				int numbers[25];
				GetNumberArray(CurrentNumber, numbers, 25, GetTableType());
				ClearCanva();
				DrawTable(numbers, 25);
                state = CLICK;
				break;
			case BLANK:
				ClearCanva();
				rge = Settings->getRange(RangeBackground);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = PLUS;
                TrialCount++;
				break;
			default:
				break;
		}

        Protocol->AddData(millis(), state);
	}

private:
	State state;
    int TrialCount = 0;

	int type_count[2];
	int CurrentNumber;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class TWorkingMemoryBlock : public TBaseTask
{
	enum SettingsName : int
	{
		Enable = 0,
		TrialMax = 1,
		RangeBackground = 2,
		RangePlus = 3,
		RangeBackground2 = 4,
		RangeStimul = 5,
        RangeStimulWait = 6
	};

	enum State : int
	{
		PLUS = 10,
		NUMBERS,
		BLANK,
		SAMPLE,
		BLANK2,
        CLICK
	};

public:
	TWorkingMemoryBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{

		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "Триалы", GuiType::TEdit, 8);
			Settings->Add(RangeBackground, "Фон 1", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground2, "Фон 2", GuiType::TRange, "1000:1500");
			Settings->Add(RangeStimul, "Стимул", GuiType::TRange, "1000:1500");
            Settings->Add(RangeStimulWait, "Ожидание реакции", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}
	}

	void InitTask(AnsiString Path) override {
		state = PLUS;
		TrialCount = 0;

		memset(array,0, sizeof(int)*7);
		memset(complexity_count,0, sizeof(int)*3);
		memset(samples_count,0, sizeof(int)*2);

		Protocol->AddDescription("Описание протокола "+ TaskName);
		Protocol->AddDescription("10 - плюс\n11 - числа\n12 - фон1\n13 - стимул\n14 - фон2\n15 - реакция\n");
	}

	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= Settings->getInt(TrialMax)*3) return true;
        else return false;
	}

    void CloseTask() override {}

	int calc_complexity()
	{
		int complexity_value[3] = {2,4,7};
		int n1;
		do
		{
			n1 = RandomRange(0,3);
		}
		while(complexity_count[n1] == Settings->getInt(TrialMax));
		complexity_count[n1]++;
        return complexity_value[n1];
	}

	int find_element(int array[], int size, int value)
	{
       for(int i = 0; i < size; i++)
	   {
		  if(array[i] == value)
		  {
			  return 1;
		  }
	   }
       return 0;
    }

	void get_unique_numbers(int array[], int size, int AFrom, int ATo)
	{
		int counter = 0;
		do
		{
		   int n1 = RandomRange(AFrom, ATo);

		   if(!find_element(array,size,n1))
		   {
			  array[counter] = n1;
			  counter++;
		   }
		}
		while(counter < size);
    }

	void generate_numbers(int array[])
	{
		int complexity = calc_complexity();

		int *array_number = new int[complexity];
		get_unique_numbers(array_number,complexity,10,100);

		int *array_places = new int[complexity];
		memset(array_places,-1, sizeof(int)*complexity);
		get_unique_numbers(array_places,complexity,0,7);

		for(int i = 0; i < complexity; i++)
		{
		   array[array_places[i]] = array_number[i];
        }

		delete[] array_places;
		delete[] array_number;
	}

	int get_sample_type()
	{
		int n1;
		do
		{
			n1 = RandomRange(0,2);
		}
		while(samples_count[n1] == (3*Settings->getInt(TrialMax))/2);
		samples_count[n1]++;
		return n1;
	}

	int get_sample()
	{
		int n1 = 0;
		int type = get_sample_type();
		if(type == 0)
		{
			do
				n1 = RandomRange(10,100);
			while(!find_element(array,7,n1));
		}
		else
		{
			do
				n1 = RandomRange(10,100);
			while(find_element(array,7,n1));
		}

        return n1;
    }

	void StateManager() override
	{
		range_t rge;
		switch(state) {
			case PLUS:
                ClearCanva();
				DrawPlus();
					Protocol->AddData(millis(), state);
				rge = Settings->getRange(RangePlus);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = NUMBERS;
				break;
			case NUMBERS:
				ClearCanva();
                memset(array,0, sizeof(int)*7);
				generate_numbers(array);
				DrawSymbols(array,124);
					Protocol->AddData(millis(), state);
				rge = Settings->getRange(RangeStimul);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = BLANK;
				break;
			case BLANK:
				ClearCanva();
					Protocol->AddData(millis(), state);
				rge = Settings->getRange(RangeBackground);
				Timer->Interval  = RandomRange(rge.min,rge.max);
				state = SAMPLE;
				break;
			case SAMPLE:
				DrawOneNumber(get_sample());
					Protocol->AddData(millis(), state);
				rge = Settings->getRange(RangeStimulWait);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = CLICK;
				break;
			case BLANK2:
			    ClearCanva();
                	Protocol->AddData(millis(), state);
				rge = Settings->getRange(RangeBackground2);
				Timer->Interval = RandomRange(rge.min,rge.max);
				state = PLUS;
                TrialCount++;
				break;
   			case CLICK:
				state = BLANK2;
				StateManager();
				break;
			default:
                break;
		}
	}

	void UserMouseDown(int X, int Y) override {
		 if(state == CLICK)
		 {
			state = BLANK2;
            StateManager();
		 }
	}

private:
	State state;
    int TrialCount = 0;

	int array[7] = {0};
	int complexity_count[3] = {0};
	int samples_count[2] = {0};
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class TMentalArithmeticBlock : public TBaseTask
{
	enum SettingsName : int
	{
		Enable = 0,
		TrialMax = 1,
		RangeBackground = 2,
		RangePlus = 3,
		RangeEquation = 5,
        RangeAnswerWait = 6
	};

	enum State
	{
		PLUS = 0,
		EQUATION,
		BLANK,
		CLICK
	};

public:
	// Описание триала для протокола
	struct TrialProtocol : TrialBase
	{
		uint32_t PLUS_TIME = 0;
		uint32_t EQUATION_TIME = 0;
		uint32_t BLANK_TIME = 0;
		int X;
		int N;
        int R;

		static TrialProtocol* CreateTrial(protocol_ptr _protocol) {
		   std::shared_ptr<TrialProtocol> trial = std::make_shared<TrialProtocol>();
		   _protocol->SaveTrial(trial);
		   return trial.get();
		}
	};

	TMentalArithmeticBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "Триалы", GuiType::TEdit, 8);
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeEquation, "Уравнение", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground, "Фон", GuiType::TRange, "1000:1500");
			Settings->Add(RangeAnswerWait, "Ожидание реакции", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}
	}

	void InitTask(AnsiString Path) override {
		state = PLUS;
		TrialCount = 0;

		Protocol->AddDescription("Описание протокола: "+ TaskName);
		//Protocol->AddDescription("10 - плюс\n11 - числа\n12 - фон1\n13 - стимул\n14 - фон2\n15 - реакция\n");
		calc_complexity();
        calc_samples_queue();
	}

	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= Settings->getInt(TrialMax)*3) return true;
		else return false;
	}

	void CloseTask() override {}
	//--------------------------------------------------------------------------
	void calc_complexity()
	{
	   complexity.clear();
	   for(int i = 0; i < Settings->getInt(TrialMax)*3; i++) {
		complexity.push_back((i%3)+1);
	   }
	   std::shuffle(complexity.begin(), complexity.end(), std::random_device());
	}

	void calc_samples_queue()
	{
		samples_queue.clear();
		for(int i = 0; i < Settings->getInt(TrialMax)*3; i++) {
           samples_queue.push_back(i%2);
		}
        std::shuffle(samples_queue.begin(), samples_queue.end(), std::random_device());
	}

	UnicodeString GetEquation()
	{
		int X  = RandomRange(10,100);
		int d = RandomRange(0,4);
		int sign = RandomRange(0,2);

		int N = complexity[TrialCount];
		int R = X - N;

		if(samples_queue[TrialCount] == 0) {
			R = (sign == 0) ? R - d : R + d;
        }
		#ifdef PROTOCOL_LOGGER
			Trial->X = X; Trial->N = N; Trial->R = R;
		#endif
        return IntToStr(X)+"-"+IntToStr(N)+"="+IntToStr(R);
	}

	void StateManager() override
	{
		switch(state) {
			case PLUS:
                ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = EQUATION;
				#ifdef PROTOCOL_LOGGER
					Trial = TrialProtocol::CreateTrial(Protocol);
                    Trial->PLUS_TIME = millis();
				#endif
				break;
			case EQUATION:
				ClearCanva();
				DrawText(GetEquation(),220);
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial->EQUATION_TIME = millis();
				#endif
				break;
		   case BLANK:
				ClearCanva();
				Timer->Interval = Settings->getRandFromRange(RangeBackground);
                state = PLUS;
				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial->BLANK_TIME = millis();
				#endif
				break;
			default:
				break;
		}
	}

	void UserMouseDown(int X, int Y) override {
		 if(state == CLICK)
		 {
			state = BLANK;
			#ifdef PROTOCOL_LOGGER
				Trial->click = {X, Y, millis(), 0};
			#endif
			StateManager();
		 }
	}

private:
	State state;
	int TrialCount = 0;

	std::vector<int> complexity;
	std::vector<int> samples_queue;
    TrialProtocol* Trial;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class TFunctionCombination : public TBaseTask
{
public:
	enum SettingsName : int
	{
		Enable = 0,
		RangePlus = 2,
        RangeAnswerWait = 3
	};

	enum State
	{
		INFO = 500,
		PLUS,
		MATRIX,
		CLICK
	};

    // Описание триала для протокола
	struct TrialProtocol : TrialBase
	{
		int number;
		int complexity;
		uint32_t INFO_TIME;
        uint32_t PLUS_TIME;
		uint32_t MATRIX_TIME;
		uint32_t INFO_CLICK_TIME;
		uint32_t MATRIX_CLICK_TIME;
		std::vector<int> varray;

		static TrialProtocol* CreateTrial(protocol_ptr _protocol) {
		   std::shared_ptr<TrialProtocol> trial = std::make_shared<TrialProtocol>();
		   _protocol->SaveTrial(trial);
		   return trial.get();
		}
	};

	TFunctionCombination(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeAnswerWait, "Ожидание реакции", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}
	}

	void InitTask(AnsiString Path) override {
		state = INFO;
		TrialCount = 0;
		complexity_index = 0;
        complexity.clear();

		std::vector<int> _complexity{1, 2, 3};
		srand(time(NULL));
		for(int i = 0; i < 3; i++) {
			std::random_shuffle(begin(_complexity), end(_complexity));
			complexity.insert(end(complexity), begin(_complexity), end(_complexity));
		}

		Protocol->AddDescription("Описание протокола: "+ TaskName);
		//Protocol->AddDescription("10 - плюс\n11 - числа\n12 - фон1\n13 - стимул\n14 - фон2\n15 - реакция\n");

	}

	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;

		if(TrialCount >= 25) {
		 TrialCount = 0;
         complexity_index++;
         state = INFO;
		 return true;
		}
		else return false;
	}

	void CloseTask() override {}
	//--------------------------------------------------------------------------
	void get_array()
	{
		array.clear();

		Number = RandomRange(10+24*complexity[complexity_index],100);
		array.push_back(Number);
		for(int i = 1; i < 25; i++) {
			array.push_back(array.back()-complexity[complexity_index]);
		}
	}

	void StateManager() override
	{
		switch(state) {
			case INFO:
				ClearCanva();
				get_array();
				DrawText("Максимальное число: "+IntToStr(Number)+", Число N: "+IntToStr(complexity[complexity_index]), 80);
				Timer->Interval = 0;
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial = TrialProtocol::CreateTrial(Protocol);
					Trial->number = Number;
					Trial->complexity = complexity[complexity_index];
                    Trial->INFO_TIME = millis();
				#endif
				break;
			case PLUS:
                ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = MATRIX;
				#ifdef PROTOCOL_LOGGER
					Trial->PLUS_TIME = millis();
				#endif
				break;
			case MATRIX:
				ClearCanva();
                srand(time(NULL));
				std::random_shuffle(begin(array), end(array));
				DrawTable(array.data(),array.size());
				Timer->Interval = 0;
				state = CLICK;
				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial = TrialProtocol::CreateTrial(Protocol);
					Trial->varray = array;
					Trial->MATRIX_TIME = millis();
				#endif
				break;
			default:
				break;
		}
	}

	void UserMouseDown(int X, int Y) override {
		 if(state == CLICK)
		 {
			if(TrialCount == 0) {
			 state = PLUS;
			 Trial->INFO_CLICK_TIME = millis();
			}
			else {
			  state = MATRIX;
			  Trial->MATRIX_CLICK_TIME = millis();
            }

			Trial->click = {X, Y, millis(), 0};
			if(Parent) Parent->StateManager();
		 }
	}

private:
	State state;
	int TrialCount = 0;

	int complexity_index = 0;
	std::vector<int> complexity;

	int Number = 0;
	std::vector<int> array;
	TrialProtocol* Trial;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////Элементарные когнитивные функции////////////////


class TElementaryCognitiveFunctions : public TBaseTask
{
public:
	enum SettingsName : int
	{
		RangeBackground = 0,
		RangeBreak = 1,
        PartsCount = 2
    };

	enum State : int
	{
		 BACKGROUND = 0,
		 RUN_BLOCK,
		 PAUSE
	} state;

	// Описание триала для протокола
	struct TrialProtocol : TrialBase
	{
		uint32_t BACKGROUND_TIME;
		uint32_t RUN_BLOCK_TIME;
		uint32_t PAUSE_TIME;

		static TrialProtocol* CreateTrial(protocol_ptr _protocol) {
		   std::shared_ptr<TrialProtocol> trial = std::make_shared<TrialProtocol>();
		   _protocol->SaveTrial(trial);
		   return trial.get();
		}
	};

	TElementaryCognitiveFunctions(AnsiString _name);

	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y) override;
	void Draw() override;
	void CloseTask() override;

private:
	int cur_block;
	int PartCount = 0;

    std::vector<std::vector<int>> BlockSequence;
    TrialProtocol* Trial;
};



#endif
