# Forge Engine alpha test guide

This guide describes the current native editor/runtime baseline. Do not use old bundles or scripts that push directly to `main`; development should remain on a feature branch and go through a pull request.

## Build on Windows

Install CMake 3.20+, Visual Studio 2022 C++ desktop tools, and Windows SDK. From PowerShell:

```powershell
cmake -S . -B build -A x64 -DFORGE_BUILD_TESTS=ON
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

The legacy `Build-ForgeEngine-Windows.cmd` can fetch a large portable compiler archive; use it only if you intentionally choose that route.

## Editor test

Run:

```powershell
.\build\Release\ForgeEngine.exe
```

1. Create a 3D project.
2. Confirm the project manifest and `Scenes/Main.forgeScene` are created under `ForgeProjects`.
3. Select an entity and confirm the software wireframe preview and read-only inspector appear.
4. Use arrow keys or W/S to translate the selected entity.
5. Save with Ctrl+S, close normally, and reopen the project.
6. Press F5 to start and stop play mode. Stopping should restore the in-memory editor scene snapshot.
7. Use the Package action and confirm a content payload and manifest are created. This is not an executable game export.
8. Run Project Doctor and inspect its summary.

There is no login, AI tool, GPU renderer, model importer, undo/redo, or interactive project folder picker.

## Runtime test

```powershell
.\build\Release\ForgeRuntime.exe .\samples\ProfessionalStarter3D
```

The runtime needs an interactive Windows desktop. It displays a software wireframe scene and accepts basic input; it does not establish full game-runtime support.

## Reporting

Record the exact OS/compiler, commands, complete output, and whether editor/runtime windows launched. Remove credentials and private project data from logs. A CMake or CTest pass does not prove interactive GUI behavior.
