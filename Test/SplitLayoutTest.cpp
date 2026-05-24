#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/EditBox.h>
#include <ViewDesign/view/widget/TextBoxConverter.h.>

#include "trait_name.h"


using namespace ViewDesign;


template<template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond> class SplitLayout, class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
void Test() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			Name<SplitLayout<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>>::text,
			new CenterFrame<Fixed, Fixed>(
				new BorderFrame(
					Border(2.0f, Color::Yellow),
					new SplitLayout(
						new BorderFrame(
							Border(2.0f, Color::Green),
							TextBoxConverter<WidthTraitFirst, HeightTraitFirst>(
								new EditBox(EditBox::Style(), u"Edit here ...")
							)
						),
						new BorderFrame(
							Border(2.0f, Color::Red),
							TextBoxConverter<WidthTraitSecond, HeightTraitSecond>(
								new EditBox(EditBox::Style(), u"Edit here, too ...")
							)
						)
					)
				)
			)
		)
	);
	desktop.EventLoop();
}


void App() {
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
