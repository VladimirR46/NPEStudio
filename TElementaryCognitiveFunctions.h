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
class TDrawHint
{
public:
	TDrawHint(TBitmap *_bitmap);
	~TDrawHint();

	void __fastcall TimerEvent(TObject* Sender);
	void Draw();
	bool Contains(int _x, int _y);
	void Hide();
	void SetPosition(int _x, int _y);
	void Show(UnicodeString _text);
    bool Visible();

private:
	TBitmap *bitmap;
    TTimer* timer;
	System::Types::TRectF rect;
	UnicodeString text = "";
    int X = 0, Y = 0;
	int sizeX = 180, sizeY = 90;
    bool isShow = false;
};
///////////////////////////////////////////////////////////////////////////////
class TDrawButton
{
public:
	TDrawButton(TBitmap *_bitmap, UnicodeString _text) : bitmap(_bitmap), text(_text)
	{
		fill_color = TAlphaColors::Black;
	}

	void Draw()
	{
		rect = System::Types::TRectF(X-sizeX,Y-sizeY,X+sizeX,Y+sizeY);
		float thick = Thickness/2;
		System::Types::TRectF fill_rect(rect.left+thick, rect.top+thick, rect.right-thick, rect.bottom-thick);

		bitmap->Canvas->BeginScene();
		bitmap->Canvas->Stroke->Thickness = Thickness;
		bitmap->Canvas->Font->Size = 50;
		bitmap->Canvas->Fill->Color = fill_color;
		bitmap->Canvas->FillRect(rect, 40, 40, AllCorners, 1, TCornerType::Round);
		bitmap->Canvas->DrawRect(rect, 40, 40, AllCorners, 1, TCornerType::Round);
		bitmap->Canvas->Fill->Color = TAlphaColors::White;
		bitmap->Canvas->FillText(rect, text, false, 100, TFillTextFlags(),TTextAlign::Center, TTextAlign::Center);
		bitmap->Canvas->EndScene();
	}

	void SetPosition(int _x, int _y) { X = _x; Y = _y; }

	void SetFillColor(TAlphaColor color)
	{
	   fill_color = color;
    }

