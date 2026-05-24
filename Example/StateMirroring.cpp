// state mirroring: one view updates the state and the other read-only views mirror the state

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/layout/ListLayout.h>
#include <ViewDesign/view/control/EditBox.h>
#include <ViewDesign/view/control/TextBox.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/widget/TextBoxConverter.h.>
#include <ViewDesign/messaging/state.h>


using namespace ViewDesign;


class EditView : public EditBox {
public:
	using Style = EditBox::Style;
public:
	EditView(const Style& style, u16string text) : EditBox(style, std::move(text)), text_state(this->text) {}
public:
	State<u16string> text_state;
private:
	virtual void OnTextUpdate() override {
		EditBox::OnTextUpdate();
		text_state.Set(text);
	}
};


class TextView : public TextBox {
public:
	using Style = TextBox::Style;
public:
	TextView(const Style& style, const State<u16string>& text_state) : TextBox(style, text_state.Get()), text_state_watcher(text_state, [&](const u16string& text) { Assign(text); }) {}
private:
	State<u16string>::Watcher text_state_watcher;
};


struct TextViewStyleA : TextView::Style {
	TextViewStyleA() {
		font.size(30.0f).color(Color::Black);
	}
};

struct TextViewStyleB : TextView::Style {
	TextViewStyleB() {
		font.size(50.0f).color(Color::Blue);
	}
};


void App() {
	std::unique_ptr<EditView> edit_view = create<EditView>(EditView::Style(), u"Type something here...");
	EditView& edit_view_ref = *edit_view;

	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"StateMirroring",
			new DefaultBackground<ScrollFrame<Vertical>>(
				new ClipFrame<Fixed, Auto, Left>(
					new ListLayoutVertical(
						5.0f,
						create<BackgroundFrame<Relative, Auto>>(
							Color::LightSalmon,
							new PaddingFrame(
								Padding(20.0f),
								TextBoxConverter<Relative, Auto>(
									new TextView(TextViewStyleA(), edit_view_ref.text_state)
								)
							)
						),
						create<BorderFrame<Relative, Auto>>(
							Border(2.0f, Color::Orange),
							new PaddingFrame(
								Padding(10.0f),
								TextBoxConverter<Relative, Auto>(
									std::move(edit_view)
								)
							)
						),
						create<BackgroundFrame<Relative, Auto>>(
							Color::LightSkyBlue,
							TextBoxConverter<Relative, Auto>(
								create<TextView>(TextViewStyleB(), edit_view_ref.text_state)
							)
						)
					)
				)
			)
		)
	);

	edit_view_ref.Edit();

	desktop.EventLoop();
}
