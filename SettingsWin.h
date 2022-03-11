//---------------------------------------------------------------------------

#ifndef SettingsWinH
#define SettingsWinH
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
#include <FMX.TreeView.hpp>
#include "SettingsBase.h"
#include <FMX.ImgList.hpp>
#include <System.ImageList.hpp>
#include <FMX.Dialogs.hpp>
#include <FMX.Printer.hpp>
//---------------------------------------------------------------------------
class TRange : public TCustomEdit
{
	public:
	   __property Fmx::Controls::TControl *Parent  = { write=SetParent };
	   __property Fmx::Types::TAlignLayout Align = { write=SetAlign };
	   __property UnicodeString MinText = { read=GetMinText, write=SetMinText };
	   __property UnicodeString MaxText = { read=GetMaxText, write=SetMaxText };
	   __property UnicodeString Text = { read=GetText, write=SetText };

	   void __fastcall SetParent(Fmx::Controls::TControl* _parent) { layout->Parent = _parent;  }
	   void __fastcall SetAlign(const Fmx::Types::TAlignLayout _align) { layout->Align = _align; }
	   void __fastcall SetMinText(UnicodeString value) {edit1->Text = value;}
	   UnicodeString __fastcall GetMinText() const { return edit1->Text; }
	   void __fastcall SetMaxText(UnicodeString value) {edit2->Text = value;}
	   UnicodeString __fastcall GetMaxText() const { return edit2->Text; }
	   void __fastcall SetText(const UnicodeString value) {
			TStringDynArray data = SplitString(value,":");
			edit1->Text = data[0];
            edit2->Text = data[1];
	   }
	   UnicodeString __fastcall GetText() const { return edit1->Text + ":" + edit2->Text; };

	   TRange(System::Classes::TComponent* AOwner) : TCustomEdit(AOwner)
	   {
			layout = new TLayout(AOwner);
            layout->Height = 22;
			layout->Width = 106;

			edit1 = new TEdit(layout);
			edit1->Parent = layout;
            edit2 = new TEdit(layout);
			edit2->Parent = layout;

			label = new TLabel(layout);
			label->Parent = layout;
			label->Text = ":";
			label->Align = TAlignLayout::Client;
			label->StyledSettings = label->StyledSettings >> TStyledSetting::Size;
			label->TextSettings->Font->Size = 20;
			label->TextSettings->HorzAlign = TTextAlign::Center;
			label->Margins->Bottom = 5;



			edit1->Align = TAlignLayout::Left;
			edit1->Width = 35;
			edit1->FilterChar = "0123456789";
			edit2->Align = TAlignLayout::Right;
			edit2->Width = 35;
			edit2->FilterChar = "0123456789";
       }

	private:
       TLabel *label;
	   TLayout *layout;
	   TEdit *edit1;
	   TEdit *edit2;
};
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
  __published: // IDE-managed Components
    TPanel* Panel1;
    TButton* Button1;
	TLayout *Layout1;
	TTreeView *TreeView1;
	TSplitter *Splitter1;
	TLayout *Layout2;
	TLabel *Label1;
	TListBox *ListBox1;
	TButton *Button2;
	TImageList *ImageList1;
	TLayout *Layout3;
	TTreeViewItem *TreeViewItem1;
	TTreeViewItem *TreeViewItem2;
	TTabControl *TabControl1;
	TTabItem *TabItem1;
	TTabItem *TabItem2;
	TTabItem *TabItem3;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TTreeViewItem *TreeViewItem3;
	TTabItem *TabItem4;
	TGroupBox *GroupBox1;
	TEdit *EditClientTP;
	TLabel *Label2;
    void __fastcall FormShow(TObject* Sender);
    void __fastcall FormHide(TObject* Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall TreeView1Change(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
    void __fastcall ButtonClick(TObject *Sender);
  private: // User declarations
  	AnsiString SettingsDir;
  public: // User declarations
	__fastcall TForm3(TComponent* Owner);
	void __fastcall UpdateSettings(SettingsBase *settings);
	void __fastcall SaveSettings(SettingsBase *settings);
	void SaveIDESettings();
    void LoadIDESettings();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3* Form3;
//---------------------------------------------------------------------------
#endif

