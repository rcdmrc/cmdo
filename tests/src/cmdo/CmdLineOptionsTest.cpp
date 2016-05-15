#include "cmdo/CmdLineOptionsTest.h"

CmdLineOptionsTest::CmdLineOptionsTest()
    : noopHandler_([](cmdo::CmdLineOptions::StringList const &,
                     cmdo::CmdLineOptions::StringList const &,
                     cmdo::CmdLineOptions::StringList const &,
                     cmdo::CmdLineOptions::StringList const &) {

}) {

}

void CmdLineOptionsTest::create_input(int *argc, char ***argv) {

  std::array<std::string, 11> args{{
                                      "test_program",
                                      "-s1",
                                      "-s2",
                                      "-a1",
                                      "a1_value",
                                      "-a5",
                                      "good value",
                                      "-a6",
                                      "bad value",
                                      "-a7",
                                      "125"
                                  }};
  int argc_ = 0;
  char **argv_;

  argv_ = new char *[args.size()];
  for (std::string const &s : args) {
    asprintf(&(argv_[argc_++]), "%s", s.c_str());
  }

  *argc = argc_;
  *argv = argv_;
}

