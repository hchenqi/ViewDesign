// state mirroring: one view updates the state and the other read-only views mirror the state

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/ScrollFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/layout/ListLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>
#include <ViewDesign/messaging/state.h>
#include <ViewDesign/messaging/signal.h>


using namespace ViewDesign;


using ListView = DeferredReflow<ListLayoutVertical<Relative>>;


class TextInput : public TextEditor {
public:
	using Style = TextEditor::Style;
public:
	TextInput(const Style& style, u16string text) : TextEditor(style, std::move(text)), text_state(this->text) {}
public:
	StateRef<u16string> text_state;
	Signal text_update_signal;
private:
	virtual void OnTextUpdate() override {
		TextEditor::OnTextUpdate();
		text_state.Notify();
		text_update_signal.Notify();
	}
};


class TextLabel : public TextView {
public:
	using Style = TextView::Style;
public:
	TextLabel(const Style& style, const StateRef<u16string>& text_state) : TextView(style, text_state.Get()), text_state_watcher(text_state, [&](const u16string& text) { Assign(text); }) {}
private:
	StateRef<u16string>::Watcher text_state_watcher;
};


struct TextStyleA : TextLabel::Style {
	TextStyleA() {
		font.size(30.0f).color(Color::Black);
	}
};

struct TextStyleB : TextLabel::Style {
	TextStyleB() {
		font.size(50.0f).color(Color::Blue);
	}
};


void App() {
	std::unique_ptr<TextInput> text_input = create<TextInput>(TextInput::Style(), u"Type something here...");
	TextInput& text_input_ref = *text_input;

	ref_ptr<ListView> list_view;
	Signal::Listener text_update_listener(text_input_ref.text_update_signal, [&]() { list_view->Reflow(); });

	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"StateMirroring",
			new ScrollFrame<Vertical>(
				new ClipFrame<Fixed, Auto, Left>(
					list_view = new ListView(
						5.0f,
						create<BackgroundFrame<Relative, Auto>>(
							Color::LightSalmon,
							new PaddingFrame(
								Padding(20.0f),
								TextViewAdapter<Relative, Auto>(
									new TextLabel(TextStyleA(), text_input_ref.text_state)
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
								create<TextLabel>(TextStyleB(), text_input_ref.text_state)
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
