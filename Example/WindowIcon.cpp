#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/Box.h>
#include <ViewDesign/view/widget/FilledButton.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/wrapper/HitTestHelper.h>
#include <ViewDesign/view/wrapper/Background.h>

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>


using namespace ViewDesign;


namespace {

const char* icon_paths[] = { "icon1.png", "icon2.png", "icon3.png" };
size_t icon_index = 0;

std::vector<char> ReadFile(const char* path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		throw std::runtime_error(std::string("failed to open ") + path);
	}
	return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

} // namespace


void App() {
	DefaultBackground<DefaultWindow>* window = nullptr;

	auto* content = new CenterFrame<Fixed, Fixed>(
		new HitSelf<FilledButton<Box<Bounded, Bounded>>>(
			FilledButtonStyle(), [&window] {
				icon_index = (icon_index + 1) % std::size(icon_paths);
				auto bytes = ReadFile(icon_paths[icon_index]);
				window->SetIcon(bytes.data(), bytes.size());
			},
			Margin(0.0f), Border(2.0f, 3.0f, ColorCode::Gray), color_transparent, Padding(3.0f),
			new TextView(TextView::Style(), u"Change Icon")
		)
	);

	window = new DefaultBackground<DefaultWindow>(DefaultWindow::Style(), u"Window Icon", content);

	auto initial_icon = ReadFile(icon_paths[icon_index]);
	window->SetIcon(initial_icon.data(), initial_icon.size());

	desktop.AddWindow(window);
	desktop.EventLoop();
}