	bool Contains(int _x, int _y)
	{
		if(rect.Contains(System::Types::TPoint(_x, _y))) return true;
        else return false;
    }

private:
	TBitmap *bitmap;
	System::Types::TRectF rect;
	TAlphaColor fill_color;
	int X = 0, Y = 0;
	int sizeX = 150, sizeY = 60;
    int Thickness = 6;
	UnicodeString text = "";
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TVisualSearchBlock : public TBaseTask
{
public:
	enum SettingsName : int
	{
		Enable = 0,
		TrialMax = 1,
		RangeNumber = 2,
		RangeBackground = 3,
		RangeTable = 4,
		RangePlus = 5
	};

	enum State : int
	{
        INSTRUCTION = -1,
		PLUS,
		NUMBER,
		TABLE,
		BLANK,
		CLICK,
        Size
	};

	struct Sample
	{
		int number;
		std::vector<int> numbers;
        int table_type;
	};

	TVisualSearchBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "Триалы", GuiType::TEdit, 8);
            Settings->Add(RangeNumber, "Число", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground, "Фон", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

        ButtonSkip = new TDrawButton(bitmap.get(), "Пропустить");
	}
    // Описание для протокола --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			ClickInfo Click;
			int number;
            int user_choice;
            std::vector<int> array;
		};
		std::vector<std::shared_ptr<Trial>> Trials;

		Trial* CreateTrial() {
			std::shared_ptr<Trial> trial = std::make_shared<Trial>();
            Trials.push_back(trial);
			return trial.get();
		}
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
		protocol->BlockName = GetTaskName();
		OwnProtocol = protocol.get();
		return protocol;
	}
	//--------------------------------------------------------------------------
    std::vector<int> gen_sequence(int size, int max_number)
	{
		std::vector<int> ret(size);
		for(int i = 0; i < size; i++)
			ret[i] = i % max_number;
		return ret;
	}
	//--------------------------------------------------------------------------
	void InitTask(AnsiString Path) override {
		randomize();
		std::srand(time(NULL));

		state = INSTRUCTION;
		TrialCount = 0;

		ButtonSkip->SetPosition(Form->Width - 170, Form->Height - 70);

		int trials_count = Settings->getInt(TrialMax)*2;

		std::vector<int> table_types = gen_sequence(trials_count,2);
		std::random_shuffle(table_types.begin(), table_types.end());

		_samples.clear();
		_samples.resize(trials_count);
		for(int i = 0; i < trials_count; i++)
		{
			 _samples[i] = GenerateSample(GetRandomNumber(), table_types[i]);
        }
	}
    //--------------------------------------------------------------------------
	Sample GenerateSample(int number, int table_type)
	{
		 Sample sample;
		 sample.number = number;
		 sample.table_type = table_type;
		 sample.numbers = GetNumberArray(number, table_type);
		 return sample;
    }
	//--------------------------------------------------------------------------
	bool Finished() override {
		if(Settings->get(Enable).ToInt() == 0) return true;
		if(TrialCount >= _samples.size())
		{
         InitTask("");
		 return true;
		}
		return false;
	}
    //--------------------------------------------------------------------------
	bool isEnable() override {
      return (bool) Settings->getInt(Enable);
	}
    void CloseTask() override {}
	//--------------------------------------------------------------------------
	int GetRandomNumber()
	{
		int number  = RandomRange(10,100);
		while(int(number/10) == int(number%10))
			number  = RandomRange(10,100);
		return number;
	}
	//--------------------------------------------------------------------------
	std::vector<int> GetNumberArray(int number, int table_type)
	{
		std::vector<int> array(25,0);
		for(int i = 0; i < array.size(); )
		{
			int n = (table_type == 0) ? GetNumberType1(number) : GetNumberType2(number);
			if(std::find(array.begin(), array.end(), n) == array.end() && n != number)
			{
				array[i] = n;
				i++;
			}
		}
		array[rand() % array.size()] = number;
		return array;
	}
	//--------------------------------------------------------------------------
	int GetNumberType1(int number)
	{
       int n1 = 0;
	   int digit[2] = {number / 10, number % 10};

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
	   int digit[2] = {number / 10, number % 10};

	   do
	   {
		n1 = RandomRange(10,100);
	   }
	   while(n1%10 == digit[0] || n1%10 == digit[1] || n1/10 == digit[0] || n1/10 == digit[1]);
	   return n1;
	}
	//--------------------------------------------------------------------------
	void UserMouseDown(int X, int Y, TMouseButton Button) override
	{
		if(state == CLICK)
		{
			if(old_state == INSTRUCTION)
			{
				state = PLUS;
				StateManager();
			}
			else
			{
				for(int i = 0; i < table_objects.size(); i++)
				{
				   if(table_objects[i].rect.Contains(System::Types::TPoint(X,Y)))
				   {
					  select_obj = table_objects[i];

					  Trial->user_choice = select_obj.text.ToInt();

					  state = BLANK;
					  HighlightObject(select_obj, TAlphaColors::Lightslategray);
					  if(_samples[TrialCount].number != select_obj.text.ToInt())
					  {
						 _samples.push_back(GenerateSample(GetRandomNumber(), _samples[TrialCount].table_type));
					  }

					  Timer->Interval = 400;
					  break;
                   }
				}

				if(ButtonSkip->Contains(X,Y))
				{
				 _samples.push_back(GenerateSample(GetRandomNumber(), _samples[TrialCount].table_type));
				 ButtonSkip->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonSkip->Draw();
				 state = BLANK;
				 Timer->Interval = 500;
				}

				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[CLICK] = millis();
					Trial->Click = {X, Y, millis(), 0};
				#endif
            }

		}
	}
    //--------------------------------------------------------------------------
	void StateManager() override
	{
        old_state = state;
		switch(state) {
			case INSTRUCTION:
                ClearCanva();
				DrawText("Инструкция, визуальный поиск", 80);
				state = CLICK;
                Timer->Interval = 0;
				#ifdef PROTOCOL_LOGGER
					Protocol->SetInstractionTime(millis());
				#endif
				break;
			case PLUS:
                ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = NUMBER;
                #ifdef PROTOCOL_LOGGER
					Trial = OwnProtocol->CreateTrial();
					Trial->StateTime[PLUS] = millis();
				#endif
				break;
			case NUMBER:
				ClearCanva();
				DrawOneNumber(_samples[TrialCount].number);
				Timer->Interval = Settings->getRandFromRange(RangeNumber);
				state = TABLE;
                #ifdef PROTOCOL_LOGGER
                    Trial->number = _samples[TrialCount].number;
					Trial->StateTime[NUMBER] = millis();
				#endif
				break;
			case TABLE:
				ClearCanva();
                ButtonSkip->Draw();
				table_objects = DrawTable(_samples[TrialCount].numbers.data(), _samples[TrialCount].numbers.size());
                //Timer->Interval = Settings->getRandFromRange(RangeTable);
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[TABLE] = millis();
                    Trial->array = _samples[TrialCount].numbers;
				#endif
				break;
			case BLANK:
				ClearCanva();
                ButtonSkip->SetFillColor(TAlphaColors::Black);
				Timer->Interval = Settings->getRandFromRange(RangeBackground);
				state = PLUS;
				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[BLANK] = millis();
				#endif
				break;
            case CLICK:
				break;
			default:
				break;
		}
	}
