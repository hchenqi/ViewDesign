// state mirroring: one view updates the state and the other read-only views mirror the state

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/layout/ListLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>
#include <ViewDesign/messaging/state.h>


using namespace ViewDesign;


class TextInput : public TextEditor {
public:
	using Style = TextEditor::Style;
public:
	TextInput(const Style& style, u16string text) : TextEditor(style, std::move(text)), text_state(this->text) {}
public:
	State<u16string> text_state;
private:
	virtual void OnTextUpdate() override {
		TextEditor::OnTextUpdate();
		text_state.Set(text);
	}
};


class Text : public TextView {
public:
	using Style = TextView::Style;
public:
	Text(const Style& style, const State<u16string>& text_state) : TextView(style, text_state.Get()), text_state_watcher(text_state, [&](const u16string& text) { Assign(text); }) {}
private:
	State<u16string>::Watcher text_state_watcher;
};


struct TextStyleA : Text::Style {
	TextStyleA() {
		font.size(30.0f).color(Color::Black);
	}
};

struct TextStyleB : Text::Style {
	TextStyleB() {
		font.size(50.0f).color(Color::Blue);
	}
};


void App() {
	std::unique_ptr<TextInput> text_input = create<TextInput>(TextInput::Style(), u"Type something here...");
	TextInput& text_input_ref = *text_input;

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
								TextViewAdapter<Relative, Auto>(
									new Text(TextStyleA(), text_input_ref.text_state)
								)
							)
						),
						create<BorderFrame<Relative, Auto>>(
							Border(2.0f, Color::Orange),
							new PaddingFrame(
								Padding(10.0f),
								TextViewAdapter<Relative, Auto>(
									std::move(text_input)
								)
							)
						),
						create<BackgroundFrame<Relative, Auto>>(
							Color::LightSkyBlue,
							TextViewAdapter<Relative, Auto>(
								create<Text>(TextStyleB(), text_input_ref.text_state)
							)
						)
					)
				)
			)
		)
	);

	text_input_ref.Edit();

	desktop.EventLoop();
}
