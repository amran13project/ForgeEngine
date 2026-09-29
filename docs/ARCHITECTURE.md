# Forge Engine Professional Architecture

## Process model

ForgeEngine is the editor process. ForgeRuntime is the standalone game runtime process. Both share the same engine core modules but the editor-only layer is not linked into the runtime target.

## Module ownership

- core: lifecycle, registry, shared math
- project: project creation/opening and metadata
- scene: entity and scene persistence
- renderer: 3D projection foundation
- assets: asset indexing and metadata
- physics: simulation baseline
- ai: behavior and provider abstraction
- scripting: visual graph execution baseline
- audio: WAV inspection baseline
- network: deterministic network simulation lab
- profiler: frame/sample measurement
- build: content packaging and runtime bundling
- doctor: structural diagnostics
- recovery: project snapshots
- plugins: plugin discovery
- testing: lightweight test runner
- world: procedural scene generation baseline
- platform: native window/input abstraction
- editor: Forge Hub + editor UI
- runtime: standalone game application shell

## Dependency principle

The Forge System registry owns initialization order and declares dependencies. New systems should extend the registry instead of creating competing global managers.

## Windows packaging

The Windows builder downloads a native `llvm-mingw-<tag>-ucrt-x86_64.zip` toolchain. The package is kept under `Toolchain/` and is not installed into PATH. The compiler is invoked directly with a console subsystem and `mainCRTStartup` entry point so the application remains compatible with a standard C++ `main()` function.
