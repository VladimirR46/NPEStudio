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
		bitmap->Canvas->Font->Size = 55;
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
	int sizeX = 120, sizeY = 60;
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

	TVisualSearchBlock(TBaseTask* _parent, AnsiString _name) : TBaseTask(_parent, _name)
	{
		if(!Settings->Load(_parent->GetTaskName())){
			Settings->Add(Enable, "��������", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "������", GuiType::TEdit, 8);
            Settings->Add(RangeNumber, "�����", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground, "���", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "�����", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}
	}
    // �������� ��� ��������� --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			ClickInfo Click;
			int number;
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
	void InitTask(AnsiString Path) override {
        state = INSTRUCTION;
		TrialCount = 0;
	}
    //--------------------------------------------------------------------------
	bool Finished() override {
		if(Settings->get(Enable).ToInt() == 0) return true;
		if(TrialCount >= Settings->getInt(TrialMax)*2){
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
	void GetRandomNumber()
	{
		int number  = RandomRange(10,100);
		while(int(number/10) == int(number%10))
			number  = RandomRange(10,100);
		CurrentNumber = number;
		Trial->number = CurrentNumber;
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
	void UserMouseDown(int X, int Y, TMouseButton Button) override
	{
		if(state == CLICK)
		{
			if(old_state == INSTRUCTION){
				state = PLUS;
				StateManager();
			} else {

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
                      break;
                   }
				}

				if(CurrentNumber == select_obj.text.ToInt())
				{
				   state = BLANK;
				   HighlightObject(select_obj, TAlphaColors::Lightgreen);
				   Timer->Interval = 500;
				}
				else
				{
				   select_obj.isSelected = true;
				   HighlightObject(select_obj, TAlphaColors::Lightcoral);
                   Timer->Interval = 400;
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
				DrawText("����������, ���������� �����", 80);
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
				GetRandomNumber();
				DrawOneNumber(CurrentNumber);
				Timer->Interval = Settings->getRandFromRange(RangeNumber);
				state = TABLE;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[NUMBER] = millis();
				#endif
				break;
			case TABLE:
				int numbers[25];
				GetNumberArray(CurrentNumber, numbers, 25, GetTableType());
				ClearCanva();
				table_objects = DrawTable(numbers, 25);
                //Timer->Interval = Settings->getRandFromRange(RangeTable);
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[TABLE] = millis();
                    Trial->array.assign(numbers,numbers+25);
				#endif
				break;
			case BLANK:
				ClearCanva();
				Timer->Interval = Settings->getRandFromRange(RangeBackground);
				state = PLUS;
				TrialCount++;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[BLANK] = millis();
				#endif
				break;
            case CLICK:
				if(select_obj.isSelected)
				{
                    select_obj.isSelected = false;
					HighlightObject(select_obj, TAlphaColors::Black);
                    Timer->Interval = 0;
				}
				break;
			default:
				break;
		}
	}
private:
	State state;
    State old_state;
	int TrialCount = 0;
	int type_count[2];
	int CurrentNumber;
	TDrawObject select_obj;
	std::vector<TDrawObject> table_objects;

	DProtocol* OwnProtocol;
    DProtocol::Trial* Trial;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TWorkingMemoryBlock : public TBaseTask
{
public:
	enum SettingsName : int
	{
		Enable = 0,
		TrialMax = 1,
		RangeBackground = 2,
		RangePlus = 3,
		RangeBackground2 = 4,
		RangeStimul = 5,
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

	// �������� ��� ��������� --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
			ClickInfo Click;
			int ResponseTimeOut = 0;
			int Stimul = 0;
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
			Settings->Add(Enable, "��������", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "������", GuiType::TEdit, 8);
			Settings->Add(RangeBackground, "��� 1", GuiType::TRange, "1000:1500");
			Settings->Add(RangePlus, "�����", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground2, "��� 2", GuiType::TRange, "1000:1500");
			Settings->Add(RangeStimul, "������", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

        ButtonYes = new TDrawButton(bitmap.get(), "��");
		ButtonNo = new TDrawButton(bitmap.get(), "���");
	}

	~TWorkingMemoryBlock()
	{
       delete ButtonYes;
	   delete ButtonNo;
	}

	void InitTask(AnsiString Path) override {
		state = INSTRUCTION;
		TrialCount = 0;
		memset(array,0, sizeof(int)*7);
		memset(complexity_count,0, sizeof(int)*3);
		memset(samples_count,0, sizeof(int)*2);

        ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
		ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);
	}

	bool Finished() override {
		if(Settings->getInt(Enable) == 0)
			return true;

		if(TrialCount >= Settings->getInt(TrialMax)*3)
		{
         InitTask("");
		 return true;
		}
		else
			return false;
	}

	bool isEnable() override {
      return (bool) Settings->getInt(Enable);
	}

	void CloseTask() override
	{

	}

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
        Trial->Stimul = n1;
		return n1;
	}

	void StateManager() override
	{
        old_state = state;
		switch(state) {
			case INSTRUCTION:
                ClearCanva();
				DrawText("����������, ������� ������", 80);
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
				ClearCanva();
                memset(array,0, sizeof(int)*7);
				generate_numbers(array);
				DrawSymbols(array,124);
				Timer->Interval = Settings->getRandFromRange(RangeStimul);
				state = BLANK;
                #ifdef PROTOCOL_LOGGER
					Trial->StateTime[NUMBERS] = millis();
					for(int i = 0; i < 7; i++) {
						Trial->varray.push_back(array[i]);
                    }
				#endif
				break;
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
					Trial = OwnProtocol->CreateTrial();
					Trial->StateTime[PLUS2] = millis();
				#endif
				break;
			case SAMPLE:
			{
                ClearCanva();
				DrawOneNumber(get_sample());

				ButtonYes->Draw();
				ButtonNo->Draw();

				Timer->Interval = 0;
				state = CLICK;
                #ifdef PROTOCOL_LOGGER
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
				}

				if(ButtonNo->Contains(X,Y))
				{
				 ButtonNo->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonNo->Draw();
				 state = BLANK2;
				 Timer->Interval = 500;
				}

				/*
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[CLICK] = millis();
					Trial->Click = {X, Y, millis(), 0};
				#endif
				*/
            }
		 }
	}
private:
	State state;
    State old_state;
    int TrialCount = 0;
	int array[7] = {0};
	int complexity_count[3] = {0};
	int samples_count[2] = {0};

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
		Enable = 0,
		TrialMax = 1,
		RangeBackground = 2,
		RangePlus = 3,
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

	// �������� ��� ��������� --------------------------------------------------
	struct DProtocol : ProtocolBase
	{
		struct Trial
		{
			unsigned int StateTime[State::Size] = {0};
            ClickInfo Click;
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
			Settings->Add(Enable, "��������", GuiType::TCheckBox , 1);
			Settings->Add(TrialMax, "������", GuiType::TEdit, 8);
			Settings->Add(RangePlus, "�����", GuiType::TRange, "1000:1500");
			Settings->Add(RangeBackground, "���", GuiType::TRange, "1000:1500");
			Settings->Save(_parent->GetTaskName());
		}

        ButtonYes = new TDrawButton(bitmap.get(), "��");
		ButtonNo = new TDrawButton(bitmap.get(), "���");
	}

	~TMentalArithmeticBlock()
	{
       delete ButtonYes;
	   delete ButtonNo;
	}
	//--------------------------------------------------------------------------
	void InitTask(AnsiString Path) override {
		state = INSTRUCTION;
		TrialCount = 0;
		calc_complexity();
		calc_samples_queue();

        ButtonYes->SetPosition(int(Form->Width) / 5, (int(Form->Height) / 4) * 3);
		ButtonNo->SetPosition((int(Form->Width) / 5)*4,(int(Form->Height) / 4) * 3);
	}
	//--------------------------------------------------------------------------
	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= Settings->getInt(TrialMax)*3){
         InitTask("");
		 return true;
		}
		else return false;
	}
	//-------------------------------------------------------------------------
	bool isEnable() override {
      return (bool) Settings->getInt(Enable);
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
	//--------------------------------------------------------------------------
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
        old_state = state;
		switch(state) {
			case INSTRUCTION:
				ClearCanva();
				DrawText("����������, ���������", 80);
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
				DrawText(GetEquation(),220);
				ButtonYes->Draw();
				ButtonNo->Draw();
				state = CLICK;
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[EQUATION] = millis();
				#endif
				break;
		   case BLANK:
				ClearCanva();
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
			} else {

				if(ButtonYes->Contains(X,Y))
				{
				 ButtonYes->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonYes->Draw();
				 state = BLANK;
				 Timer->Interval = 500;
				}

				if(ButtonNo->Contains(X,Y))
				{
				 ButtonNo->SetFillColor(TAlphaColors::Lightslategray);
				 ButtonNo->Draw();
				 state = BLANK;
				 Timer->Interval = 500;
				}
				/*
				#ifdef PROTOCOL_LOGGER
					Trial->StateTime[CLICK] = millis();
					Trial->Click = {X, Y, millis(), 0};
				#endif
                */
            }

			StateManager();
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
    // �������� ��� ���������
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
			Settings->Add(Enable, "��������", GuiType::TCheckBox , 1);
			Settings->Add(RangePlus, "�����", GuiType::TRange, "1000:1500");
			Settings->Add(RangeAnswerWait, "�������� �������", GuiType::TRange, "1000:1500");
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
		state = INSTRUCTION;
		TrialCount = 0;
		complexity_index = 0;
        complexity.clear();
		std::vector<int> _complexity{1, 2, 3};
		srand(time(NULL));
		for(int i = 0; i < 3; i++) {
			std::random_shuffle(begin(_complexity), end(_complexity));
			complexity.insert(end(complexity), begin(_complexity), end(_complexity));
		}

		//int(Form->Width) / 5, (int(Form->Height) / 4) * 3
		hint->SetPosition(Form->Width - 200,Form->Height - 100);
	}
	//--------------------------------------------------------------------------
	bool Finished() override {
		if(Settings->getInt(Enable) == 0) return true;
		if(TrialCount >= 25) {
		 TrialCount = 0;
         complexity_index++;
         state = INSTRUCTION;
		 return true;
		}
		else return false;
	}
	//--------------------------------------------------------------------------
	bool isEnable() override {
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
				DrawText("������������ �����: "+IntToStr(Number)+", ����� N: "+IntToStr(complN), 80);
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
				srand(time(NULL));
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
                if(select_obj.isSelected)
				{
                    select_obj.isSelected = false;
					HighlightObject(select_obj, TAlphaColors::Black);
                    Timer->Interval = 0;
				}
				break;
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
						select_obj.isSelected = true;
						HighlightObject(select_obj, TAlphaColors::Lightcoral);
						Timer->Interval = 400;
					  }
					  break;
				   }
				}

				if(hint->Contains(X,Y))
				{
					hint->Show("���� "+IntToStr(TargetNumber)+"\n �������� "+IntToStr(complN));
                }

				//Trial->StateTime[CLICK] = millis();
				//Trial->Click = {X, Y, millis(), 0};

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
///////////////////////////////������������ ����������� �������////////////////

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
         Size
	} state;

	// �������� ��� ���������
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
