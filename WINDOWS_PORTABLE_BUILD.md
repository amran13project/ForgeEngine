# Windows Portable Build

This builder does not install CMake, Visual Studio, or a compiler into Windows system locations.

The first build downloads a native Windows LLVM-MinGW toolchain into `Toolchain/`. Subsequent builds reuse the cached archive.

The builder invokes `clang++.exe` directly, so the build does not depend on the user's CMake installation.

The expected compiler layout is discovered automatically, including the C++ standard library headers under `include/c++/v1`.

The linker is explicitly configured for the Windows console subsystem and `mainCRTStartup`, preventing the common `WinMain` mismatch for a C++ `main()` entry point.

Build command:

    .\Build-ForgeEngine-Windows.cmd

Run editor:

    .\Run-ForgeEngine.cmd

Run a built project with the runtime:

    .\Run-ForgeRuntime.cmd "C:\Path\To\ForgeProject"
