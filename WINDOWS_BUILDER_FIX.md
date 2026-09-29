# Windows Builder — FIXED6

The linker error `undefined symbol: WinMain` is fixed by explicitly selecting the Windows console subsystem with `-mconsole`. The project entry point remains the portable C++ `int main()` in `src/main.cpp`.

This is a linker/subsystem configuration issue, not a missing C++ standard library issue.

The CMake Windows non-MSVC target uses the same option for consistency.


## FIXED7
The native builder now explicitly requests the Windows console subsystem and mainCRTStartup entry point in addition to -mconsole. This avoids lld/MinGW GUI-subsystem inference selecting WinMainCRTStartup for Forge Engine's int main() entry point.
