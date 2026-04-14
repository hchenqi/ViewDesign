#include "ViewDesign/view/Global.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/view/widget/TitleBarFrame.h"


using namespace ViewDesign;


struct MainFrameStyle : TitleBarFrame::Style {
	MainFrameStyle() {
		width.normal(800px);
		height.normal(500px);
		title.text.assign(L"SplitLayoutTest");
	}
};


template<class WidthTrait, class HeightTrait>
view_ptr<WidthTrait, HeightTrait> Wrap(alloc_ptr<TextBox> text_box);

template<>
view_ptr<Fixed, Fixed> Wrap<Fixed, Fixed>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Fixed, TopLeft>(text_box);
}

template<>
view_ptr<Fixed, Auto> Wrap<Fixed, Auto>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Fixed, Auto, Left>(text_box);
}

template<>
view_ptr<Auto, Fixed> Wrap<Auto, Fixed>(alloc_ptr<TextBox> text_box) {
	return new ClipFrame<Auto, Fixed, Top>(new MaxFrame<Auto, Auto>(size_max, text_box));
}

template<>
view_ptr<Auto, Auto> Wrap<Auto, Auto>(alloc_ptr<TextBox> text_box) {
	return new MaxFrame<Auto, Auto>(size_max, text_box);
}


template<template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond> class SplitLayout, class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
void Test() {
	global.Add(
		new TitleBarFrame(
			MainFrameStyle(),
			new CenterFrame<Fixed, Fixed>(
				new BorderFrame(
					Border(2px, Color::Yellow),
					new SplitLayout(
						new BorderFrame(
							Border(2px, Color::Green),
							Wrap<WidthTraitFirst, HeightTraitFirst>(new EditBox(EditBox::Style(), L"Edit here ..."))
						),
						new BorderFrame(
							Border(2px, Color::Red),
							Wrap<WidthTraitSecond, HeightTraitSecond>(new EditBox(EditBox::Style(), L"Edit here, too ..."))
						)
					)
				)
			)
		)
	);
	global.EventLoop();
}


int main() {
	// Test<SplitLayoutVertical, Fixed, Fixed, Fixed, Fixed>();
	// Test<SplitLayoutVertical, Auto, Fixed, Fixed, Fixed>();
	Test<SplitLayoutVertical, Fixed, Auto, Fixed, Fixed>();
	Test<SplitLayoutVertical, Auto, Auto, Fixed, Fixed>();
	// Test<SplitLayoutVertical, Fixed, Fixed, Auto, Fixed>();
	// Test<SplitLayoutVertical, Auto, Fixed, Auto, Fixed>();
	// Test<SplitLayoutVertical, Fixed, Auto, Auto, Fixed>();
	Test<SplitLayoutVertical, Auto, Auto, Auto, Fixed>();
	Test<SplitLayoutVertical, Fixed, Fixed, Fixed, Auto>();
	// Test<SplitLayoutVertical, Auto, Fixed, Fixed, Auto>();
	Test<SplitLayoutVertical, Fixed, Auto, Fixed, Auto>();
	Test<SplitLayoutVertical, Auto, Auto, Fixed, Auto>();
	Test<SplitLayoutVertical, Fixed, Fixed, Auto, Auto>();
	Test<SplitLayoutVertical, Auto, Fixed, Auto, Auto>();
	Test<SplitLayoutVertical, Fixed, Auto, Auto, Auto>();
	Test<SplitLayoutVertical, Auto, Auto, Auto, Auto>();

	// Test<SplitLayoutHorizontal, Fixed, Fixed, Fixed, Fixed>();
	Test<SplitLayoutHorizontal, Auto, Fixed, Fixed, Fixed>();
	// Test<SplitLayoutHorizontal, Fixed, Auto, Fixed, Fixed>();
	Test<SplitLayoutHorizontal, Auto, Auto, Fixed, Fixed>();
	Test<SplitLayoutHorizontal, Fixed, Fixed, Auto, Fixed>();
	Test<SplitLayoutHorizontal, Auto, Fixed, Auto, Fixed>();
	// Test<SplitLayoutHorizontal, Fixed, Auto, Auto, Fixed>();
	Test<SplitLayoutHorizontal, Auto, Auto, Auto, Fixed>();
	// Test<SplitLayoutHorizontal, Fixed, Fixed, Fixed, Auto>();
	// Test<SplitLayoutHorizontal, Auto, Fixed, Fixed, Auto>();
	// Test<SplitLayoutHorizontal, Fixed, Auto, Fixed, Auto>();
	Test<SplitLayoutHorizontal, Auto, Auto, Fixed, Auto>();
	Test<SplitLayoutHorizontal, Fixed, Fixed, Auto, Auto>();
	Test<SplitLayoutHorizontal, Auto, Fixed, Auto, Auto>();
	Test<SplitLayoutHorizontal, Fixed, Auto, Auto, Auto>();
	Test<SplitLayoutHorizontal, Auto, Auto, Auto, Auto>();
}
