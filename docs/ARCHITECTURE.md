# Forge Engine architecture

## Executables and core library

- `ForgeEngine` is the native editor process.
- `ForgeRuntime` opens a project directory and runs the standalone runtime loop.
- `ForgeCore` is a static C++20 library shared by the executables and core tests.
- The current renderer is a software projection/wireframe path drawn by the native window layer. It is not a GPU backend.

## Current module ownership

- `core`: engine system registry and shared math.
- `platform`: native Win32/X11 window, input events, and environment access.
- `project`: local project creation, metadata, discovery, and open.
- `scene`: entity data, transforms, and version-2 JSON persistence.
- `renderer`: software projection helpers.
- `assets`: file enumeration and extension classification, not asset import.
- `runtime`: separate project-driven scene display and basic input behavior.
- `editor`: Forge Hub, scene list, wireframe preview, inspector display, diagnostics, and play mode.
- `build`: project-content copying and a manifest, not executable compilation.
- Other physics, audio, animation, scripting, account, AI, and creator modules are foundation code unless a user-facing workflow is explicitly identified in the feature status.

## Scene persistence

Version 2 scene files are JSON objects with an `entities` array. The loader validates the supported version, entity IDs, required fields, JSON escapes, vector values, and duplicate IDs before replacing the current scene. A failed load leaves the current scene unchanged. Selection is transient and not serialized. No version-1 migration is currently provided.

## Boundaries

- Project files are local and no telemetry or network service is enabled by the editor.
- Windows builds use the Win32 window layer and Linux builds use X11.
- External model import, GPU rendering, executable export, and store submission are not implemented.
- Account and AI scaffolding from prior repository work is not exposed by the current editor startup or toolbar.
