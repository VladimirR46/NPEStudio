//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "SettingsWin.h"
#include "DrawWin.h"
#include "MainWin.h"

//#include "SpiralTask.h"
#include <fstream>
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

	}
	ListBox1->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormShow(TObject* Sender)
{
	TreeView1->Items[Form1->ComboBox1->ItemIndex+1]->Select();
	TreeView1->Items[Form1->ComboBox1->ItemIndex+1]->Expand();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::SaveSettings(SettingsBase *settings)
{
	parameter_list &list = settings->GetParametersList();
	int i = 0;
	for (parameter_list::iterator it = list.begin(); it != list.end(); it++, i++) {

		if (it->second.type == GuiType::TEdit) {
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
}
//---------------------------------------------------------------------------

void __fastcall TForm3::TreeView1Change(TObject *Sender)
{
    if(!TreeView1->Selected)  return;

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

	Label1->Text = TreeView1->Selected->Text;

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
	SaveSettings(CurSettings);

	std::vector<task_ptr>& Tasks = Form2->Tasks;
	for (int i = 0; i < Tasks.size(); i++) {
		Tasks[i]->SaveSettings();
	}

    isSavePress = true;
	Form3->Close();
}
//---------------------------------------------------------------------------

