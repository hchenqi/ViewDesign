#pragma once

#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/CenterFrame.h"


namespace ViewDesign {


template<size_trait WidthTrait, size_trait HeightTrait>
view_ptr<WidthTrait, HeightTrait> TextViewAdapter(view_ptr<Bounded, Bounded> view);


template<>
inline view_ptr<Bounded, Bounded> TextViewAdapter<Bounded, Bounded>(view_ptr<Bounded, Bounded> view) {
	return view;
}

template<>
inline view_ptr<Auto, Auto> TextViewAdapter<Auto, Auto>(view_ptr<Bounded, Bounded> view) {
	return create<MaxFrame<Auto, Auto>>(size_infinite, std::move(view));
}

template<>
inline view_ptr<Bounded, Auto> TextViewAdapter<Bounded, Auto>(view_ptr<Bounded, Bounded> view) {
	return create<MaxFrame<Bounded, Auto>>(length_infinite, std::move(view));
}

template<>
inline view_ptr<Auto, Bounded> TextViewAdapter<Auto, Bounded>(view_ptr<Bounded, Bounded> view) {
	return create<MaxFrame<Auto, Bounded>>(length_infinite, std::move(view));
}

template<>
inline view_ptr<Fixed, Fixed> TextViewAdapter<Fixed, Fixed>(view_ptr<Bounded, Bounded> view) {
	return create<CenterFrame<Fixed, Fixed>>(std::move(view));
}

template<>
inline view_ptr<Bounded, Fixed> TextViewAdapter<Bounded, Fixed>(view_ptr<Bounded, Bounded> view) {
	return create<CenterFrame<Bounded, Fixed>>(std::move(view));
}

template<>
inline view_ptr<Fixed, Bounded> TextViewAdapter<Fixed, Bounded>(view_ptr<Bounded, Bounded> view) {
	return create<CenterFrame<Fixed, Bounded>>(std::move(view));
}

template<>
inline view_ptr<Fixed, Auto> TextViewAdapter<Fixed, Auto>(view_ptr<Bounded, Bounded> view) {
	return create<CenterFrame<Fixed, Auto>>(TextViewAdapter<Bounded, Auto>(std::move(view)));
}

template<>
inline view_ptr<Auto, Fixed> TextViewAdapter<Auto, Fixed>(view_ptr<Bounded, Bounded> view) {
	return create<CenterFrame<Auto, Fixed>>(TextViewAdapter<Auto, Bounded>(std::move(view)));
}


} // namespace ViewDesign
