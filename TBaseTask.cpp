//---------------------------------------------------------------------------
#pragma hdrstop
#include "TBaseTask.h"

#include <FMX.FontGlyphs.Win.hpp>
#include <FMX.Helpers.Win.hpp>

TCanvas* TBaseTask::Canvas;
TTimer* TBaseTask::Timer;
TForm* TBaseTask::Form;
TVisualAnalogScale* TBaseTask::VAS;
lsl_ptr TBaseTask::lsl;
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBaseTask::TBaseTask(AnsiString _name) : TaskName(_name)
{
	bitmap = bitmap_ptr(new TBitmap());
	//bitmap->SetSize(int(Screen->Size().Width),int(Screen->Size().Height));
    //bitmap->SetSize(int(Form->Width),int(Form->Height));
	//bitmap->Clear(TAlphaColorRec::Null ); //Alpha
	Settings = settings_ptr(new SettingsBase(_name));
	Protocol = protocol_ptr(new TProtocol(_name));
}
TBaseTask::TBaseTask(TBaseTask* _parent, AnsiString _name) : TBaseTask(_name)
{
	Parent = _parent;
    bitmap.reset();
	bitmap = _parent->GetBitmap();
    Protocol.reset(); // Удаляем свой протокол
	Protocol = _parent->GetProtocol();
}
//--------------------------------------------------------------------------
void TBaseTask::Init(AnsiString Path, SubjectInfo _sub)
{
	bitmap->SetSize(int(Form->Width),int(Form->Height));
	bitmap->Clear(TAlphaColorRec::Null ); //Alpha

    VAS->OnFinished = VasFinished;

	if(!Parent)	Protocol->Init(Path, _sub);
	InitTask(Path);
	for(int i = 0; i < Blocks.size(); i++){
		Blocks[i]->Init(Path, _sub);
	}
}
//--------------------------------------------------------------------------
void  TBaseTask::StartTask()
{
      StateManager();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawPlus()
{
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Thickness = 18;
	bitmap->Canvas->Stroke->Color = claWhite;
	bitmap->Canvas->DrawLine(
		TPointF(bitmap->Width / 2 - 360, bitmap->Height / 2),
		TPointF(bitmap->Width / 2 + 360, bitmap->Height / 2), 1);
	bitmap->Canvas->DrawLine(
		TPointF(bitmap->Width / 2, bitmap->Height / 2 - 360),
		TPointF(bitmap->Width / 2, bitmap->Height / 2 + 360), 1);
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawOneNumber(int number)
{
	TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Font->Size = 360;
	bitmap->Canvas->FillText(MyRect, IntToStr(number), false, 100,
		TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawBitmap(TBitmap *_bitmap)
{
	bitmap->Canvas->BeginScene();
	TRectF SrcRect(0, 0, _bitmap->Width, _bitmap->Height);
	TRectF DstRect((bitmap->Width/2)-(_bitmap->Width/2), 0, (bitmap->Width/2)-(_bitmap->Width/2)+_bitmap->Width, _bitmap->Height);
	bitmap->Canvas->DrawBitmap(_bitmap, SrcRect, DstRect, 1, false);
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawNoise(TBitmap *_bitmap)
{
	TBitmap *bitmap = this->bitmap.get();
	if(_bitmap) bitmap = _bitmap;

	TBitmapData data;
    TAlphaColorRec color;
	bitmap->Map(TMapAccess::ReadWrite, data);
	srand (time(NULL));
	for(int i = 0; i < data.Width; i++)
		for(int j = 0; j < data.Height; j++)
		{
		   color.R = rand() % 256 + 1;
		   color.G = rand() % 256 + 1;
		   color.B = rand() % 256 + 1;
		   color.A = 255;
		   data.SetPixel(i,j, color.Color);
		}
    bitmap->Unmap(data);
}
//--------------------------------------------------------------------------
void TBaseTask::DrawPoint(int size)
{
	int CenterX = bitmap->Width/2;
	int CenterY = bitmap->Height/2;
	TRectF MyRect(CenterX-size, CenterY-size, CenterX+size, CenterY+size);
	bitmap->Canvas->BeginScene();
	//bitmap->Canvas->Stroke->Color = TAlphaColorRec::Red;
	//bitmap->Canvas->Stroke->Thickness = 4;
	bitmap->Canvas->Fill->Color = TAlphaColorRec::Red;
    bitmap->Canvas->FillEllipse(MyRect, 1);
	//bitmap->Canvas->DrawEllipse(MyRect, 1);
    bitmap->Canvas->EndScene();
}

void TBaseTask::DrawPoint(int CenterX, int CenterY, int size, TAlphaColor color)
{
	TRectF MyRect(CenterX-size, CenterY-size, CenterX+size, CenterY+size);
	bitmap->Canvas->BeginScene();
	//bitmap->Canvas->Stroke->Color = TAlphaColorRec::Red;
	//bitmap->Canvas->Stroke->Thickness = 4;
	bitmap->Canvas->Fill->Color = color;
    bitmap->Canvas->FillEllipse(MyRect, 1);
	//bitmap->Canvas->DrawEllipse(MyRect, 1);
    bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawText(AnsiString text, int size, int horShift, TAlphaColor color)
{
	bitmap->Canvas->BeginScene();
	TRectF MyRect(horShift, 0, bitmap->Width-horShift, bitmap->Height);
	bitmap->Canvas->Font->Size = size;
	bitmap->Canvas->Fill->Color = color;
	bitmap->Canvas->FillText(MyRect, text, true, 1,
		TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawTable(int numbers[], int size)
{
	int RectSize = Form->Height * 0.8 / 5;
	int x = Form->Width / 2 - 2.5 * RectSize;
	int y = Form->Height / 2 - 2.5 * RectSize;
	int FontSize = RectSize * 0.6;
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Font->Size = FontSize;
	bitmap->Canvas->Stroke->Thickness = 8;
	for (int i = 0; i < sqrt(float(size)); i++) {
		for (int j = 0; j < sqrt(float(size)); j++) {
			int x_begin = i * RectSize + x;
			int y_begin = j * RectSize + y;
			int x_end = i * RectSize + RectSize + x;
			int y_end = j * RectSize + RectSize + y;
			bitmap->Canvas->DrawRect(TRectF(x_begin, y_begin, x_end, y_end),
				0, 0, AllCorners, 100);
			bitmap->Canvas->FillText(TRectF(x_begin, y_begin, x_end, y_end),
				IntToStr(numbers[5 * i + j]), false, 100, TFillTextFlags(),
				TTextAlign::Center, TTextAlign::Center);
		}
	}
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawSymbols(int array[], int FontSize)
{
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Font->Size = FontSize;
	bitmap->Canvas->Fill->Color = claWhite;
	int x_begin = 0;
	int y_begin = 0;
	int x_end = bitmap->Width/3;
	int y_end = bitmap->Height/2;
	for(int i = 0; i < 3; i++)
	{
		bitmap->Canvas->FillText(TRectF(x_begin, y_begin, x_end, y_end), (array[i] == 0)?"*":IntToStr(array[i]), false, 100,
			TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
		x_begin += bitmap->Width/3;
		x_end += bitmap->Width/3;
	}
	x_begin = 0;
	y_begin = bitmap->Height/2;
	x_end = bitmap->Width/4;
	y_end = bitmap->Height;
	for(int i = 0; i < 4; i++)
	{
		bitmap->Canvas->FillText(TRectF(x_begin, y_begin, x_end, y_end), (array[i+3] == 0)?"*":IntToStr(array[i+3]), false, 100,
			TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);
		x_begin += bitmap->Width/4;
		x_end += bitmap->Width/4;
	}
	bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::DrawSymArray(std::vector<std::string> array)
{
   bitmap->Canvas->Fill->Color = claWhite;

   int row = 2;
   int col = 4;
   int RectSize = Form->Height * 0.6 / row;
   int FontSize = RectSize * 0.6;
   int x = Form->Width / 2 - col/2 * RectSize;
   int y = Form->Height / 2 - row/2 * RectSize;
   bitmap->Canvas->Font->Size = FontSize;
   bitmap->Canvas->Font->Family = "Arial";

	System::Types::TRect rect;
    int idx = 0;
    bitmap->Canvas->BeginScene();
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int x_begin = j * RectSize + x;
			int y_begin = i * RectSize + y;
			int x_end = j * RectSize + RectSize + x;
			int y_end = i * RectSize + RectSize + y;
			rect = System::Types::TRect(x_begin, y_begin, x_end, y_end);
            bitmap->Canvas->Stroke->Thickness = 1;
			bitmap->Canvas->Stroke->Color = TAlphaColors::White;
			bitmap->Canvas->DrawRect(rect, 0, 0, AllCorners, 1);
			bitmap->Canvas->FillText(rect, array[idx++].c_str(), false, 1, TFillTextFlags(), TTextAlign::Center, TTextAlign::Center);

			System::Types::TRect bbox = GetBoundingBox(rect, UnicodeString(array[idx-1].c_str()), bitmap->Canvas->Font,TTextAlign::Center, TTextAlign::Center);
			bitmap->Canvas->Stroke->Thickness = 2;
			bitmap->Canvas->Stroke->Color = TAlphaColors::Red;
			bitmap->Canvas->DrawRect(bbox, 0, 0, AllCorners, 100);

		}
	}

   bitmap->Canvas->EndScene();
}
//--------------------------------------------------------------------------
void TBaseTask::SaveSettings()
{
	if(Parent) {
		Settings->Save(Parent->GetTaskName());
	} else {
		Settings->Save(TaskName);
		for(int i = 0; i < Blocks.size(); i++){
            Blocks[i]->SaveSettings();
		}
    }
}
//--------------------------------------------------------------------------
void TBaseTask::LoadSettings()
{
   	if(Parent) {
		Settings->Load(Parent->GetTaskName());
	} else {
		Settings->Load(TaskName);
		for(int i = 0; i < Blocks.size(); i++){
            Blocks[i]->LoadSettings();
		}
    }
}
//--------------------------------------------------------------------------
void TBaseTask::ClearCanva()
{
	bitmap->Clear(TAlphaColorRec::Black);
}
//--------------------------------------------------------------------------
void TBaseTask::ClearCanva(TAlphaColor color)
{
	bitmap->Clear(color);
}
//--------------------------------------------------------------------------
void TBaseTask::AddBlock(task_ptr block)
{
      Blocks.push_back(block);
}
//-------------------------------------------------------------------------
 std::vector<task_ptr> &TBaseTask::GetBlocks()
 {
	 return Blocks;
 }
//--------------------------------------------------------------------------
/*
long micros()
{
	boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	return current_date_microseconds.time_of_day().total_microseconds();
}
//--------------------------------------------------------------------------
long millis()
{
	boost::posix_time::ptime current_date_microseconds = boost::posix_time::microsec_clock::local_time();
	return current_date_microseconds.time_of_day().total_milliseconds();
}
*/
TBaseTask::chrono_time TBaseTask::midnight_time(chrono_time& now)
{
    time_t tnow = std::chrono::system_clock::to_time_t(now);
	tm* date = std::localtime(&tnow);
	date->tm_hour = 0;
	date->tm_min = 0;
	date->tm_sec = 0;
	return std::chrono::system_clock::from_time_t(std::mktime(date));
}
//-------------------------------------------------------------------------
unsigned int TBaseTask::millis()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - midnight_time(now)).count();
}
//-------------------------------------------------------------------------
unsigned int TBaseTask::micros()
{
   std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
   return std::chrono::duration_cast<std::chrono::microseconds>(now - midnight_time(now)).count();
}
//-------------------------------------------------------------------------
AnsiString TBaseTask::GetTaskName()
{
	return TaskName;
}
//-------------------------------------------------------------------------
settings_ptr TBaseTask::GetSettings()
{
	return Settings;
}
//-------------------------------------------------------------------------
bitmap_ptr TBaseTask::GetBitmap()
{
	return bitmap;
}
//-------------------------------------------------------------------------
protocol_ptr TBaseTask::GetProtocol()
{
	return Protocol;
}
//-------------------------------------------------------------------------
TTextMetric TBaseTask::GetTextMetrics(Fmx::Graphics::TFont* const Font)
{
    TTextMetric metric;
	HDC DC;
	HFONT tFont;

	DC = CreateCompatibleDC(0);

	int Height = -round(Font->Size * GetDCScale(DC));

	tFont = ::CreateFont(Height, 0, 0, 0, FontWeightToWinapi(Font->StyleExt.Weight),
	  Cardinal(1),
      0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	  DEFAULT_PITCH | FF_DONTCARE, Font->Family.w_str());

	SelectObject(DC, tFont);
	::GetTextMetrics(DC, &metric);

	DeleteObject(tFont);
	DeleteDC(DC);

	return metric;
}
//-------------------------------------------------------------------------
ftGlyphMetrics TBaseTask::GetGlyphMetrics(UnicodeString symbol, Fmx::Graphics::TFont* const Font)
{
   ftGlyphMetrics metrics = {0};

   TCHAR windir[MAX_PATH];
   GetWindowsDirectory(windir, MAX_PATH);

   AnsiString family = AnsiString(windir)+"\\Fonts\\"+Font->Family+".ttf";

   FT_Library ft;
   if (FT_Init_FreeType(&ft))
		return metrics;

   FT_Face face;
   if (FT_New_Face(ft, family.c_str(), 0, &face))
		return metrics;

   FT_Set_Pixel_Sizes(face, 0, Font->Size);

   if (FT_Load_Char(face, symbol.w_str()[0], FT_LOAD_RENDER))
		return metrics;

   metrics.width = face->glyph->bitmap.width;
   metrics.height = face->glyph->bitmap.rows;
   metrics.bearingY = face->glyph->bitmap_top;
   metrics.bearingX = face->glyph->bitmap_left;
   metrics.advance = face->glyph->advance.x;

   return metrics;
}
//-------------------------------------------------------------------------
System::Types::TRect TBaseTask::GetBoundingBox(System::Types::TRect textRect, UnicodeString symbol, Fmx::Graphics::TFont* const Font, const Fmx::Types::TTextAlign ATextAlign, const Fmx::Types::TTextAlign AVTextAlign)
{
   TRect rect;
   int x_offset = 0;
   int y_offset = 0;
   TTextMetric tMetric = GetTextMetrics(Font);
   ftGlyphMetrics gMetric = GetGlyphMetrics(symbol, Font);
   int BaseLine = TFontGlyphManager::Current->GetBaseline(Font,bitmap->Canvas->Scale) + tMetric.tmExternalLeading;

   if(ATextAlign == TTextAlign::Center)
	x_offset = (textRect.Right - textRect.Left)/2 - (gMetric.advance/(64*2));
   if(AVTextAlign == TTextAlign::Center)
   	y_offset = ((textRect.bottom - textRect.top) - (tMetric.tmHeight + tMetric.tmExternalLeading))/2;


   rect.Left = textRect.Left + gMetric.bearingX + x_offset;
   rect.Top = textRect.Top + (BaseLine - gMetric.bearingY) + y_offset;
   rect.Right = textRect.Left + gMetric.bearingX + gMetric.width + x_offset;
   rect.Bottom =  textRect.Top + BaseLine + (gMetric.height - gMetric.bearingY) + y_offset;
   return rect;
}
//-------------------------------------------------------------------------
TBaseTask::~TBaseTask()
{
}
//------------------------------------------------------------------------------
