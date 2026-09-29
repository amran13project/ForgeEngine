# Forge Engine — Professional QA Report

Date: 2026-09-29
Repository baseline: GitHub `main` commit `1b88209`
Local QA branch tip: `39ef482`

## Automated result

- Native CMake build: PASS
- CTest: 2/2 PASS
- ForgeCoreTests: PASS
- ForgeProfessionalQA: 38/38 PASS
- Native editor smoke test: PASS
- Native runtime smoke test: PASS

## What was exercised

Engine initialization; system registry; project create/open; project metadata; default scene creation; scene load/save/reload; entity selection; transform persistence; visibility/lock/selection persistence; asset scanning; physics integration; visual-script execution and state mutation; network delivery; profiler sampling; Build Center packaging and manifest; Project Doctor; recovery snapshot; procedural world generation; testing center; publish validation; publish submission manifest; compliance baseline; localization catalog; security secret scan; date-of-birth validation; birthday matching; AI project inspection; AI planning; AI publish permission boundary.

## Bugs found and fixed during QA

### Scene selection persistence
The first professional QA pass found that the selected entity was not preserved after reopening a scene. The loader always selected the first entity. The scene format now persists `visible`, `locked`, and `selected` state, and only falls back to the first entity when no selection exists.

## Native smoke test

On this Linux test host, both native executables stayed alive for the full smoke window under a virtual X display. This verifies basic process startup/window initialization; it is not a substitute for a human Windows UI test.

## Windows status

The portable Windows builder exists in the repository, but this Linux QA host did not execute the Windows binary/builder. Windows end-to-end verification therefore remains a required external test on the target laptop.

## Production-boundary notes

The QA suite validates implemented foundations and workflows. It does not claim full Unity/Godot/Unreal feature parity. Some large systems in the architecture remain foundations or require platform-specific toolchains/integrations, especially production-grade 3D rendering, hosted authentication, external AI provider execution, native mobile packaging, and direct store submission.

## Release handoff

Use `PROFESSIONAL_QA.md` for the recurring test procedure. Use the Git bundle for transferring the tested commit history into another clone.
