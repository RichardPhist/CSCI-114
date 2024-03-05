Use Cygwin64 Terminal with a C++11 compiler.

To compile and run use the following:
g++ -std=c++11 main.cpp -o output.o -Ipthread && ./output.o -tc=A -tp=B -qs=C

where A is the sleep range limit for consuming threads
      B is the sleep range limit for producing threads
      C is the size of the bounded buffer queue

Video Link to Execution Demonstration
https://www.youtube.com/watch?v=xPsN8MAkgpQ