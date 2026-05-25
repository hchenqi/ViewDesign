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

### Button

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

The `wrapper` sub-folder includes templates that inherits and modifies a view component class.

### Cursor

### Background

### HitTestHelper

### DelayedUpdate

## widget

The `widget` sub-folder includes more complex components that are derived from or constructed with simple components.

### DefaultWindow

### UndecoratedWindow

### TitleBarWindow

### Tooltip
