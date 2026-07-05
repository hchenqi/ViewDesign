#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/MaxFrame.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/frame/StretchFrame.h>
#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/view/control/ImageView.h>
#include <ViewDesign/view/wrapper/HitTestHelper.h>
#include <ViewDesign/view/wrapper/Button.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/widget/Box.h>
#include <ViewDesign/view/widget/TextViewAdapter.h>
#include <ViewDesign/messaging/context.h>

#include <optional>


using namespace ViewDesign;


class LoadImageWindow : public DefaultBackground<DefaultWindow>, private ContextProvider {
private:
	struct Style : DefaultWindow::Style {
		Style() {
			width.min(px(500.0f));
			height.fix(px(100.0f));
		}
	};

public:
	LoadImageWindow(std::optional<Image>& image) : Base(
		Style(),
		u"Load Image",
		new CenterFrame<Fixed, Fixed>(
			new SplitLayoutHorizontal(
				filename_input_box = new Box(
					Margin(5.0f, 0.0f), Border(2.0f, border_color_normal), color_transparent, Padding(3.0f),
					TextViewAdapter<Fixed, Fixed>(
						filename_input = new TextEditor(TextEditor::Style(), filename_default)
					)
				),
				new LoadButton()
			)
		)
	), ContextProvider(AsViewBase()), image(image) {}

private:
	class LoadButton : public HitSelf<Button<DefaultBackground<ViewFrame>>>, public SizeTrait<Auto, Auto>, private Context<LoadImageWindow> {
	public:
		LoadButton() : Base(
			new MaxFrame(
				size_infinite,
				new Box(
					Margin(0.0f), Border(1.0f, ColorCode::Black), color_transparent, Padding(20.0f, 5.0f),
					new TextView(TextView::Style(), u"load")
				)
			)
		), Context(AsViewBase()) {}
	private:
		static constexpr Color background_normal = ColorCode::White;
		static constexpr Color background_hovered = ColorCode::LightGray;
		static constexpr Color background_pressed = ColorCode::Gray;
	private:
		virtual void OnHover() override { SetBackground(background_hovered); }
		virtual void OnPress() override { SetBackground(background_pressed); }
		virtual void OnLeave() override { SetBackground(background_normal); }
	private:
		virtual void OnClick() override { Context::Get().TryLoadImage(); }
	};

private:
	inline static const u16string filename_default = uR"(C:\Windows\Web\Wallpaper\Windows\img0.jpg)";
private:
	static constexpr Color border_color_normal = ColorCode::Black;
	static constexpr Color border_color_error = ColorCode::Red;
private:
	ref_ptr<TextEditor> filename_input;
	ref_ptr<Box<Fixed, Fixed>> filename_input_box;
	Timer reset_border_timer = Timer([&]() { filename_input_box->SetBorderColor(border_color_normal); });
private:
	std::optional<Image>& image;
private:
	void TryLoadImage() {
		try {
			image.emplace(filename_input->GetText());
			Close();
		} catch (...) {
			filename_input_box->SetBorderColor(border_color_error);
			reset_border_timer.Set(3000);
		}
	}
};


void App() {
	std::optional<Image> image;
	desktop.AddWindow(new LoadImageWindow(image));
	desktop.EventLoop();

	if (!image.has_value()) {
		return;
	}

	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (centered) (hint: zoom with Ctrl + scroll)",
			new CenterFrame<Fixed, Fixed>(
				new ScaleFrame(
					new Stateful::ImageView(*image)
				)
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (stretched)",
			new StretchFrame<Fixed, Fixed>(
				new Stateful::ImageView(*image)
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (stretched uniformly)",
			new CenterFrame<Fixed, Fixed>(
				new StretchFrameUniform(
					new Stateful::ImageView(*image)
				)
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (repeating) (hint: zoom with Ctrl + scroll)",
			new ScaleFrame(
				new Stateful::ImageRepeatView(*image, Point(500.0f, 300.0f))
			)
		)
	);
	desktop.EventLoop();
}
