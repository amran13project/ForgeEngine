# Windows Rendering Fix

This build replaces direct per-call GetDC drawing with a native off-screen back buffer.

Fixes included:
- Draws the entire editor frame into a memory bitmap.
- Presents the completed frame during WM_PAINT via BitBlt.
- Removes direct painting from the main loop.
- Invalidates at the frame cadence instead of painting outside WM_PAINT.
- Recreates the back buffer safely after resize.
- Releases GDI resources on shutdown.
- Adds basic viewport sizing guards for smaller windows.

Why this matters:
The previous implementation performed many independent GetDC/ReleaseDC operations for every rectangle, line and text element. Windows can repaint or erase the window between those operations, causing flicker, torn frames and visual overlap. Double-buffered presentation makes each frame appear atomically.
