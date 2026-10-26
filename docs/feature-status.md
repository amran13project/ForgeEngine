# Forge Engine feature status

Status is intentionally conservative. A data model or helper module is not equivalent to a finished editor workflow.

## Available in the current source

| Area | Status | Boundary |
|---|---|---|
| Native application | Implemented, not locally validated in this environment | C++20 editor and separate runtime; Win32 and Linux/X11 window paths |
| Project creation | Implemented | Creates a local project manifest, standard folders, and a 3D starter scene |
| Project opening | Partial | Discovers projects in the default `ForgeProjects` folder and opens the first result; no folder picker or recent-project selection |
| Scene data | Implemented baseline | Version-2 JSON scene data with entity IDs, names, types, transforms, visibility, and lock state |
| Scene persistence | Implemented baseline | Validates input and preserves the current scene on a failed load; selection is transient and is not saved |
| Editor scene view | Partial | Software-projected wireframe preview; selection, add-cube, basic keyboard translation, delete, and read-only transform values |
| Play mode | Partial | A basic in-editor physics step; edit scene snapshot is restored when play stops |
| Standalone runtime | Partial | Opens a project directory and displays the scene wireframe in a native window; Windows/Linux GUI behavior still needs platform testing |
| Asset browser | Partial | Scans and classifies files; no model or texture importer, GPU upload, or material workflow |
| Project Doctor | Partial | Checks basic project/scene structure; diagnostics are not a comprehensive validator |
| Project package | Partial | Copies project files and emits a manifest. It does not compile an executable and is not a platform export |
| Automated tests | Implemented in source | CTest targets cover core, creator systems, project/scene persistence, and professional QA; this task's changes require runner validation |
| Sample | Implemented | `samples/ProfessionalStarter3D` has a manifest and a version-2 scene with a camera, mesh, and light entity |

## Not available in this alpha

- GPU rendering, depth testing, lighting, shadows, materials, PBR, and 2D rendering.
- Hierarchy parenting, editable inspector fields, transform gizmos, undo/redo, dirty-state prompts, and unsaved-close protection.
- General file chooser, recent-project selection, or project templates other than the 3D starter.
- glTF/OBJ import, image decoding, asset conversion, or runtime asset loading.
- Physics engine integration, audio playback, animation playback, scripting, multiplayer, and production plugin sandboxing.
- A tested standalone-game export/package workflow or store submission.
- AI chat, model/provider access, remote AI requests, telemetry, account login, or hosted services in the user-facing editor. Pre-existing local account/AI scaffolding remains in the repository but is not presented as an alpha feature.
- A custom app icon. A CMake option is provided for user-supplied Windows `.ico` artwork when it becomes available.

## Supported development targets

- Windows x64 with a C++20 compiler, CMake 3.20+, and the Windows SDK.
- Linux with CMake 3.20+, a C++20 compiler, and X11 development libraries.
- The above are build targets declared by the source; release support is not established until CI and runtime checks pass.
