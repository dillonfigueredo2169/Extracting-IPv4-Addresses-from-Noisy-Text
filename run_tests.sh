#!/bin/bash
# Compiles the program and checks every case in tests.txt.
g++ -std=c++17 -Wall -Wextra -o ipv4_extract ipv4_extract.cpp || exit 1

PROMPT="Enter a string (or 'END' to quit): "
pass=0; fail=0
while IFS= read -r entry; do
    [[ "$entry" == \#* ]] && continue
    input="${entry%%|*}"
    expected="${entry#*|}"
    actual=$(printf '%s\nEND\n' "$input" | ./ipv4_extract | head -n 1)
    actual="${actual#"$PROMPT"}"
    if [[ "$actual" == "$expected" ]]; then
        ((pass++))
    else
        ((fail++))
        echo "FAIL: [$input]"
        echo "  expected: $expected"
        echo "  actual:   $actual"
    fi
done < tests.txt
echo "$pass passed, $fail failed"
