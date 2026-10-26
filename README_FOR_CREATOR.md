# Forge Engine 3.2.0 - Creator Guide

Forge Engine is an early native C++20 editor/runtime foundation. See [current feature status](docs/feature-status.md) before relying on a workflow.

## Build

Install CMake 3.20+, Visual Studio 2022 C++ desktop tools, and the Windows SDK. From PowerShell:

```powershell
cmake -S . -B build -A x64 -DFORGE_BUILD_TESTS=ON
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

The legacy `Build-ForgeEngine-Windows.cmd` can download a compiler archive when no compatible toolchain is present; prefer the documented CMake build when possible.

## Create and run

Launch `.\build\Release\ForgeEngine.exe`. Create a 3D project or put `samples/ProfessionalStarter3D` in the default `ForgeProjects` folder and choose Open Project. The current Open action opens the first discovered project.

Select an entity, use arrow keys and W/S to translate it, and press Ctrl+S to save. The Inspector is read-only. Press F5 to start/stop basic play mode. The separate runtime accepts a project path:

```powershell
.\build\Release\ForgeRuntime.exe .\samples\ProfessionalStarter3D
```

The viewport is a software wireframe preview. The Package command copies project content only; it does not build a game executable.

## Not available

The current editor has no account/login, AI assistant, model/texture importer, GPU renderer, undo/redo, editable inspector, or store upload. Publishing screens validate metadata only. See the [roadmap](docs/roadmap.md) for planned work.
