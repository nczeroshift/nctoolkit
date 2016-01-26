
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckTimeline.h"

_MATH_BEGIN
 
template < class T > 
TimelineItem<T>::TimelineItem(float start, float end, T object, int layer) {
	mStart = start;
	mEnd = end;
	mObject = object;
	mLayer = layer;
}

template < class T >
TimelineItem<T>::~TimelineItem(){
}

template < class T >
float TimelineItem<T>::GetStart() const{
	return mStart;
}

template < class T >
float TimelineItem<T>::GetEnd() const{
	return mEnd;
}

template<class T>
int TimelineItem<T>::GetLayer() const{
	return mLayer;
}

template < class T >
T TimelineItem<T>::GetObject() const {
	return mObject;
}

_MATH_END
