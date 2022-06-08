//---------------------------------------------------------------------------

#ifndef LabStreamingLayerH
#define LabStreamingLayerH
//---------------------------------------------------------------------------
#include <fmx.h>
#include <chrono>
#include <thread>
#include <lsl_cpp.h>
#include <map>
#include "TProtocol.h"

class TLabStreamingLayer;

 typedef std::vector<std::map<std::string, std::string>> samples_t;
 typedef std::shared_ptr<TLabStreamingLayer> lsl_ptr;

 class TLabStreamingLayer
 {
 public:
	 TLabStreamingLayer();

	 void SetSubjectInfo(SubjectInfo& subject);
	 void start(samples_t& lsl_samples);
     void stop();
	 void init_meta(lsl::xml_element e);
	 void sendSample(std::string data);
	 void sendSample(int data);
	 void sendFeedback(float data);

     double get_clock();

	 // Lab recorder cli
	 void start_rec(AnsiString Path);
     void stop_rec();

	 ~TLabStreamingLayer();

 private:
	 std::shared_ptr<lsl::stream_outlet> outletSample;
	 std::shared_ptr<lsl::stream_outlet> outletFeedback;

     PROCESS_INFORMATION pi = { };
	 HANDLE hStdInPipeRead = NULL;
	 HANDLE hStdInPipeWrite = NULL;
	 HANDLE hStdOutPipeRead = NULL;
	 HANDLE hStdOutPipeWrite = NULL;

	 SubjectInfo _subject;

     bool isRecord = false;
 };



#endif
