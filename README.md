# Matrix Multiplication code test

This is also available on git hub at [github.com](https://github.com/chisolm/MMatrix).

---

[//]: # (Image References)

[image1]: ./perf_graph.png "Eigen is much better"

## Dependencies

The matrix class and implentation have no external dependancies than c++11 and
STL.  They were tested clang:

My test program currently depends on the Eigan library.  See the discussion at the
bottom of this README.md about the use of the Eigan library.  That was a choice 
made in knowledge of the instructions about external use, again, please see below.

The Eigen library may be downloaded at: [current version](http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz)

Note file name may differ:
```
gunzip eigen-eigen-5a0156e40feb.tar.gz
tar -xvf eigen-eigen-5a0156e40feb.tar
```

Clang used on OSX:
```
$ clang++ --version
Apple LLVM version 9.0.0 (clang-900.0.39.2)
Target: x86_64-apple-darwin17.3.0
Thread model: posix
InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
```

Also on Linux with:

```
c++ (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## Build instructions

### OSX
```
clang++ -march=core2 -O3 -Wall -g -std=c++11 -I . -I eigen-eigen-5a0156e40feb/ mmatrix_test.cpp -o mmult
```

### Linux
```
c++ -march=core2 -O3 -Wall -g -std=c++11 -I . -I eigen-eigen-5a0156e40feb/ mmatrix_test.cpp -o mmult
```


## Run instructions

To run all the matrix test cases, simply use the -a option.
```
$ ./mmult -a
```

If the goal is to check performance, see setting matrix size with -m and -n.

## Discussion

I'll cover a short discussion of a couple design points for the matrix class.

I used a single dimension vector to simplify the matrix class.  It allows the use
of standard constructors and destructors.

Other than matrix multiply, the other methods are very straight forward.

Performance.  I spent more time than I should have on the performance of the long
pole item, matrix multiply.  It turns out that most obvious choices of precomputation,
loop unrolling and vectorization are handling by modern compilers and implementations
to do those manually do not generate a benefit.

The only performance change I tested that was useful was re-ordering the loop order
from the naive implementation to increase locality of access.

![alt text][image1]

As you can see from the image above.  The Eigen library is clearly the better 
performer.

I did make the choice to remove the less performant matrix multiple methods.

### Test program

The test program runs through a series of tests to test each method of the class. It 
tests:
- declaration
- initialization
- multiplication
- transposition
- performance

This test program uses the eigan library.  I read the instructions a couple times before I
decided to use the library as a reference check.  You did state that the client 
could not/would not use an external library.  I read that as implying that we as
the provider we were not under that restriction and were free to use an external library
to bootstrap the process.  I used the eigan library for a correctness check and a scaling 
reference.  My understanding is that this would not ship to the client and likely
should be segregated to a separate test.  My apologies if you feel that this was 
finessing the rules too much.


