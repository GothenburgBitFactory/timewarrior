Contributing to Timewarrior
===========================

Contributing a patch
--------------------

The public stable branch for the latest release is `master`. We never commit to
the master branch.

The main branch for development is named something like `1.2.3`. This is the
branch where your changes must go. If you see two branches, `1.2.3` and
`1.2.4`, then choose the higher-numbered branch.

Any patch against `master`, or an older development branch, means additional
merge work for maintainers, and will not be accepted. If your change is on the
wrong branch, rebase it onto the right branch. Then retest before submitting.

By contributing a patch, you are declaring that you have the right to submit
the code under the project licensing terms.


Do I need to be a developer to contribute?
------------------------------------------

No, we need help from a variety of skilled or organized individuals.

Here are the different ways you might conѕider contributing. Anyone can
contribute, and everyone is encouraged to do so. Here are many ways to
contribute that may not be immediately obvious to you:

  - Use the project, become familiar with it, and make suggestions. There are
    always ongoing discussions about new features and changes to existing
    features.

  - Join us in the #taskwarrior IRC channel on freenode.net. Many great ideas,
    suggestions, testing and discussions have taken place there. It is also
    the quickest way to get help, or confirm a bug.

  - Join https://answers.tasktools.org and help us by asking, answering and
    voting on questions and answers, directly helping those who ask, and
    helping future users who search for existing answers.

  - Review documentation: there are man pages, online articles, tutorials and
    so on, and these contain many errors, or they may not convey ideas in the
    best way. Perhaps you can help improve it. Contact us - documentation is
    a separate effort from the codebase, and includes all web sites, and all
    are available using git.

  - Take a look at the bug database, and help triage the bug list. This is a
    review process that involves confirming bugs, providing additional data,
    information or analysis. Bug triage is very useful and much needed. You
    could check to see that an old bug is still relevant - sometimes they are
    not. Need permissions to modify issues? Ask.

  - Review the source code, and point out inefficiencies, problems, unreadable
    functions, bugs and assumptions.

  - Fix a bug. For this you'll need C++ and Git skills, but this is one of
    the largest ways you can contribute. We welcome all bug fixes, provided
    the work is done well and doesn't create other problems or introduce new
    dependencies. We recommend talking to us before starting. Seriously.

  - Add unit tests. Unit tests are likely the most useful contributions of
    all, because they not only improve the quality of the code, but prevent
    future regressions, therefore maintaining quality of subsequent releases.
    Plus, broken tests are a great motivator for us to fix the causal defect.
    You'll need Python/C++ skills.

  - Add a feature. Well, let's be very clear about this: adding a feature is
    not usually well-received, and if you add a feature and send a patch, it
    will most likely be rejected. The reason for this is that there are many
    efforts under way, in various code branches. There is a very good chance
    that the feature you add is either already in progress, or being done in a
    way that is more fitting when considering other work in progress. So if
    you want to add a feature, please don't. Start by talking to us, and find
    out what is currently under way or planned. You might find that we've
    already rejected such a feature for some very good reasons. So please
    check first, so we don't duplicate effort or waste anyone's time.

  - Spread the word. Help others become more effective at managing tasks.

  - Encouragement. Tell us what works for you, and what doesn't.

  - Request a feature. This not only tells us that you think something is
    missing from the software, but gives us insights into how you use it.
    Plus, you might get your feature implemented.


How to make a patch
-------------------

Clone the repository from the right place. Do not clone from our github mirror,
we don't use it. Do not patch the contents of one of our tarballs.

    $ git clone --recursive https://git.tasktools.org/TM/timew.git timew.git
    $ cd timew.git

Find the latest development branch, and checkout.

    $ git branch -a
    * master
    remotes/origin/1.2.3
    remotes/origin/1.2.4
    remotes/origin/HEAD -> origin/master
    remotes/origin/master
    $ git checkout 1.2.4
    $ git submodule init
    $ git submodule update

Prepare your fix. Use a commit message that matches the prevailing format.
See 'git log' for examples.

    $ git commit -m '<category>: <description>'

Make sure the test suite passes:

    $ cd test
    $ make
    $ ./run_all

If your fix doesn't pass all tests, change it, and squash your commits into one
commit that works. We do continuous integration, and we want all commits, if
possible, to not fail.

If you add new functionality, add new tests.

Make a patch file:

    $ git format-patch HEAD^

Take the resulting 0001-xxxxxxx.patch file and attach it to an email that is
addressed to support@taskwarrior.org. Do not paste the patch inline in the
message, attach it instead. This allows the maintainer to save the attachment
intact and unmodified by the mail client.

In your message, be sure to convince us that you ran the test suite. If you
want to go the extra mile, update the ChangeLog and add your name to the
AUTHORS file in the right place.


What happens next?
------------------

Your patch will be reviewed by the maintainers. You may receive feedback, which
may include requested changes. Your patch may be accepted and applied
immediately.

Your authorship will be preserved in the commit. We want you to be properly
credited for your work.

Near the end of the development cycle, when a release is close, your patch may
be held until the next development cycle begins, unless it is shown to be
minimal and not destabilizing. In mid-cycle your patch may be immediately
applied and pushed.


What are the causes for a patch to be rejected?
-----------------------------------------------

Most likely if your patch is rejected, there are some good reasons. This should
be made clear, with recommendations for improving the patch. Please rework the
patch and resubmit.

Here are some reasons that cause patch rejection:

- The code wasn't tested, or breaks the test suite. Every commit should improve
  the project.

- New functionality was submitted without supporting tests. Without tests, how
  would we know it works on all platforms?

- New functionality, or modified behavior was not first cleared by the
  maintainers as being desirable. We don't want you to waste your time either.
  But we do have a plan, and all changes must conform to the plan, otherwise
  it morphs the project into something we don't want.

- The code does not follow the formatting of the surrounding code. Your code
  should blend in perfectly, and not stand out for reasons of formatting,
  naming convention, tabs etc.

- The changes need supporting documentation changes which were not made.

- Inefficient, non-portable, or broken code.


http://taskwarrior.org
