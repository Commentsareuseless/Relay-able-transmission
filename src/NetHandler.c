#include "NetHandler.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "FileHandler.h"

#if defined(USE_STATIC_IP)
#   define SERVER_ADDRESS "192.168.1.13"
#else
#   define SERVER_ADDRESS INADDR_ANY
#endif

static int ToS = -1;

// Receipient
static struct sockaddr_in recvAddr;
// Transmitter
static struct sockaddr_in sendAddr;
static int sockfd = 0;
static unsigned long progress = 0;
static unsigned long progressMax = 0;

static inline void ConvertIpToBin(const char* strAddr, in_addr_t* binAddr)
{
    int ret = 0;

    if ('\0' == strAddr[0])
    {
        *binAddr = INADDR_ANY;
        return;
    }

    ret = inet_pton(AF_INET, strAddr, binAddr);
    if (ret <= 0) {
        if (ret == 0)
            fprintf(stderr, "ERROR:\tInvalid IPv4 format\n");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }
}

static inline int SendData(const char* buff, size_t buffSize)
{
    ssize_t ret = -1;
    ret = sendto(sockfd, (const char *)buff, buffSize, MSG_CONFIRM,
            (const struct sockaddr *) &recvAddr, sizeof(recvAddr));
    if (ret < 0)
    {
        fprintf(stderr, "Sending error after %ld bytes\n", progress);
        return -1;
    }
    return ret;
}

static void Cleanup(FILE* handle, int sock)
{
    close(sock);
    fclose(handle);
}

int InitNetHandler(int tos, const char* ipAddr, long port)
{
    in_addr_t binAddress = 0;

    ConvertIpToBin(ipAddr, &binAddress);
    ToS = tos;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    memset(&recvAddr, 0, sizeof(recvAddr));
    memset(&sendAddr, 0, sizeof(sendAddr));

    switch (tos)
    {
    case NH_TOS_RECEIVER:
            // Filling server information
            recvAddr.sin_family        = AF_INET; // IPv4
            recvAddr.sin_addr.s_addr   = binAddress;
            recvAddr.sin_port          = htons(port);

            // Bind the socket with the server address
            if (bind(sockfd, (const struct sockaddr *)&recvAddr, sizeof(recvAddr)) < 0)
            {
                perror("Bind failed");
                exit(EXIT_FAILURE);
            }
        break;

    case NH_TOS_TRANSMITTER:
            sendAddr.sin_family        = AF_INET; // IPv4
            sendAddr.sin_addr.s_addr   = binAddress;
            sendAddr.sin_port          = htons(port);
        break;

    default:
        perror("Invalid Type of Service");
        close(sockfd);
        return -1;
    }

    return 0;
}

int SendFile(const char* fileName)
{
    int ret = 0;
    long fileSize = -1;
    char sendBuff[READ_BUFF_SIZE] = {0};
    FILE* file2Send = NULL;

    if(ToS != NH_TOS_TRANSMITTER)
    {
        fprintf(stderr, "ERROR:\tCannot send files in receipient mode\n");
        return -1;
    }

    InitFileHandler(fileName, file2Send, FH_MODE_READ);

    /* Check file size (we assume processing
       only text files so this tells file size in bytes) */
    fseek(file2Send, 0, SEEK_END);
    fileSize = ftell(file2Send);
    progressMax = fileSize;

    if (fileSize <= 0)
    {
        perror("ERROR:\tFile is empty or error occured during checking file size");
        return -1;
    }

    /* First transmission - send fileName */
    strncpy(sendBuff, fileName, strlen(fileName));
    ret = SendData(sendBuff, sizeof(sendBuff));
    if (ret < 0) {return -1;}

    memset(sendBuff, 0, sizeof(sendBuff));
    ret = snprintf(sendBuff, sizeof(sendBuff), "%ld", fileSize);
    if (ret <= 0)
    {
        perror("ERROR:\tsprintf: Cannot correctly write to buff");
        return -1;
    }
    ret = SendData(sendBuff, sizeof(sendBuff));
    if (ret < 0) {return -1;}

    // Synchronization and stuff :)
    sleep(1);

    do
    {
        ret = ReadTxtFile(sendBuff, sizeof(sendBuff), file2Send);
        if (0 > ret)
        {
            perror("ERROR:\tError while reading file");
            return -1;
        }

        ret = SendData(sendBuff, sizeof(sendBuff));
        if (ret < 0) {return -1;}

        progress += ret;
    } while (ret);

    Cleanup(file2Send, sockfd);
    return 0;
    /*
        Open file
        Check file size
        send filename to server
        send file size to server
        wait 1ms for synchronization

        while(eof)
        {
            read batch of data from file
            send data to server
            update progress variable
        }
        close file
    */
}

int WaitForTransmission()
{
    FILE* recvdFile = NULL;

    if(ToS != NH_TOS_RECEIVER)
    {
        fprintf(stderr, "ERROR:\tCannot receive files in transmitter mode\n");
        return -1;
    }

    /*
        recvfrom(client)
        create file with name received from first transmission
        save received fileSize
        while(received fileSize bytes)
        {
            write data to file
        }
    */
    Cleanup(recvdFile, sockfd);
    return 0;
}
