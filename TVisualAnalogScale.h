//---------------------------------------------------------------------------

#ifndef TVisualAnalogScaleH
#define TVisualAnalogScaleH
//---------------------------------------------------------------------------
#include <fmx.h>
//#include <System.Types.hpp>
#include <vector>

//---------------------------------------------------------------------------
enum VASTypes : int
{
	vasMental,
	vasPhysical,
	vasBoredom,
	vasEffort
};

struct VAS
{
	VASTypes type;
	AnsiString text = "";
	int global_counter = 0;
    int value = 0;
};


class TVisualAnalogScale
{
public:
   std::vector<VAS> vasQueue;

   TText* text;
   TTrackBar* track;
   TButton* button;
   TRectangle *rectangle;
   TLayout* layout;
   TText *textButton;

    // Events
	__property void OnFinished = { write=SetOnFinished }; // !!!!!!!!!
	void __fastcall SetOnFinished(const TNotifyEvent fn) {OnFinishedEvent = fn; };

	TVisualAnalogScale(TForm* Form);
	void Resize(System::Types::TRect rec);

	void __fastcall TextClick(TObject *Sender);
	void __fastcall TextMouseEnter(TObject *Sender);
	void __fastcall TextMouseLeave(TObject *Sender);
    void __fastcall TimerEvent(TObject *Sender);

	void Init(std::vector<VASTypes> types);
	void Run();

    ~TVisualAnalogScale();

private:
    TTimer* timer;
	TNotifyEvent OnFinishedEvent;


	int vasIndex = 0;

    int VASGlobalCount = 0;
};




#endif
