//---------------------------------------------------------------------------

#pragma hdrstop

#include "SettingsBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TParameter::TParameter()
{
}

void TParameter::init(std::string _name, GuiType _gtype)
{
	name = _name;
	gtype = _gtype;
}

void TParameter::odata(ofstream& stream)
{
	size_t len = name.length();
	stream.write((char*)&len, sizeof(size_t));
	stream.write(name.data(), len);
	len = value.length();
	stream.write((char*)&len, sizeof(size_t));
	stream.write(value.data(), len);

	stream.write((char*)&gtype, sizeof(int));
}

void TParameter::idata(ifstream& stream)
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

typedef std::map<int, TParameter> parameters_list;
//----------------------------------------------------------------------------

void SettingsBase::save(AnsiString fileName)
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

void SettingsBase::load(AnsiString fileName)
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

parameters_list& SettingsBase::GetList()
{
	return list;
}
