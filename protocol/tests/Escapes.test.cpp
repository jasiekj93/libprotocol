/**
 * @file Escapes.test.cpp
 * @author Adrian Szczepanski
 * @date 2025-05-06
 */

#include <libprotocol/Escapes.hpp>

#include <CppUTest/TestHarness.h>

using namespace protocol;

TEST_GROUP(EscapesTest)
{
    static constexpr Byte ESC = 0x7E;
    static constexpr Byte STR = 0x01;
    static constexpr Byte END = 0x02;

    const etl::vector<Byte, 2> flags = { STR, END };

    EscapeConfig config = {
        .escapeChar = ESC,
        .escapedCharacters = flags
    };
   
	void setup()
	{

	}

	void teardown()
	{
		
	}
};

TEST(EscapesTest, addEscapes_noEscapes)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { STR, END };
    etl::vector<Byte, 10> expected = { ESC, STR, ESC, END };

    CHECK(addEscapes(input, output, config));
    CHECK(output == expected);
}

TEST(EscapesTest, addEscapes_noEscapes_outputTooSmall)
{
    etl::vector<Byte, 2> output;
    etl::vector<Byte, 10> input = { STR, END };

    CHECK_FALSE(addEscapes(input, output, config));
}

TEST(EscapesTest, addEscapes_withEscapes)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { STR, ESC, END };
    etl::vector<Byte, 10> expected = { ESC, STR, ESC, ESC, ESC, END };

    CHECK(addEscapes(input, output, config));
    CHECK(output == expected);
}

TEST(EscapesTest, removeEscapes_noEscapes)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { STR, END };
    etl::vector<Byte, 10> expected = { STR, END };

    CHECK(removeEscapes(input, output, config));
    CHECK(output == expected);
}

TEST(EscapesTest, removeEscapes_noEscapes_outputTooSmall)
{
    etl::vector<Byte, 1> output;
    etl::vector<Byte, 10> input = { STR, END };

    CHECK_FALSE(removeEscapes(input, output, config));
}

TEST(EscapesTest, removeEscapes_withEscapes)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { ESC, STR, ESC, END };
    etl::vector<Byte, 10> expected = { STR, END };

    CHECK(removeEscapes(input, output, config));
    CHECK(output == expected);
}

TEST(EscapesTest, removeEscapes_withEscapesInOutput)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { ESC, STR, ESC, ESC, ESC, END };
    etl::vector<Byte, 10> expected = { STR, ESC, END };

    CHECK(removeEscapes(input, output, config));
    CHECK(output == expected);
}

TEST(EscapesTest, removeEscapes_onlyEscapes)
{
    etl::vector<Byte, 10> output;
    etl::vector<Byte, 10> input = { ESC, ESC, ESC, ESC, ESC, ESC };
    etl::vector<Byte, 10> expected = { ESC, ESC, ESC };

    CHECK(removeEscapes(input, output, config));
    CHECK(output == expected);
}