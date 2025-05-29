/**
 * @file Main.cpp
 * @author Adrian Szczepanski
 * @date 2025-05-27
 */

#include <iostream>

#include <unistd.h>

#include "Handler.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <serial_port> [file_name]" << std::endl;
        return 1;
    }

    std::string serialPort = argv[1];
    std::string fileName = (argc > 2) ? argv[2] : "";

    // try
    // {
        auto handler = std::make_shared<Handler>(serialPort, fileName);
        handler->run();
    // }
    // catch (const std::exception& e)
    // {
        // std::cerr << "Error: " << e.what() << std::endl;
        // return 1;
    // }

	return 0;
}
