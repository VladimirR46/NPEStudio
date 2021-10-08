//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit1.h"

#include "Unit2.h"
#include "Unit3.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TForm1* Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject* Sender)
{
    Form2->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject* Sender)
{
    Form3->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBox1Change(TObject* Sender)
{
    Form2->CurrentTask = Form1->ComboBox1->ItemIndex;
}
//---------------------------------------------------------------------------


