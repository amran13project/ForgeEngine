# Forge Engine 3.0 validation

Validated on the development environment:

- CMake configure: PASS
- ForgeEngine native editor build: PASS
- ForgeRuntime native runtime build: PASS
- ForgeCoreTests: PASS
- CTest: 1/1 PASS (100%)
- Native editor smoke launch: PASS (process stayed alive under X11 smoke test until external timeout)

Windows `.exe` output must still be generated on Windows using the included portable builder because the validation environment is Linux.
