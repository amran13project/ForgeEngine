# Windows Builder FIXED12

This build removes all PowerShell `Where-Object` based compiler discovery from the Windows portable builder.

The builder now uses explicit `foreach` checks so Windows PowerShell 5.1 cannot mis-parse `PathType` arguments.

It also detects `clang++.exe` by walking the toolchain directory and validating C++ headers without relying on a fixed folder shape.
