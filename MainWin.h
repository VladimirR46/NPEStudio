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
#include <FMX.ImgList.hpp>
#include "TActiCHamp.h"
#include "TTriggerBox.h"

#include <System.ImageList.hpp>
#include <FMX.Dialogs.hpp>
#include <FMX.SearchBox.hpp>
#include <FMX.Objects.hpp>//---------------------------------------------------------------------------
class TForm1 : public TForm
{
  __published: // IDE-managed Components
    TComboBox* ComboBox1;
    TLabel* Label1;
    TMainMenu* MainMenu1;
    TMenuItem* MenuItem1;
    TMenuItem* MenuItem2;
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
	TToolBar *ToolBar1;
	TImageList *ImageList1;
	TButton *Button1;
	TButton *Button2;
	TButton *Button5;
	TComboBox *cbMonitors;
	TButton *btnStart;
	TListBoxItem *ListBoxItem12;
	TPanel *Panel1;
	TRectangle *Rectangle1;
	TImage *Image1;
	TText *Text1;
	TPanel *Panel2;
	TRectangle *Rectangle3;
	TText *Text3;
	TMemo *Memo1;
	TPanel *Panel3;
	TRectangle *Rectangle2;
	TText *Text2;
	TButton *Button6;
	TListBox *ListBox2;
	TListBoxItem *ListBoxItem13;
	TListBoxItem *ListBoxItem15;
	TButton *Button7;
	TAniIndicator *AniIndicator1;
    void __fastcall ComboBox1Change(TObject* Sender);
	void __fastcall MenuItem3Click(TObject *Sender);
	void __fastcall MenuItem5Click(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
  private: // User declarations
	TActiCHamp *actiCHamp;

  public: // User declarations
	__fastcall TForm1(TComponent* Owner);
  TTriggerBox *triggerbox;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1* Form1;
//---------------------------------------------------------------------------
#endif

