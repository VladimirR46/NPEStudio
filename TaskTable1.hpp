#ifndef TASKTABLE1
#define TASKTABLE1

#include "TaskBase.hpp"

class Task1 : public TaskBase
{
public:

	Task1(TForm* _form, TTimer* _timer, TBitmap* _bitmap, AnsiString _name) : TaskBase(_form, _timer, _bitmap, _name)
	{
	}

	enum Steps
	{
		PLUS = 0,
		NUMBER,
		TABLE,
		BLANK
	};

	//--------------------------------------------------------------------------
	void GetRandomNumber()
	{
		int number  = RandomRange(10,99);

		while(int(number/10) == int(number%10))
			number  = RandomRange(10,99);

		CurrentNumber = number;
	}
	//--------------------------------------------------------------------------
	bool Finished()
	{
		if(type_count[0] == TrialMax && type_count[1] == TrialMax) return true;
		return false;
	}
	//--------------------------------------------------------------------------
	void UserClick()
	{

	}
	//--------------------------------------------------------------------------
	void UserMouseMove(int X, int Y)
	{

	}
	//--------------------------------------------------------------------------
	void UserTouch(const TTouches Touches)
	{

	}
    //--------------------------------------------------------------------------
	void UserMouseDown(int X, int Y)
	{
       if(steps == TABLE)
	   {
		   steps = BLANK;

		   ClearCanva();

		   int interval  = RandomRange(1000,2000);
		   Timer->Interval = interval;
		   Timer->Enabled = true;
	   }
	}
	void UserMouseUp(int X, int Y)
	{

    }
	//--------------------------------------------------------------------------
	int GetTableType()
	{
		int TableType = RandomRange(0,2);

		if(type_count[TableType] < TrialMax)
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

	void InitTask(AnsiString Path)
	{
		steps = BLANK;

		type_count[0] = 0;
		type_count[1] = 0;
        TrialMax = 8;

		int interval  = RandomRange(1500,2000);
		Timer->Interval = interval;
		Timer->Enabled = true;
	}
	//--------------------------------------------------------------------------
	void StateManager()
	{
		if(steps == BLANK)
		{
		   steps = PLUS;
		   ClearCanva();
		   DrawPlus();

		   int interval  = RandomRange(1500,2000);
		   Timer->Interval = interval;
		}
		else if(steps == PLUS)
		{
		   steps = NUMBER;
		   ClearCanva();
		   GetRandomNumber();
           DrawOneNumber(CurrentNumber);

		   int interval  = RandomRange(1500,2000);
		   Timer->Interval = interval;
		}
		else if(steps == NUMBER)
		{
		   steps = TABLE;

		   int numbers[25];
		   GetNumberArray(CurrentNumber, numbers, 25, GetTableType());
           ClearCanva();
		   DrawTable(numbers, 25);
		   Timer->Enabled = false;
		}
		else if(steps == TABLE)
		{
		   steps = BLANK;
		   ClearCanva();

		   int interval  = RandomRange(1000,2000);
		   Timer->Interval = interval;
		}
	}
	//--------------------------------------------------------------------------
	void CloseTask()
	{

	}
    //--------------------------------------------------------------------------

private:
	Steps steps;
	int CurrentNumber;

	int type_count[2];
	int TrialMax;
};

#endif