private:
	State state;
    State old_state;
	int TrialCount = 0;
	TDrawObject select_obj;
	std::vector<TDrawObject> table_objects;

    std::vector<Sample> _samples;

	DProtocol* OwnProtocol;
	DProtocol::Trial* Trial;

    TDrawButton* ButtonSkip;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TWorkingMemoryBlock : public TBaseTask
{
public:
	enum SettingsName : int
	{
		Enable,
		MaxTrialPerCmplexity,
		MaxSymbolCount,
		Complexity,
		RangeBackground,
		RangePlus,
		RangeBackground2,
		RangeStimul,
	};

	enum State : int
	{
        INSTRUCTION = -1,
		PLUS,
		NUMBERS,
		BLANK,
        PLUS2,
		SAMPLE,
		CLICK,
		BLANK2,
        Size
	};

	struct Sample
	{
	   std::vector<int> numbers;
	   int number;
	   int targeted; // проба
	   int complexity;
	};

	// Описание для протокола --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			ClickInfo Click;
			int ResponseTimeOut = 0;
			int Stimul = 0;
            int user_choice = -1;
            std::vector<int> varray;
		};
		std::vector<std::shared_ptr<Trial>> Trials;

		Trial* CreateTrial() {
			std::shared_ptr<Trial> trial = std::make_shared<Trial>();
            Trials.push_back(trial);
			return trial.get();
		}

	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
		protocol->BlockName = GetTaskName();
		OwnProtocol = protocol.get();
		return protocol;
	}
	//--------------------------------------------------------------------------

	TWorkingMemoryBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);

			Settings->Add(MaxTrialPerCmplexity, "Триалов на сложность и пробы", GuiType::TEdit, "1");
			Settings->Add(MaxSymbolCount, "Кол. символов", GuiType::TEdit, "7");
			Settings->Add(Complexity, "Уровни сложности", GuiType::TEdit, "2,3,4,5,6,7");

			Settings->Add(RangeBackground, "Фон 1", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground2, "Фон 2", GuiType::TRange, "1000:1500");
			Settings->Add(RangeStimul, "Стимул", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

        ButtonYes = new TDrawButton(bitmap.get(), "Да");
		ButtonNo = new TDrawButton(bitmap.get(), "Нет");
	}

	~TWorkingMemoryBlock()
	{
       delete ButtonYes;
	   delete ButtonNo;
	}
	//--------------------------------------------------------------------------
	std::vector<int> str_to_vecint(const char* str, std::string delim)
	{
		TStringDynArray data = SplitString(str,delim.c_str());
		std::vector<int> vec(data.size());
		for(int i = 0; i < data.size(); i++) vec[i] = data[i].ToInt();
		return vec;
	}
	//--------------------------------------------------------------------------
	std::vector<int> gen_sequence(int size, int max_number)
	{
		std::vector<int> ret(size);
		for(int i = 0; i < size; i++)
			ret[i] = i % max_number;
		return ret;
	}
	//--------------------------------------------------------------------------
	void InitTask(AnsiString Path) override {
        randomize();
		std::srand(time(NULL));

		state = INSTRUCTION;
		TrialCount = 0;

        ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
		ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);


		int trial_per_comp = Settings->getInt(MaxTrialPerCmplexity);
		int symbol_count = Settings->getInt(MaxSymbolCount);
		std::vector<int> complexity_list = str_to_vecint(AnsiString(Settings->get(Complexity)).c_str(), ",");

		int trial_count = trial_per_comp*complexity_list.size()*2;

		std::vector<int> targeted = gen_sequence(trial_count, 2);
		std::vector<int> complexity(trial_count);

		for(int i = 0; i < trial_count; i++) complexity[i] = complexity_list[i%complexity_list.size()];

		std::random_shuffle(targeted.begin(), targeted.end());
		std::random_shuffle(complexity.begin(), complexity.end());

		_samples.clear();
		_samples.resize(trial_count);
		for(int i = 0; i < _samples.size(); i++)
		{
			Sample sample;
			sample.targeted = targeted[i];
			sample.complexity = complexity[i];

			std::vector<int> numbers = get_unique_numbers(symbol_count, 10,99);
			set_complexity(numbers, complexity[i]);

			sample.numbers = numbers;
			sample.number = (targeted[i] == 0) ? get_ungoal_number(numbers) : get_goal_number(numbers);
			_samples[i] = sample;
		}
	}
    //--------------------------------------------------------------------------
	bool Finished() override
	{
		if(Settings->getInt(Enable) == 0)
			return true;

		if(TrialCount >= _samples.size())
		{
         InitTask("");
		 return true;
		}
		else
			return false;
	}
	//--------------------------------------------------------------------------
	bool isEnable() override
	{
      return (bool) Settings->getInt(Enable);
	}
	//--------------------------------------------------------------------------
	void CloseTask() override
	{

	}
	//--------------------------------------------------------------------------
	void set_complexity(std::vector<int>& array, int complexity)
	{
		std::vector<int> indexes = gen_sequence(array.size(), array.size());
		std::random_shuffle(indexes.begin(), indexes.end());

		for(int i = 0; i < array.size() - complexity; i++)
			array[indexes[i]] = 0;
	}
	//--------------------------------------------------------------------------
	std::vector<int> get_unique_numbers(int size, int begin, int end)
	{
		 std::vector<int> numbers(end-begin);
		 for(int i = 0; i < numbers.size(); i++) numbers[i] = begin+i;
		 std::random_shuffle(numbers.begin(), numbers.end());

		 std::vector<int> ret(size);
		 std::copy(numbers.begin(),numbers.begin()+size, ret.begin());
         return ret;
	}

	int get_goal_number(const std::vector<int>& array)
	{
		std::vector<int> numbers = array;
		numbers.erase(std::remove(numbers.begin(), numbers.end(), 0), numbers.end());
		int item =  std::rand() % numbers.size();
		return numbers[item];
	}

	int get_ungoal_number(const std::vector<int>& array)
	{
		 while(true)
		 {
			 int number = RandomRange(10,100);
			 if(std::find(array.begin(), array.end(), number) == array.end())
				return number;
         }
	}

	void StateManager() override
	{
        old_state = state;
		switch(state) {
			case INSTRUCTION:
                ClearCanva();
				DrawText("Инструкция, рабочая память", 80);
				state = CLICK;
                Timer->Interval = 0;
				#ifdef PROTOCOL_LOGGER
					Protocol->SetInstractionTime(millis());
				#endif
				break;
			case PLUS:

				ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = NUMBERS;
                #ifdef PROTOCOL_LOGGER
					Trial = OwnProtocol->CreateTrial();
					Trial->StateTime[PLUS] = millis();
				#endif
				break;
			case NUMBERS:
			{
				ClearCanva();
				std::vector<std::string> varray(_samples[TrialCount].numbers.size());
				for(int i = 0; i < varray.size(); i++) varray[i] = (_samples[TrialCount].numbers[i] == 0 ? "*" : std::to_string(_samples[TrialCount].numbers[i]));
				DrawSymArray(varray);
				Timer->Interval = Settings->getRandFromRange(RangeStimul);
				state = BLANK;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[NUMBERS] = millis();
					Trial->varray = _samples[TrialCount].numbers;
				#endif
				break;
            }
			case BLANK:
				ClearCanva();
				Timer->Interval  = Settings->getRandFromRange(RangeBackground);
				state = PLUS2;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[BLANK] = millis();
				#endif
				break;
			case PLUS2:
                ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = SAMPLE;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[PLUS2] = millis();
				#endif
				break;
			case SAMPLE:
			{
                ClearCanva();
				DrawOneNumber(_samples[TrialCount].number);

				ButtonYes->Draw();
				ButtonNo->Draw();

				Timer->Interval = 0;
				state = CLICK;
                #ifdef PROTOCOL_LOGGER
					Trial->Stimul = _samples[TrialCount].number;
					Trial->StateTime[SAMPLE] = millis();
				#endif
				break;
            }
			case BLANK2:
				ClearCanva();
				ButtonYes->SetFillColor(TAlphaColors::Black);
				ButtonNo->SetFillColor(TAlphaColors::Black);
				Timer->Interval = Settings->getRandFromRange(RangeBackground2);
				state = PLUS;
				TrialCount++;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[BLANK2] = millis();
				#endif
				break;
   			case CLICK:
				state = BLANK2;
				StateManager();
				#ifdef PROTOCOL_LOGGER
					Trial->ResponseTimeOut = millis();
                    Trial->StateTime[CLICK] = millis();
				#endif
				break;
			default:
                break;
		}
	}

	void UserMouseDown(int X, int Y, TMouseButton Button) override {
		 if(state == CLICK)
		 {
            if(old_state == INSTRUCTION){
				state = PLUS;
				StateManager();
			} else {

				if(ButtonYes->Contains(X,Y))
				{
				 ButtonYes->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonYes->Draw();
				 state = BLANK2;
				 Timer->Interval = 500;
                 Trial->user_choice = 1;
				}

				if(ButtonNo->Contains(X,Y))
				{
				 ButtonNo->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonNo->Draw();
				 state = BLANK2;
				 Timer->Interval = 500;
                 Trial->user_choice = 0;
				}

				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[CLICK] = millis();
					Trial->Click = {X, Y, millis(), 0};
				#endif

            }
		 }
	}
