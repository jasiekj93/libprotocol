#include "Presenter.hpp"

#include <iostream>

using namespace protocol;

static std::string getStateName(state::Type state);
static std::string getFrameType(Frame::Control::Type type);
static void printFrame(const Frame& frame);

void Presenter::receivedFrameCallback(state::Type type, const Frame& frame)
{
    std::cout << "[" << getStateName(type) << "]: "
        << "Received ";
    printFrame(frame);
}

void Presenter::sentFrameCallback(state::Type type, const Frame& frame)
{
    std::cout << "[" << getStateName(type) << "]: "
        << "Sent ";
    printFrame(frame);
}

void Presenter::stateChangedCallback(state::Type previous, state::Type actual, bool wasError)
{
    std::cout << "[" << getStateName(previous) << "]: "
        << "State changed to [" << getStateName(actual) << "]"
        << (wasError ? " with error" : "")
        << "\n";
}

void Presenter::allDataSentCallback(state::Type type)
{
    std::cout << "[" << getStateName(type) << "]: "
        << "All data sent\n";
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

void printFrame(const Frame& frame)
{
    std::cout << "Frame: [" 
        << getFrameType(frame.getType()) 
        << ", ID: " << (frame.getId()) 
        << ", P/F: " << frame.isFinal() 
        << "] - SIZE = " << frame.data.size() << "\n"; 

    for(auto byte : frame.data)
        std::cout << std::hex << static_cast<int>(byte) << " ";

    if(not frame.data.empty())
        std::cout << std::dec << std::endl;
}