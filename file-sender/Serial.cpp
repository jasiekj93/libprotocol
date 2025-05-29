#include "Serial.hpp"

#include <stdexcept>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

Serial::Serial(std::string_view path)
{
    handle = open(path.data(), O_RDWR);

    if (handle < 0)
        throw std::runtime_error("Error opening serial port: " + std::string(path));

    termios tty;

    if(tcgetattr(handle, &tty) != 0)
        throw std::runtime_error("Error getting terminal attributes");

        tty.c_cflag &= ~PARENB; //Parity None
        // tty.c_cflag |= PARENB; //Parity Odd 

        tty.c_cflag &= ~CSTOPB; //StopBits One
        // tty.c_cflag |= CSTOPB; // StopBits Two

    tty.c_cflag &= ~CSIZE; 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)

    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control 
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_lflag &= ~ICANON; //Wyłącz odbieranie kanoniczne, czyli otrzymywanie wejścia dopiero po nowej linii \r\n
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

    //VMIN and VTIME
    // VMIN = 0, VTIME = 0: No blocking, return immediately with what is available

    // VMIN > 0, VTIME = 0: This will make read() always wait for bytes (exactly how many is determined by VMIN), 
    //     so read() could block indefinitely.

    // VMIN = 0, VTIME > 0: This is a blocking read of any number chars with a maximum timeout (given by VTIME). 
    //     read() will block until either any amount of data is available, or the timeout occurs. 
    //     This happens to be my favourite mode (and the one I use the most).

    // VMIN > 0, VTIME > 0: Block until either VMIN characters have been received, or VTIME after first character has elapsed. 
    // Note that the timeout for VTIME does not begin until the first character is received.    

    tty.c_cc[VTIME] = 1; //deciseconds = 100ms
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // // Save tty settings, also checking for error
    if (tcsetattr(handle, TCSANOW, &tty) != 0) 
        throw std::runtime_error("Error setting terminal attributes");
            
}

Serial::~Serial()
{
    if (handle >= 0)
    {
        close(handle);
        handle = -1;
    }
}

void Serial::send(etl::span<const uint8_t> data)
{
    ::write(handle, data.data(), data.size());
    tcdrain(handle);   //wait to be sent
}

etl::span<const uint8_t> Serial::read()
{
    ssize_t bytesRead = ::read(handle, buffer.data(), BUFFER_SIZE);
    
    if (bytesRead < 0)
        throw std::runtime_error("Error reading from serial port");

    return etl::span<const uint8_t>(buffer.data(), static_cast<size_t>(bytesRead));
}