# Git workflow

Forge Engine uses Git as its source of truth.

Recommended flow:

1. `git pull --rebase`
2. Make one coherent change.
3. Build editor/runtime.
4. Run CTest.
5. Review `git diff`.
6. Commit with a focused message.
7. Push to `main` or a feature branch.

The repository URL for the current project is:

`https://github.com/amran13project/ForgeEngine.git`
