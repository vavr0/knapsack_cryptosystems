# Knapsack Cryptosystems

Bachelor project repository focused on the classical Merkle-Hellman knapsack cryptosystem, its cryptanalysis, and selected variants.

## Repository structure

- `code/` - C implementation and benchmarks
- `thesis/` - thesis source files
- `seminar-site/` - Hugo site for weekly seminar progress
- `sources/` - local reference material (private research copies)

## Quick start (code)

Requirements:
- C compiler (`cc`)
- `make`
- GMP library (`libgmp`)

Build:
```bash
cd code
make
```

Run demo:
```bash
./build/knapsack demo
```

Run benchmark:
```bash
./build/knapsack bench
```

Clean:
```bash
make clean
```

## Seminar site

Local preview:
```bash
cd seminar-site
hugo server -D
```

Create weekly log entry:
```bash
cd seminar-site
hugo new --kind log log/YYYY-MM-DD-week-XX.md
```

Build static site:
```bash
cd seminar-site
hugo --minify
```

## Notes

- Weekly log entries use Sunday dates.
- PDFs in `sources/` are for personal research workflow; they are not hosted by the site.
