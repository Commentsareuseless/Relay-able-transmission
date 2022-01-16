#pragma once

/* Type of service, Net handler can be initialized
   to listen on particular port or transmit a file */
#define NH_TOS_RECEIVER   0
#define NH_TOS_TRANSMITTER  1

/**
 * @brief This function creates socket and binds
 *        to it. To send or receive anything use SendFile() or
 *        WaitForTransmission()
 *
 * @param tos Type of Service, configure NH as transmitter or receiver
 * @param ipAddr IP of receipient, should be NULL when tos == NH_TOS_RECEIVER
 * @param port Depending on ToS: port to listen or send file
 * @return [0] - on success [-1] - on error
 */
int InitNetHandler(int tos, const char* ipAddr, long port);

/**
 * @brief This function will fail if NH was configured as receiver
 *
 * @param fileName File to transmit
 * @return [0] - on success, [-1] - on error
 */
int SendFile(const char* fileName);

/**
 * @brief This function will fail if NH was convigured as transmitter
 *
 * @return [0] - on success, [-1] - on error
 */
int WaitForTransmission();

/**
 * @brief For summary, this should not be called before SendFile()
 * 
 * @return Size in bytes of sent file
 */
unsigned long GetFinalNumOfBytesSent();
