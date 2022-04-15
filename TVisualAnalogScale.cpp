//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVisualAnalogScale.h"
#include <algorithm>
#include <random>
//---------------------------------------------------------------------------
#pragma package(smart_init)

TVisualAnalogScale::TVisualAnalogScale(TForm* Form)
{
    timer = new TTimer(NULL);
	timer->OnTimer = &TimerEvent;
	timer->Interval = 0;


	layout = new TLayout(Form);
	layout->Parent = Form;
	layout->Height = Form->Height;
	layout->Width = Form->Width;

	text = new TText(layout);
	text->Parent = layout;
	text->Position->Y = 1000;
	text->Height = 100;
	text->Text = "";
	text->TextSettings->Font->Size = 44;
	text->TextSettings->FontColor = TAlphaColorRec::White;
	text->Align = TAlignLayout::Top;

    TLayout *layout2 = new TLayout(layout);
	layout2->Parent = layout;
	layout2->Align = TAlignLayout::Top;

	TText *textLeft = new TText(layout2);
	textLeft->Parent = layout2;
	textLeft->Text = "Низкая";
	textLeft->Width = 150;
	textLeft->Align = TAlignLayout::Left;
	textLeft->TextSettings->Font->Size = 33;
	textLeft->TextSettings->FontColor = TAlphaColorRec::White;

	TText *textRight = new TText(layout2);
	textRight->Parent = layout2;
	textRight->Text = "Высокая";
	textRight->Width = 150;
	textRight->Align  = TAlignLayout::Right;
	textRight->TextSettings->Font->Size = 33;
	textRight->TextSettings->FontColor = TAlphaColorRec::White;

	track = new TTrackBar(layout2);
	track->StylesData["htrack.Height"] = TValue::From<int>(4);
	track->Parent = layout2;
	track->Position->Y = 1000;
	track->Scale->X = 2;
	track->Scale->Y = 2;
	track->Align = TAlignLayout::Client;


	rectangle = new TRectangle(layout);
	rectangle->Parent = layout;

	TText *textButton = new TText(rectangle);
	textButton->Parent = rectangle;
	textButton->Text = "Продолжить";
	textButton->Align = TAlignLayout::Client;
	textButton->TextSettings->Font->Size = 33;
	textButton->TextSettings->FontColor = TAlphaColorRec::White;
	textButton->OnMouseEnter = TextMouseEnter;
	textButton->OnMouseLeave = TextMouseLeave;
	textButton->OnClick = TextClick;

	rectangle->Height = 80;
	rectangle->Stroke->Thickness = 4;
	rectangle->Stroke->Color = TAlphaColorRec::White;
	rectangle->Fill->Color = TAlphaColorRec::Black;
	rectangle->Margins->Left = 880;
	rectangle->Align = TAlignLayout::Bottom;

    layout->Visible = false;
}
//------------------------------------------------------------------------------
void TVisualAnalogScale::Resize(System::Types::TRect rec)
{
	layout->Visible = false;
	VASGlobalCount = 0;

	layout->Position->X = rec.Left;
	layout->Position->Y = rec.Top;
	layout->Height = rec.Bottom;
	layout->Width = rec.Right;
}

void __fastcall TVisualAnalogScale::TextClick(TObject *Sender)
{
	layout->Visible = false;

	vasQueue[vasIndex].value = track->Value;
    vasQueue[vasIndex].global_counter = VASGlobalCount;

	if(vasIndex < vasQueue.size()-1)  // -1 т.к. первый показ в Run()
	{
     	timer->Interval = 300;
		vasIndex++;
	} else {
        timer->Interval = 0;
		OnFinishedEvent(Sender);
    }
}
//------------------------------------------------------------------------------
void __fastcall TVisualAnalogScale::TextMouseEnter(TObject *Sender)
{
   rectangle->Fill->Color = 0xFF292929;//TAlphaColorRec::Lightgray;
}
//------------------------------------------------------------------------------
void __fastcall TVisualAnalogScale::TextMouseLeave(TObject *Sender)
{
   rectangle->Fill->Color = TAlphaColorRec::Black;
}
//------------------------------------------------------------------------------
void TVisualAnalogScale::Init(std::vector<VASTypes> types)
{
   vasIndex = 0;
   vasQueue.clear();

   for(int i = 0; i < types.size(); i++)
   {
		VAS vas_test;

		if(types[i] == vasMental){
		   vas_test.text = "Ментальная усталость";
		   vas_test.type = vasMental;
		}
		if(types[i] == vasPhysical){
		   vas_test.text = "Физическая усталость";
		   vas_test.type = vasPhysical;
		}
		if(types[i] == vasBoredom){
		   vas_test.text = "Уровень интереса";
		   vas_test.type = vasBoredom;
		}
		if(types[i] == vasEffort){
		   vas_test.text = "Усилие";
		   vas_test.type = vasEffort;
		}

		vasQueue.push_back(vas_test);
   }

   srand(time(NULL));
   std::random_shuffle(begin(vasQueue), end(vasQueue));
}
//------------------------------------------------------------------------------
void __fastcall TVisualAnalogScale::TimerEvent(TObject *Sender)
{
    timer->Interval = 0;
	text->Text = vasQueue[vasIndex].text;
	track->Value = 0;
	layout->Visible = true;
    VASGlobalCount++;
}
//------------------------------------------------------------------------------
void TVisualAnalogScale::Run()
{
   if(vasQueue.size() == 0) return;

   text->Text = vasQueue[vasIndex].text;
   track->Value = 0;

   layout->Visible = true;
   VASGlobalCount++;
}
//------------------------------------------------------------------------------
TVisualAnalogScale::~TVisualAnalogScale()
{
   timer->Free();
}
