/**
 * @file Frame.test.cpp
 * @author Adrian Szczepanski
 * @date 2025-05-06
 */

#include <libprotocol/Frame.hpp>

#include <CppUTest/TestHarness.h>

using namespace protocol;

TEST_GROUP(FrameTest)
{
	void setup()
	{

	}

	void teardown()
	{
		
	}
};

TEST(FrameTest, control_fromByte)
{
    Frame::Control control(0xA7);

    CHECK_EQUAL(control.isFinal, true);
    CHECK(control.type == Frame::Control::Type::CONNECT);
    CHECK_EQUAL(control.id.get(), 0xA);
}

TEST(FrameTest, control_toByte)
{
    Frame::Control control(0xA7);
    CHECK_EQUAL(control.toByte(), 0xA7);
}

TEST(FrameTest, serialize)
{
    etl::vector<Byte, 10> payload = { 0xF1, 0xF2, 0xF3, 0xF4 };
    etl::vector<Byte, 100> expected = { Frame::START, 0xA6, 0xF1, 0xF2, 0xF3, 0xF4, 0xDE, 0x61, Frame::END };
    etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> output;

    Frame frame(0xA6, payload);
    auto size = frame.serialize(output);

    CHECK_EQUAL(expected.size(), size);
    MEMCMP_EQUAL(expected.data(), output.data(), expected.size());
}

TEST(FrameTest, serialize_withEscapes)
{
    etl::vector<Byte, 10> payload = { Frame::START, Frame::END, Frame::ESC };
    etl::vector<Byte, 100> expected = { Frame::START, 0xA6, Frame::ESC, Frame::START, Frame::ESC, Frame::END, Frame::ESC, Frame::ESC, 0xF2, 0x08, Frame::END };
    etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> output;

    Frame frame(0xA6, payload);
    auto size = frame.serialize(output);

    CHECK_EQUAL(expected.size(), size);
    MEMCMP_EQUAL(expected.data(), output.data(), expected.size());
}

TEST(FrameTest, serialize_noPayload)
{
    etl::vector<Byte, 100> expected = { Frame::START, 0xA6, 0x80, 0x7A, Frame::END };
    etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> output;

    Frame frame(0xA6, {});
    auto size = frame.serialize(output);

    CHECK_EQUAL(expected.size(), size);
    MEMCMP_EQUAL(expected.data(), output.data(), expected.size());
}

TEST(FrameTest, serialize_controlIsEscaped)
{
    etl::vector<Byte, 100> expected = { Frame::START, Frame::ESC, Frame::START, 0xC1, 0xC0, Frame::END };
    etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> output;

    Frame frame(Frame::START, {});
    auto size = frame.serialize(output);

    CHECK_EQUAL(expected.size(), size);
    MEMCMP_EQUAL(expected.data(), output.data(), expected.size());
}

TEST(FrameTest, serialize_crcIsEscaped)
{
    etl::vector<Byte, 100> expected = { Frame::START, 0x03, 0x40, Frame::ESC ,0x01, Frame::END };
    etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> output;

    Frame frame(0x03, {});
    auto size = frame.serialize(output);

    CHECK_EQUAL(expected.size(), size);
    MEMCMP_EQUAL(expected.data(), output.data(), expected.size());
}

TEST(FrameTest, find)
{
    etl::vector<Byte, 10> payload = { 0xF1, 0xF2, 0xF3, 0xF4 };
    etl::vector<Byte, 100> input = { Frame::START, 0xA6, 0xF1, 0xF2, 0xF3, 0xF4, 0xDE, 0x61, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == input.end());
    CHECK(result.has_value());
    CHECK(Frame(0xA6, payload) == result.value());
}

TEST(FrameTest, find_withEscapes)
{
    etl::vector<Byte, 10> payload = { Frame::START, Frame::END, Frame::ESC };
    etl::vector<Byte, 100> input = { Frame::START, 0xA6, Frame::ESC, Frame::START, Frame::ESC, Frame::END, Frame::ESC, Frame::ESC, 0xF2, 0x08, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == input.end());
    CHECK(result.has_value());
    CHECK(Frame(0xA6, payload) == result.value());
}

TEST(FrameTest, find_noPayload)
{
    etl::vector<Byte, 100> input = { Frame::START, 0xA6, 0x80, 0x7A, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == input.end());
    CHECK(result.has_value());
    CHECK(Frame(0xA6, {}) == result.value());
}

TEST(FrameTest, find_controlIsEscaped)
{
    etl::vector<Byte, 100> input = { Frame::START, Frame::ESC, Frame::START, 0xC1, 0xC0, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == input.end());
    CHECK(result.has_value());
    CHECK(Frame(Frame::START, {}) == result.value());
}

TEST(FrameTest, find_crcIsEscaped)
{
    etl::vector<Byte, 100> input = { Frame::START, 0x03, 0x40, Frame::ESC ,0x01, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == input.end());
    CHECK(result.has_value());
    CHECK(Frame(0x03, {}) == result.value());
}

TEST(FrameTest, find_twoFrames)
{
    etl::vector<Byte, 100> input = { Frame::START, 0xA6, 0x80, 0x7A, Frame::END, 
        0x00, 0x00, 0x00, 0x00, 0x00, 
        Frame::START, 0xA6, 0x80, 0x7A, Frame::END };

    auto [result, it] = Frame::find(input);

    CHECK(it == etl::next(input.begin(), 5));
    CHECK(result.has_value());
    CHECK(Frame(0xA6, {}) == result.value());

    auto [result2, it2] = Frame::find({it, input.end()});

    CHECK(it2 == input.end());
    CHECK(result2.has_value());
    CHECK(Frame(0xA6, {}) == result2.value());
}