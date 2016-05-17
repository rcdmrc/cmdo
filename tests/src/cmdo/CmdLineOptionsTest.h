#ifndef CMDO_CMDLINEOPTIONSTEST_H
#define CMDO_CMDLINEOPTIONSTEST_H

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <string>
#include <cmdo/CmdLineOptions.h>

class CmdLineOptionsTest : public ::testing::Test {
public:
  CmdLineOptionsTest();

  static void create_argv(int *argc_out, char ***argv_out,
                          std::vector<std::string> const &args,
                          std::string const &program_name = "test_program");

protected:
  // avoids program exit.
  cmdo::CmdLineOptions::ParserResultHandler noopHandler_;
};


TEST_F(CmdLineOptionsTest, Create) {
  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);

  EXPECT_TRUE(gf.program_name().empty());
  EXPECT_EQ(desc, gf.program_description());
}

TEST_F(CmdLineOptionsTest, Parse_Switch) {
  int argc;
  char **argv;
  std::vector<std::string> const args{{"-s1"},
                                      {"-s2"}};
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.set_parser_result_handler(noopHandler_);

  gf.add_switch("-s1", "switch #1", false);
  gf.add_switch("-s2", "switch #2", true);
  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);
  EXPECT_TRUE(gf.get_switch("-s1"));
  EXPECT_FALSE(gf.get_switch("-s2"));
  EXPECT_TRUE(leftOvers.empty());
}

TEST_F(CmdLineOptionsTest, Parse_Optional) {
  int argc;
  char **argv;
  std::vector<std::string> const args{{"-a1"},
                                      {"a1_value"}};
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.set_parser_result_handler(noopHandler_);
  gf.add_optional("-a1", "argument #1", "empty");
  gf.add_optional("-a2", "argument #2", "empty");
  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);
  EXPECT_EQ("a1_value", gf.get_option("-a1"));
  EXPECT_EQ("empty", gf.get_option("-a2"));
  EXPECT_TRUE(leftOvers.empty());
}

TEST_F(CmdLineOptionsTest, Parse_Required) {
  int argc;
  char **argv;
  std::vector<std::string> const args{{"-a1"},
                                      {"a1_value"}};
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  cmdo::CmdLineOptions::StringList undefList;
  cmdo::CmdLineOptions::ParserResultHandler captureUndef = [&undefList](
      cmdo::CmdLineOptions::StringList const &,
      cmdo::CmdLineOptions::StringList const &undef,
      cmdo::CmdLineOptions::StringList const &,
      cmdo::CmdLineOptions::StringList const &) {
    undefList = undef;
  };
  gf.set_parser_result_handler(captureUndef);

  gf.add_required("-a1", "argument #1");
  gf.add_required("-a2", "argument #2");

  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);

  EXPECT_EQ("a1_value", gf.get_option("-a1"));
  EXPECT_THROW(gf.get_option("-a2"), cmdo::OptionNotSet);
  EXPECT_TRUE(!undefList.empty() && undefList.at(0) == "-a2");
  EXPECT_TRUE(leftOvers.empty());
}

TEST_F(CmdLineOptionsTest, Get_Undefined_Throws) {
  int argc;
  char **argv;
  std::vector<std::string> const args;
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.set_parser_result_handler(noopHandler_);

  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);

  EXPECT_THROW(gf.get_option("-a3"), cmdo::UndefinedOption);
}

TEST_F(CmdLineOptionsTest, Get_Throws_On_Type_Cast_Fail) {
  int argc;
  char **argv;
  std::vector<std::string> const args{{"-a1"},
                                      {"a1_value"}};
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.set_parser_result_handler(noopHandler_);

  gf.add_required("-a1", "argument #1");
  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);

  EXPECT_THROW(gf.get_option_as<int>("-a1"), cmdo::BadCast);
}

TEST_F(CmdLineOptionsTest, Set_Parser_Result_Handler_Throws_On_Bad_Handler) {
  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  cmdo::CmdLineOptions::ParserResultHandler badHandler;

  EXPECT_THROW(gf.set_parser_result_handler(badHandler), cmdo::BadFunction);
}

TEST_F(CmdLineOptionsTest, Attach_Validator) {
  int argc;
  char **argv;
  std::vector<std::string> const args{{"-a5"},
                                      {"good value"}};
  create_argv(&argc, &argv, args);

  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.set_parser_result_handler(noopHandler_);
  gf.add_optional("-a5", "expects 'good value'", "bad value");

  bool valueIsGood1(false);
  cmdo::CmdLineOptions::ValidatorFunction v1
      = [&valueIsGood1](std::string const &name,
                        std::string const &value) -> bool {

        valueIsGood1 = (value == "good value");
        return valueIsGood1;
      };
  gf.attach_validator("-a5", v1);

  bool valueIsGood2(false);
  cmdo::CmdLineOptions::ValidatorFunction v2
      = [&valueIsGood2](std::string const &name,
                        std::string const &value) -> bool {

        valueIsGood2 = (value.size() == strlen("good value"));
        return valueIsGood2;
      };

  gf.attach_validator("-a5", v2);
  cmdo::CmdLineOptions::StringList leftOvers;
  gf.parse(argc, argv, leftOvers);
  EXPECT_TRUE(valueIsGood1);
  EXPECT_TRUE(valueIsGood2);
}

TEST_F(CmdLineOptionsTest, Attach_Validator_Throws_On_Undefined_Option) {
  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  bool valueIsGood1(false);
  cmdo::CmdLineOptions::ValidatorFunction v1
      = [&valueIsGood1](std::string const &name,
                        std::string const &value) -> bool {

        valueIsGood1 = (value == "good value");
        return valueIsGood1;
      };

  EXPECT_THROW(gf.attach_validator("-a5", v1), cmdo::UndefinedOption);
}

TEST_F(CmdLineOptionsTest, Attach_Validator_Throws_On_Bad_Function) {
  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  gf.add_optional("-a5", "expects 'good value'", "bad value");

  cmdo::CmdLineOptions::ValidatorFunction v1;

  EXPECT_THROW(gf.attach_validator("-a5", v1), cmdo::BadFunction);
}

TEST_F(CmdLineOptionsTest, Add_Throws_On_Bad_Option) {
  std::string const desc("test program");
  cmdo::CmdLineOptions gf(desc);
  EXPECT_THROW(gf.add_optional("", "this will never work", "bad value"),
               cmdo::BadOption);
  EXPECT_THROW(gf.add_required("", "this will never work"),
               cmdo::BadOption);
  EXPECT_THROW(gf.add_switch("", "this will never work", true),
               cmdo::BadOption);
}

TEST_F(CmdLineOptionsTest, Parse_Stores_Unknown_Options_In_LeftOvers) {
  int argc;
  char **argv;
  std::vector<std::string> const args{
      {"-in"},
      {"input_file.txt"},
      {"unknown1"},
      {"-out"},
      {"output_file.txt"},
      {"unknown2"}
  };
  create_argv(&argc, &argv, args);

  cmdo::CmdLineOptions options("test");
  options.add_required("-in", "input file");
  options.add_required("-out", "output file");
  cmdo::CmdLineOptions::StringList leftOvers;
  options.parse(argc, argv, leftOvers);
//  ASSERT_EQ(2, leftOvers.size());
//  EXPECT_EQ(args.at(2), leftOvers[0]);
//  EXPECT_EQ(args.at(5), leftOvers[1]);
}


#endif //CMDO_CMDLINEOPTIONSTEST_H
