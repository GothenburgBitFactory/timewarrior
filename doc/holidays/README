# Timewarrior Holiday Files

The holiday files were created by the `refresh` script using data from [holidata.net](https://holidata.net).
They can be updated using the following command:

```shell
$ ./refresh
```

This updates all present holiday files with holiday data for the current and the following year (default).

If you need another locale (for example `sv-SE`), do this:

```shell
$ ./refresh --locale sv-SE
```

This creates a file `holidays.sv-SE` containing holiday data for the current and following year.
The id for the locale is composed of the [ISO 639-1 language code](https://en.wikipedia.org/wiki/ISO_639-1) and the [ISO 3166-1 alpha-2 country code](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2).

If you need a specific locale region, do this:

```shell
$ ./refresh --locale de-CH --region BE
```

For regions use the corresponding [ISO 3166-2 code for principal subdivisions](https://en.wikipedia.org/wiki/ISO_3166-2).

To specify a set of years to update, do this:

```shell
$ ./refresh --locale en-US --year 2020 2021 2022
```

If the locale is not yet supported by [holidata.net](https://holidata.net), or there is no data available for the requested year, you will see an error.
