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
    // * The input files will be raw binary data. Do not try to treat them as if they contain strings.
    // *********************************
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        FATAL << "Issue opening file " << inputFilename << std::endl;
        return(1);
    }

    try {

        // ***************************************************************
        // * Initialize your timer, window and the unreliableTransport etc.
        // **************************************************************
        timerC timer = timerC();
        timer.setDuration(MAX_PAYLOAD_LENGTH); // precaution from ed
        unreliableTransportC transport(hostname, portNum);

        // array with an initial size of 10 store sent, unacknowledged packets
        std::array<datagramS, WINDOW_SIZE> sndpkt; 
        int nextsequnum = 1;
        // int base = 0; // don't need the base of only keeping array of WINDOW_SIZE

        // // Use modular arithmetic when indexing the sndpkt array, ensuing there are never more than 10 datagrams in the array. 
        // sndpkt[sequnum % 10].seqNum = nextseqnum;
        // const size_t datagramSize = sizeof(datagramS);
        char buffer[MAX_PAYLOAD_LENGTH]; // HiTA suggested using a buffer to get the data from the fs


        // ***************************************************************
        // * Send the file one datagram at a time until they have all been
        // * acknowledged
        // **************************************************************
        bool allSent(false);
        bool allAcked(false);
        while ((!allSent)){ // FIXME: GBN && (!allAcked)) {
	
            // Is there space in the window? If so, read some data from the file and send it.
            // WINDOW_SIZE - 1 reflects a full sndpkt array. 
            // if ((nextsequnum % WINDOW_SIZE) < (WINDOW_SIZE-1) ) {// FIXME: GNB
                
                // Read the data in chunks of datagrams... 
                inputFile.read(buffer, MAX_PAYLOAD_LENGTH);
                size_t bytesread = inputFile.gcount();
                TRACE << "Read " << bytesread << " from " << inputFilename << std::endl;

                // if there was data read: 
                if (bytesread > 0) {
                    // add datagram to the sndpkt for safe keeping.
                    datagramS &datagram = sndpkt[nextsequnum % WINDOW_SIZE];

                    // fill in the datagram
                    memcpy(datagram.data, buffer, bytesread);
                    datagram.seqNum = nextsequnum;
                    datagram.payloadLength = bytesread; // FIXME: Might need to cast to uint8_t?  
                    datagram.checksum = computeChecksum(datagram);

                    // send datagram
                    transport.udt_send(datagram);
                    TRACE << "Sending datagram " << nextsequnum << std::endl;

                } else {
                    allSent = true;
                    TRACE << "All data has been sent." << std::endl; 

                    // When a valid datagram with a payload length of zero is received, the server assumes the end of the file. 
                    // It closes the output file and quits.
                    datagramS &datagram = sndpkt[nextsequnum % WINDOW_SIZE];
                    datagram.seqNum = nextsequnum;
                    datagram.payloadLength = 0; // FIXME: Might need to cast to uint8_t?  
                    datagram.checksum = computeChecksum(datagram);

                    // send datagram
                    transport.udt_send(datagram);
                }



            // } // FIXME: gbn


            // Call udt_recieve() to see if there is an acknowledgment.  If there is, process it.
            while (!transport.udt_receive(sndpkt[nextsequnum % WINDOW_SIZE]))
            {
                // Check to see if the timer has expired. If timeout, resend according to gbn
                if (timer.timeout()) {
                    TRACE << "Timeout occured for " << sndpkt[nextsequnum % WINDOW_SIZE].seqNum << std::endl;
                }
            }
            
            TRACE << "Recieved ack for " << sndpkt[nextsequnum % WINDOW_SIZE].seqNum << std::endl;

            nextsequnum++;
        }

        // cleanup and close the file and network.

    } catch (std::exception &e) {
        FATAL<< "Error: " << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
