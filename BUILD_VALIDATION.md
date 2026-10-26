# Historical validation report

> This report describes a previous repository state. It is not evidence that the current branch or current changes build and pass tests.

Previous development-environment results:

- CMake configure: PASS
- ForgeEngine native editor build: PASS
- ForgeRuntime native runtime build: PASS
- ForgeCoreTests: PASS
- CTest: 1/1 PASS (100%)
- Native editor smoke launch: PASS (process stayed alive under X11 smoke test until external timeout)

Windows `.exe` output must still be generated on Windows using the included portable builder because the validation environment is Linux.
