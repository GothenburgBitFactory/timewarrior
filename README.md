# Timewarrior

[![tests](https://github.com/GothenburgBitFactory/timewarrior/actions/workflows/tests.yaml/badge.svg)](https://github.com/GothenburgBitFactory/timewarrior/actions/workflows/tests.yaml)

Thank you for taking a look at Timewarrior!

Timewarrior is a time tracking utility that offers simple stopwatch features as well as sophisticated calendar-based backfill, along with flexible reporting.
It is a portable, well supported and very active Open Source project.

## Installing

### From Package

Thanks to the community, there are binary packages available [here](https://timewarrior.net/docs/install.html#distributions).

### Building Timewarrior

Building Timewarrior yourself requires

* git
* cmake
* make
* C++ compiler with full C++14 support, currently gcc 4.8.1+ or clang 3.3+ 
* Python 3, for running the testsuite

There are two ways to retrieve the Timewarrior sources:

* Clone the repository from Github,

      git clone --recurse-submodules https://github.com/GothenburgBitFactory/timewarrior
      cd timewarrior

* Or download the tarball with curl,

      curl -O https://github.com/GothenburgBitFactory/timewarrior/releases/download/v1.4.2/timew-1.4.2.tar.gz

  and expand the tarball

      tar xzf timew-1.4.2.tar.gz
      cd timew-1.4.2

Build Timewarrior, optionally run the test suite (note: the tarball does not contain tests), and install it.

    cmake -DCMAKE_BUILD_TYPE=release
    make
    [make test]
    sudo make install

This copies files into the right place (default under `/usr/local`), and installs man pages.

Add the optional parameter `-DCMAKE_INSTALL_PREFIX=/path/to/your/install/location` to the `cmake` command if you want to install Timewarrior at a location other than `/usr/local`.
The `make install` command may not require `sudo` depending on your choice of install location.

## Documentation

There is extensive online documentation.
You'll find all the details at [timewarrior.net/docs/](https://timewarrior.net/docs/).

There you will find the documentation, downloads, news and more.

## Contributing

Your contributions are especially welcome.
Whether it comes in the form of code patches, ideas, discussion, bug reports, encouragement or criticism, your input is needed.

For support options, take a look at [CONTRIBUTING.md](CONTRIBUTING.md) or visit [taskwarrior.org](https://taskwarrior.org/support).

Visit [Github](https://github.com/GothenburgBitFactory/timewarrior) and participate in the future of Timewarrior.

## License

Timewarrior is released under the MIT license.
For details check the [LICENSE](LICENSE) file.
