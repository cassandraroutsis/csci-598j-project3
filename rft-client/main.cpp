//
// Created by Phillip Romig on 7/16/24.
//
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <array>

#include "timerC.h"
#include "unreliableTransport.h"
#include "logging.h"


#define WINDOW_SIZE 10 // "Your client must limit the window size to 10 datagrams"
#define DEFAULT_TIMER_DURATION 100
int main(int argc, char* argv[]) {

    // Defaults
    uint16_t portNum(12345);
    std::string hostname("isengard.mines.edu");
    std::string inputFilename("input.dat");

    int opt;
    try {
        while ((opt = getopt(argc, argv, "f:h:p:d:")) != -1) {
            switch (opt) {
                case 'p':
                    portNum = std::stoi(optarg);
                    break;
                case 'h':
                    hostname = optarg;
                    break;
                case 'd':
                    LOG_LEVEL = std::stoi(optarg);
                    break;
                case 'f':
                    inputFilename = optarg;
                    break;
                case '?':
                default:
                    std::cout << "Usage: " << argv[0] << " [-h hostname] [-p port] [-d debug_level]" << std::endl;
                    break;
            }
        }
    } catch (std::exception &e) {
        FATAL << "Invalid command line arguments: " << e.what() << std::endl;
        return(1);
    }

    TRACE << "Command line arguments parsed." << std::endl;
    TRACE << "\tServername: " << hostname << std::endl;
    TRACE << "\tPort number: " << portNum << std::endl;
    TRACE << "\tDebug Level: " << LOG_LEVEL << std::endl;
    TRACE << "\tOutput file name: " << inputFilename << std::endl;

    // *********************************
    // * Open the input file
    // *********************************
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        FATAL << "Issue opening file " << inputFilename << std::endl;
    }

    try {

        // ***************************************************************
        // * Initialize your timer, window and the unreliableTransport etc.
        // **************************************************************
        timerC timer(DEFAULT_TIMER_DURATION);
        unreliableTransportC transport(hostname, portNum);

        std::array<datagramS, WINDOW_SIZE> sndpkt; // array with an initial size of 10 store sent, unacknowledged packets
        int sequnum = 0; 
        int nextsequnum = 0;

        // Use modular arithmetic when indexing the sndpkt array, ensuing there are never more than 10 datagrams in the array. 
        sndpkt[sequnum % 10].seqNum = nextseqnum;


        // ***************************************************************
        // * Send the file one datagram at a time until they have all been
        // * acknowledged
        // **************************************************************
        bool allSent(false);
        bool allAcked(false);
        while ((!allSent) && (!allAcked)) {
	
            // Is there space in the window? If so, read some data from the file and send it.

                    // Call udt_recieve() to see if there is an acknowledgment.  If there is, process it.
    
                    // Check to see if the timer has expired.

        }

        // cleanup and close the file and network.

    } catch (std::exception &e) {
        FATAL<< "Error: " << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
