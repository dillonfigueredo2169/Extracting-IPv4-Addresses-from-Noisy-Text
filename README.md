# Extracting IPv4 Addresses from Noisy Text

A C++ program that reads lines of text and extracts a single valid IPv4 address,
optionally followed by a `:port`, embedded anywhere in the line. All parsing and
digit accumulation is done by hand. No `atoi`/`strtol`/`stoi`, no `inet_*`
functions, and no regex.

## Files

- `ipv4_extract.cpp`: the program (`extractIPv4` plus the input loop in `main`)
- `tests.txt`: test cases, one per line, as `input|expected output`
- `run_tests.sh`: compiles the program and checks every case in `tests.txt`

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra -o ipv4_extract ipv4_extract.cpp
./ipv4_extract
```

Type a line of text at the prompt. Enter `END` to quit.

## Run the tests

```bash
chmod +x run_tests.sh
./run_tests.sh
```

## How the parsing works

1. The line is split into **maximal runs** of digits, periods, and colons. Any
   other character ends a run.
2. Each run is checked against the grammar `octet.octet.octet.octet[:port]` in
   full. A run that fails is rejected as a whole and is never searched for a
   smaller valid piece.
3. The first run that matches is returned. If none match, no address is found.

Octets are 1 to 3 digits with a value from 0 to 255. Ports are 1 to 5 digits
with a value from 0 to 65535. Neither may have a leading zero unless the value
is exactly 0.
