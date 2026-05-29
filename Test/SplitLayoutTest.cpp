// testing:
// - correct layout calculation of SplitLayoutVertical and SplitLayoutHorizontal with all allowed size traits
// - decorating TextView/TextEditor with TextViewAdapter

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>

#include "trait_name.h"


using namespace ViewDesign;


template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
struct Name<SplitLayoutVertical<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>> {
	inline static const u16string text = u"SplitLayoutVertical<" + GetName<WidthTraitFirst>() + u", " + GetName<HeightTraitFirst>() + u", " + GetName<WidthTraitSecond>() + u", " + GetName<HeightTraitSecond>() + u">";
};

template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
struct Name<SplitLayoutHorizontal<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>> {
	inline static const u16string text = u"SplitLayoutHorizontal<" + GetName<WidthTraitFirst>() + u", " + GetName<HeightTraitFirst>() + u", " + GetName<WidthTraitSecond>() + u", " + GetName<HeightTraitSecond>() + u">";
};

template<template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond> class SplitLayout, class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
void Test() {
	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			GetName<SplitLayout<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>>(),
			new CenterFrame<Fixed, Fixed>(
				new ScaleFrame(
					Scale(2.0f),
					new BorderFrame(
						Border(2.0f, ColorCode::Yellow),
						new SplitLayout(
							new BorderFrame(
								Border(2.0f, ColorCode::Green),
								TextViewAdapter<WidthTraitFirst, HeightTraitFirst>(
									new PaddingFrame(
										Padding(5.0f),
										new TextEditor(TextEditor::Style(), u"Type here...")
									)
								)
							),
							new BorderFrame(
								Border(2.0f, ColorCode::Red),
								TextViewAdapter<WidthTraitSecond, HeightTraitSecond>(
									new PaddingFrame(
										Padding(5.0f),
										new TextEditor(TextEditor::Style(), u"Type here, too...")
									)
								)
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