private:
	State state;
    State old_state;
    int TrialCount = 0;
    std::vector<Sample> _samples;

	DProtocol* OwnProtocol;
	DProtocol::Trial* Trial;

	TDrawButton* ButtonYes;
	TDrawButton* ButtonNo;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TMentalArithmeticBlock : public TBaseTask
{
public:
	enum SettingsName : int
	{
		Enable,
		TrialMax,
		Complexity,
		RangeBackground,
		RangePlus,
	};
	enum State
	{
		INSTRUCTION = -1,
		PLUS,
		EQUATION,
		BLANK,
		CLICK,
		Size
	};

	struct Sample
	{
		// X-N = R+-d
		int X;
		int N;
		int R;
    };

	// Описание для протокола --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			ClickInfo Click;
            int user_choice = -1;
			int X = 0, N = 0, R = 0;
		};
		std::vector<std::shared_ptr<Trial>> Trials;

		Trial* CreateTrial() {
			std::shared_ptr<Trial> trial = std::make_shared<Trial>();
            Trials.push_back(trial);
			return trial.get();
        }
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
		protocol->BlockName = GetTaskName();
		OwnProtocol = protocol.get();
		return protocol;
	}
	//--------------------------------------------------------------------------
	TMentalArithmeticBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "Триалов на сложность", GuiType::TEdit, 8);
            Settings->Add(Complexity, "N: (X-N=+-d)", GuiType::TEdit, "1,2,3");
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground, "Фон", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

        ButtonYes = new TDrawButton(bitmap.get(), "Да");
		ButtonNo = new TDrawButton(bitmap.get(), "Нет");
	}

	~TMentalArithmeticBlock()
	{
       delete ButtonYes;
	   delete ButtonNo;
	}
	//--------------------------------------------------------------------------
	std::vector<int> str_to_vecint(const char* str, std::string delim)
	{
		TStringDynArray data = SplitString(str,delim.c_str());
		std::vector<int> vec(data.size());
		for(int i = 0; i < data.size(); i++) vec[i] = data[i].ToInt();
		return vec;
	}
	//--------------------------------------------------------------------------
	std::vector<int> gen_sequence(int size, int max_number)
	{
		std::vector<int> ret(size);
		for(int i = 0; i < size; i++)
			ret[i] = i % max_number;
		return ret;
	}
	//--------------------------------------------------------------------------
	void InitTask(AnsiString Path) override {
        randomize();
		std::srand(time(NULL));

		state = INSTRUCTION;
		TrialCount = 0;

        ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
		ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);

		std::vector<int> complexity_list = str_to_vecint(AnsiString(Settings->get(Complexity)).c_str(), ",");

		int trial_count = Settings->getInt(TrialMax)*complexity_list.size()*2;

		std::vector<int> targeted = gen_sequence(trial_count, 2);
		std::vector<int> complexity(trial_count);

		for(int i = 0; i < trial_count; i++) complexity[i] = complexity_list[i%complexity_list.size()];

		std::random_shuffle(targeted.begin(), targeted.end());
		std::random_shuffle(complexity.begin(), complexity.end());

		_samples.resize(trial_count);
		for(int i = 0; i < _samples.size(); i++)
		{
			// Generate Equation
			int X = RandomRange(10,100);
			int N = complexity[i];
			int R = X-N;
			if(targeted[i] == 0) // не целевая проба
			{
				while(true)
				{
					int d = RandomRange(1,3);
					int sign = RandomRange(0,2);
					int ans = (sign == 0) ? (R - d) : (R + d);
					if(X>ans)
					{
                        R = ans;
						break;
					}
				}
			}

			Sample sample;
			sample.N = N;
			sample.R = R;
			sample.X = X;
			_samples[i] = sample;
        }
	}
	//--------------------------------------------------------------------------
	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= _samples.size())
		{
         InitTask("");
		 return true;
		}
		else return false;
	}
	//-------------------------------------------------------------------------
	bool isEnable() override
	{
      return (bool) Settings->getInt(Enable);
	}

	void CloseTask() override {}
	//--------------------------------------------------------------------------

	UnicodeString GetEquation(Sample& sample)
	{
		Trial->X = sample.X; Trial->R = sample.R; Trial->N = sample.N;
		return IntToStr(sample.X)+"-"+IntToStr(sample.N)+"="+IntToStr(sample.R);
	}

	void StateManager() override
	{
        old_state = state;
		switch(state) {
			case INSTRUCTION:
				ClearCanva();
				DrawText("Инструкция, уравнение", 80);
				state = CLICK;
                Timer->Interval = 0;
				#ifdef PROTOCOL_LOGGER
					Protocol->SetInstractionTime(millis());
				#endif
				break;
			case PLUS:
				ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = EQUATION;
				#ifdef PROTOCOL_LOGGER
					Trial = OwnProtocol->CreateTrial();
					Trial->StateTime[PLUS] = millis();
				#endif
				break;
			case EQUATION:
				ClearCanva();
				DrawText(GetEquation(_samples[TrialCount]),220);
				ButtonYes->Draw();
				ButtonNo->Draw();
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[EQUATION] = millis();
				#endif
				break;
		   case BLANK:
				ClearCanva();
				ButtonYes->SetFillColor(TAlphaColors::Black);
				ButtonNo->SetFillColor(TAlphaColors::Black);

				Timer->Interval = Settings->getRandFromRange(RangeBackground);
                state = PLUS;
				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[BLANK] = millis();
				#endif
				break;
			default:
				break;
		}
	}

	void UserMouseDown(int X, int Y, TMouseButton Button) override {
		 if(state == CLICK)
		 {
			if(old_state == INSTRUCTION){
				state = PLUS;
                StateManager();
			} else {

				if(ButtonYes->Contains(X,Y))
				{
				 ButtonYes->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonYes->Draw();
				 state = BLANK;
				 Timer->Interval = 500;
                 Trial->user_choice = 1;
				}

				if(ButtonNo->Contains(X,Y))
				{
				 ButtonNo->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonNo->Draw();
				 state = BLANK;
				 Timer->Interval = 500;
                 Trial->user_choice = 0;
				}

				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[CLICK] = millis();
					Trial->Click = {X, Y, millis(), 0};
				#endif

            }
		 }
	}

