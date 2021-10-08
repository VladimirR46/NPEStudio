#ifndef SETTINGSBASE
#define SETTINGSBASE

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
	TParameter()
    {
	}

	void init(std::string _name, GuiType _gtype)
	{
		name = _name;
		gtype = _gtype;
    }

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

    void odata(ofstream& stream)
    {
		size_t len = name.length();
        stream.write((char*)&len, sizeof(size_t));
		stream.write(name.data(), len);
		len = value.length();
        stream.write((char*)&len, sizeof(size_t));
		stream.write(value.data(), len);

        stream.write((char*)&gtype, sizeof(int));
    }

    void idata(ifstream& stream)
    {
        size_t len = 0;

        char buf[255];
        stream.read((char*)&len, sizeof(size_t));
        stream.read((char*)&buf, len);
        buf[len] = 0;
        name = buf;

        stream.read((char*)&len, sizeof(size_t));
        stream.read((char*)&buf, len);
        buf[len] = 0;
        value = buf;

        stream.read((char*)&gtype, sizeof(int));
    }

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

	void save(AnsiString fileName)
    {
        ofstream fs;
        fs.open(fileName.c_str(), std::fstream::out | std::fstream::binary);

        size_t size = list.size();
        fs.write((char*)&size, sizeof(size_t));

        parameters_list::iterator it;
        for (it = list.begin(); it != list.end(); it++) {
            fs.write((char*)&it->first, sizeof(int));
            it->second.odata(fs);
        }

        fs.close();
	}

    void load(AnsiString fileName)
    {
        ifstream fs;
        fs.open(fileName.c_str(), std::fstream::in | std::fstream::binary);

        list.clear();

        size_t size;
        fs.read((char*)&size, sizeof(size_t));

        for (int i = 0; i < size; i++) {
            int id = 0;
            fs.read((char*)&id, sizeof(int));

            TParameter parameter;
            parameter.idata(fs);
            list[id] = parameter;
        }

        fs.close();
	}

	template <typename T>
	T get(int i)
	{
        return list[i].get<T>();
    }


    parameters_list& GetList()
    {
        return list;
    }

private:
    parameters_list list;
};

#endif

