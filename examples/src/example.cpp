//
// Created by Ricardo Mauricio on 5/15/16.
//

#include <cmdo/CmdLineOptions.h>
#include <iostream>

int main(int argc, char **argv) {
  cmdo::CmdLineOptions cmdo("Program that shows you how this thing works.");
  cmdo.add_required("-in", "an input file");
  cmdo.add_required("-out", "an output file");
  cmdo.add_switch("-super", "do the work in super mode.", false);
  cmdo.add_switch("-v", "enable verbose mode", false);
  cmdo.add_optional("-opt1", "this is optional", "5");

  cmdo::CmdLineOptions::StringList leftOvers;
  cmdo.parse(argc, argv, leftOvers);

  std::string const &inputFile = cmdo.get_option("-in");

  std::string const &outputFile = cmdo.get_option("-out");

  std::cout << "input = " << inputFile << "\n";
  std::cout << "output = " << outputFile << "\n";

  bool const verbose = cmdo.get_switch("-v");

  if (cmdo.get_switch("-super")) {
    // make it super
    std::cout << "about to make whatever this is super :D\n";
  } else {
    // make it regular
    std::cout << "about to make whatever this is regular :|\n";
  }
  if (verbose) {
    std::cout << "blah blah blah\n";
  }

  return 0;
}