//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "MainWin.h"

#include "Unit2.h"
#include "SettingsWin.h"
#include "mat.h"
#pragma comment (lib,"borland/libmat.lib")
#pragma comment (lib,"borland/libmx.lib")

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


void __fastcall TForm1::Button3Click(TObject *Sender)
{
double data[9] = { 1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0 };


MATFile *pmat;
mxArray *pa1, *pa2, *pa3;

pmat = matOpen("test.mat", "w");


pa1 = mxCreateDoubleMatrix(3,3,mxREAL);
pa2 = mxCreateDoubleMatrix(3,3,mxREAL);
memcpy((void *)(mxGetPr(pa1)), (void *)data, sizeof(data));
memcpy((void *)(mxGetPr(pa2)), (void *)data, sizeof(data));

pa3 = mxCreateString("MATLAB: the language of technical computing");



matPutVariable(pmat, "LocalDouble", pa1);
matPutVariableAsGlobal(pmat, "GlobalDouble", pa2);
matPutVariable(pmat, "LocalString", pa3);

// create a scalar struct array with two fields
const char *fieldnames[2] = {"a", "b"};
mxArray *s = mxCreateStructMatrix(1, 1, 2, fieldnames);

// fill struct fields
for (mwIndex i=0; i<2; i++) {
	// 10x1 vector
	mxArray *arr = mxCreateDoubleMatrix(10, 1, mxREAL);
	double *x = mxGetPr(arr);
	std::fill(x, x+10, i);

	// assign field
	mxSetField(s, 0, fieldnames[i], arr);
}

matPutVariable(pmat, "my_struct", s);

mxDestroyArray(s);

mxDestroyArray(pa1);
mxDestroyArray(pa2);
mxDestroyArray(pa3);
matClose(pmat);
}
//---------------------------------------------------------------------------

