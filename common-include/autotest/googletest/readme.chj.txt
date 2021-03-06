[2018-04-15] Where do I get these files? (Use googletest 1.8.0 today)

https://github.com/google/googletest/blob/release-1.8.0/googletest/docs/AdvancedGuide.md#fusing-google-test-source-files

Fusing Google Test Source Files

Google Test's implementation consists of ~30 files (excluding its own tests). Sometimes you may want them to be packaged up in two files (a .h and a .cc) instead, such that you can easily copy them to a new machine and start hacking there. For this we provide an experimental Python script fuse_gtest_files.py in the scripts/ directory (since release 1.3.0). Assuming you have Python 2.4 or above installed on your machine, just go to that directory and run

	python fuse_gtest_files.py OUTPUT_DIR

and you should see an OUTPUT_DIR directory being created with files 

	gtest/gtest.h and gtest/gtest-all.cc 

in it. These files contain everything you need to use Google Test. Just copy them to anywhere you want and you are ready to write tests. You can use the scripts/test/Makefile file as an example on how to compile your tests against them.

