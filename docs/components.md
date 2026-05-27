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

The `frame` sub-folder includes frames as view components that decorate a single child view.

The frames can be classified as follows:
- content-preserving: `ViewFrame`, `ReferenceFrame`, `MutableFrame`, `LayerFrame`
- size-preserving (but not content-preserving): `BackgroundFrame`, `InnerBorderFrame`
- size-traits-preserving (but not size-preserving): `PaddingFrame`, `BorderFrame`, `ScaleFrame`
- size-traits-converting:
  - `Relative`->`Fixed`: `ClipFrame`, `CenterFrame`, `ScrollFrame`, `StretchFrame`
  - `Fixed`->`Auto`: `FixedFrame`
  - `Relative`->`Auto`: `MaxFrame`, `MinFrame`

### ViewFrame

`ViewFrame` is the simplest frame which implements the `ViewBase` virtual interfaces by just forwarding the same arguments between its own parent view and its child view. Most frames inherit `ViewFrame` modifying the default forwarding behaviour.

### ReferenceFrame

`ReferenceFrame` is similar to `ViewFrame` that just forwards the same arguments. The only difference between them is that `ViewFrame` accepts a child view by `unique_ptr` and manages its lifetime, while `ReferenceFrame` accepts a child view by reference whose lifetime is managed externally by the user, and the user must ensure the lifetime of the child view is longer than `ReferenceFrame`.

Most frame and layout components only accept a child view as `unique_ptr` to simplify the lifetime management, nevertheless, they can be combined with `ReferenceFrame` to also accept a child view as references so that the child view can persist when its parent view destructs. This can probably be used to avoid re-creating stateless view components during hot reload.

### MutableFrame

### LayerFrame

### BackgroundFrame

### InnerBorderFrame

### PaddingFrame

### BorderFrame

### ScaleFrame

### ClipFrame

### CenterFrame

### ScrollFrame

### StretchFrame

### FixedFrame

### MaxFrame

### MinFrame

## layout

The `layout` sub-folder includes layout components which could have two or more child views.

### StackLayout

### SplitLayout

### DivideLayout

### ListLayout

### OverlapLayout

## wrapper

The `wrapper` sub-folder includes wrapper templates that inherits and modifies a view component class.

Wrappers and frames all could decorate a view component. A frame is an individual parent view component in the view tree, while a wrapper is a class template that extends the interfaces of the view component class. Therefore, decorating a view component with wrapper is simpler slightly more efficient than decorating with frame, though wrappers are less functional than frames. Wrappers are not able to intercept `SizeUpdated` or `Redraw` calls from the view to modify the reflow or redraw behaviour, but a frame as a parent view component can.

### SizeTrait

The wrapper `SizeTraitOverride` overrides the size traits of the wrapped view component. It's not recommended to use it with normal components because it forces the change of their size traits which might introduce undefined behaviour. However, it can be used for decorating `ViewFrame` or other components that are not originally templated on size traits.

### Cursor

### Background

### HitTestHelper

### Button

The `Button` wrapper turns a view component to a button-like component that reacts to mouse click events with `OnClick` callback.

`Button` is not provided as a control, but as a wrapper and is considered as an abstract state machine that reacts to mouse and focus events and changes its state. This way, all view components can function like a button and it simplifies customization of buttons with text, padding, etc.

### DeferredUpdate

The wrappers `DeferredReflow` and `DeferredRedraw` are provided to accumulate multiple child size update or child redraw requests and commit them in one go by explicitly calling `Reflow` or `Redraw`.

This is useful when multiple views in the view tree are updated in a single operation, to avoid repetitive reflow or redraw propagations eagerly initiated by each updated view.

`DeferredReflow` and `DeferredRedraw` logics need to be implemented for specific layout components, like in `ListLayout.h`. They can also directly decorate `ViewFrame` or certain frames for the frame to accumulate all updates of its subtree and then propagate.

## widget

The `widget` sub-folder includes more complex components that are derived from or constructed with simple components.

### DefaultWindow

### UndecoratedWindow

### TitleBarWindow

### Tooltip

### TextViewAdapter
