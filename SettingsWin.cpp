//---------------------------------------------------------------------------
#include <fmx.h>
#pragma hdrstop
#include "SettingsWin.h"
#include "DrawWin.h"
#include "MainWin.h"
//#include "SpiralTask.h"
#include <fstream>
#include "HotKeyName.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm3* Form3;
TFmxObject *object[50];
SettingsBase *CurSettings = nullptr;
bool isSavePress = false;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TForm3::SaveIDESettings()
{
    TStringList *jsonFile = new TStringList;
	TJSONObject *o = new TJSONObject();
	__try {
		 o->AddPair( new TJSONPair("ShowFPS",CheckBox1->IsChecked) );
		 o->AddPair( new TJSONPair("ShowDrawDevices",CheckBox2->IsChecked) );
		 o->AddPair( new TJSONPair("OpenWorkFolder",CheckBox3->IsChecked) );
		 o->AddPair( new TJSONPair("GlobalKeyHook",cbGlobalKeyHook->IsChecked) );

		 // Hotkey List
		 TEdit *eHotKeyList[] = {eHotKey1, eHotKey2, eHotKey3, eHotKey4, eHotKey5};
		 TSpinBox *sbTrBxInList[] = {sbTrBxIn1, sbTrBxIn2, sbTrBxIn3, sbTrBxIn4, sbTrBxIn5};
		 for(int i = 0; i < 5; i++)
		 {
			o->AddPair( new TJSONPair("eHotKey"+IntToStr(i+1), eHotKeyList[i]->Text) );
			o->AddPair( new TJSONPair("eHotKeyType"+IntToStr(i+1), eHotKeyList[i]->Tag) );
			o->AddPair( new TJSONPair("sbTrBxIn"+IntToStr(i+1), sbTrBxInList[i]->Value) );
		 }

		 // Lsl threads name
		 o->AddPair( new TJSONPair("HideLslWin",cbHideLslWin->IsChecked) );
		 o->AddPair( new TJSONPair("StopLslRec",cbStopLslRec->IsChecked) );

		 TJSONArray *a = new TJSONArray();
		 o->AddPair("lslThreadsName", a);

		 for(int i = 0; i < lbLslThreadsName->Count; i++)
		 {
			 TListBoxItem* item = lbLslThreadsName->ItemByIndex(i);
			 TFmxObject* obj = item->Children->Items[1];
			 if(obj->ClassType() == __classid(TEdit))
			 {
				  TEdit *edit = static_cast<TEdit*>(obj);

				  TJSONObject *info = new TJSONObject();
				  info->AddPair( new TJSONPair("name",edit->Text) );
				  a->AddElement(info);
			 }
         }

		 jsonFile->Text = o->ToString();
		 jsonFile->SaveToFile(SettingsDir+"IDE.json", TEncoding::Unicode);
	}
    __finally
	{
	  o->Free();
      jsonFile->Free();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::LoadIDESettings()
{
	TStringList *jsonFile = new TStringList;
	jsonFile->LoadFromFile(SettingsDir+"IDE.json", TEncoding::Unicode);
	TJSONObject *o = (TJSONObject*) TJSONObject::ParseJSONValue(jsonFile->Text);
	__try
	{
		CheckBox1->IsChecked = o->Values["ShowFPS"]->AsType<bool>();
		CheckBox2->IsChecked = o->Values["ShowDrawDevices"]->AsType<bool>();
		CheckBox3->IsChecked = o->Values["OpenWorkFolder"]->AsType<bool>();
		cbGlobalKeyHook->IsChecked = o->Values["GlobalKeyHook"]->AsType<bool>();

		// Hotkey List
		TEdit *eHotKeyList[] = {eHotKey1, eHotKey2, eHotKey3, eHotKey4, eHotKey5};
		TSpinBox *sbTrBxInList[] = {sbTrBxIn1, sbTrBxIn2, sbTrBxIn3, sbTrBxIn4, sbTrBxIn5};
		for(int i = 0; i < 5; i++)
		{
			eHotKeyList[i]->Text = o->Values["eHotKey"+IntToStr(i+1)]->AsType<UnicodeString>();
			eHotKeyList[i]->Tag = o->Values["eHotKeyType"+IntToStr(i+1)]->AsType<int>();
			sbTrBxInList[i]->Value = o->Values["sbTrBxIn"+IntToStr(i+1)]->AsType<int>();
		}

		// Lsl threads name
		cbHideLslWin->IsChecked = o->Values["HideLslWin"]->AsType<bool>();
		cbStopLslRec->IsChecked = o->Values["StopLslRec"]->AsType<bool>();

        lbLslThreadsName->Clear();
		TJSONArray *a = (TJSONArray*) o->Get("lslThreadsName")->JsonValue;
		for (int idx = 0; idx < a->Size(); idx++)
		{
		  TJSONObject *info = (TJSONObject*) a->Get(idx);
		  for (int idy = 0; idy < info->Count; idy++) {
            //book->Pairs[idy]->JsonString->ToString() = name
			AddLslThreadInfo(info->Pairs[idy]->JsonValue->AsType<UnicodeString>(),"");
		  }
		}

	}
	__finally {
		o->Free();
		jsonFile->Free();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::UpdateSettings(SettingsBase *settings)
{
	ListBox1->Clear();
	parameter_list &list = settings->GetParametersList();
	int i = 0;
    ListBox1->BeginUpdate();
	for (parameter_list::iterator it = list.begin(); it != list.end(); it++, i++) {
        TListBoxItem* item = new TListBoxItem(ListBox1);
        item->Parent = ListBox1;
        item->Text = it->second.desc;
        item->StyledSettings = item->StyledSettings >> TStyledSetting::Size;
        item->TextSettings->Font->Size = 12;
        item->Height = 30;
		item->Padding->Left = 190;
		item->Padding->Right = 10;
		if (it->second.type == GuiType::TEdit) {
			TEdit *edit = new TEdit(item);
			edit->Parent = item;
			edit->Align = TAlignLayout::VertCenter;
			edit->Text = it->second.value;
			object[i] = edit;
		}
		if (it->second.type == GuiType::TComboColorBox) {
			TComboColorBox* colorBox = new TComboColorBox(item);
            colorBox->Parent = item;
            colorBox->Align = TAlignLayout::VertCenter;
			colorBox->Color = TAlphaColor(it->second.value.ToInt());
			object[i] = colorBox;
		}
		if (it->second.type == GuiType::TSwitch) {
			TSwitch* Switch = new TSwitch(item);
			Switch->Parent = item;
			Switch->Align = TAlignLayout::VertCenter;
			Switch->Cursor = crHandPoint;
            Switch->Height = 22;
			Switch->IsChecked = bool(it->second.value.ToInt());
			object[i] = Switch;
		}
		if (it->second.type == GuiType::TCheckBox)
		{
			TCheckBox* CheckBox = new TCheckBox(item);
			CheckBox->Parent = item;
			CheckBox->Align = TAlignLayout::VertCenter;
			CheckBox->IsChecked = bool(it->second.value.ToInt());
			object[i] = CheckBox;
		}
		if (it->second.type == GuiType::TRange)
		{
			TRange *range = new TRange(item);
			range->Parent = item;
			range->Align = TAlignLayout::VertCenter;
			range->Text = it->second.value;
			object[i] = range;
		}
		if(it->second.type == GuiType::TDirectoryPath)
		{
           item->Padding->Left = 120;
		   TEdit *edit = new TEdit(item);
		   edit->Parent = item;
		   edit->Align = TAlignLayout::VertCenter;
		   TEditButton *ebutton = new TEditButton(edit);
		   ebutton->Parent = edit;
           ebutton->StyledSettings = ebutton->StyledSettings >> TStyledSetting::Family;
		   ebutton->TextSettings->Font->Family = "Bodoni MT";
		   ebutton->Text = "...";
		   ebutton->Name = "DirPatchButton";
		   ebutton->OnClick = ButtonClick;
		   edit->Text = it->second.value;
		   object[i] = edit;
        }
	}
	ListBox1->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonClick(TObject *Sender)
{
	if (((TControl*)Sender)->Name == "DirPatchButton")
	{
		TCustomEdit *edit = ((TEditButton*)Sender)->GetEdit();
		UnicodeString patch;
		if (SelectDirectory("Укажите путь к папке", "",patch))
			edit->Text = patch;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormShow(TObject* Sender)
{
	TreeView1->Items[Form1->ComboBox1->ItemIndex+1]->Select();
	TreeView1->Items[Form1->ComboBox1->ItemIndex+1]->Expand();
    LoadIDESettings();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::SaveSettings(SettingsBase *settings)
{
	parameter_list &list = settings->GetParametersList();
	int i = 0;
	for (parameter_list::iterator it = list.begin(); it != list.end(); it++, i++)
	{
		if (it->second.type == GuiType::TEdit || it->second.type == GuiType::TDirectoryPath)
		{
		   TEdit *edit = static_cast<TEdit*>(object[i]);
		   it->second.value = edit->Text;
		}
		if (it->second.type == GuiType::TComboColorBox) {
			TComboColorBox* colorBox = static_cast<TComboColorBox*>(object[i]);
			it->second.value = (int)colorBox->Color;
		}
		if (it->second.type == GuiType::TCheckBox) {
		   TCheckBox *CheckBox =  static_cast<TCheckBox*>(object[i]);
		   it->second.value = UnicodeString(int(CheckBox->IsChecked));
		}
		if (it->second.type == GuiType::TRange)
		{
			TRange *range = static_cast<TRange*>(object[i]);
			it->second.value = range->Text;
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormHide(TObject* Sender)
{
	if(!isSavePress){
		LoadIDESettings();
		std::vector<task_ptr>& Tasks = Form2->Tasks;
		for (int i = 0; i < Tasks.size(); i++) {
			Tasks[i]->LoadSettings();
		}
	}
	//TreeView1->Items[TreeView1->Selected->Index]->Deselect();
	TreeView1->Selected->Deselect();
	CurSettings = nullptr;
	ListBox1->Clear();
    isSavePress = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormCreate(TObject *Sender)
{
	SettingsDir = "Settings\\";
    CreateDir(SettingsDir);
	if(!FileExists(SettingsDir+"IDE.json")) SaveIDESettings();
	std::vector<task_ptr>& Tasks = Form2->Tasks;
	for (int i = 0; i < Tasks.size(); i++) {
		TTreeViewItem *ItemTask= new TTreeViewItem(this);
		ItemTask->Text = Tasks[i]->GetTaskName();
		for(int j = 0; j < Tasks[i]->GetBlocks().size(); j++)
		{
			TTreeViewItem* Item = new TTreeViewItem(this);
			Item->Text = Tasks[i]->GetBlocks()[j]->GetTaskName();
			ItemTask->AddObject(Item);
		}
		TreeView1->AddObject(ItemTask);
	}
    LoadIDESettings();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::TreeView1Change(TObject *Sender)
{
    if(!TreeView1->Selected)  return;
    Label1->Text = TreeView1->Selected->Text;
	if(TreeView1->Selected->ParentItem()){
		if(TreeView1->Selected->ParentItem()->Index == 0){
            TabControl1->TabIndex = TreeView1->Selected->Index+2;
            return;
		}
	} else {
		if(TreeView1->Selected->Index == 0){
			TabControl1->TabIndex = 1;
            return;
		}
    }
	TabControl1->TabIndex = 0;
    std::vector<task_ptr>& Tasks = Form2->Tasks;
	SettingsBase *settings;
	if(TreeView1->Selected->ParentItem()) {
	   settings = Tasks[TreeView1->Selected->ParentItem()->Index-1]->GetBlocks()[TreeView1->Selected->Index]->GetSettings().get();
	} else{
	   settings = Tasks[TreeView1->Selected->Index-1]->GetSettings().get();
	}
	if(CurSettings)	SaveSettings(CurSettings);
	CurSettings = settings;
	UpdateSettings(settings);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button2Click(TObject *Sender)
{
	Form3->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button1Click(TObject *Sender)
{
    SaveIDESettings();
	SaveSettings(CurSettings);
	std::vector<task_ptr>& Tasks = Form2->Tasks;
	for (int i = 0; i < Tasks.size(); i++) {
		Tasks[i]->SaveSettings();
	}
    isSavePress = true;
	Form3->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::cbTBAutoConnectChange(TObject *Sender)
{
	Form1->triggerbox->AutoConnect(cbTBAutoConnect->IsChecked);
	Form1->AniIndicator1->Enabled = cbTBAutoConnect->IsChecked;
	Form1->AniIndicator1->Visible = cbTBAutoConnect->IsChecked;
	Form1->Button7->Enabled = !cbTBAutoConnect->IsChecked;
}
//---------------------------------------------------------------------------

void __fastcall TForm3::cbGlobalKeyHookChange(TObject *Sender)
{
	if(cbGlobalKeyHook->IsChecked)
		Form1->SetKeyHook();
	else
        Form1->UnSetKeyHook();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::HotKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift)
{
	TEdit *edit = dynamic_cast <TEdit*>(Sender);

	if(KeyChar) {
		edit->Text = "<"+UnicodeString(KeyChar)+">";
		edit->Tag = VkKeyScan(KeyChar);
	}

	if(Key) {
		edit->Text = UnicodeString(getkeyname((int)Key));
		edit->Tag = (int)Key;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::AddLslThreadInfo(AnsiString name, AnsiString type)
{
	TListBoxItem* item = new TListBoxItem(lbLslThreadsName);
	item->Parent = lbLslThreadsName;
	item->Text = "Имя:";
	item->StyledSettings = item->StyledSettings >> TStyledSetting::Size;
	item->TextSettings->Font->Size = 12;
	item->Height = 25;
	item->Padding->Left = 40;
    item->Padding->Right = 10;

	TEdit *edit = new TEdit(item);
	edit->Parent = item;
	edit->Align = TAlignLayout::VertCenter;
    edit->Text = name;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::bAddLslThreadClick(TObject *Sender)
{
	AddLslThreadInfo("","");
}
//---------------------------------------------------------------------------
void __fastcall TForm3::bDeleteLslThreadClick(TObject *Sender)
{
  if(lbLslThreadsName->ItemIndex != -1)
  	lbLslThreadsName->Items->Delete(lbLslThreadsName->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::cbHideLslWinChange(TObject *Sender)
{
   if(cbHideLslWin->IsChecked)
    cbStopLslRec->IsChecked = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::cbStopLslRecChange(TObject *Sender)
{
	if(cbHideLslWin->IsChecked) cbStopLslRec->IsChecked = true;
}
//---------------------------------------------------------------------------

