# AI roadmap

**Status: Planned. AI is not available in the current editor.**

Pre-existing local orchestration/provider scaffolding remains in the repository for future evaluation. It is not an active editor feature, does not make network requests, and is not represented as a working AI assistant.

Any later implementation must meet these boundaries before release:

- Provider adapters are optional, explicit, and independently testable.
- Clearly distinguish local inference from cloud requests and require informed consent before sending any project content externally.
- Never transmit source files, asset contents, prompts, logs, credentials, or project metadata by default.
- Keep API keys out of project files, source control, logs, and crash output; use the operating system's credential facilities if a cloud provider is ever enabled.
- Limit filesystem and editor actions to explicit user-approved permissions; destructive actions require confirmation.
- Show the plan and affected files before edits, support cancellation, and keep an auditable local action log.
- Document retention, data processing, provider terms, privacy choices, and failure behavior.
- Add privacy, permission, cancellation, offline, and prompt-injection tests before exposing a user interface.

No AI button, model download, API-key setting, remote request, or AI-generated success response belongs in this alpha.
