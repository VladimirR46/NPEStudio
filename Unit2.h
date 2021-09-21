//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>

#include "TaskBase.hpp"
#include <FMX.Memo.hpp>
#include <FMX.Memo.Types.hpp>
#include <FMX.ScrollBox.hpp>
//#include <boost/shared_ptr.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
  __published: // IDE-managed Components
    TImage* Image1;
    TTimer* Timer1;
    void __fastcall FormKeyDown(TObject* Sender, WORD &Key,
        System::WideChar &KeyChar, TShiftState Shift);
    void __fastcall FormActivate(TObject* Sender);
    void __fastcall FormCreate(TObject* Sender);
    void __fastcall FormShow(TObject* Sender);
    void __fastcall Timer1Timer(TObject* Sender);
    void __fastcall Image1MouseMove(
        TObject* Sender, TShiftState Shift, float X, float Y);
    void __fastcall Image1MouseDown(TObject* Sender, TMouseButton Button,
        TShiftState Shift, float X, float Y);
    void __fastcall Image1MouseUp(TObject* Sender, TMouseButton Button,
        TShiftState Shift, float X, float Y);
    void __fastcall FormTouch(
        TObject* Sender, const TTouches Touches, const TTouchAction Action);
  private: // User declarations
    typedef std::shared_ptr<TaskBase> Task_ptr;
  public: // User declarations
    __fastcall TForm2(TComponent* Owner);
    int CurrentTask;
    std::vector<Task_ptr> Tasks;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2* Form2;
//---------------------------------------------------------------------------
#endif

