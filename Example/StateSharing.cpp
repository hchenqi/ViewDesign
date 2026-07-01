// state sharing: multiple views share the same state and each view can update the state that is synchronized with the others

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/layout/DivideLayout.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/messaging/signal.h>


using namespace ViewDesign;


class TextInput : public Stateful::TextEditor {
public:
	TextInput(State& state, const Signal& state_update_signal) : TextEditor(Style(), state), state_update_signal(state_update_signal), state_listener(state_update_signal, [&] { OnStateUpdate(); }) {}
public:
	const Signal& state_update_signal;
	Signal::Listener state_listener;
private:
	virtual void OnOperationEnd() override {
		state_update_signal.Notify();
	}
};


void App() {
	TextEditor::State state;
	Signal state_update_signal;

	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"StateMirroring",
			new DivideLayoutHorizontal(
				TextViewAdapter<Fixed, Fixed>(
					new PaddingFrame(
						Padding(10.0f),
						new TextInput(state, state_update_signal)
					)
				),
				TextViewAdapter<Fixed, Fixed>(
					new BackgroundFrame(
						ColorCode::LightSalmon,
						new PaddingFrame(
							Padding(20.0f),
							new TextInput(state, state_update_signal)
						)
					)
				)
			)
		)
	);
	desktop.EventLoop();
}
