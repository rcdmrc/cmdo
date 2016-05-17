#include "cmdo/CmdLineOptionsTest.h"

CmdLineOptionsTest::CmdLineOptionsTest()
    : noopHandler_([](cmdo::CmdLineOptions::StringList const &,
                      cmdo::CmdLineOptions::StringList const &,
                      cmdo::CmdLineOptions::StringList const &,
                      cmdo::CmdLineOptions::StringList const &) {

}) {

}

void CmdLineOptionsTest::create_argv(int *argc_out, char ***argv_out,
                                     const std::vector<std::string> &args,
                                     std::string const &program_name) {
  if (program_name.empty()) {
    throw std::runtime_error("program name cannot be empty.");
  }

  int argc_ = 0;
  char **argv_;

  argv_ = new char *[args.size() + 1];

  asprintf(&(argv_[argc_++]), "%s", program_name.c_str());
  for (std::string const &s : args) {
    asprintf(&(argv_[argc_++]), "%s", s.c_str());
  }

  *argc_out = argc_;
  *argv_out = argv_;
}


