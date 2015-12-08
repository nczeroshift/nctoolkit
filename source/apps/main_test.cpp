
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckWindow.h"
#include "nckThread.h"
#include "nckGraphics.h"
#include "nckVec3.h"
#include "nckMat44.h"
#include "nckException.h"
#include "nckUtils.h"
#include "nckTimeline.h"

//
//template < typename T >
//class TimelineItem {
//public:
//	TimelineItem(float start, float end, T object) {
//		mStart = start;
//		mEnd = end;
//		mObject = object;
//	}
//
//	~TimelineItem() {
//
//	}
//
//	float GetStart() const {
//		return mStart;
//	}
//
//	float GetEnd() const {
//		return mEnd;
//	}
//
//	T GetObject() const {
//		return mObject;
//	}
//private:
//	T mObject;
//	float mStart;
//	float mEnd;
//};
//
//template < typename T >
//class TimelineNode {
//public:
//	TimelineNode() {
//		mA = NULL;
//		mB = NULL;
//		mStart = 0.0;
//		mEnd = 0.0;
//	}
//
//
//	TimelineNode(float start, float end) {
//		mA = NULL;
//		mB = NULL;
//		mStart = start;
//		mEnd = end;
//	}
//
//	~TimelineNode() {
//		SafeDelete(mA);
//		SafeDelete(mB);
//	}
//
//	float GetStart() const {
//		return mStart;
//	}
//
//	float GetEnd() const {
//		return mEnd;
//	}
//
//	void Insert(TimelineItem<T> item) {
//		mItems.push_back(item);
//	}
//
//	void AddToList(std::list<TimelineItem<T>> * list) {
//		Core::ListFor(TimelineItem, mItem, i) {
//			list->push_back(*i);
//		}
//	}
//
//	bool IsEmpty() const {
//		return mItems.size() == 0 && mA == NULL && mB == NULL;
//	}
//
//	void Build(const int depth = 0, int const maxDepth = 4) {
//		bool inited = false;
//		float s, e;
//		
//		if (mItems.size() == 0)
//			return;
//
//		ListFor(TimelineItem<T>, mItems, i) {
//			if (!inited) {
//				s = i->GetStart();
//				e = i->GetEnd();
//				inited = true;
//			}
//			else {
//				s = MIN(s, i->GetStart());
//				e = MAX(s, i->GetEnd());
//			}
//		}
//
//		mStart = s;
//		mEnd = e;
//
//		float split = (s + e) / 2.0f;
//
//		if (depth + 1 < maxDepth) {
//			mA = new TimelineNode<T>(mStart, split);
//			mB = new TimelineNode<T>(split, mEnd);
//
//			std::list<TimelineItem<T>> keep;
//			ListFor(TimelineItem<T>, mItems, i) {
//				if (i->GetStart() >= mA->GetStart() && i->GetEnd() <= mA->GetEnd())
//					mA->Insert(*i);
//				else if (i->GetStart() >= mB->GetStart() && i->GetEnd() <= mB->GetEnd())
//					mB->Insert(*i);
//				else
//					keep.push_back(*i);
//			}
//
//			mA->Build(depth + 1, maxDepth);
//			mB->Build(depth + 1, maxDepth);
//
//			if (mA->IsEmpty()) 
//				SafeDelete(mA);
//			
//			if (mB->IsEmpty()) 
//				SafeDelete(mB);
//
//			mItems.clear();
//			mItems = keep;
//		}
//	}
//
//private:
//	std::list<TimelineItem<T>> mItems;
//	float mStart;
//	float mEnd;
//	TimelineNode<T> * mA, * mB;
//};

class GraphicRendering : public virtual Core::Threadable
{
public:
	GraphicRendering(Core::Window * window) : Threadable(){
		wnd = window;
		dev = NULL;
	}
	
	~GraphicRendering(){
	}
	
	void Run(){
		wnd->SetTitle("nctoolkit");
		
		try{
			dev = Graph::CreateDevice(wnd,Graph::DEVICE_AUTO,wnd->GetWidth(),wnd->GetHeight());
			dev->ClearColor(0.5,0.5,0.5,1);
			dev->ClearFlags(Graph::BUFFER_COLOR_BIT|Graph::BUFFER_DEPTH_BIT);
		}
		catch(const Core::Exception & ex){
			ex.PrintStackTrace();
			SafeDelete(dev);
			return;
		}

		Math::TimelineNode<float> items;
		items.Insert(Math::TimelineItem<float>(0, 10, 1.0));
		items.Insert(Math::TimelineItem<float>(8, 9.5, 2.0,1));
		items.Insert(Math::TimelineItem<float>(10, 20, 1.0));
		items.Build();

		std::list<Math::TimelineItem<float>> res;
		items.Get(9, &res);


		while(!IsTearingDown())
		{
			dev->Clear();

			dev->PresentAll();
		}

		SafeDelete(dev);
	}

	void UpdateWndInput()
	{

		if (wnd->GetKeyStatus(Core::BUTTON_ESCAPE) != Core::BUTTON_STATUS_UP)
			Teardown();

	}

	void SelectDemo(int demoId)
	{
		
	}

private:
	Graph::Device	*dev;
	Core::Window	*wnd;
};

void Core::Application_Main(const std::vector<std::string> & CmdLine)
{
	// Create a window.
	Core::Window * wnd = Core::CreateWindow("nctoolkit", 1024, 768, false);

	// Create the thread where the graphics rendering will run.
	GraphicRendering * grThread = new GraphicRendering(wnd);

	// Start graphics thread.
	grThread->Start();

	// Get window events.
	while(wnd->Peek(true))
	{
		// If on selector frontend, escape will tell
		// the thread to enter shutdown.
		if(!grThread->IsRunning())
			break;

		grThread->UpdateWndInput();
	}

	// Tell the thread to end and wait.
	grThread->TearDownAndJoin();

	// Release stuff from memory.
	SafeDelete(grThread);
	SafeDelete(wnd);
}

