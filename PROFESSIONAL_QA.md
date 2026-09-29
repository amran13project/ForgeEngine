# Forge Engine Professional QA

This suite is a release-candidate engineering check, not a claim of feature parity with other engines.

## Automated checks

Run the complete automated QA pass:

```bash
./scripts/professional-qa.sh
```

Run on any CMake-capable host:

```bash
cmake -S . -B build-qa -DFORGE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-qa -j2
ctest --test-dir build-qa --output-on-failure
```

The suite covers engine initialization, project lifecycle, scene persistence, transforms, assets, physics, visual scripting, networking, profiler, build packaging, Project Doctor, recovery, procedural world generation, testing center, publishing validation/manifest generation, compliance, localization, security baseline, account DOB policy, and AI orchestration/permissions.

## Native application smoke test

The automated runner also executes the native editor and runtime under a virtual X display where available. An application remaining alive for the full smoke window is a pass; an early exit/crash is a failure.

### Windows

Run:

```text
Build-ForgeEngine-Windows.cmd
Run-ForgeEngine.cmd
```

Acceptance:

- no missing runtime DLLs
- native window appears
- Forge Hub/editor renders
- close operation exits cleanly

### Linux

Run:

```bash
cmake -S . -B build -DFORGE_BUILD_TESTS=ON
cmake --build build -j2
xvfb-run -a timeout 5s ./build/ForgeEngine
```

A timeout exit after the application remains alive for the test window is acceptable; an immediate crash is a failure.

## Manual professional QA matrix

P0 — Startup / project lifecycle
- Launch
- Hub
- Create 2D
- Create 3D
- Open
- Save
- Close
- Reopen
- Recovery

P0 — Scene editing
- Select
- Add object
- Delete
- Transform
- Scene persistence
- Invalid project handling

P0 — Runtime
- Play
- Stop
- Input
- Physics
- Scene rendering
- Runtime error reporting

P1 — Build / release
- Development build
- QA/Testing build
- Release build
- Artifact manifest
- Publish validation
- Missing-toolchain errors

P1 — Security / safety
- No secrets in Git
- Project-root boundaries
- Security scan
- AI permission prompts/boundaries
- Recovery before risky modifications

P1 — AI
- Project inspection
- Planning
- Read permission
- Modify permission
- Build permission
- Publish permission
- No invented project evidence

P2 — Extended systems
- 2D
- 3D
- Audio
- Animation
- UI
- Visual scripting
- Networking
- Localization
- Accessibility
- Profiler
- Plugins

## Release verdict rules

PASS requires:

- automated CTest suite passes
- native startup smoke test passes
- no P0 blocker
- no unexplained crash
- documented limitations match actual implementation

Any missing platform toolchain must be reported as BLOCKED/UNAVAILABLE rather than marked as PASS.
