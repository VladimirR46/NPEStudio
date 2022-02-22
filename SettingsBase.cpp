//---------------------------------------------------------------------------

#pragma hdrstop

#include "SettingsBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

SettingsBase::SettingsBase(UnicodeString _TaskName) : TaskName(_TaskName)
{
	SettingsDir = "Settings\\";
	CreateDir(SettingsDir);
}

bool SettingsBase::Load(UnicodeString FileName)
{
	bool ret = false;
	parameters.clear();

	if(!FileExists(SettingsDir+FileName+".json")) return ret;

	TStringList *jsonFile = new TStringList;
	jsonFile->LoadFromFile(SettingsDir+FileName+".json", TEncoding::Unicode);

	TJSONObject *o = (TJSONObject*) TJSONObject::ParseJSONValue(jsonFile->Text);

	__try
	{
		if(o->Get(TaskName))
		{
			TJSONArray *a = (TJSONArray*) o->Get(TaskName)->JsonValue;

			for (int idx = 0; idx < a->Size(); idx++)
			{
				 parameters[a->Items[idx]->GetValue<int>("name")] = {a->Items[idx]->GetValue<UnicodeString>("description"),
																			   (GuiType)a->Items[idx]->GetValue<int>("type"),
																			   a->Items[idx]->GetValue<UnicodeString>("value")};
			}
			ret = true;
		}
	}
	__finally {
		o->Free();
		jsonFile->Free();
	}
	return ret;
}

bool SettingsBase::Save(UnicodeString FileName)
{
	TStringList *jsonFile = new TStringList;
	TJSONObject *o;

	if(FileExists(SettingsDir+FileName+".json"))
	{
		jsonFile->LoadFromFile(SettingsDir+FileName+".json", TEncoding::Unicode);
		o = (TJSONObject*) TJSONObject::ParseJSONValue(jsonFile->Text);
	} else {
		o = new TJSONObject();
	}

	__try {

        if(o->Get(TaskName)) o->RemovePair(TaskName); //Get(TaskName)->Destroy();

		TJSONArray *a = new TJSONArray();
		o->AddPair(TaskName,a);
		for(auto it = parameters.begin(); it != parameters.end(); it++)
		{
			TJSONObject *param = new TJSONObject();
			param->AddPair( new TJSONPair("name",it->first) );
			param->AddPair( new TJSONPair("description",it->second.desc) );
			param->AddPair( new TJSONPair("type", (int)it->second.type) );
			param->AddPair( new TJSONPair("value",it->second.value) );
			a->AddElement(param);
		}

	}
	__finally
	{
	  jsonFile->Text = o->ToString();
	  o->Free();
	}

	jsonFile->SaveToFile(SettingsDir+FileName+".json", TEncoding::Unicode);
	jsonFile->Free();

	return true;
}

SettingsBase::~SettingsBase()
{

}


