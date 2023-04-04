<div align="center">
<img src="https://avatars.githubusercontent.com/u/36100920?s=200&u=24da05914c20c4ccfe8485310f7b83049407fa9a&v=4"></br>

[![Tests](https://github.com/GothenburgBitFactory/timewarrior/actions/workflows/tests.yaml/badge.svg)](https://github.com/GothenburgBitFactory/timewarrior/actions/workflows/tests.yaml)
[![Release](https://img.shields.io/github/v/release/GothenburgBitFactory/timewarrior)](https://github.com/GothenburgBitFactory/timewarrior/releases/latest)
[![Release date](https://img.shields.io/github/release-date/GothenburgBitFactory/timewarrior)](https://github.com/GothenburgBitFactory/timewarrior/releases/latest)
![Commits since release](https://img.shields.io/github/commits-since/GothenburgBitFactory/timewarrior/latest)
</br>
[![Twitter](https://img.shields.io/twitter/follow/timewarrior_net?style=social)](https://twitter.com/timewarrior_net)
</div>

# Timewarrior
![OS-X downloads](https://img.shields.io/homebrew/installs/dy/timewarrior?label=OS-X%20downloads)
![GitHub downloads](https://img.shields.io/github/downloads/GothenburgBitFactory/timewarrior/total?label=GitHub%20downloads)
![Linux downloads](https://img.shields.io/badge/Linux%20downloads-unknown-gray)

Thank you for taking a look at Timewarrior!

Timewarrior is a time tracking utility that offers simple stopwatch features as well as sophisticated calendar-based backfill, along with flexible reporting.
It is a portable, well-supported and very active Open Source project.

Please visit [timewarrior.net](https://timewarrior.net/docs/) for extensive documentation, downloads, news and more.

## Installing
[![Arch](https://img.shields.io/archlinux/v/community/x86_64/timew)](https://archlinux.org/packages/community/x86_64/timew/)
[![Debian](https://img.shields.io/debian/v/timewarrior/testing)](https://packages.debian.org/search?keywords=timewarrior&searchon=names&suite=all&section=all)
[![Fedora](https://img.shields.io/fedora/v/timew)](https://bodhi.fedoraproject.org/updates/?packages=timew)
[![Homebrew](https://img.shields.io/homebrew/v/timewarrior)](https://formulae.brew.sh/formula/timewarrior#default)
[![Ubuntu](https://img.shields.io/ubuntu/v/timew)](https://packages.ubuntu.com/search?keywords=timewarrior&searchon=names&suite=hirsute&section=all)

### From Package

Thanks to the community, there are binary packages available [here](https://timewarrior.net/docs/install.html#distributions).

### Building Timewarrior

Building Timewarrior yourself requires

* git
* cmake
* make
* C++ compiler with full C++14 support, currently gcc 6.1+ or clang 3.4+ 
* Python 3 (for running the testsuite)
* Asciidoctor (for creating documentation)

There are two ways to retrieve the Timewarrior sources:

* Clone the repository from GitHub,

    git clone --recurse-submodules https://github.com/GothenburgBitFactory/timewarrior
    cd timewarrior

* Or download the tarball with curl,

    curl -O https://github.com/GothenburgBitFactory/timewarrior/releases/download/v1.4.3/timew-1.4.3.tar.gz

  and expand the tarball

    tar xzf timew-1.4.3.tar.gz
    cd timew-1.4.3

Build Timewarrior, optionally run the test suite, and install it.

    cmake -DCMAKE_BUILD_TYPE=release
    make
    [make test]
    sudo make install

This copies files into the right place (default under `/usr/local`), and installs man pages.

Add the optional parameter `-DCMAKE_INSTALL_PREFIX=/path/to/your/install/location` to the `cmake` command if you want to install Timewarrior at a location other than `/usr/local`.
The `make install` command may not require `sudo` depending on your choice of install location.

## Community
[![Twitter](https://img.shields.io/twitter/follow/timewarrior_net?style=social)](https://twitter.com/timewarrior_net)
[![Reddit](https://img.shields.io/reddit/subreddit-subscribers/taskwarrior?style=social)](https://reddit.com/r/taskwarrior/)
[![Libera.chat](https://img.shields.io/badge/IRC%20libera.chat-online-green)](https://web.libera.chat/#taskwarrior)
[![Discord](https://img.shields.io/discord/796949983734661191?label=discord)](https://discord.gg/HYpSAeVH)
[![GitHub discussions](https://img.shields.io/github/discussions/GothenburgBitFactory/timewarrior?label=GitHub%20discussions)](https://github.com/GothenburgBitFactory/timewarrior/discussions)

Timewarrior has a lively community on many places on the internet.
The project has its own Twitter account, and shares community spaces on IRC and Discord with [Taskwarrior](https://github.com/GothenburgBitFactory/taskwarrior).

Best place to ask questions is our [discussions forum on GitHub](https://github.com/GothenburgBitFactory/timewarrior/discussions).
For other support options, take a look at [timewarrior.net/support](https://timewarrior.net/support)

## Contributing
[![Contributors](https://img.shields.io/github/contributors/GothenburgBitFactory/timewarrior)](https://github.com/GothenburgBitFactory/timewarrior/graphs/contributors)
[![Milestone progress](https://img.shields.io/github/milestones/progress/GothenburgBitFactory/timewarrior/11?label=current%20milestone%20issues)](https://github.com/GothenburgBitFactory/timewarrior/milestone/11)
[![Good first issues](https://img.shields.io/github/issues/GothenburgBitFactory/timewarrior/good%20first%20issue)](https://github.com/GothenburgBitFactory/timewarrior/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22)

Contributions are greatly appreciated.
Whether in the form of code patches, ideas, discussion, bug reports, encouragement or criticism, we need you!

For support options, take a look at [CONTRIBUTING.md](CONTRIBUTING.md) or visit [timewarior.net](https://timewarrior.net/support).

Visit [GitHub](https://github.com/GothenburgBitFactory/timewarrior) and participate in the future of Timewarrior.

## License

Timewarrior is released under the MIT license.
For details check the [LICENSE](LICENSE) file.
