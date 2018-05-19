# Timewarrior

Thank you for taking a look at Timewarrior!

Timewarrior is a time tracking utility that offers simple stopwatch features as well as sophisticated calendar-base backfill, along with flexible reporting.
It is a portable, well supported and very active Open Source project.

## Installing

### From Package

Thanks to the community, there are binary packages available [here](https://taskwarrior.org/docs/timewarrior/download.html#Distributions).

### Building Timewarrior

Building Timewarrior yourself requires

* git
* cmake
* make
* C++ compiler, currently gcc 4.7+ or clang 3.3+ for full C++11 support
* Python, for running the testsuite

There are two ways to retrieve the Timewarrior sources:

* Clone the repository from Github,

      git clone --recurse-submodules https://github.com/GothenburgBitFactory/timewarrior
      cd timewarrior

* Or download the tarball with curl,

      curl -O https://taskwarrior.org/download/timew-1.1.1.tar.gz

  and expand the tarball

      tar xzf timew-1.1.1.tar.gz
      cd timew-1.1.1

Build Timewarrior, optionally run the test suite (note: the tarball does not contain tests), and install it.

    cmake -DCMAKE_BUILD_TYPE=release .
    make
    [make test]
    sudo make install

This copies files into the right place, and installs man pages.

To build and install Timewarrior for a single user, pass `-DCMAKE_INSTALL_PREFIX=$HOME` to `cmake`:

    cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$HOME .
    make
    make install

## Documentation

There is extensive online documentation.
You'll find all the details at:

    https://taskwarrior.org/docs/timewarrior

At the site you'll find online documentation, downloads, news and more.

## Contributing

Your contributions are especially welcome.
Whether it comes in the form of code patches, ideas, discussion, bug reports, encouragement or criticism, your input is needed.

For support options, take a look at [CONTRIBUTING.md](CONTRIBUTING.md) or visit

    https://taskwarrior.org/support

Visit [Github](https://github.com/GothenburgBitFactory/timewarrior) and participate in the future of Timewarrior.

## License

Timewarrior is released under the MIT license.
For details check the [LICENSE](LICENSE) file.
