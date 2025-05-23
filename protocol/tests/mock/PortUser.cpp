#include "PortUser.hpp"

using namespace protocol;
using namespace protocol::mock;

PortUser::PortUser(std::string_view name, std::ostream& out)
    : name(name)
    , sendPool{}
    , receivePool{}
    , frameReceivePool{}
    , packet(0)
    , out(out)
{
}

void PortUser::init(Port* p)
{
    port = p;
}

void PortUser::sendTo(PortUser *recipent)
{
    if(sendBuffer.empty())
        throw std::runtime_error("Send buffer is empty");

    recipent->receive(sendBuffer);
    sendBuffer.clear();
}

void PortUser::sendPacket(const Packet& packet)
{
    port->sendPacket(packet);
}

void PortUser::receive(etl::span<const uint8_t> data)
{
    port->process(data);
}

void PortUser::send(etl::span<const uint8_t> data)
{
    if(data.empty())
        throw std::runtime_error("Data is empty");

    sendBuffer.insert(sendBuffer.end(), data.begin(), data.end());
}

void PortUser::receivedPacketCallback(const Packet& packet)
{
    resultData.insert(resultData.end(), packet.getData().begin(), packet.getData().end());
    this->packet = Packet(packet.getType(), resultData);
    isPacketReceived = true;

    out << "{" << name << "}: "
            << "Received packet: " << packet.getType() << "\n";
}

void PortUser::allDataSentCallback()
{
    isAllDataSent = true;
}

std::string getStateName(state::Type state)
{
    switch (state)
    {
        case state::Type::IDLE: return "IDLE";
        case state::Type::PRIMARY: return "PRMN";
        case state::Type::SECONDARY: return "SMRN";
        default: return "ERRO";
    }
}   

std::string getFrameType(Frame::Control::Type type)
{
    switch (type)
    {
        case Frame::Control::Type::DATA: return "DATA";
        case Frame::Control::Type::RECEIVE_READY: return "RR";
        case Frame::Control::Type::RECEIVE_NOT_READY: return "RNR";
        case Frame::Control::Type::CONNECT: return "CONN";
        case Frame::Control::Type::CONNECT_ACK: return "CACK";
        case Frame::Control::Type::DISCONNECT: return "DISC";
        default: return "ERRO";
    }
}

void printFrame(const Frame& frame, std::ostream& out)
{
    out << "Frame: [" 
        << getFrameType(frame.getType()) 
        << ", ID: " << (frame.getId()) 
        << ", P/F: " << frame.isFinal() 
        << "] - SIZE = " << frame.data.size() << "\n"; 

    for(auto byte : frame.data)
        out << std::hex << static_cast<int>(byte) << " ";

    if(not frame.data.empty())
        out << std::dec << std::endl;
}

void PortUser::receivedFrameCallback(state::Type type, const Frame& frame)
{
    out << "{" << name << "}: "
        << "[" << getStateName(type) << "]: "
        << "Received ";
    printFrame(frame, out);
}

void PortUser::sentFrameCallback(state::Type type, const Frame& frame)
{
    out << "{" << name << "}: "
        << "[" << getStateName(type) << "]: "
        << "Sent ";
    printFrame(frame, out);
}

void PortUser::stateChangedCallback(state::Type previous, state::Type actual, bool wasError)
{
    out << "{" << name << "}: "
        << "[" << getStateName(previous) << "]: "
        << "State changed to [" << getStateName(actual) << "]"
        << (wasError ? " with error" : "")
        << "\n";
}

void PortUser::allDataSentCallback(state::Type type)
{
    out << "{" << name << "}: "
        << "[" << getStateName(type) << "]: "
        << "All data sent\n";
}