#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./sensei "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
    else
	echo "$input => $expected expected, but got $actual"
	exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 25 " 10 + 20 - 5 "
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '( 3+5 )/2'
assert 15 '-20 + 35'
assert 1 '20 == 20'
assert 0 '25 <= 20'
assert 0 '25 < 20'
assert 1 '25 > 20'
assert 1 '(25 != 20)'
assert 1 '(100)==(99+1)'

echo OK
