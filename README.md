# FST-and-Bimachines

These programs are implementation of some important functions with
transducers and bimachines. <br />
This program construct bimachine by regex if the transducer is functional.

<b>To compile the project you can run:</b> <br />
$  g++ src/main.cpp -o bm src/bimachine.cpp src/check-functionality.cpp src/operations-over-FST.cpp src/construct-fst.cpp

<b>Then the program can be run:</b><br />
$ ./bm \<regex\>

The program takes one string as an argument - regex of ordered pairs of strings.

<b>Regex:</b> <br />
The ordered pair is in the form <a,b>, where *a* and *b* are strings.
The regular expressions can be constructed by concatenating, uniting and using
star operation.<br />
The concatenation is without special symbol.<br />
The union is done using |.<br />
The star operation is done using * after the subregex.<br />
<b>Example:</b><br />
$ ./bm "(<a,A>\*<b,B>\*)\*"

This program will construct a bimachine, because the corresponding transducer is
functional. Then the user can enter a word over the alphabet {a, b}.<br />
The result of the program will be the same string, but uppercase. <br />
The same result can be obtained using the regex (<a,A>|<b,B>)\*

The parsing of the regex must be improved, becuse currently a subregex constructed 
by concatenation must be surrounded by backets before applying the union operation.<br />
<b>For example:</b><br />
$ ./bm "(<a,A>\*<b,B>\*|<c,C>)\*"<br />
This program will not run properly, because <a,A>\*<b,B>\* must be in brackets.<br />
The correct form of runnig the program will be:<br />
$ ./bm "((<a,A>\*<b,B>\*)|<c,C>)\*"
