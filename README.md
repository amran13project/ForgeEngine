# Forge Engine 3.2.0

**CREATE WITHOUT LIMITS**

Forge Engine is an early, native C++20 editor/runtime foundation. This repository contains two native programs, a local project format, a scene editor baseline, a software wireframe preview, and CMake/CTest build definitions.

This is an alpha foundation, not a claim of feature parity with Unity, Unreal, or Godot.

## Current capabilities

- Native `ForgeEngine` editor and separate `ForgeRuntime`.
- Local project creation and project discovery under the default `ForgeProjects` folder.
- Version-2 JSON scene persistence with validated entity IDs, names, transforms, and selection state kept transient.
- Basic scene selection, keyboard translation, entity creation/deletion, and play snapshot/restore.
- Software-projected wireframe preview; no GPU renderer or lighting.
- Asset file scanning/classification; no model or texture importer.
- A Project Doctor baseline and local content package operation.
- A runnable sample project in `samples/ProfessionalStarter3D`.

See [feature status](docs/feature-status.md) and the [roadmap](docs/roadmap.md) for known limits. The user-facing editor has no account or AI feature and makes no network requests. AI is a planned later phase.

## Build and test

### Windows x64

Requirements: CMake 3.20+, Visual Studio 2022 C++ desktop workload, and Windows SDK.

```powershell
cmake -S . -B build -A x64 -DFORGE_BUILD_TESTS=ON
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

The repository's Windows CI runs Debug and Release builds and CTest on `windows-latest`. A CI pass does not by itself verify interactive GUI behavior.

### Linux/X11 development

Requirements: a C++20 compiler, CMake 3.20+, and X11 development libraries.

```bash
cmake -S . -B build -DFORGE_BUILD_TESTS=ON
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
```

Linux is not included in the current hosted CI matrix.

## Run

```powershell
.\build\Release\ForgeEngine.exe
.\build\Release\ForgeRuntime.exe .\samples\ProfessionalStarter3D
```

See [quick start](docs/quick-start.md) and [Windows build guide](docs/build-windows.md). The editor's Package command copies project files and writes a manifest; it does not compile a game executable.

## Website and release

The static website is in [`website/`](website/). Run `python3 scripts/validate-website.py` to check its local links. GitHub Actions validates native builds and the website; Pages deployment is configured for `main` only.

No verified alpha release asset was available at the time of this update. Check the live [GitHub Releases page](https://github.com/amran13project/ForgeEngine/releases). Do not treat a source build or content package as a game export.

## Branding and license

The final app icon is pending owner-provided artwork. See [`assets/branding/README.md`](assets/branding/README.md) for the optional Windows CMake icon setting. Source is under the [MIT License](LICENSE.txt); third-party dependencies are recorded in [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
