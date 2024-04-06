# Assignment 3
## Contributor(s)
1. Emmett Kogan

## Notes
1. The max length of a line of input isn't actually specified, for now I have a buffer 128 bytes long
2. The depth of the FIFOs isn't specified (and it won't be), for now I just have them 5 entries deep

## Build Instructions
1. The Makefile has a `default` rule and a `clean` rule. Default produces `a3` which is the program to run. `clean` deletes this file.
2. `a3` expects two command line arguements, where arg1 is the path to an input file, and arg2 is the number of worker threads to spawn.
3. I also implemented `test.sh` that tests all tests in `testiles` with various numbers of threads and reports on any issues/deadlock. Note that I used the `-w` flag in diff to ignore whitespace because the given input/output files uses CRLF instead of LF, which throws things off a bit.
