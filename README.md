# Forge Engine Professional 3.0

**CREATE WITHOUT LIMITS**

Forge Engine is a native game-development platform foundation for creators ranging from first-time users to professional teams. This repository contains the editor, standalone runtime, project workflow, diagnostics, AI orchestration boundary, and publishing foundations.

## 3.0 Creator + AI Platform

This version adds:

- Forge Account foundation: sign up, login, local-first profile, date of birth, region, language and birthday greeting.
- Forge AI foundation: project-aware orchestration interface with explicit modes, plans, confirmations and provider abstraction.
- Forge Publish Center: platform selection, metadata model, validation and release-build workflow.
- Compliance Checker: blocking issues and warnings before submission.
- Localization Manager: multilingual platform foundation.
- Accessibility settings foundation.
- Security Center: project secret-file detection and source-control guidance.
- Release Manager: version/build/channel manifests.
- Workspace profiles for Beginner, Creator, Developer and Professional experiences.

## Honest capability boundary

The editor and runtime are native C++20 applications. Account data is offline-first for development and local workflows; production public accounts require a hosted authentication provider. Store upload is intentionally gated behind connected developer credentials and platform approval. Forge does not claim automatic store acceptance.

## Build

### Linux development

```bash
cmake -S . -B build -G Ninja -DFORGE_BUILD_TESTS=ON
cmake --build build --target ForgeEngine ForgeRuntime ForgeCoreTests
ctest --test-dir build --output-on-failure
```

### Windows creator build

Run `Build-ForgeEngine-Windows.cmd`. The portable builder downloads/reuses a native Windows LLVM-MinGW toolchain inside `Toolchain/`; it does not require a system CMake installation.

## Repository policy

Git is the source of truth. Development artifacts, toolchains, build output and account data must not be committed.
