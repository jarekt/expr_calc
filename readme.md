# Simple expression calc

A simple calculator with operator precedence based on the same principles as a language interpreter ([lexical analysis](https://en.wikipedia.org/wiki/Lexical_analysis) & parsing). Operator precedence is handled using the [shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm).

## operators supported
* ^ / * - +

## How to run
* check the releases tab for executables (linux & windows supported)
* or compile for yourself 
    * some compilers that dont need libm may cause an error, to fix it just change the link_libm flag in CMakeLists.txt
    * visual studio has hard time compiling this (as it's written in c not c++) and therefore requires you to use the debug runtime library (hence the big windows executable size)
* or [run it online](https://repl.it/repls/exprcalc)