# Git workflow

Forge Engine uses Git as its source of truth.

Recommended flow:

1. Update from the intended base branch without discarding local work.
2. Create or use a feature branch; do not push directly to `main`.
3. Make one coherent change.
4. Build editor/runtime and run CTest.
5. Review `git diff` and `git status`; stage only intended files.
6. Commit with a focused message.
7. Push the feature branch and open a pull request.

Never force-push or use a helper that renames the current branch to `main` or stages every file indiscriminately.

The repository URL for the current project is:

`https://github.com/amran13project/ForgeEngine.git`
