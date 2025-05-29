/**
 * @file PortImpl.test.cpp
 * @author Adrian Szczepanski
 * @date 2025-05-06
 */

#include <sstream>

#include <libprotocol/PortImpl.hpp>

#include "mock/PortUser.hpp"

#include <CppUTest/TestHarness.h>

using namespace protocol;

TEST_GROUP(PortImplTest)
{
	void setup()
	{

	}

	void teardown()
	{
		
	}

    void process(const std::shared_ptr<mock::PortUser>& sender, const std::shared_ptr<mock::PortUser>& receiver)
    {
        int counter = 0;

        while(not sender->isAllDataSent or
          not receiver->isPacketReceived)
        {
            sender->sendTo(receiver.get());
            receiver->sendTo(sender.get());
            counter++;
        }
    }
};

TEST(PortImplTest, send)
{
    std::ostringstream oss;
    etl::array<uint8_t, Packet::MAX_DATA_SIZE> payload;
    std::fill(payload.begin(), payload.end(), 0xAB);
    Packet packet(0x12, payload);

    std::shared_ptr<mock::PortUser> sender = std::make_shared<mock::PortUser>("Sender", oss);
    std::shared_ptr<mock::PortUser> receiver = std::make_shared<mock::PortUser>("Receiver", oss);

    auto sendPort = PortFactory::createDebugPort(sender, sender, sender, sender->sendPool, sender->receivePool, sender->frameReceivePool);
    auto receivePort = PortFactory::createDebugPort(receiver, receiver, receiver, receiver->sendPool, receiver->receivePool, receiver->frameReceivePool);
    sender->init(sendPort.get());
    receiver->init(receivePort.get());

    sender->sendPacket(packet);
    process(sender, receiver);

    CHECK(sender->isAllDataSent);
    CHECK_EQUAL(0x12, receiver->packet.getType());
    CHECK_EQUAL(payload.size(), receiver->packet.getLength());
    MEMCMP_EQUAL(payload.data(), receiver->packet.getData().data(), receiver->packet.getLength());
}
