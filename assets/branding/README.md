# App icon integration

The final Forge Engine artwork will be supplied by the project owner. No placeholder graphic is represented as the final logo.

For Windows, add the approved `.ico` file in this folder and configure CMake with:

```powershell
cmake -S . -B build -A x64 -DFORGE_APP_ICON="assets/branding/forge-engine.ico"
```

`FORGE_APP_ICON` is optional and only applies to Windows builds. When unset, the native applications build with the platform default icon. The CMake resource is attached to both ForgeEngine and ForgeRuntime, so the release workflow can consume the future asset without changing application initialization. The legacy portable PowerShell compiler script does not use this option.

For a future website favicon, supply approved `.png` or `.svg` artwork separately and update the static site metadata. The website currently uses a text wordmark and has no icon artwork.
