# Contributing to RayForge

Thanks for helping improve RayForge, the C++ ray tracer project.

## Development workflow

1. Create a focused branch from `main`.
2. Keep each pull request centered on one improvement or bug fix.
3. Build the project locally before opening a pull request.
4. Add or update tests when behavior changes.
5. Explain the change, validation steps, and any known limitations in the pull request description.

## Code guidelines

- Prefer clear, small C++ functions over dense implementations.
- Keep rendering, geometry, materials, and scene setup responsibilities separated.
- Avoid introducing dependencies when the standard library or existing project code is sufficient.
- Document non-obvious rendering math and numerical stability decisions.

## Pull requests

Please use descriptive titles and include screenshots or rendered output when a visual change is involved. Reviewers should be able to reproduce the result from the pull request description.