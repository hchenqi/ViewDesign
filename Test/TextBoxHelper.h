#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/control/TextBox.h"


using namespace ViewDesign;


template<class WidthTrait, class HeightTrait>
view_ptr<WidthTrait, HeightTrait> WrapTextBox(owner_ptr<TextBox> text_box);


template<>
view_ptr<Relative, Relative> WrapTextBox<Relative, Relative>(owner_ptr<TextBox> text_box) {
	return text_box;
}

template<>
view_ptr<Auto, Auto> WrapTextBox<Auto, Auto>(owner_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Auto>(size_max, text_box);
}

template<>
view_ptr<Relative, Auto> WrapTextBox<Relative, Auto>(owner_ptr<TextBox> text_box) {
	return new MaxFrame<Relative, Auto>(length_max, text_box);
}

template<>
view_ptr<Auto, Relative> WrapTextBox<Auto, Relative>(owner_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Relative>(length_max, text_box);
}

template<>
view_ptr<Relative, Fixed> WrapTextBox<Relative, Fixed>(owner_ptr<TextBox> text_box) {
	return new ClipFrame<Relative, Fixed, Top>(text_box);
}

template<>
view_ptr<Fixed, Relative> WrapTextBox<Fixed, Relative>(owner_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Relative, Left>(text_box);
}

template<>
view_ptr<Auto, Fixed> WrapTextBox<Auto, Fixed>(owner_ptr<TextBox> text_box) {
	return new ClipFrame<Auto, Fixed, Top>(WrapTextBox<Auto, Relative>(text_box));
}

template<>
view_ptr<Fixed, Auto> WrapTextBox<Fixed, Auto>(owner_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Auto, Left>(WrapTextBox<Relative, Auto>(text_box));
}

template<>
view_ptr<Fixed, Fixed> WrapTextBox<Fixed, Fixed>(owner_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Fixed, TopLeft>(text_box);
}
