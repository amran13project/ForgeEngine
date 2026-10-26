# Forge Engine platform boundaries

The current native application targets Windows and Linux/X11 development environments. Windows x64 is the first CI target. These are source/build targets, not a guarantee of release-ready support until CI and platform runtime checks pass.

The editor does not require an account and does not make network requests. Account and AI code retained from earlier repository work is not part of the active editor workflow.

The publishing module validates local metadata and can write a submission manifest. It does not upload a build or submit to a store. External signing, credentials, platform approval, and a real executable export are not implemented.

## Environment abstraction

Forge platform-dependent environment-variable access is centralized in `src/platform/Environment.*`.
Engine code should use `forge::platform::getEnvironmentVariable`, `setEnvironmentVariable`, and `unsetEnvironmentVariable` instead of calling POSIX or Windows environment APIs directly.