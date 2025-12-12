#!/bin/bash
echo 
echo "=== Lexer Tests ==="
echo 

mkdir -p tests/lexer/tmp
rm -f tests/lexer/tmp/*.out

for file in tests/lexer/input/*.woo; do 
    base=$(basename "$file" .woo)
    ./main "$file" > "tests/lexer/tmp/$base.out"
    expected="tests/lexer/output/$base.out"
    actual="tests/lexer/tmp/$base.out"

    if diff "$actual" "$expected" > /dev/null; then
        echo "PASS: $base"
    else 
        echo "FAIL: $base"
        echo " diff $actual $expected" 
    fi 

done 





