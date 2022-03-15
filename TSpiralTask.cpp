//---------------------------------------------------------------------------

#pragma hdrstop

#include "TSpiralTask.h"
#include "DrawWin.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TSpiralTask::TSpiralTask(AnsiString _name) : TBaseTask(_name)
{
	isDrawLine = false;
	isTrialRun = false;

    isTrialFinish = false;

	Spiral.StartPoint = TPointF(0, 0);
	Spiral.FinalPoint = TPointF(0, 0);

	if(!Settings->Load(_name)){
		Settings->Add(LineThick, "Толщина ведомой линии (mm)", GuiType::TEdit, 4);
		Settings->Add(SpiralThick, "Толщина спирали (mm)", GuiType::TEdit, 4);
		Settings->Add(TrialMaxCount, "Кол. триалов", GuiType::TEdit, 2);
		Settings->Add(ExternalSpiralRadius, "Внешний радиус спирали (см)", GuiType::TEdit, 8);
		Settings->Add(LineColor, "Цвет ведомой линии", GuiType::TComboColorBox, (int)TAlphaColorRec::Lime);
		Settings->Add(StartPointColor, "Цвет точки начала", GuiType::TComboColorBox, (int)TAlphaColorRec::Red);
		Settings->Add(SpiralColor, "Цвет спирали", GuiType::TComboColorBox, (int)TAlphaColorRec::White);
		Settings->Add(EndPointColor, "Цвет точки конца", GuiType::TComboColorBox, (int)0xFF73BCF0);
		Settings->Add(MonitorDiagonal, "Диагональ монитора", GuiType::TEdit, 23.6);
		Settings->Add(ShowResults, "Показывать результат", GuiType::TSwitch, 0);
		Settings->Add(TurnsCount, "Количество витков", GuiType::TEdit, 3);
		Settings->Add(InternalSpiralRadius, "Внутренний радиус спирали (см)", GuiType::TEdit, 1.5);
		Settings->Save(_name);
	}

   StartTimer = new TTimer(NULL);
   StartTimer->OnTimer = &TimerEvent;
   StartTimer->Enabled = false;

}
// --------------------------------------------------------------------------
bool TSpiralTask::Finished()
{
	if (Settings->getInt(TrialMaxCount) * 4 == CurrentTrial + 1) {
		//Logger.SaveLogFiles();

		if(Settings->getInt(ShowResults) == 1)
		{
			steps = RESULT;
			return false;
		}

		return true;
	}

	return false;
}

// --------------------------------------------------------------------------
void TSpiralTask::UserClick() { }
// --------------------------------------------------------------------------
void TSpiralTask::DrawUserLine(TPointF point)
{
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Color = Settings->getInt(LineColor);
	bitmap->Canvas->Stroke->Thickness = Settings->getInt(LineThick) / pixelSize;
	bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;
	bitmap->Canvas->DrawLine(CurrentPoint, point, 2);
	bitmap->Canvas->EndScene();
}

// --------------------------------------------------------------------------
void TSpiralTask::UserMouseMove(int X, int Y)
{
	if (isTrialRun && isDrawLine) {

		if (abs(Spiral.FinalPoint.x - X) < 8 && abs(Spiral.FinalPoint.y - Y) < 8) {
			isDrawLine = false;
			PlaySound(L"beep-09.wav", 0, SND_ASYNC);

            isTrialFinish = true;

			Timer->Interval = 1000;
			Timer->Enabled = true;
		}

		if (pow((Spiral.StartPoint.x - X),2)+pow((Spiral.StartPoint.y - Y),2) > 64/*abs(Spiral.StartPoint.x - X) > 8 || abs(Spiral.StartPoint.y - Y) > 8*/)
		{
            DrawUserLine(TPointF(X, Y));
			AddLog(millis(), X, Y);
		}

	}
	CurrentPoint = TPointF(X, Y);
}

//---------------------------------------------------------------------------
void __fastcall TSpiralTask::TimerEvent(TObject *Sender)
{
	if (abs(TargetPoint.X - CurrentPoint.X) < 8 && abs(TargetPoint.Y - CurrentPoint.Y) < 8)
	{
		PlaySound(L"beep-07a.wav", 0, SND_ASYNC);
		isTrialRun = true;
	}

	StartTimer->Enabled = false;
}

// --------------------------------------------------------------------------
void TSpiralTask::UserMouseDown(int X, int Y, TMouseButton Button)
{
	if (abs(TargetPoint.X - X) < 8 && abs(TargetPoint.Y- Y) < 8) {
		isDrawLine = true;

		if(isTrialRun == false) StartTimer->Enabled = true;

	}

	CurrentPoint = TPointF(X, Y);
}

