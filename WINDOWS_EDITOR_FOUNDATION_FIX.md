# Windows Editor Foundation Fix — Forge Engine Professional 2.1

This revision addresses the editor presentation layer rather than adding new feature panels.

Changes:
- Responsive editor layout using computed panel geometry instead of fixed viewport/panel coordinates.
- Proper separation of top toolbar, Scene, Viewport, Inspector, bottom workspace and status bar.
- DPI-aware Windows font creation with cached Segoe UI fonts.
- Native back-buffer presentation with WM_ERASEBKGND suppression.
- Mouse hover state for editor controls.
- Improved project hub and create-project screens.
- Inspector and bottom workspace no longer overlap.
- Native Windows message path remains independent from project/runtime logic.

This is still a renderer/editor foundation, not a claim of production D3D12/Vulkan rendering.
