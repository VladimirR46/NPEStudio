//---------------------------------------------------------------------------

#ifndef MainWinH
#define MainWinH
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
#include <FMXTee.Chart.hpp>
#include <FMXTee.Engine.hpp>
#include <FMXTee.Procs.hpp>
#include <FMXTee.Series.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include <FMX.Layouts.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
  __published: // IDE-managed Components
    TButton* Button1;
    TComboBox* ComboBox1;
    TLabel* Label1;
    TMainMenu* MainMenu1;
    TMenuItem* MenuItem1;
    TMenuItem* MenuItem2;
	TButton *Button3;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem4;
	TMenuItem *MenuItem5;
	TMenuItem *MenuItem6;
	TListBox *ListBox1;
	TListBoxItem *ListBoxItem2;
	TListBoxItem *ListBoxItem3;
	TListBoxItem *ListBoxItem4;
	TListBoxItem *ListBoxItem5;
	TListBoxItem *ListBoxItem6;
	TComboBox *ComboBox2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TComboBox *ComboBox3;
	TListBoxItem *ListBoxItem7;
	TListBoxItem *ListBoxItem8;
	TListBoxItem *ListBoxItem9;
	TListBoxItem *ListBoxItem10;
	TListBoxItem *ListBoxItem11;
	TEdit *Edit1;
	TListBoxItem *ListBoxItem1;
    void __fastcall Button1Click(TObject* Sender);
    void __fastcall ComboBox1Change(TObject* Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall MenuItem3Click(TObject *Sender);
	void __fastcall MenuItem5Click(TObject *Sender);
  private: // User declarations
  public: // User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1* Form1;
//---------------------------------------------------------------------------
#endif

