#include <gtest/gtest.h>
#include <sstream>
#include <tuple>
#include "CSVParser.h"

TEST(CSVParserTest, BasicCSVParsing) {
std::string input = "1,John,30\n2,Alice,25\n";
std::istringstream stream(input);
CSVParser<int, std::string, int> parser(stream);

auto it = parser.begin();
auto row1 = *it;
++it;
auto row2 = *it;

EXPECT_EQ(std::get<0>(row1), 1);
EXPECT_EQ(std::get<1>(row1), "John");
EXPECT_EQ(std::get<2>(row1), 30);

EXPECT_EQ(std::get<0>(row2), 2);
EXPECT_EQ(std::get<1>(row2), "Alice");
EXPECT_EQ(std::get<2>(row2), 25);
}

TEST(CSVParserTest, QuotedStringParsing) {
std::string input = "1,\"John, Doe\",30\n2,\"Alice, Wonderland\",25\n";
std::istringstream stream(input);
CSVParser<int, std::string, int> parser(stream);

auto it = parser.begin();
auto row1 = *it;
++it;
auto row2 = *it;

EXPECT_EQ(std::get<0>(row1), 1);
EXPECT_EQ(std::get<1>(row1), "John, Doe");
EXPECT_EQ(std::get<2>(row1), 30);

EXPECT_EQ(std::get<0>(row2), 2);
EXPECT_EQ(std::get<1>(row2), "Alice, Wonderland");
EXPECT_EQ(std::get<2>(row2), 25);
}

TEST(CSVParserTest, SkipLines) {
std::string input = "header1,header2,header3\n1,John,30\n2,Alice,25\n";
std::istringstream stream(input);
CSVParser<int, std::string, int> parser(stream, 1);

auto it = parser.begin();
auto row1 = *it;
++it;
auto row2 = *it;

EXPECT_EQ(std::get<0>(row1), 1);
EXPECT_EQ(std::get<1>(row1), "John");
EXPECT_EQ(std::get<2>(row1), 30);

EXPECT_EQ(std::get<0>(row2), 2);
EXPECT_EQ(std::get<1>(row2), "Alice");
EXPECT_EQ(std::get<2>(row2), 25);
}

TEST(CSVParserTest, ParsingError) {
std::string input = "1,John,30\n2,Alice\n";
std::istringstream stream(input);
CSVParser<int, std::string, int> parser(stream);

auto it = parser.begin();

EXPECT_THROW({
++it;
}, std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
