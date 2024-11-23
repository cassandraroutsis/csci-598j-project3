//
// Created by Phillip Romig on 7/16/24.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <array>

#include "timerC.h"
#include "unreliableTransport.h"
#include "logging.h"


#define WINDOW_SIZE 10 // "Your client must limit the window size to 10 datagrams"
#define DEFAULT_TIMER_DURATION 100

void makeDatagram(datagramS &datagram, size_t bytesread, int nextsequnum, char* buffer) {
// fill in the datagram
    memcpy(datagram.data, buffer, bytesread);
    datagram.seqNum = nextsequnum;
    datagram.payloadLength = bytesread; // FIXME: Might need to cast to uint8_t?  
    datagram.checksum = computeChecksum(datagram);
    TRACE << "Created datagram: " << toString(datagram) << std::endl;
}


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

    INFO << "Command line arguments parsed." << std::endl;
    INFO << "\tServername: " << hostname << std::endl;
    INFO << "\tPort number: " << portNum << std::endl;
    INFO << "\tDebug Level: " << LOG_LEVEL << std::endl;
    INFO << "\tOutput file name: " << inputFilename << std::endl;

    // *********************************
    // * Open the input file
    // * The input files will be raw binary data. Do not try to treat them as if they contain strings.
    // *********************************
    TRACE << "Opening input file: " << inputFilename << std::endl;
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        FATAL << "Issue opening file " << inputFilename << std::endl;
        return(1);
    }
    TRACE << "Input file opened." << std::endl;


    try {

        // ***************************************************************
        // * Initialize your timer, window and the unreliableTransport etc.
        // **************************************************************
        TRACE << "Initializing timer, window and the unreliableTransport etc." << std::endl;
        timerC timer = timerC();
        timer.setDuration(MAX_PAYLOAD_LENGTH); // precaution from ed
        unreliableTransportC transport(hostname, portNum);
        TRACE << "unreliableTransport initialized with hostname, portNum: " << hostname << ", " << portNum << std::endl;

        // array with an initial size of 10 store sent, unacknowledged packets
        TRACE << "Initializign sndpkt array with WINDOW_SIZE: " << WINDOW_SIZE << std::endl;
        std::array<datagramS, WINDOW_SIZE> sndpkt; 
        int nextsequnum = 1;
        int base = 1;

        char buffer[MAX_PAYLOAD_LENGTH]; // HiTA suggested using a buffer to get the data from the fs


        // ***************************************************************
        // * Send the file one datagram at a time until they have all been
        // * acknowledged
        // **************************************************************
        bool allSent(false);
        bool allAcked(false);
        while ((!allSent) && (!allAcked)) {
            // save the next index in sndpkt
            int windowIndex = nextsequnum % WINDOW_SIZE; 
            int baseIndex = base % WINDOW_SIZE;
	
            // Is there space in the window? If so, read some data from the file and send it.
            if (nextsequnum < (base + WINDOW_SIZE) ) {
                
                // clear the buffer
                memset(buffer, 0, sizeof(buffer));

                // Read the data in chunks of datagrams... 
                TRACE << "Reading a chunck of data" << std::endl;
                inputFile.read(buffer, MAX_PAYLOAD_LENGTH);
                size_t bytesread = inputFile.gcount();
                DEBUG << "Read " << bytesread << " from " << inputFilename << ", data: " << buffer << std::endl;

                // if there was data read: 
                if (bytesread > 0) {
                    // add datagram to the sndpkt for safe keeping. Use modular arithmetic when indexing the sndpkt array, ensuing there are never more than WINDOW_SIZE datagrams in the array. 
                    datagramS &datagram = sndpkt[windowIndex];
                    makeDatagram(datagram, bytesread, nextsequnum, buffer);

                    // send datagram
                    DEBUG << "Sending datagram: " << toString(datagram) << std::endl;
                    transport.udt_send(datagram);
                    TRACE << "Sent datagram" << std::endl;

                } else {
                    INFO << "All data has been sent." << std::endl; 
                    allSent = true;

                    // When a valid datagram with a payload length of zero is received, the server assumes the end of the file. 
                    // It closes the output file and quits.
                    DEBUG << "Send datagram with payload length of zero so server knows eof" << std::endl;
                    datagramS &datagram = sndpkt[windowIndex];
                    makeDatagram(datagram, bytesread, nextsequnum, buffer);

                    // send datagram
                    DEBUG << "Sending datagram: " << toString(datagram) << std::endl;
                    transport.udt_send(datagram);
                }
                nextsequnum++;
                TRACE << "Incrementing sequence number" << std::endl;
            } else {
                DEBUG << "No space in sndpkt" << std::endl;
            }


            // Call udt_recieve() to see if there is an acknowledgment.  If there is, process it.
            TRACE << "udt_recieve() to see if there is an acknowledgment" << std::endl;
            if (transport.udt_receive(sndpkt[windowIndex]) == 0)
            {
                // Check to see if the timer has expired. If timeout, resend according to gbn
                if (timer.timeout()) {
                    DEBUG << "Timeout occured for " << sndpkt[windowIndex].seqNum << std::endl;

                    for (int i = baseIndex; i < windowIndex; i++) {
                        transport.udt_send(sndpkt[i]);
                        TRACE << "Resending datagram: " << toString(sndpkt[i]);
                    }
                }
            } else {
                DEBUG << "Recieved ack for " << sndpkt[windowIndex].seqNum << std::endl;
                base = base + windowIndex;
            }
        
            if (base == nextsequnum) {
                TRACE << "All packets have been acked." << std::endl;
                allAcked = true;
            } else {
                TRACE << "Some packets need to be acked." << std::endl;
                allAcked = false;
            }
        }

        // cleanup and close the file and network.
        inputFile.close();
        transport.~unreliableTransportC();

    } catch (std::exception &e) {
        FATAL<< "Error: " << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
