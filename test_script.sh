 2214  [2015-03-09 16:35:50 -0700] GTEST_DIR=`pwd`/third_party/googletest/
 2215  [2015-03-09 16:35:52 -0700] export GTEST_DIR=`pwd`/third_party/googletest/
 2216  [2015-03-09 16:36:32 -0700] g++ -I$GTEST_DIR/include -I$GTEST_DIR -c $GTEST_DIR/src/gtest-all.cc
 2217  [2015-03-09 16:36:42 -0700] ls
 2218  [2015-03-09 16:36:49 -0700] ar -rv libgtest.a gtest-all.o 
 2219  [2015-03-09 16:37:09 -0700] pico test.cpp
 2220  [2015-03-09 16:37:42 -0700] g++ -I$GTEST_DIR/include -pthread test.cpp libgtest.a -o test
 2221  [2015-03-09 16:37:47 -0700] ./test 
 2222  [2015-03-09 16:38:33 -0700] git status
 2223  [2015-03-09 16:38:38 -0700] git add test.cpp
 2224  [2015-03-09 16:38:41 -0700] history

