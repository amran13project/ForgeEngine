# Forge Engine roadmap

Priorities are ordered by the alpha's missing acceptance criteria, not by the number of existing module stubs.

## P0 - Validate and stabilize the native baseline

- Run Windows x64 Debug and Release configure/build/CTest on GitHub Actions.
- Resolve all compiler, test, workflow, and website-validator failures before tagging a release.
- Exercise editor and runtime startup and shutdown on a real Windows desktop.
- Test create, save, close, reopen, and runtime launch with `ProfessionalStarter3D`.

## P1 - Make scene editing dependable

- Add an explicit project folder picker and project list selection.
- Add dirty tracking, save prompts, safer scene-save replacement, and undo/redo.
- Make inspector fields editable and add validated translation controls.
- Add hierarchy parenting only after local/world transform semantics are tested.
- Expand scene migration tests and malformed-file diagnostics.

## P2 - Improve the creator workflow

- Replace the wireframe-only preview with a maintained renderer backend after evaluating real platform/toolchain cost.
- Add one documented model-import path and an asset error/reimport workflow.
- Add real material/light rendering, then a small verifiable sample.
- Make build/export distinguish a copied content payload from an executable package.

## P3 - Runtime and extensibility

- Add separate, tested 2D and 3D runtime workflows.
- Evaluate production physics, audio, animation, and scripting integrations independently.
- Add profiling only when measurements come from the actual frame/runtime path.
- Add plugin API versioning and process/lifecycle boundaries before third-party plugin support.

## Planned later phase: AI

AI remains **Planned**. See [AI roadmap](./ai-roadmap.md). No model, provider, network, or AI UI is part of this alpha.
