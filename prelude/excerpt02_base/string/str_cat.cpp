// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "build/build_config.h" // defines OS_WIN
#include "testing/gtest/include/gtest/gtest.h"

#include "base/strings/strcat.h"
#include "base/strings/string16.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"

#include <string>

using namespace base;

namespace prelude {
namespace string_samples {

TEST(CharBasic, SizeMatters) {
  char c1{'a'};
  std::string s1{
      c1, 'b'}; // std::string operate with sequence of single-byte characters
  EXPECT_EQ("ab", s1);

  string16 s2 = ASCIIToUTF16("a"); // note the parameter is a string, not char

  // sizeof operator returns size in byte
  // depending on the system, a byte may consist 8 bits or more. defined in
  // CHAR_BIT so, size of char is one unit of 'byte'
  auto size_unit = sizeof(char);
  EXPECT_EQ(sizeof(s1[0]), size_unit);
  EXPECT_EQ(sizeof(s2[0]), size_unit * 2);
}

#ifdef OS_WIN
TEST(CharBasic, NonAsciiChars) {
  char16 c1_16{L'中'};
  string16 s1_16{c1_16, L'文'};
  string16 s2_16 = StrCat({s1_16, ASCIIToUTF16("123")});
  EXPECT_EQ(L"中文123", s2_16);
}
#endif

TEST(StrCat, OneByte) {
  std::string s1("1");
  std::string s2("22");
  std::string s3("333");

  EXPECT_EQ("", StrCat({std::string()}));
  EXPECT_EQ("1", StrCat({s1}));
  EXPECT_EQ("122", StrCat({s1, s2}));
  EXPECT_EQ("122333", StrCat({s1, s2, s3}));
}

TEST(StrCat, TwoBytes) {
  string16 arg1 = ASCIIToUTF16("1");
  string16 arg2 = ASCIIToUTF16("22");
  string16 arg3 = ASCIIToUTF16("333");

  EXPECT_EQ(ASCIIToUTF16(""), StrCat({string16()}));
  EXPECT_EQ(ASCIIToUTF16("1"), StrCat({arg1}));
  EXPECT_EQ(ASCIIToUTF16("122"), StrCat({arg1, arg2}));
  EXPECT_EQ(ASCIIToUTF16("122333"), StrCat({arg1, arg2, arg3}));
}

} // namespace string_samples
} // namespace prelude
