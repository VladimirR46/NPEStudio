//---------------------------------------------------------------------------

#ifndef DrawWinH
#define DrawWinH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>

#include "TBaseTask.h"
#include <FMX.Memo.hpp>
#include <FMX.Memo.Types.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMXTee.Chart.hpp>
#include <FMXTee.Engine.hpp>
#include <FMXTee.Procs.hpp>
#include <FMXTee.Series.hpp>
#include <FMX.Media.hpp>
//#include <boost/shared_ptr.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published: // IDE-managed Components
    TTimer* Timer1;
	TPanel *Panel1;
	TPaintBox *PaintBox1;
	TTimer *Timer2;
	TLabel *Label1;
	TLabel *Label2;
	TMediaPlayer *MediaPlayer1;
    void __fastcall FormKeyDown(TObject* Sender, WORD &Key,
        System::WideChar &KeyChar, TShiftState Shift);
    void __fastcall FormCreate(TObject* Sender);
    void __fastcall FormShow(TObject* Sender);
    void __fastcall Timer1Timer(TObject* Sender);
	void __fastcall FormTouch(
        TObject* Sender, const TTouches Touches, const TTouchAction Action);
	void __fastcall PaintBox1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          float X, float Y);
	void __fastcall PaintBox1MouseMove(TObject *Sender, TShiftState Shift, float X,
          float Y);
	void __fastcall PaintBox1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          float X, float Y);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall PaintBox1Paint(TObject *Sender, TCanvas *Canvas);
	void __fastcall ExternalTrigger(int trigger);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

  private: // User declarations

  public: // User declarations
    __fastcall TForm2(TComponent* Owner);
    int CurrentTask;
	std::vector<task_ptr> Tasks;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2* Form2;
//---------------------------------------------------------------------------
#endif

