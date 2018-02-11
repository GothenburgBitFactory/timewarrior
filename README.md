# Timewarrior

Thank you for taking a look at Timewarrior!

Timewarrior is a time tracking utility that offers simple stopwatch features as well as sophisticated calendar-base backfill, along with flexible reporting.
It is a portable, well supported and very active Open Source project.

## Installing

### From Package

Thanks to the community, there are binary packages available [here](https://taskwarrior.org/docs/timewarrior/download.html#Distributions).

### From Tarball

Building Timewarrior yourself requires

* git
* cmake
* make
* C++ compiler, currently gcc 4.7+ or clang 3.3+ for full C++11 support

You can download the tarball with curl, as an example of just one of many ways to download the tarball.

    $ curl -O https://taskwarrior.org/download/timew-1.1.1.tar.gz

Expand the tarball, build Timewarrior, optionally run the test suite, and install it.
This copies files into the right place, and installs man pages.

    $ tar xzf timew-1.1.1.tar.gz
    $ cd timew-1.1.1
    $ cmake -DCMAKE_BUILD_TYPE=release .
    ...
    $ make
    ...
    [$ make test]
    ...
    $ sudo make install
    
## Documentation

There is extensive online documentation.
You'll find all the details at:

    http://taskwarrior.org/docs/timewarrior

At the site you'll find online documentation, downloads, news and more.

## Contributing

Your contributions are especially welcome.
Whether it comes in the form of code patches, ideas, discussion, bug reports, encouragement or criticism, your input is needed.

For support options, take a look at:

    http://taskwarrior.org/support

Please use pull requests, or send your code patches to:

    support@gothenburgbitfactory.org

Visit https://github.com/GothenburgBitFactory/timewarrior and participate in the future of Timewarrior.

## License

Timewarrior is released under the MIT license.
For details check the [LICENSE](LICENSE) file.
