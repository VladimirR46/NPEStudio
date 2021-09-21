//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.TabControl.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.ListView.Adapters.Base.hpp>
#include <FMX.ListView.Appearances.hpp>
#include <FMX.ListView.hpp>
#include <FMX.ListView.Types.hpp>
#include <FMX.Colors.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
  __published: // IDE-managed Components
    TGroupBox* GroupBox1;
    TPanel* Panel1;
    TButton* Button1;
    TListBox* ListBox1;
    void __fastcall FormShow(TObject* Sender);
    void __fastcall Button1Click(TObject* Sender);
    void __fastcall FormHide(TObject* Sender);
  private: // User declarations
  public: // User declarations
    __fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3* Form3;
//---------------------------------------------------------------------------
#endif

