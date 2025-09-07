# Timewarrior Performance Testing Tools

Benchmark Timewarrior commands using [hyperfine](https://github.com/sharkdp/hyperfine) and generate JSON reports for performance analysis.

## Usage

### Docker Execution

Run tests in a container with the latest stable Timewarrior version:

```bash
./run-performance-test <output_dir> <database_dir>
```

* `output_dir`: Directory for benchmark results
* `database_dir`: Directory containing preloaded database entries

### Local Execution 

Prerequisites:

* Timewarrior in `PATH`
* Hyperfine 1.19.0+

```bash
# Run tests (default output: ./performance-test/output)
./performance-test.sh [output_directory]
```

## Output
JSON files (`timew-{command}.json`) containing:

* Execution time metrics (mean, median, min/max, standard deviation)
* Raw measurement data

See [hyperfine documentation](github.com/sharkdp/hyperfine) for output format details.
