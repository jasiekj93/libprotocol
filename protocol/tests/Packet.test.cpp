/**
 * @file Packet.test.cpp
 * @author Adrian Szczepanski
 * @date 2025-05-06
 */

#include <libprotocol/Packet.hpp>

#include <CppUTest/TestHarness.h>

using namespace protocol;

TEST_GROUP(PacketTest)
{
	void setup()
	{

	}

	void teardown()
	{
		
	}
};

TEST(PacketTest, find)
{
    uint8_t data[] = { 0x01, 0x02, 0x00, 0xAB, 0xCD };
    uint8_t payload[] = { 0xAB, 0xCD };
    auto [packet, iterator] = Packet::find(etl::span<uint8_t>(data, sizeof(data)));
    CHECK(packet.has_value());
    CHECK_EQUAL(0x01, packet->getType());
    CHECK_EQUAL(2, packet->getLength());
    MEMCMP_EQUAL(payload, packet->getData().data(), packet->getLength());
    CHECK(iterator == std::end(data));
}

TEST(PacketTest, find_notEnoughData)
{
    uint8_t data[] = { 0x01, 0x02 };
    auto [packet, iterator] = Packet::find(etl::span<uint8_t>(data, sizeof(data)));
    CHECK_FALSE(packet.has_value());
    CHECK(iterator == std::end(data));
}

TEST(PacketTest, serialize)
{
    etl::vector<Byte, 10> payload = { 0xF1, 0xF2, 0xF3, 0xF4 };
    etl::vector<Byte, 10> expected = { 0xA6, 0x04, 0x00, 0xF1, 0xF2, 0xF3, 0xF4 };
    etl::circular_buffer<Byte, Packet::MAX_PACKET_SIZE> output;

    Packet packet(0xA6, payload);
    auto result = packet.serialize(output);
    auto span = etl::span<Byte>(output.begin(), output.end());
    
    CHECK(result);
    CHECK_EQUAL(expected.size(), output.size());
    MEMCMP_EQUAL(expected.data(), span.data(), expected.size());
}