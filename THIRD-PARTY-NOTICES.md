# Third-party notices

## Native application

The current C++ source does not vendor a third-party library. Windows builds use operating-system SDK libraries (`user32`, `gdi32`, `shell32`, `ole32`, and `ws2_32`). Linux builds use the system X11 development libraries. These are supplied by the target platform/toolchain and are not copied into the source repository.

No model, texture, or other third-party sample artwork is bundled.

## GitHub Actions

The workflow files use GitHub-maintained actions pinned to full commit SHAs:

- `actions/checkout` v6: `d23441a48e516b6c34aea4fa41551a30e30af803` (CI/release workflow).
- `actions/checkout` v6: `1af3b93b6815bc44a9784bd300feb67ff0d1eeb3` (Pages workflow).
- `actions/configure-pages` v5: `983d7736d9b0ae728b81ab479565c72886d7745b`.
- `actions/upload-pages-artifact` v4: `7b1f4a764d45c48632c6b24a0339c27f5614fb0b`.
- `actions/deploy-pages` v4: `d6db90164ac5ed86f2b6aed7e0febac5b3c0c03e`.

The GitHub Pages workflow follows the official custom-workflow requirements for a Pages artifact, `pages: write`, `id-token: write`, and the `github-pages` environment. See the [official Pages workflow documentation](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages).

The release workflow uses the GitHub-hosted `gh` CLI and the workflow's scoped `GITHUB_TOKEN`; it does not add another action or store a personal token.
