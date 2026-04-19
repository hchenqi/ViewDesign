# ViewDesign

A C++ GUI library

## Introduction

`ViewDesign` is a GUI framework completely written in C++, currently working on Windows platform. It provides a basic component library including `TextBox`, `ListLayout`, `ScrollFrame`, etc for intuitive and explicit UI building with high-performance rendering and is highly flexible and easily extensible.

### Highlights

- completely written in modern C++ without any markup language
- minimal, flexible and powerful
- clear and simple reflow and redraw logic
- static check for layout compatibility by `SizeTrait`
- conceptual separation of control, frame and layout

### Limitations

- lacks comprehensive documentation
- currently Windows-only
- incomplete component library
- lack of extensive correctness and performance testing
- lack of a high-level and user-friendly designer for UI building
- lack of accessibility support

## Build Instruction

The library and test executables can be built by CMake with MSVC build tools and Ninja on Windows.

## Example

From Test/TextBoxTest.cpp:

```cpp
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"

using namespace ViewDesign;

struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"Example");
	}
};

struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75).color(Color::Black);
	}
};

int main() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), L"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
```

## Concepts

### View / Desktop / Window

A view is a component derived from `ViewBase` defining its own logic of calculating layout, drawing content and handling events.

Each view can have a parent view and multiple child views. All views form a view tree.

`Desktop` is the virtual root of the view tree that manages all windows of the program on the system desktop. Each `Window` as a direct child view of `Desktop` presents one window and receives system events for this window.

`view_ref` holds a reference to a view, and `view_ptr` holds an allocated unique pointer to a view.

### Control / Frame / Layout

Control, frame and layout are different types of views.

- A control has no child view and does basic functions or displays raw content, like `Button`, `TextBox`, `ImageBox`, etc.

- A frame has one child view to decorate, like `BorderFrame` and `BackgroundFrame`.

- A layout may have multiple child views that are visually arranged in a certain way, like `ListLayout`, `SplitLayout`, `OverlapLayout`, etc.

### Fixed / Auto / Relative

Each view occupies a rectangular region on its parent view. Usually the position of a view on its parent view is decided by the parent view completely, but the size of a view can be decided by both the view and its parent view.

The size traits `Fixed`, `Auto` and `Relative` marks how the width or the height of a view should be decided.

- `Fixed` means a dimension of a view is assigned by its parent view.

- `Auto` means a dimension of a view is determined by the view itself.

- `Relative` means a dimension of a view is calculated by the view based on some information provided by its parent view.

`Fixed` and `Auto` are naturally also `Relative`, but not vice versa.

A view and a `view_ref` or a `view_ptr` can be marked by a certain size trait. And a parent view may only accept child views with certain size traits. This is checked at compile-time.

Some frames act as a conversion layer for converting size traits of child views:

- `ClipFrame`, `CenterFrame` and `ScrollFrame` convert a dimension from `Auto` or `Relative` to `Fixed`, clipping the overflowing part of the child view. `ClipFrame` puts the child view at a corner. `CenterFrame` puts the child view at the center. `ScrollFrame` makes it possible to scroll to the overflowing part of the child view.

- `FixedFrame` converts a dimension from `Fixed` to `Auto`. It sets a fixed value to the dimension for the child view.

- `MaxFrame` and `MinFrame` convert a dimension from `Relative` to `Auto`, normalizing its child view with a max or min size.

Some particular layouts have their own mechanism of calculating child region bypassing the size traits, like `OverlapLayout`.

### Reflow / Redraw

Reflow is the process of propagating size change and calculating the layout of a view, determining the sizes and positions of its child views. Redraw is the process of propagating an updated region of a view and drawing its content including child views on the updated region.

In general, the parent view provides a size reference `size_ref` to a child view, the child view calculates its size based on the `size_ref` or on its own, and returns its actual size to the parent view, which then calculates its layout and its own size.

If a view later changes its size by itself, it notifies its parent view, and the parent view calculates its layout again and might also change its own size. Reflow stops at the first parent view who updated its layout but doesn't change its own size. This final parent view then initiates redraw over its updated region.

A view whose content is updated without size change can also initiate redraw. A view who changes its size doesn't initiate redraw because the final parent view will.

A view initiating redraw notifies its parent view about its updated region, and the parent view translates and clips the region as its own region and notifies its own parent view until the `Window` which then draws all child views in the updated region and presents the window.

### Figure / Canvas / Layer

A `Figure` can draw itself on a render target. Different figure types like `Rectangle`, `Circle`, `TextBlock` and `Image` all inherit the `Figure` base class.

Each view can draw its content as figures on a `Canvas` provided to it. It can pass the `Canvas` to its child views to draw on.

A `Canvas` is a virtual render target for views, which actually collects the figures drawn by the views as drawing commands for rendering on the actual render target `Bitmap` of a `Layer`.

A `LayerFrame` maintains a `Layer` like a cache where its child views are directly drawn on. The layer itself as a figure can be drawn on another layer.

Each `Window` maintains a `WindowLayer` for finally rendering the content in the window.

### MouseEvent / KeyEvent / FocusEvent

A `Window` receives mouse events from the system and dispatches the events as instances of `MouseEvent` which includes the event type, the mouse position and key statuses. The view that consumes the mouse event is selected by hit test.

During hit test, each view checks the mouse event and returns a child view that should further process the event, or itself if it will consume the event, or a `nullptr` when a view is not found. The mouse position in the event is translated to a point on the child view.

If a view to consume the mouse event is finally found, this view is tracked by `Desktop` in a stack, and all parent views of the view and the view itself will be sent a `MouseEnter` event as `FocusEvent`. This view in addition will be sent a `MouseOver` event as `FocusEvent` and then the mouse event. If a view to consume the mouse event is not found, then the last view that processed the mouse event is still tracked even though it won't be sent the mouse event.

If another view is to be tracked or to consume the next mouse event, the view tracked before will be sent a `MouseOut` event, and the parent views that not the parent views of the new view will be sent a `MouseLeave` event.

If a view acquires mouse capture, all subsequent mouse events will be directly translated and sent to this view.

A view can acquire focus to receive key events as instances of `KeyEvent`. This view is also tracked by `Desktop` in another stack, and all its parent views and itself will receive `FocusIn` event as `FocusEvent`, and the view itself will additionally receive `Focus` event. The view which acquired focus before will receive `Blur` event and its parent views that are not the parent views of the newly tracked view will receive `FocusOut` event.
