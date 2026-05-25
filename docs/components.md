# Standard Component Library of ViewDesign

Overview of the standard component library of *ViewDesign* under [ViewDesign/view](../ViewDesign/view)

## figure

The `figure` sub-folder includes common figures types inheriting `Figure` abstract base class which are used by certain view components:
- shape:
  - `Line`
  - `Rectangle`
  - `RoundedRectangle`
  - `Ellipse`
  - `Circle`
- `TextBlock`
- `Image`

> Note: not all figures are currently supported by OpenGL and Vulkan backends.

## style

The `style` sub-folder includes common style types and layout values including `px` and `pct` which are referenced by certain view components.

## control

The `control` sub-folder includes view components which have no child views.

### Placeholder

### TextView

### TextEditor

> Known issue: `TextView` / `TextEditor` don't well support vertical text layout (the text layout direction is related with the choice of ClipFrame that wraps them).

### ImageView

## frame

The `frame` sub-folder includes view components which have a single child view.

### ViewFrame

### MutableFrame

### ClipFrame

### CenterFrame

### ScrollFrame

### FixedFrame

### MaxFrame

### MinFrame

### BorderFrame

### PaddingFrame

### BackgroundFrame

### InnerBorderFrame

### ScaleFrame

### LayerFrame

## layout

The `layout` sub-folder includes view components which could have two or more child views.

### SplitLayout

### StackLayout

### ListLayout

### DivideLayout

### OverlapLayout

## wrapper

The `wrapper` sub-folder includes wrapper templates that inherits and modifies a view component class.

Wrappers and frames all could decorate a view component. A frame is an individual parent view component in the view tree, while a wrapper is a class template that extends the interfaces of the view component class. Therefore, decorating a view component with wrapper is slightly more efficient at run-time than decorating with frame, though wrappers are less functional than frames. Wrappers are not able to intercept `SizeUpdated` or `Redraw` calls from the view and modify the reflow or redraw behaviour, but a frame as a parent view can.

### Cursor

### Background

### HitTestHelper

### Button

The `Button` wrapper turns a view component to a button-like component that reacts to mouse click events with `OnClick` callback.

`Button` is not provided as a control, but as a wrapper and is considered as an abstract state machine that reacts to mouse and focus events and changes its state. This way, all view components can function like a button and it simplifies customization of buttons with text, padding, etc.

### DelayedUpdate

## widget

The `widget` sub-folder includes more complex components that are derived from or constructed with simple components.

### DefaultWindow

### UndecoratedWindow

### TitleBarWindow

### Tooltip
