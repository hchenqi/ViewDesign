#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/control/TextBox.h"


using namespace ViewDesign;


template<class WidthTrait, class HeightTrait>
view_ptr<WidthTrait, HeightTrait> WrapTextBox(alloc_ptr<TextBox> text_box);

template<>
view_ptr<Fixed, Fixed> WrapTextBox<Fixed, Fixed>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Fixed, TopLeft>(text_box);
}

template<>
view_ptr<Fixed, Auto> WrapTextBox<Fixed, Auto>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Auto, Left>(text_box);
}

template<>
view_ptr<Fixed, Relative> WrapTextBox<Fixed, Relative>(alloc_ptr<TextBox> text_box) {
	return WrapTextBox<Fixed, Auto>(text_box);
}

template<>
view_ptr<Auto, Fixed> WrapTextBox<Auto, Fixed>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Auto, Fixed, Top>(new MaxFrame<Auto, Auto>(size_max, text_box));
}

template<>
view_ptr<Auto, Auto> WrapTextBox<Auto, Auto>(alloc_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Auto>(size_max, text_box);
}

template<>
view_ptr<Auto, Relative> WrapTextBox<Auto, Relative>(alloc_ptr<TextBox> text_box) {
	return WrapTextBox<Auto, Auto>(text_box);
}

template<>
view_ptr<Relative, Fixed> WrapTextBox<Relative, Fixed>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Relative, Fixed, Top>(text_box);
}

template<>
view_ptr<Relative, Auto> WrapTextBox<Relative, Auto>(alloc_ptr<TextBox> text_box) {
	return text_box;
}

template<>
view_ptr<Relative, Relative> WrapTextBox<Relative, Relative>(alloc_ptr<TextBox> text_box) {
	return WrapTextBox<Relative, Auto>(text_box);
}
