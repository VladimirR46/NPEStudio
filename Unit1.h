//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <FMX.Colors.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Edit.hpp>
#include <FMX.Forms.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Memo.Types.hpp>
#include <FMX.Memo.hpp>
#include <FMX.Menus.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <System.Classes.hpp>
#include <FMX.ListView.Adapters.Base.hpp>
#include <FMX.ListView.Appearances.hpp>
#include <FMX.ListView.hpp>
#include <FMX.ListView.Types.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
  __published: // IDE-managed Components
    TButton* Button1;
    TComboBox* ComboBox1;
    TLabel* Label1;
    TLabel* Label2;
    TEdit* Edit1;
    TEdit* Edit2;
    TLabel* Label3;
    TMainMenu* MainMenu1;
    TMenuItem* MenuItem1;
    TMenuItem* MenuItem2;
    TButton* Button2;
    void __fastcall Button1Click(TObject* Sender);
    void __fastcall Button2Click(TObject* Sender);
    void __fastcall ComboBox1Change(TObject* Sender);
  private: // User declarations
  public: // User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1* Form1;
//---------------------------------------------------------------------------
#endif

