# Forge publishing status

The current Publish Center is a metadata validation and local submission-manifest foundation. It does not compile a game executable, sign a platform package, upload an artifact, or submit to a store.

Destination names in the UI are validation profiles only. A real integration would require a separately implemented, tested packaging adapter, user-supplied platform credentials, signing, and platform-side approval. No credentials are requested or stored by the current editor.

The general **Package** command copies project files into a local payload and writes a manifest. It must not be described as a game release build.
