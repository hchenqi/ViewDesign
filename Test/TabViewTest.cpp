#include "ViewDesign/window/Global.h"
#include "ViewDesign/widget/TitleBarFrame.h"
#include "ViewDesign/widget/TabView.h"
#include "ViewDesign/control/EditBox.h"


using namespace ViewDesign;


struct MainFrameStyle : public TitleBarFrame::Style {
	MainFrameStyle() {
		width.min(200px).normal(800px).max(100pct);
		height.min(200px).normal(500px).max(100pct);
		position.setHorizontalCenter().setVerticalCenter();
		padding = Padding(2px);
		border.width(2px).radius(5px).color(0xc4b7d7);
		title.bar.background_color(0xc4b7d7);
		title.font.color(Color::White);
		title.text.assign(L"TabViewTest");
	}
};


class MyTabView : public TabView {
public:
	MyTabView() {
		AddTab(std::make_unique<HomePage>());
	}

private:
	class MyTab : public Tab {
	public:
		MyTab() : Tab(title = new Title(L"New Tab")) {}
	private:
		ref_ptr<Title> title;
		std::wstring content;
	private:
		class MyEditBox : public EditBox {
		private:
			struct Style : EditBox::Style {
				Style() {
					paragraph.line_spacing(25px);
					paragraph.baseline_spacing(20px);
					font.family(L"Calibri").size(20px);
				}
			};
		public:
			MyEditBox(MyTab& tab) : EditBox(Style(), tab.content), tab(tab) {}
		private:
			MyTab& tab;
		private:
			virtual void OnTextUpdate() override {
				EditBox::OnTextUpdate();
				tab.content = GetText();
				tab.title->SetText(tab.content.substr(0, tab.content.find_first_of(L"\r\n")));
			}
		};
	private:
		virtual child_ptr<Assigned, Assigned> OnShow() override {
			return new ClipFrame<Assigned, Assigned, TopLeft>(new PaddingFrame(Padding(20px), new MyEditBox(*this)));
		};
	};

	class HomePage : public Tab {
	public:
		HomePage() : Tab(new Title(L"Home"), new Placeholder<Auto, Assigned>(0px)) {}
	private:
		class Content : public Placeholder<Assigned, Assigned>, Context {
		public:
			Content() : Placeholder(), Context(AsWndObject()) {}
		private:
			virtual void OnMouseEvent(MouseEvent event) override {
				switch (event.type) {
				case MouseEvent::LeftDown: Context::Get<TabView>().AddTab(std::make_unique<MyTab>()); break;
				}
			}
		};
	private:
		virtual child_ptr<Assigned, Assigned> OnShow() override {
			return new Content();
		};
	};
};


int main() {
	global.AddWnd(new TitleBarFrame(MainFrameStyle(), new MyTabView()));
	global.EventLoop();
	return 0;
}
