# FST-and-Bimachines

These are programs implementing some important functions with
transducers and bimachines.
This program construct bimachine by regex if the transducer is functional.

To compile the project you can run:
$  g++ src/main.cpp -o bm src/bimachine.cpp src/check-functionality.cpp src/operations-over-FST.cpp src/construct-fst.cpp

Then the program can be run:
$ ./bm <regex>

The program takes one string as an argument - regex of ordered pairs of strings.

Regex:
The ordered pair is in the form <a,b>, where *a* and *b* are strings.
The regular expressions can be constructed by concatenating, uniting and using
star operation.
The concatenation is without special symbol.
The union is done using |.
The star operation is done using * after the subregex.
Example:
./bm "(<a,A>\*<b,B>\*)\*"

This program will construct a bimachine, because the corresponding transducer is
functional. Then the user can enter a word over the alphabet {a, b}.
The result of the program will be the same string, but uppercase. 
The same result can be obtained using the regex (<a,A>|<b,B>)\*

The parsing of the regex must be improved, becuse currently a subregex constructed 
by concatenation must be surrounded by backets before applying the union operation.
For example:
./bm "(<a,A>\*<b,B>\*|<c,C>)\*"
This program will not run properly, because <a,A>\*<b,B>\* must be in brackets.
The correct form of runnig the program will be:
./bm "((<a,A>\*<b,B>\*)|<c,C>)\*"