// --------------------------------------------------------------------------
void TSpiralTask::UserMouseUp(int X, int Y)
{
	if(StartTimer->Enabled) StartTimer->Enabled = false;


	if(isTrialRun && isDrawLine) TargetPoint = TPointF(X, Y);
	isDrawLine = false;

	//AddLog(millis(), -1, -1);
}
//--------------------------------------------------------------------------
void TSpiralTask::UserTouch(const TTouches Touches, const TTouchAction Action)
{
    TMouseButton Button;

	if (Touches.Length == 1 && !isTrialRun && Action == TTouchAction::Down) {
		UserMouseDown(Touches[0].Location.X, Touches[0].Location.Y, Button);
	}

	if(Touches.Length == 1 && Action == TTouchAction::Up) {
	   UserMouseUp(Touches[0].Location.X, Touches[0].Location.Y);
	}
}
//---------------------------------------------------------------------------
void TSpiralTask::Draw()
{
	 TRectF MyRect(0, 0, bitmap->Width, bitmap->Height);
	 Canvas->DrawBitmap(bitmap.get(), MyRect, MyRect, 1, false);

     DrawPoints();

	 Canvas->Stroke->Thickness = 3;
	 Canvas->Stroke->Color = TAlphaColorRec::Khaki;
	 if(isDrawLine) Canvas->DrawLine(CurrentPoint, HelpCurrentPoint , 1);
	 else Canvas->DrawLine(TargetPoint, HelpCurrentPoint , 1);
}
// --------------------------------------------------------------------------
void TSpiralTask::DrawCircle(TAlphaColor color, int r, TPointF point)
{
	TRectF MyRect(point.x - r, point.y - r, point.x + r, point.y + r);

	Canvas->Stroke->Color = color;
	// Bitmap->Canvas->Stroke->Kind = TBrushKind::Solid;
	// Bitmap->Canvas->Stroke->Dash = TStrokeDash::Solid;
	Canvas->Stroke->Thickness = 4;
	Canvas->Fill->Color = color;

	Canvas->FillEllipse(MyRect, 1);
	Canvas->DrawEllipse(MyRect, 1);
}

// --------------------------------------------------------------------------
void TSpiralTask::DrawPoints()
{
	if (steps == SPIRAL)
	{
		DrawCircle(Settings->getInt(StartPointColor), 12, Spiral.StartPoint);

		if(isTrialFinish) {
			DrawCircle(Settings->getInt(EndPointColor), 12, Spiral.FinalPoint);
		}
	}
}
//---------------------------------------------------------------------------
void TSpiralTask::DrawSpiral()
{
	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Thickness = Settings->getInt(SpiralThick) / pixelSize;
	bitmap->Canvas->Stroke->Color = Settings->getInt(SpiralColor);
	bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;
	// Bitmap->Canvas->Stroke->Kind = TBrushKind::Solid;
	// Bitmap->Canvas->Stroke->Dash = TStrokeDash::Solid;

	for(int i = 1; i < spiral_points.size(); i++)
	{
		bitmap->Canvas->DrawLine(spiral_points[i-1], spiral_points[i], 1);
	}
    bitmap->Canvas->EndScene();
}
// --------------------------------------------------------------------------
void TSpiralTask::CalcSpiral()
{
    spiral_points.clear();

	if (Spiral.MoveDir == OUTSIDE) {
		Spiral.StartPoint = GetSpiral(Spiral.Dir, Spiral.ExternalAngle, Spiral.CenterPoint, Spiral.ExternalRadius);
		Spiral.FinalPoint = GetSpiral(Spiral.Dir, Spiral.InternalAngle, Spiral.CenterPoint, Spiral.ExternalRadius);
	}

	if (Spiral.MoveDir == INSIDE) {
		Spiral.StartPoint = GetSpiral(Spiral.Dir, Spiral.InternalAngle, Spiral.CenterPoint, Spiral.ExternalRadius);
		Spiral.FinalPoint = GetSpiral(Spiral.Dir, Spiral.ExternalAngle, Spiral.CenterPoint, Spiral.ExternalRadius);
	}

	float a = Spiral.InternalAngle;

	TPointF p1 = GetSpiral(Spiral.Dir, a, Spiral.CenterPoint, Spiral.ExternalRadius);
    HelpCurrentPoint = Spiral.StartPoint;
	spiral_points.push_back(p1);

	do {

		TPointF p2 = GetSpiral(Spiral.Dir, a, Spiral.CenterPoint, Spiral.ExternalRadius);
        spiral_points.push_back(p2);
		p1 = p2;

		a += 0.05;
	} while (a <= Settings->getDouble(TurnsCount)*2*M_PI); // Добавить количество витков

}
//--------------------------------------------------------------------------
TPointF TSpiralTask::GetSpiral(int dir, float a, TPointF p1, float r) {
	TPointF ret;
	ret.x = p1.x + (r * a * sin(a));
	ret.y = p1.y + (r * a * cos(a))*dir;
	return ret;
}
//--------------------------------------------------------------------------
void TSpiralTask::InitTrialSequence()
{
	TrialSequence.clear();

	do {
		int trial = RandomRange(0, 4);
		int count = std::count(TrialSequence.begin(), TrialSequence.end(), trial);

		if (count < Settings->getInt(TrialMaxCount))
			TrialSequence.push_back(trial);
	} while (TrialSequence.size() < Settings->getInt(TrialMaxCount) * 4);

}