private:
	State state;
    State old_state;
	int TrialCount = 0;
	std::vector<int> complexity;
	std::vector<int> samples_queue;
	DProtocol* OwnProtocol;
	DProtocol::Trial* Trial;

    std::vector<Sample> _samples;

	TDrawButton* ButtonYes;
	TDrawButton* ButtonNo;
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
		INSTRUCTION = -1,
		PLUS,
		MATRIX,
		CLICK,
        Size
	};
    // Описание для протокола
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
            ClickInfo Click;
			std::vector<int> varray;
		};
		std::vector<std::shared_ptr<Trial>> Trials;

		Trial* CreateTrial() {
			std::shared_ptr<Trial> trial = std::make_shared<Trial>();
            Trials.push_back(trial);
			return trial.get();
        }

        unsigned int PLUS_TIME = 0;
		int number = 0;
		int complexity = 0;
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
		protocol->BlockName = GetTaskName();
		OwnProtocol = protocol.get();
		return protocol;
    }
	//--------------------------------------------------------------------------
	TFunctionCombination(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "Включить", GuiType::TCheckBox , 1);
			Settings->Add(RangePlus, "Крест", GuiType::TRange, "1000:1500");
			Settings->Add(RangeAnswerWait, "Ожидание реакции", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

		hint = new TDrawHint(bitmap.get());
	}
	//--------------------------------------------------------------------------
	~TFunctionCombination()
	{
       delete hint;
    }
    //--------------------------------------------------------------------------
	void InitTask(AnsiString Path) override {
        randomize();
		std::srand(time(NULL));

		state = INSTRUCTION;
		TrialCount = 0;
		complexity_index = 0;
        complexity.clear();
		std::vector<int> _complexity{1, 2, 3};

		for(int i = 0; i < 3; i++) {
			std::random_shuffle(begin(_complexity), end(_complexity));
			complexity.insert(end(complexity), begin(_complexity), end(_complexity));
		}

		//int(Form->Width) / 5, (int(Form->Height) / 4) * 3
		hint->SetPosition(Form->Width - 200,Form->Height - 100);
	}
	//--------------------------------------------------------------------------
	bool Finished() override
	{
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= 25 && state != CLICK)
		{
		 TrialCount = 0;
         complexity_index++;
		 state = INSTRUCTION;
         hint->Hide();
		 return true;
		}
		else return false;
	}
	//--------------------------------------------------------------------------
	bool isEnable() override
	{
      return (bool) Settings->getInt(Enable);
	}
    //--------------------------------------------------------------------------
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
    //--------------------------------------------------------------------------
	void StateManager() override
	{
		switch(state) {
			case INSTRUCTION:
				ClearCanva();
				get_array();
				complN = complexity[complexity_index];
				TargetNumber = Number;
				DrawText("Максимальное число: "+IntToStr(Number)+", Число N: "+IntToStr(complN), 80);
				Timer->Interval = 0;
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					OwnProtocol->number = Number;
					OwnProtocol->complexity = complexity[complexity_index];
					Protocol->SetInstractionTime(millis());
				#endif
				break;
			case PLUS:
                ClearCanva();
				DrawPlus();
				Timer->Interval = Settings->getRandFromRange(RangePlus);
				state = MATRIX;
				#ifdef PROTOCOL_LOGGER
					OwnProtocol->PLUS_TIME = millis();
				#endif
				break;
			case MATRIX:
				ClearCanva();
				hint->Draw();
				select_obj.isSelected = false;
				std::random_shuffle(begin(array), end(array));
				table_objects = DrawTable(array.data(),array.size());
				Timer->Interval = 0;
				state = CLICK;

				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial = OwnProtocol->CreateTrial();
					Trial->varray = array;
					Trial->StateTime[MATRIX] = millis();
				#endif
				break;
			case CLICK:
			{
                if(select_obj.isSelected)
				{
                    select_obj.isSelected = false;
					HighlightObject(select_obj, TAlphaColors::Black);

					ClearCanva();
					std::random_shuffle(begin(array), end(array));
					table_objects = DrawTable(array.data(),array.size());
					hint->Draw();
                    Timer->Interval = 0;
				}
				if(hint->Visible())
				{
                    ClearCanva();
					std::random_shuffle(begin(array), end(array));
					table_objects = DrawTable(array.data(),array.size());
					hint->Draw();
					Timer->Interval = 0;
                }
				break;
			}
			default:
				break;
		}
	}
    //--------------------------------------------------------------------------
	void UserMouseDown(int X, int Y, TMouseButton Button) override {
		 if(state == CLICK)
		 {
			if(TrialCount == 0) {
			 state = PLUS;
			 if(Parent) Parent->StateManager();
			}
			else {

				if(select_obj.isSelected)
				{
                    select_obj.isSelected = false;
					HighlightObject(select_obj, TAlphaColors::Black);
				}

				for(int i = 0; i < table_objects.size(); i++)
				{
				   if(table_objects[i].rect.Contains(System::Types::TPoint(X,Y)))
				   {
					  select_obj = table_objects[i];
                      Trial->Click.numbers = select_obj.text.ToInt();
					  if(TargetNumber == select_obj.text.ToInt())
					  {
						state = MATRIX;
						HighlightObject(select_obj, TAlphaColors::Lightgreen);
						TargetNumber -= complN;
						hint->Hide();
						Timer->Interval = 500;
					  }
					  else
					  {
						hint->Show("Ищем "+IntToStr(TargetNumber)+"\n Отнимаем "+IntToStr(complN));
						select_obj.isSelected = true;
						HighlightObject(select_obj, TAlphaColors::Lightcoral);
						Timer->Interval = 800;
					  }
					  break;
				   }
				}

				if(hint->Contains(X,Y))
				{
					hint->Show("Ищем "+IntToStr(TargetNumber)+"\n Отнимаем "+IntToStr(complN));
                    Timer->Interval = 800;
                }

				Trial->StateTime[CLICK] = millis();
				Trial->Click.X = X;
				Trial->Click.Y = Y;
				Trial->Click.time = millis();

            }
		 }
	}
