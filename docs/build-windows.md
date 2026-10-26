# Build Forge Engine on Windows

## Requirements

- Windows 10/11 x64.
- CMake 3.20 or newer.
- Visual Studio 2022 C++ desktop workload and Windows SDK, or another supported C++20 Windows toolchain.
- Git to obtain the source.

The GitHub Actions workflow uses `windows-latest` and the Visual Studio generator. The editor uses a native Win32 window; the runtime also needs an interactive Windows desktop to display its window.

## Configure, build, and test

From the repository root in PowerShell:

```powershell
cmake -S . -B build -A x64 -DFORGE_BUILD_TESTS=ON
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

To use Debug, build with `--config Debug` and run `ctest --test-dir build -C Debug --output-on-failure`.

## Run

```powershell
.\build\Release\ForgeEngine.exe
.\build\Release\ForgeRuntime.exe .\samples\ProfessionalStarter3D
```

The editor creates projects under the user's `ForgeProjects` directory by default. Its current Open Project action discovers that directory and opens the first result; it does not yet offer a folder picker.

## Optional user-provided Windows icon

The user-provided icon is intentionally not included. After placing a legitimate `.ico` file in the repository, configure with:

```powershell
cmake -S . -B build -A x64 -DFORGE_APP_ICON="assets/branding/forge-engine.ico"
```

With the option omitted, the editor and runtime build normally with the platform default icon. The old portable PowerShell builder does not consume this CMake option.

## Local portable builder

`Build-ForgeEngine-Windows.cmd` may download and extract a large LLVM-MinGW archive into ignored local directories when the compiler is absent. It was not run during the initial audit because the requested path is a hosted Windows runner and no local compiler was present.

## Troubleshooting

- If CMake cannot find a generator, install the Visual Studio 2022 C++ workload or select an installed generator with `-G`.
- If the icon option is set, the path must name an existing Windows `.ico` file.
- For a failed project run, retain the full command output and attach the failing scene/manifest only if it contains no private data.
