# Forge Engine — Father Test Package

This package is based on the current Forge Engine Git workflow.

## Apply the latest changes to the existing GitHub clone

From inside the local `ForgeEngine` repository:

```bat
git status
git fetch "C:\Path\To\ForgeEngine_FULL_2026-09-29.bundle" refs/heads/forge-full-20260929:refs/remotes/forge-bundle/forge-full-20260929
git cherry-pick 6933f2b 38353e5
git push origin main
```

The cherry-pick is intentional: the existing GitHub `main` already contains the 3.1.0 creator-platform commit as `1b88209`; the bundle's 3.1 changes were created from the same 3.1 tree with a different local commit identity.

## Windows build

Run:

```bat
Build-ForgeEngine-Windows.cmd
```

The portable builder prepares the LLVM-MinGW Windows compiler when it is not already cached, then builds:

- `bin\ForgeEngine.exe`
- `bin\ForgeRuntime.exe`

The required compiler runtime DLLs are copied next to the executables.

## Editor test

Run:

```bat
Run-ForgeEngine.cmd
```

Test this workflow:

1. Create or log into a Forge account.
2. Create a 3D project.
3. Confirm the real project folder appears under `ForgeProjects`.
4. Confirm Scene Tree, viewport and Inspector appear.
5. Select the Cube.
6. Move it with the arrow/W/S keys.
7. Press `Ctrl+S` / Save.
8. Close Forge.
9. Reopen the project and confirm the saved transform remains.
10. Press Play and confirm Play Mode runs the simulation.
11. Stop and confirm the editor state is restored.
12. Press Build and confirm a real `Builds\Development` payload and manifest are created.
13. Open Project Doctor, AI, Tests and Publish to inspect their current truthful status.

## Runtime test

After building, run:

```bat
Run-ForgeRuntime.cmd "C:\Users\<user>\ForgeProjects\<project>"
```

The runtime should open the project scene and respond to W/A/S/D or arrow-key input.

## Important truthfulness rule

This is a professional native foundation and an expanding engine. Some advanced systems remain foundation/partial implementations. The UI must never claim a platform build, AI provider connection, store upload, or other external action succeeded when it did not actually occur.
