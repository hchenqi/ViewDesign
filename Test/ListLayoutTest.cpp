// testing:
// - ListLayout AppendChild
// - correct layout calculation of ListLayoutVertical and ListLayoutHorizontal with all possible size traits
// - layer tiling with TileLayerFrame under scaling context

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/LayerFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/frame/InnerBorderFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/layout/ListLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/wrapper/HitTestHelper.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>

#include "trait_name.h"


using namespace ViewDesign;


template<template<class Trait> class ListLayout, class Trait>
class ListView : public HitSelfFallback<ListLayout<Trait>> {
private:
	using Base = HitSelfFallback<ListLayout<Trait>>;

public:
	using Base::Base;

private:
	template<class WidthTrait, class HeightTrait>
	class ItemFrame : public InnerBorderFrame<WidthTrait, HeightTrait> {
	private:
		using Base = InnerBorderFrame<WidthTrait, HeightTrait>;
	public:
		ItemFrame(Base::child_type child) : Base(border_normal, std::move(child)) {}
	private:
		constexpr static Border border_normal = Border(2.0f, 5.0f, Color::CadetBlue);
		constexpr static Border border_focused = Border(2.0f, 5.0f, Color::Orange);
	private:
		virtual void OnFocusEvent(FocusEvent event) override {
			switch (event) {
			case FocusEvent::FocusIn: Base::SetBorder(border_focused); break;
			case FocusEvent::FocusOut: Base::SetBorder(border_normal); break;
			}
		}
	};

	template<class T>
	ItemFrame(T) -> ItemFrame<extract_width_trait<T>, extract_height_trait<T>>;

	class Item : public TextEditor {
	private:
		struct Style : public TextEditor::Style {
			Style() {
				font.size(20);
			}
		};
	public:
		Item() : TextEditor(Style(), u"Type something here...") {}
	};

private:
	void AppendItem() {
		ref_ptr<Item> item;
		Base::AppendChild(
			new ItemFrame(
				new PaddingFrame(
					Padding(10.0f),
					TextViewAdapter<typename Base::width_trait, typename Base::height_trait>(
						item = new Item()
					)
				)
			)
		);
		item->Edit();
	}

private:
	virtual void OnMouseEvent(MouseEvent event) override {
		if (event.type == MouseEvent::LeftDown) {
			AppendItem();
		}
	}
};


template<class WidthTrait>
struct Name<ListLayoutVertical<WidthTrait>> {
	inline static const u16string text = u"ListLayoutVertical<" + GetName<WidthTrait>() + u">";
};

template<class HeightTrait>
struct Name<ListLayoutHorizontal<HeightTrait>> {
	inline static const u16string text = u"ListLayoutHorizontal<" + GetName<HeightTrait>() + u">";
};

template<template<class Trait> class ListLayout, class Trait>
void Test() {
	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			GetName<ListLayout<Trait>>(),
			new ScrollFrame(
				new LayerFrameTiled(
					new ScaleFrame(
						new LayerFrameTiled(
							new InnerBorderFrame(
								Border(1.0f, 0.0f, Color::Black),
								new PaddingFrame(
									Padding(5.0f),
									new ListView<ListLayout, Trait>(5)
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
	Test<ListLayoutVertical, Fixed>();
	Test<ListLayoutVertical, Auto>();
	Test<ListLayoutVertical, Relative>();

	Test<ListLayoutHorizontal, Fixed>();
	Test<ListLayoutHorizontal, Auto>();
	Test<ListLayoutHorizontal, Relative>();
}
