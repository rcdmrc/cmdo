#ifndef CMDO_STRINGUTILTEST_H
#define CMDO_STRINGUTILTEST_H

#include <gtest/gtest.h>
#include <cmdo/StringUtil.h>

class StringUtilTest : public ::testing::Test {

};

TEST_F(StringUtilTest, to_string) {
  EXPECT_EQ("1", cmdo::to_string(1));
  EXPECT_EQ("0.5", cmdo::to_string(0.5));
  EXPECT_EQ("what's up", cmdo::to_string("what's up"));
  EXPECT_EQ("false", cmdo::to_string(false));
  EXPECT_EQ("true", cmdo::to_string(true));
}

TEST_F(StringUtilTest, from_string) {
  EXPECT_FALSE(cmdo::from_string<bool>("false"));
  EXPECT_FALSE(cmdo::from_string<bool>("no"));
  EXPECT_TRUE(cmdo::from_string<bool>("true"));
  EXPECT_TRUE(cmdo::from_string<bool>("yes"));
  EXPECT_EQ(0.5, cmdo::from_string<float>("0.5"));
  EXPECT_EQ(1234, cmdo::from_string<int>("1234"));
}

TEST_F(StringUtilTest, trim) {
  std::string s1 = "    spaces in front";
  std::string s2 = "spaces in back    ";
  std::string s3 = "     spaces front and back    ";
  std::string s4 = "     ";
  std::string s5 = " ";
  std::string s6 = "";

  cmdo::trim(s1);
  EXPECT_EQ("spaces in front", s1);
  cmdo::trim(s2);
  EXPECT_EQ("spaces in back", s2);
  cmdo::trim(s3);
  EXPECT_EQ("spaces front and back", s3);
  cmdo::trim(s4);
  EXPECT_EQ("", s4);
  cmdo::trim(s5);
  EXPECT_EQ("", s5);
  cmdo::trim(s6);
  EXPECT_EQ("", s6);
}

TEST_F(StringUtilTest, split){
  std::string s1 = "b=1";
  std::vector<std::string> v1;
  cmdo::split(v1, s1, '=');
  ASSERT_EQ(2, v1.size());
  EXPECT_EQ("b", v1.at(0));
  EXPECT_EQ("1", v1.at(1));
}

TEST_F(StringUtilTest, split_compresses_separator){
  std::string s1 = "b======1";
  std::vector<std::string> v1;
  cmdo::split(v1, s1, '=');
  ASSERT_EQ(2, v1.size());
  EXPECT_EQ("b", v1.at(0));
  EXPECT_EQ("1", v1.at(1));
}

TEST_F(StringUtilTest, split_skips_empty){
  {
    std::string s1 = "b======";
    std::vector<std::string> v1;
    cmdo::split(v1, s1, '=');
    ASSERT_EQ(1, v1.size());
    EXPECT_EQ("b", v1.at(0));
  }

  {
    std::string s2 = "======b";
    std::vector<std::string> v2;
    cmdo::split(v2, s2, '=');
    ASSERT_EQ(1, v2.size());
    EXPECT_EQ("b", v2.at(0));
  }

  {
    std::string s3 = "======";
    std::vector<std::string> v3;
    cmdo::split(v3, s3, '=');
    ASSERT_TRUE(v3.empty());
  }

}

#endif //CMDO_STRINGUTILTEST_H
