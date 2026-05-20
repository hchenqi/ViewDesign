#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/ScrollFrame.h"
#include "ViewDesign/view/frame/LayerFrame.h"
#include "ViewDesign/view/frame/ScaleFrame.h"
#include "ViewDesign/view/frame/InnerBorderFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/layout/ListLayout.h"
#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/view/wrapper/HitTestHelper.h"

#include "trait_name.h"
#include "text_box_helper.h"


using namespace ViewDesign;


template<class WidthTrait, class HeightTrait>
class ScaleView : public ScaleFrame<WidthTrait, HeightTrait> {
private:
	using Base = ScaleFrame<WidthTrait, HeightTrait>;
public:
	ScaleView(Base::child_type child) : Base(Scale(1.0), std::move(child)) {}
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.ctrl && event.type == MouseEvent::WheelVertical) {
			return this;
		}
		return Base::HitTest(event);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		Base::SetScale(Base::scale * Scale(powf(1.1f, event.wheel_delta / 120.0f)));
	}
};

template<class T>
ScaleView(T) -> ScaleView<extract_width_trait<T>, extract_height_trait<T>>;


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

	class EditView : public EditBox {
	private:
		struct Style : public EditBox::Style {
			Style() {
				font.size(20);
			}
		};
	public:
		EditView() : EditBox(Style(), u"Type something here...") {}
	public:
		void Edit() {
			SetCaret(TextRange(-1, 0));
			SetFocus();
			CaretStartBlinking();
		}
	};

private:
	void AppendItem() {
		ref_ptr<EditView> edit_view;
		Base::AppendChild(
			new ItemFrame(
				new PaddingFrame(
					Padding(10.0f),
					WrapTextBox<typename Base::width_trait, typename Base::height_trait>(edit_view = new EditView())
				)
			)
		);
		edit_view->Edit();
	}

private:
	virtual void OnMouseEvent(MouseEvent event) override {
		if (event.type == MouseEvent::LeftDown) {
			AppendItem();
		}
	}
};


template<template<class Trait> class ListLayout, class Trait>
void Test() {
	desktop.AddWindow(
		new DefaultWindow(
			Name<ListLayout<Trait>>::text, DefaultWindow::Style(),
			new ScrollFrame(
				new LayerFrameTiled(
					new ScaleView(
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
