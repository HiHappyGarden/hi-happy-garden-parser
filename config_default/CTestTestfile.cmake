# CMake generated Testfile for 
# Source directory: /home/antoniosalsi/projects/hi-happy-garden-parser
# Build directory: /home/antoniosalsi/projects/hi-happy-garden-parser/config_default
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[hi-happy-garden-parser_gtest]=] "hi-happy-garden-parser_test")
set_tests_properties([=[hi-happy-garden-parser_gtest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/antoniosalsi/projects/hi-happy-garden-parser/CMakeLists.txt;66;add_test;/home/antoniosalsi/projects/hi-happy-garden-parser/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
subdirs("osal")
