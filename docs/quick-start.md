# Forge Engine quick start

## Build from source

On Windows, follow [Build Forge Engine on Windows](./build-windows.md). The GitHub Actions Windows job runs the same CMake/CTest flow in Debug and Release.

## Open the included sample

The `samples/ProfessionalStarter3D` folder is a real project with `ForgeProject.json` and `Scenes/Main.forgeScene`.

1. Build the editor and runtime.
2. Run `ForgeEngine.exe`.
3. Copy the sample folder into the default `ForgeProjects` directory if you want to open it in Forge Hub. Choose **Open Project**; the current editor opens the first discovered project.
4. Select a scene entity to see its read-only transform. Use arrow keys to move it in the preview; use W/S to change its vertical position.
5. Choose **Save** or press Ctrl+S to write the version-2 scene file.
6. Choose **Play** or press F5. Stop play to restore the editor scene snapshot.
7. Run the standalone runtime with `ForgeRuntime.exe <project-directory>`.

The viewport is a software wireframe preview. The current Package action copies project files only; it does not compile a game executable.

## Controls

- F5: start/stop play mode.
- Ctrl+S: save the current scene.
- F6: copy project content to the development package directory.
- F7: run Project Doctor.
- F9: run the editor's registered quick checks.
- F10: open publishing validation.
- Arrow keys: translate the selected entity in the preview; W/S move vertically.
- Delete: delete the selected, unlocked entity.
- A: open the Assets panel.
- R: create a local recovery snapshot.

AI, accounts, networked services, model import, and store upload are not available as editor features in this alpha.
