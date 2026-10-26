# Forge Engine 3.2.0 source package

- `src/` Native engine/editor/runtime source
- `tests/` Core integration tests
- `templates/` Starter project templates
- `docs/` Architecture, build, feature status, roadmap, and privacy boundaries
- `website/` Responsive static product site, docs, and tutorial
- `samples/ProfessionalStarter3D/` Project manifest and version-2 sample scene
- `.github/workflows/` Windows CI, Pages, and tagged release automation
- `THIRD-PARTY-NOTICES.md` Dependency and action inventory
- `Build-ForgeEngine-Windows.cmd` Portable Windows build entry point
- `Run-ForgeEngine.cmd` Editor launcher
- `Run-ForgeRuntime.cmd` Runtime launcher
- `PUSH_TO_GITHUB.cmd` Push this Git repository to the user's ForgeEngine repository
- `GIT_WORKFLOW.md` Git source-of-truth workflow

Do not commit `Toolchain/`, `Cache/`, `build/`, `bin/`, or local project/account data. No public release package exists until a tagged Windows build and test workflow succeeds.
