# Standard Component Library of ViewDesign

Overview of the standard component library of *ViewDesign* under [ViewDesign/view](../ViewDesign/view)

## figure

The `figure` subfolder includes common figures types inheriting `Figure` abstract base class which are used by certain view components:
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

The `style` subfolder includes common style types and layout values including `px` and `pct` which are referenced by certain view components.

## control

The `control` subfolder includes control components, which have no child views.

### Placeholder

`Placeholder` is the simplest control that takes up a certain space and draws nothing.

### TextView

### TextEditor

> Known issue: `TextEditor` doesn't well support vertical text layout (the text layout direction of a `TextView` can be related with the choice of the ClipFrame that wraps it, because `TextView` itself doesn't care about the position within its parent view).

### ImageView

## frame

The `frame` subfolder includes frames as view components that decorate a single child view.

The frames can be classified as follows:
- content-preserving: `ViewFrame`, `ReferenceFrame`, `MutableFrame`, `LayerFrame`, `MirroringFrame`
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

### MirroringFrame

`MirroringFrame` can mirror the content of its child view to multiple `MirrorView` controls.

Normally a view component can only have one parent view on the view tree. `MirroringFrame` makes it possible to mirror a view component on other view tree branches. A `MirrorView` control is less functional than the original view, which has `Auto` size traits and is always kept in the same size as the original view. It redirects `OnDraw` to the `MirroringFrame` to draw the same content as the original view. 

Adding a `MirrorView` as a descendent view of the `MirroringFrame` might cause infinite recursion of drawing if no offset/scaling/clipping is performed.

### BackgroundFrame

### InnerBorderFrame

### PaddingFrame

### BorderFrame

### ScaleFrame

### ClipFrame

`ClipFrame` converts a dimension of its child view from `Relative` trait to `Fixed`. It passes the value of the dimension in `size_ref` from its parent view to its child view.

### CenterFrame

### ScrollFrame

### StretchFrame

### FixedFrame

### MaxFrame

### MinFrame

## layout

The `layout` subfolder includes layout components which could have two or more child views.

### StackLayout

A `StackLayout` stacks all its child views on top of another, whose sizes are all equal to the size of the `StackLayout` itself. The child views are drawn bottom-up, but hit test of mouse events is performed top-down if the top child view doesn't process the event. `HitTestHelper` wrappers are provided to decorate the child views for conveniently choosing the child view to consume a mouse event.

`StackLayout` accepts two child views. At least one of the child views must have `Fixed` traits for both its width and height. The size traits of the `StackLayout` is the same as the size traits of the other child view.

`StackLayoutMultiple` accepts multiple child views with `Fixed` traits for both width and height, and itself has `Fixed` traits.

### SplitLayout

### DivideLayout

### ListLayout

### OverlapLayout

## wrapper

The `wrapper` subfolder includes wrapper templates that inherits and modifies a view component class.

Wrappers and frames all could decorate a view component. A frame is an individual parent view component in the view tree, while a wrapper is a class template that extends the interfaces of the view component class. Therefore, decorating a view component with wrapper is simpler slightly more efficient than decorating with frame, though wrappers are less functional than frames. Wrappers are not able to intercept `SizeUpdated` or `Redraw` calls from the view to modify the reflow or redraw behaviour, but a frame as a parent view component can.

### SizeTrait

The wrapper `SizeTraitOverride` overrides the size traits of the wrapped view component. It's not recommended to use it with normal components because it forces the change of their size traits which might introduce undefined behaviour. However, it can be used for decorating `ViewFrame` or other components that are not originally templated on size traits.

### Cursor

### Background

### HitTestHelper

`HitTestHelper` includes wrappers related with hit test. They are often used to decorate child views of `StackLayout` to choose which child view in the stack is to receive a mouse event.

The wrappers include:
- `HitSelf`: the decorated view consumes the mouse event itself
- `HitSelfFallback`: the decorated view consumes the mouse event itself, if it couldn't find a child view to process the event
- `HitSelfFallbackRecursive`: the decorated view consumes the mouse event itself, if no descendent view could be found to consume the event
- `HitThrough`: the decorated view doesn't process the mouse event (which is then passed back to the next view in `StackLayout`)
- `HitThroughFallback`: the decorated view wouldn't process the mouse event either, if it couldn't find a child view to process the event
- `HitThroughMargin`: the decorated frame wouldn't process the mouse event, if the mouse hits the margin of its child view (only to decorate frames with margin around its child view, like `PaddingFrame`, `BorderFrame`, `ClipFrame`, `CenterFrame`, etc)

### Button

The `Button` wrapper turns a view component to a button-like component that reacts to mouse click events with `OnClick` callback.

`Button` is not provided as a control, but as a wrapper and is considered as an abstract state machine that reacts to mouse and focus events and changes its state. This way, all view components can function like a button and it simplifies customization of buttons with text, padding, etc.

### DeferredUpdate

The wrappers `DeferredReflow` and `DeferredRedraw` are provided to accumulate multiple child size update or child redraw requests and commit them in one go by explicitly calling `Reflow` or `Redraw`.

This is useful when multiple views in the view tree are updated in a single operation, to avoid repetitive reflow or redraw propagations eagerly initiated by each updated view.

`DeferredReflow` and `DeferredRedraw` logics need to be implemented for specific layout components, like in `ListLayout.h`. They can also directly decorate `ViewFrame` or certain frames for the frame to accumulate all updates of its subtree and then propagate.

## widget

The `widget` subfolder includes more complex components that are derived from or constructed with simple components.

### DefaultWindow

### UndecoratedWindow

### TitleBarWindow

### Tooltip

### TextViewAdapter