//--------------------------------------------------------------------------
void TSpiralTask::InitTask(AnsiString Path)
{
	steps = BLANK;
    LogPath = Path;

	InitTrialSequence();
	InitLogFiles();
	CurrentTrial = -1;

	Timer->Interval = 2000;
	Timer->Enabled = true;

	// Calc Pixel Size
	float D = Settings->getDouble(MonitorDiagonal) * 2.54;
	MonitorH = Form->Height;
	MonitorW = Form->Width;
	float k =  (MonitorW / MonitorH)*(MonitorW / MonitorH);
	float H = sqrt(D*D/(k+1));
	float W = sqrt(D*D/(1/k+1));
	pixelSize = H / MonitorH * 10; // mm

}

// --------------------------------------------------------------------------
void TSpiralTask::StateManager()
{
	if (steps == SPIRAL) {
		ClearCanva();
		steps = BLANK;
		Timer->Interval = RandomRange(1500, 2500);
	} else if (steps == BLANK) {
		steps = SPIRAL;

		CurrentTrial++;

		Spiral.ExternalRadius = Settings->getDouble(ExternalSpiralRadius) / (pixelSize/10.0*Settings->getInt(TurnsCount)*2*M_PI);
		Spiral.ExternalAngle = Settings->getDouble(TurnsCount)*2*M_PI;
		Spiral.InternalAngle = Settings->getDouble(InternalSpiralRadius) / (pixelSize/10.0*Spiral.ExternalRadius);
		Spiral.CenterPoint = TPointF(Form->Width / 2, Form->Height / 2);
		Spiral.Dir = FORWARD;//((TrialSequence[CurrentTrial] & 1u) == 0)?  FORWARD : REVERSE;
		Spiral.MoveDir = OUTSIDE;//static_cast<MotionDirection>((TrialSequence[CurrentTrial] >> 1) & 1u);
		Spiral.Type = 2;//TrialSequence[CurrentTrial];

		CalcSpiral();
        DrawSpiral();
		TargetPoint = Spiral.StartPoint;

		Timer->Enabled = false;
	} else if (steps == RESULT) {
	   //ClearCanva(TAlphaColorRec::White);
	   //DrawText("Результаты!", 150);

	}

	isTrialRun = false;
}
// -------------------------------------------------------------------------
void TSpiralTask::CloseTask()
{
    bitmap->SaveToFile("lol.PNG");
	SaveLogFiles();
}
//--------------------------------------------------------------------------
void TSpiralTask::DrawHelpSpiral(float angle)
{
    TPointF point = GetSpiral(Spiral.Dir, fabs(angle), Spiral.CenterPoint, Spiral.ExternalRadius);

	bitmap->Canvas->BeginScene();
	bitmap->Canvas->Stroke->Thickness = Settings->getInt(SpiralThick) / pixelSize;
	bitmap->Canvas->Stroke->Color = TAlphaColorRec::Blue;
	bitmap->Canvas->Stroke->Cap = TStrokeCap::Round;
	bitmap->Canvas->DrawLine(HelpCurrentPoint, point , 1);
	bitmap->Canvas->EndScene();

    HelpCurrentPoint = point;
}
//--------------------------------------------------------------------------
void TSpiralTask::InitLogFiles()
{
	ClearLogFiles();

	CreateDir(LogPath + "\\Type 1");
	CreateDir(LogPath + "\\Type 2");
	CreateDir(LogPath + "\\Type 3");
	CreateDir(LogPath + "\\Type 4");

	for (int i = 0; i < TrialSequence.size(); i++) {
		TStringList* list = new TStringList;
		log_files.push_back(list);

        trial_data tData;
		trials.push_back(tData);
	}
}
//--------------------------------------------------------------------------
void TSpiralTask::ClearLogFiles()
{
	for (int i = 0; i < log_files.size(); i++)
		delete log_files[i];

	log_files.clear();
}
//--------------------------------------------------------------------------
void TSpiralTask::SaveLogFiles()
{
	AnsiString patch = "";
	for (int i = 0; i < TrialSequence.size(); i++) {
		patch = LogPath + "\\Type " + IntToStr(TrialSequence.at(i) + 1) + "\\";
		log_files[i]->SaveToFile(patch + "G" + IntToStr(i + 1) + ".txt");

		/*
		TStringList* list = new TStringList;
		for(int j = 0; j < trials[i].size(); j++)
		{
		   TrialData data = trials[i][j];
		   list->Add(FloatToStr(data.phi) + " " + FloatToStr(data.ro));
        }
		list->SaveToFile(patch + "Polar" + IntToStr(i + 1) + ".txt");
		delete list;
		*/
	}
}
//---------------------------------------------------------------------------
void TSpiralTask::AddLog(unsigned int time, int X, int Y)
{
	log_files[CurrentTrial]->Add(UIntToStr(time) + " " + FloatToStr(X) + " " + FloatToStr(Y));

	if(X == -1 && Y == -1) return;

	TrialData data(time, X - MonitorW/2, (Y - MonitorH/2)*-1);

	TPointF start_point(Spiral.StartPoint.x- MonitorW/2, -(Spiral.StartPoint.y - MonitorH/2));

	float start_angle = 0;
	if(Spiral.Type == 0) start_angle = -Spiral.InternalAngle;
	if(Spiral.Type == 1) start_angle = Spiral.ExternalAngle;
	if(Spiral.Type == 2) start_angle = -Spiral.ExternalAngle; // !
	if(Spiral.Type == 3) start_angle = Spiral.ExternalAngle;

	//Calc polar coord  -----------------------------------------------
	static float relative_angle[2];
	static float cur_angle;
    static int flag;

	PolarCoord coord1 = car2pol(data.X, data.Y);

	if(trials[CurrentTrial].size() == 0) {
		relative_angle[1] = calc_relative_angle(data.X, data.Y, start_point.x, start_point.y, relative_angle[0]);

		if(relative_angle[1] > 0)
            data.phi = calc_relative_angle(data.X, data.Y, start_point.x, start_point.y, 0) + start_angle;
		else
			data.phi = start_angle;

		data.ro = coord1.ro;
        trials[CurrentTrial].push_back(data);

        relative_angle[0] = relative_angle[1];
		return;
    }

	TrialData data0 = trials[CurrentTrial].back();
	relative_angle[1] = calc_relative_angle(data.X, data.Y, data0.X, data0.Y, relative_angle[0]);

	if(data0.phi == start_angle)
	{
	   if(relative_angle[1] > 0)
	   {
		   data.phi = calc_relative_angle(data.X, data.Y, start_point.x, start_point.y, 0) + start_angle;
		   cur_angle = relative_angle[1];
	   }
	   else
	   {
		   data.phi = start_angle;
	   }
	}
	else
	{
	   float min_angle = -(start_angle + Spiral.InternalAngle);
	   if(relative_angle[1] > cur_angle && relative_angle[1] < min_angle)
	   {
		  if(flag == 0)
		  {
			 data.phi = calc_relative_angle(data.X, data.Y, data0.X, data0.Y , data0.phi);
          }
		  else
		  {
			 data.phi = data0.phi + (relative_angle[1] - cur_angle);
			 flag = 0;
		  }
		  cur_angle = relative_angle[1];
       }
	   else
	   {
		   data.phi = data0.phi;
		   flag = 1;
       }
	}

    data.ro = coord1.ro;
	trials[CurrentTrial].push_back(data);
	relative_angle[0] = relative_angle[1];




	//TForm2 *form = static_cast<TForm2*>(Form);

	/*
	if(tType != old_tType)
	{
      	form->Series1->Clear();
		form->Chart1->UndoZoom();
		old_tType = tType;
	}
	*/


	//float err = fabs((data.phi)*Spiral.ExternalRadius) - data.ro;
	//form->Series1->Add(err);

	DrawHelpSpiral(data.phi);
}
//---------------------------------------------------------------------------
float TSpiralTask::calc_relative_angle(float x1, float y1, float x0, float y0, float base_angle)
{
	float dPhi = car2pol(x1,y1).phi - car2pol(x0,y0).phi;
	if(fabs(dPhi) < M_PI)
		return (base_angle + dPhi);
	else
	{
	  if(dPhi > 0)
		  return (base_angle + dPhi - 2*M_PI);
	  else
		  return (base_angle + dPhi + 2*M_PI);
	}
}
//---------------------------------------------------------------------------
PolarCoord TSpiralTask::car2pol(float x, float y)
{
	PolarCoord ret;
	ret.ro = sqrt(x*x + y*y);

	if(ret.ro == 0) ret.phi = 0;
	else
	{
		ret.phi = acos(x/ret.ro);
		if(y < 0) ret.phi = 2*M_PI - ret.phi;
	}
    return ret;
}
//---------------------------------------------------------------------------
TSpiralTask::~TSpiralTask()
{

}



