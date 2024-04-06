# Assignment 5
## Contributor(s)
1. Emmett Kogan

## Build instructions
1. Run `make` to build the driver, run `make app` to build `devuserapp` to test the driver with. Make clean removes all created/temporary files.
2. Once `char_driver.ko` has been built, run `sudo insmod char_driver.ko` to load the driver. Note that there are params `majorno`, `numdevices`, and, `size` which can be used to modify the default values of the driver (500, 3, 16*PAGE_SIZE respectivley).
3. Now, running `devuserapp` with any of the input files in `testfiles/` as a cmd line arguement will test the driver.

I also provided a script, `test.sh`, that does all of this, and runs and diffs each of the provided test cases, just with the default params of the driver.