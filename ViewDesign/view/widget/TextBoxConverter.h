#include <ViewDesign/view/control/TextBox.h>
#include <ViewDesign/view/frame/MaxFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
view_ptr<WidthTrait, HeightTrait> TextBoxConverter(std::unique_ptr<TextBox> text_box);

template<class WidthTrait, class HeightTrait>
view_ptr<WidthTrait, HeightTrait> TextBoxConverter(owner_ptr<TextBox> text_box) { return TextBoxConverter<WidthTrait, HeightTrait>(std::unique_ptr<TextBox>(text_box)); }


template<>
view_ptr<Relative, Relative> TextBoxConverter<Relative, Relative>(std::unique_ptr<TextBox> text_box) {
	return text_box;
}

template<>
view_ptr<Auto, Auto> TextBoxConverter<Auto, Auto>(std::unique_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Auto>(size_infinite, std::move(text_box));
}

template<>
view_ptr<Relative, Auto> TextBoxConverter<Relative, Auto>(std::unique_ptr<TextBox> text_box) {
	return new MaxFrame<Relative, Auto>(length_infinite, std::move(text_box));
}

template<>
view_ptr<Auto, Relative> TextBoxConverter<Auto, Relative>(std::unique_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Relative>(length_infinite, std::move(text_box));
}

template<>
view_ptr<Relative, Fixed> TextBoxConverter<Relative, Fixed>(std::unique_ptr<TextBox> text_box) {
	return new ClipFrame<Relative, Fixed, Top>(std::move(text_box));
}

template<>
view_ptr<Fixed, Relative> TextBoxConverter<Fixed, Relative>(std::unique_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Relative, Left>(std::move(text_box));
}

template<>
view_ptr<Auto, Fixed> TextBoxConverter<Auto, Fixed>(std::unique_ptr<TextBox> text_box) {
	return new ClipFrame<Auto, Fixed, Top>(TextBoxConverter<Auto, Relative>(std::move(text_box)));
}

template<>
view_ptr<Fixed, Auto> TextBoxConverter<Fixed, Auto>(std::unique_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Auto, Left>(TextBoxConverter<Relative, Auto>(std::move(text_box)));
}

template<>
view_ptr<Fixed, Fixed> TextBoxConverter<Fixed, Fixed>(std::unique_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Fixed, TopLeft>(std::move(text_box));
}


} // namespace ViewDesign
