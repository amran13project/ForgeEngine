# Forge Engine Professional 3.0 Platform Foundation

This release extends the native engine with account, birthday, AI orchestration, publishing, compliance, localization, accessibility, security, release and workspace foundations.

The local account provider is intentionally offline-first. A production hosted authentication service can implement the same account boundary without exposing passwords to Forge AI.

Store adapters are validation/planning foundations. Actual platform submission still requires the user's developer account, platform credentials and platform-side approval.

## Environment abstraction

Forge platform-dependent environment-variable access is centralized in `src/platform/Environment.*`.
Engine code should use `forge::platform::getEnvironmentVariable`, `setEnvironmentVariable`, and `unsetEnvironmentVariable` instead of calling POSIX or Windows environment APIs directly.