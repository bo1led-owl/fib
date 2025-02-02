# Max Fibonacci number in under a second

Fun project inspired by [Sheafification of G](https://youtube.com/@sheafificationofg).

Main goal is to calculate the largest possible Fibonacci number in just a second.

## Building and running

Currently implemented algorithms are linear and matrix exponentiation ones.

To build them, run
```
make linear
```
and
```
make matexp
```
respectively.

This will produce binaries with corresponding names in `./build/`.

Passing no arguments will start the benchmarks, passing two arguments will print all Fibonacci numbers in the passed range. Other inputs are invalid.
