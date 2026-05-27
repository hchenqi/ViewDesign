#pragma once

#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"


namespace ViewDesign {


template<size_trait WidthTrait, size_trait HeightTrait>
view_ptr<WidthTrait, HeightTrait> TextViewAdapter(std::unique_ptr<ViewBase> view);

template<size_trait WidthTrait, size_trait HeightTrait>
view_ptr<WidthTrait, HeightTrait> TextViewAdapter(owner_ptr<ViewBase> view) { return TextViewAdapter<WidthTrait, HeightTrait>(std::unique_ptr<ViewBase>(view)); }


template<>
inline view_ptr<Relative, Relative> TextViewAdapter<Relative, Relative>(std::unique_ptr<ViewBase> view) {
	return view;
}

template<>
inline view_ptr<Auto, Auto> TextViewAdapter<Auto, Auto>(std::unique_ptr<ViewBase> view) {
	return create<MaxFrame<Auto, Auto>>(size_infinite, std::move(view));
}

template<>
inline view_ptr<Relative, Auto> TextViewAdapter<Relative, Auto>(std::unique_ptr<ViewBase> view) {
	return create<MaxFrame<Relative, Auto>>(length_infinite, std::move(view));
}

template<>
inline view_ptr<Auto, Relative> TextViewAdapter<Auto, Relative>(std::unique_ptr<ViewBase> view) {
	return create<MaxFrame<Auto, Relative>>(length_infinite, std::move(view));
}

template<>
inline view_ptr<Relative, Fixed> TextViewAdapter<Relative, Fixed>(std::unique_ptr<ViewBase> view) {
	return create<ClipFrame<Relative, Fixed, Top>>(std::move(view));
}

template<>
inline view_ptr<Fixed, Relative> TextViewAdapter<Fixed, Relative>(std::unique_ptr<ViewBase> view) {
	return create<ClipFrame<Fixed, Relative, Left>>(std::move(view));
}

template<>
inline view_ptr<Auto, Fixed> TextViewAdapter<Auto, Fixed>(std::unique_ptr<ViewBase> view) {
	return create<ClipFrame<Auto, Fixed, Top>>(TextViewAdapter<Auto, Relative>(std::move(view)));
}

template<>
inline view_ptr<Fixed, Auto> TextViewAdapter<Fixed, Auto>(std::unique_ptr<ViewBase> view) {
	return create<ClipFrame<Fixed, Auto, Left>>(TextViewAdapter<Relative, Auto>(std::move(view)));
}

template<>
inline view_ptr<Fixed, Fixed> TextViewAdapter<Fixed, Fixed>(std::unique_ptr<ViewBase> view) {
	return create<ClipFrame<Fixed, Fixed, TopLeft>>(std::move(view));
}


} // namespace ViewDesign