private:
	State state;
	int TrialCount = 0;
	int complexity_index = 0;
	std::vector<int> complexity;
	int Number = 0;
	int complN = 0;
    int TargetNumber = 0;
	std::vector<int> array;
	DProtocol::Trial* Trial;
	DProtocol *OwnProtocol;

    TDrawObject select_obj;
	std::vector<TDrawObject> table_objects;

    TDrawHint* hint;
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
		 BACKGROUND,
		 RUN_BLOCK,
		 PAUSE,
		 Size,
		 END,
         FINISHED
	} state;

	// Описание для протокола
	struct DProtocol : ProtocolBase
	{
		unsigned int BACKGROUND_TIME = 0;
        unsigned int PAUSE_TIME = 0;
	};

	std::shared_ptr<ProtocolBase> CreateProtocol() override
	{
		std::shared_ptr<DProtocol> protocol = std::make_shared<DProtocol>();
        protocol->BlockName = GetTaskName();
        OwnProtocol = protocol.get();
		return protocol;
    }

	TElementaryCognitiveFunctions(AnsiString _name);
	void InitTask(AnsiString Path) override;
	void StateManager() override;
	bool Finished() override;
	void UserMouseDown(int X, int Y, TMouseButton Button) override;
	void Draw() override;
	void CloseTask() override;
private:
	int cur_block;
	int PartCount = 0;
    std::vector<std::vector<int>> BlockSequence;
	DProtocol* OwnProtocol;
};

#endif
