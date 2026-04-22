#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"

#include "TextBoxHelper.h"


using namespace ViewDesign;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"SplitLayoutTest");
	}
};


template<template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond> class SplitLayout, class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
void Test() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new CenterFrame<Fixed, Fixed>(
				new BorderFrame(
					Border(2.0f, Color::Yellow),
					new SplitLayout(
						new BorderFrame(
							Border(2.0f, Color::Green),
							WrapTextBox<WidthTraitFirst, HeightTraitFirst>(new EditBox(EditBox::Style(), L"Edit here ..."))
						),
						new BorderFrame(
							Border(2.0f, Color::Red),
							WrapTextBox<WidthTraitSecond, HeightTraitSecond>(new EditBox(EditBox::Style(), L"Edit here, too ..."))
						)
					)
				)
			)
		)
	);
	desktop.EventLoop();
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
