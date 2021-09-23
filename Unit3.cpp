//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit3.h"
#include "Unit2.h"

#include "TaskSpiral.hpp"
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm3* Form3;

TFmxObject* object[10];

//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------

void __fastcall TForm3::FormShow(TObject* Sender)
{
	GroupBox1->Text =
        "Параметры задачи: " + Form2->Tasks[Form2->CurrentTask]->GetTaskName();

    SettingsBase* settings = Form2->Tasks[Form2->CurrentTask]->GetSettings();
    parameters_list &list = settings->GetList();

    int i = 0;
    ListBox1->BeginUpdate();
    for (parameters_list::iterator it = list.begin(); it != list.end();
         it++, i++) {
        TListBoxItem* item = new TListBoxItem(ListBox1);
        item->Parent = ListBox1;
        item->Text = it->second.name.c_str();

        item->StyledSettings = item->StyledSettings >> TStyledSetting::Size;
        item->TextSettings->Font->Size = 12;

        item->Height = 30;
        item->Padding->Left = 190;

		if (it->second.gtype == GuiType::TEdit) {
			TEdit *edit = new TEdit(item);
			edit->Parent = item;
			edit->Align = TAlignLayout::VertCenter;
			edit->Text = it->second.get<std::string>().c_str();
			object[i] = edit;
        }


		if (it->second.gtype == GuiType::TComboColorBox) {
			TComboColorBox* colorBox = new TComboColorBox(item);
            colorBox->Parent = item;
            colorBox->Align = TAlignLayout::VertCenter;
			colorBox->Color = TAlphaColor(it->second.get<unsigned int>());
			object[i] = colorBox;
		}

    }
	ListBox1->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button1Click(TObject* Sender)
{
	SettingsBase* settings = Form2->Tasks[Form2->CurrentTask]->GetSettings();
    parameters_list &list = settings->GetList();

    int i = 0;
    for (parameters_list::iterator it = list.begin(); it != list.end();
         it++, i++) {

		if (it->second.gtype == GuiType::TEdit) {
		   TEdit *edit = static_cast<TEdit*>(object[i]);
		   it->second.set(AnsiString(edit->Text).c_str());
		}

		if (it->second.gtype == GuiType::TComboColorBox) {
		  TComboColorBox *colorBox = static_cast<TComboColorBox*>(object[i]);
          it->second.set((int)colorBox->Color);
        }
	}

	settings->save(Form2->Tasks[Form2->CurrentTask]->GetTaskName()+".cfg");

	Form3->Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormHide(TObject* Sender)
{
    ListBox1->Clear();
}
//---------------------------------------------------------------------------


