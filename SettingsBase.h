//---------------------------------------------------------------------------

#ifndef SettingsBaseH
#define SettingsBaseH
//---------------------------------------------------------------------------
#include <fmx.h>
#include <cstdio>
#include <cstdlib>
//#include <fstream>
//#include <iostream>
#include <string>
//#include <sstream>
#include <System.JSON.hpp>
#include <map>

#define SettingsDir "Settings\\"

using namespace std;


enum class GuiType : int {
	TEdit,
	TComboColorBox,
	TSwitch,
	TCheckBox,
    TRange
};

struct TParameter
{
	UnicodeString desc;
	GuiType type;
	UnicodeString value;
};

typedef struct range {int min; int max; } range_t;
typedef std::map<int, TParameter> parameter_list;

class SettingsBase {
public:
	SettingsBase(UnicodeString TaskName);

    template <typename T>
	void Add(int ParameterName, UnicodeString description, GuiType _gtype, T v)
	{
		parameters[ParameterName] = {description, _gtype , UnicodeString(v)};
	}

	UnicodeString get(int ParameterName){
		 return parameters[ParameterName].value;
	}
	int getInt(int ParameterName){
		return parameters[ParameterName].value.ToInt();
	}
	double getDouble(int ParameterName){
		return parameters[ParameterName].value.ToDouble();
	}

	range_t getRange(int ParameterName) {
       range_t rge;
	   TStringDynArray data = SplitString(parameters[ParameterName].value,":");
	   rge.min = data[0].ToInt();
	   rge.max = data[1].ToInt();
	   return rge;
	}

	int getRandFromRange(int ParameterName)
	{
		range_t rge;
		rge = getRange(ParameterName);
		return RandomRange(rge.min, rge.max);
    }

	bool Save(UnicodeString FileName);
	bool Load(UnicodeString FileName);

	parameter_list& GetParametersList()
	{
		return parameters;
    }

    ~SettingsBase();
private:
	UnicodeString TaskName;
	parameter_list parameters;

};



#endif
