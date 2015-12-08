
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_TIMELINE_H
#define NCK_TIMELINE_H

#include "nckBoundBox.h"
#include <list>
#include <vector>

_MATH_BEGIN

template < typename T > class TimelineItem {
public:
	TimelineItem(float start, float end, T object, int layer = 0) {
		mStart = start;
		mEnd = end;
		mObject = object;
		mLayer = layer;
	}
	~TimelineItem() {}
	float GetStart() const {
		return mStart;
	}
	float GetEnd() const {
		return mEnd;
	}
	int GetLayer() const {
		return mLayer;
	}
	T GetObject() const {
		return mObject;
	}
private:
	int mLayer;
	T mObject;
	float mStart;
	float mEnd;
};

template < typename T > class TimelineNode {
public:
	TimelineNode() {
		mDepth = 0;
		mA = NULL;
		mB = NULL;
		mStart = 0.0;
		mEnd = 0.0;
	}


	TimelineNode(float start, float end) {
		mDepth = 0;
		mA = NULL;
		mB = NULL;
		mStart = start;
		mEnd = end;
	}

	~TimelineNode() {
		SafeDelete(mA);
		SafeDelete(mB);
	}

	float GetStart() const {
		return mStart;
	}

	float GetEnd() const {
		return mEnd;
	}

	void Insert(TimelineItem<T> item) {
		mItems.push_back(item);
	}
		
	bool IsEmpty() const {
		return mItems.size() == 0 && mA == NULL && mB == NULL;
	}


	void Get(float time, std::list<TimelineItem<T>> * items) {

		ListFor(TimelineItem<T>, mItems, i) {
			if (time >= i->GetStart() && time <= i->GetEnd()) {
				items->push_back(*i);
			}
		}

		if (mA != NULL)
			mA->Get(time, items);

		if (mB != NULL)
			mB->Get(time, items);

		(*items).sort(compareItemsLayer);
	}

	void Build(const int depth = 0, int const maxDepth = 4) {
		bool inited = false;
		float s, e;

		mDepth = depth;

		if (mItems.size() == 0)
			return;

		ListFor(TimelineItem<T>, mItems, i) {
			if (!inited) {
				s = i->GetStart();
				e = i->GetEnd();
				inited = true;
			}
			else {
				s = MIN(s, i->GetStart());
				e = MAX(s, i->GetEnd());
			}
		}

		mStart = s;
		mEnd = e;

		float split = (s + e) / 2.0f;

		if (depth + 1 < maxDepth) {
			mA = new TimelineNode<T>(mStart, split);
			mB = new TimelineNode<T>(split, mEnd);

			std::list<TimelineItem<T>> keep;
			ListFor(TimelineItem<T>, mItems, i) {
				if (i->GetStart() >= mA->GetStart() && i->GetEnd() <= mA->GetEnd())
					mA->Insert(*i);
				else if (i->GetStart() >= mB->GetStart() && i->GetEnd() <= mB->GetEnd())
					mB->Insert(*i);
				else
					keep.push_back(*i);
			}

			mA->Build(depth + 1, maxDepth);
			mB->Build(depth + 1, maxDepth);

			if (mA->IsEmpty())
				SafeDelete(mA);

			if (mB->IsEmpty())
				SafeDelete(mB);

			mItems.clear();
			mItems = keep;
		}
	}

private:
	static bool compareItemsLayer(const TimelineItem<T> & a, const TimelineItem<T> & b) {
		return a.GetLayer() < b.GetLayer();
	}
	int mDepth;
	std::list<TimelineItem<T>> mItems;
	float mStart;
	float mEnd;
	TimelineNode<T> * mA, *mB;
};

_MATH_END

#endif // #ifndef NCK_TIMELINE_H
