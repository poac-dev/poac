# Contributing

Please note we have a [code of conduct](https://github.com/poac-dev/.github/blob/main/CODE_OF_CONDUCT.md),
please follow it in all your interactions with the project.

You can ignore sections marked as "Under Construction".

## How to Contribute

You can contribute to this repository in one of the following ways:

1. **For Trivial Changes**<br>
   If you believe your change is minor (e.g., typo fixes, small improvements),
   feel free to make the change directly and submit a pull request (PR).
2. **For Uncertain Changes**<br>
   If you're unsure whether your proposed change aligns with the project's
   goals, we encourage you to discuss it first by opening an issue or starting
   a discussion.  This helps ensure alignment and reduces potential rework.
3. **For Exploratory Contributions**<br>
   If you're unsure but find it easier to share code, you can create a draft PR
   with the prefix `RFC:` in the title (e.g., `RFC: build: add new feature X`).
   This signals that the PR is a "Request for Comments" and invites feedback
   from the maintainers and community.

## Coding Style

Consistency is key to maintaining a clean and readable codebase. As stated in the
[LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html#introduction):

> **If you are extending, enhancing, or bug fixing already implemented code,
> use the style that is already being used so that the source is uniform and
> easy to follow.**

Please follow this principle to ensure the code remains cohesive and easy to
navigate.

### Naming Conventions (*Under Construction)

The project's naming conventions are specified in the
[.clang-tidy](.clang-tidy) file.  Here's a brief summary:

- **Files/Directories**: `PascalCase`
- **Types/Classes**: `PascalCase`
- **Variables**: `snake_case`
- **Member Variables**: `snake_case_`
- **Functions**: `camelCase`
- **Methods**: `camelCase_`

(Note: Variables use `snake_case` since they tend to be shorter than functions.)

Be mindful of when to use structs vs. classes.  For guidance, refer to the
[Google C++ Style Guide: Structs vs. Classes](https://google.github.io/styleguide/cppguide.html#Structs_vs._Classes).

### Formatting and Linting

Before submitting a PR, ensure your code adheres to the project's coding
standards by running the following tools:

1. Run the linter (`cpplint`)
   ```bash
   poac lint --exclude srcOld --exclude testsOld
   ```
2. Run the formatter (`clang-format`)
   ```bash
   poac fmt --exclude srcOld --exclude testsOld
   ```
3. Run the static analyzer (`clang-tidy`)
   ```bash
   poac tidy  # or make tidy
   ```

### Testing

Always validate your changes to ensure they do not introduce regressions or
break existing functionality:

```bash
poac test  # or make test
```

## Commit Message

We generally follow the
[Git Contribution Guidelines](https://git-scm.com/docs/SubmittingPatches#describe-changes).

- **First Line**: Ensure the first line of your commit message adheres to the
  guidelines for a concise and clear summary.
- **Body**: While not mandatory, it is recommended to follow the rest of the
  guidelines to maintain consistency and provide helpful context for future
  readers.

## Pull Request Style

When creating a PR:

1. **CI**: Verify that all CI checks pass on your fork before submitting the
   PR.  Avoid relying on the CI of this repository to catch errors, as this
   can cause delays or stalls for other contributors.
2. **Commit Structure**: There is no need to squash commits within the PR
   unless explicitly requested.  Keeping separate commits can help reviewers
   understand the progression of changes.
3. **Title and Description**: Ensure the PR title and description are clear,
   concise, and accurately summarize the changes.  These will serve as the
   commit message when the PR is merged, so take care to make them informative
   and professional.
