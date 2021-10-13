//---------------------------------------------------------------------------

#ifndef SettingsBaseH
#define SettingsBaseH
//---------------------------------------------------------------------------
#include <fmx.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

using namespace std;


enum class GuiType : int {
	TEdit,
	TComboColorBox,
    TSwitch
};

struct TParameter {
public:
	TParameter();

	void init(std::string _name, GuiType _gtype);

	template <typename T>
	void set(T i)
	{
		stringstream s;
		s << i;
		value = s.str();
	}

	template <typename T>
	T get()
	{
		T ret;
		std::istringstream iss(value);
		iss >> ret;

		if (iss.fail())
		{
			return T();
		}

		return ret;
	}

	void odata(ofstream& stream);

	void idata(ifstream& stream);

	std::string name;
	std::string value;
    GuiType gtype;
};

typedef std::map<int, TParameter> parameters_list;

class SettingsBase {
public:
	template <typename T>
	void Add(int i, std::string name, GuiType _gtype, T v)
	{
		TParameter parameter;
		parameter.init(name, _gtype);
		parameter.set(v);
		list[i] = parameter;
	}

	void save(AnsiString fileName);

	void load(AnsiString fileName);

	template <typename T>
	T get(int i)
	{
		return list[i].get<T>();
	}

	parameters_list& GetList();

private:
    parameters_list list;
};



#endif
