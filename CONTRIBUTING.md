# Contributing to Timewarrior

Do I need to be a developer to contribute?

No, we need help from a variety of skilled or organized individuals.
Anyone can contribute, and everyone is encouraged to do so.

Here are the different ways you might conѕider contributing:

1. Reviews
1. Bug reports
1. Feature requests
1. Code contribution

Please read the respective section below about the details.

Otherwise you can spread the word and recommend Timewarrior to your friends and colleagues.


## Reviews

Use the project, become familiar with it, and make suggestions.
There are always ongoing discussions about new features and changes to existing features.
Tell us what works for you, and what doesn't.

Join us in the #taskwarrior IRC channel on freenode.net.
Many great ideas, suggestions, testing and discussions have taken place there.
It is also the quickest way to get help, or confirm a bug.

Review documentation: there are man pages, online articles, tutorials and so on, and these may contain errors, or they may not convey ideas in the best way.
You can help improve it.
Documentation is a separate effort from the codebase, and includes all web sites, and all are available using git.

Take a look at the bug database, and help triage the bug list.
Bug triage is very useful and much needed.
This is a review process that involves confirming bugs, providing additional data, information or analysis.
Re-test bug reports on different environments and versions.
You could check to see that an old bug is still relevant - sometimes they are not.

Review the source code, and point out inefficiencies, problems, unreadable functions, bugs and assumptions.


## Bug reports

Before you submit a bug report, make sure you are using the latest version of Timewarrior. 
Also please take your time and scan the current bug tickets on our [Github issue tracker](https://github.com/GothenburgBitFactory/timewarrior/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3Abug) whether your issue has already been reported.

When you submit a bug report, be precise and put as much information into your bug report as possible.
You should at least provide
* your version of Timewarrior (output of `timew --version`)
* your OS version information
* your Python version (output of `python --version`)

Your bug report should describe what the bug is and provide the necessary minimal steps to reproduce it.

An example:

    When I do this
    And I do that
    And I call "timew command"
    Then I should get "FOO"
    But the actual output is "BAR"


## Feature requests

As for bug reports, you should check our [Github issue tracker](https://github.com/GothenburgBitFactory/timewarrior/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3Aenhancement) whether your feature has already been requested.

When you submit a feature request, provide a use case which captures the overall intention of your feature, not the technical implementation.

As an example

    I want to visualize time differences in reports

is better than

    I want this output to appear in red and bold letters

This not only tells us that you think something is missing from the software, but gives us insights into how you use it.
Plus, you might get your feature implemented.


## Code contribution

There are different ways you can contribute code to the project:

1. Add extensions
1. Add tests
1. Fix bugs
1. Add features


### Extensions

Extensions are standalone programs or scripts that consume Timewarrior data output.

Extensions are the easiest way to contribute as you only need to adhere to the extension API, but are otherwise free in your choice of programming language and style. 

Consult the [documentation](https://timewarrior.net/docs/api.html) on how you can use the extension API.


### Tests, Bug-fixes and Features

In general your contributions have to be associated with an issue on our [Github issue tracker](https://github.com/GothenburgBitFactory/timewarrior/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen).
See the sections above on how to submit bug reports and feature requests.

Code contributions are only accepted as pull-requests.
See the next section on how to make a pull-request.

Any code contributions should have sufficient test coverage.

When writing bug-fixes, first write a test to confirm it, then add the commit to fix it.
When implementing a feature, make sure its behaviour is defined by tests.

Tests are likely the most useful contributions of all, because they not only improve the quality of the code, but prevent future regressions, therefore maintaining quality of subsequent releases.

Nevertheless, all code has to adhere to the coding style guidelines and project architecture. See the respective section below.

If you want to go the extra mile, update the ChangeLog and add your name to the AUTHORS file in the right place.

By contributing, you are declaring that you have the right to submit the code under the project licensing terms.

 
## How to make a pull-request

The main branch for development is named `dev`.
This is the branch where your changes must go.
The `master` branch always points to the latest release.
No development takes place here.

To make a pull request you need to have a Github account.

1. Fork the [Timewarrior repository](https://github.com/GothenburgBitFactory/timewarrior) on Github.
1. Checkout the development branch.

       $ git checkout dev

1. Create a feature branch.

       $ git checkout -b feature_branch

1. Commit your changes, and finally push to the remote repository.
   Use a commit message that matches the prevailing format. (See `git log` for examples.)
   
       $ git commit --signoff -am '<issue>: <description>'
       ...
       $ git push origin feature_branch
    
    Furthermore, commits should be signed off according to the [DCO](DCO) (use `-s` / `--signoff` flag when commiting).
    
1. Create the pull request on Github.

## What happens next?

Your patch will be reviewed by the maintainers.
You may receive feedback, which may include requested changes.
Your patch may be accepted and applied immediately.

Your authorship will be preserved in the commit.
We want you to be properly credited for your work.

Near the end of the development cycle, when a release is close, your patch may be held until the next development cycle begins, unless it is shown to be minimal and not destabilizing.
In mid-cycle your patch may be immediately applied and pushed.


## What are the causes for a patch to be rejected?

Most likely if your patch is rejected, there are some good reasons.
This should be made clear, with recommendations for improving the patch.
Please rework the patch and resubmit.

Here are some reasons that cause patch rejection:

- The code wasn't tested, or breaks the test suite.
  Every commit should improve the project.

- New functionality was submitted without supporting tests.
  Without tests, how would we know it works on all platforms?

- New functionality, or modified behavior was not first cleared by the maintainers as being desirable.
  We don't want you to waste your time either.
  But we do have a plan, and all changes must conform to the plan, otherwise it morphs the project into something we don't want.

- The code does not follow the formatting of the surrounding code.
  Your code should blend in perfectly, and not stand out for reasons of formatting, naming convention, tabs etc.

- The changes need supporting documentation changes which were not made.

- Inefficient, non-portable, or broken code.


## Coding style

The general guideline is simply this:

> Make all changes and additions such that they blend in perfectly with the surrounding code, so it looks like only one person worked on the source, and that person is rigidly consistent.

To be a little more explicit, the common elements across the languages are:

* Indent code using two spaces, no tabs
* With Python, follow PEP8 as much as possible
* Surround operators and expression terms with a space
* No cuddled braces
* No need to stick slavishly to 80 characters per line, but do not make them overly long (rule of thumb: *One sentence/statement per line*)!
* Class names are capitalized, variable names are not

We target Python 3 so that our test suite runs on the broadest set of platforms.

We can safely target C++14 because all the default compilers on our supported platforms are ready.
Feel free to use C++17 and C++20 provided that all build platforms support this.
