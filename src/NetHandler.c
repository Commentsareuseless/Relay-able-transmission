#include "NetHandler.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "FileHandler.h"

#if defined(USE_STATIC_IP)
#   define SERVER_ADDRESS "192.168.1.13"
#else
#   define SERVER_ADDRESS INADDR_ANY
#endif

#define LOCALHOST_ADDRESS "127.0.0.1"
#define ACK_MESSAGE "ACK"

static int ToS = -1;

// Receipient
static struct sockaddr_in recvAddr;
// Transmitter
static struct sockaddr_in sendAddr;
static int sockfd = 0;
static unsigned long progress = 0;
static unsigned long progressMax = 0;
static unsigned long maxRecvdBytes = 0;

static void PrintErrnoMessage(unsigned errnoVal)
{
    printf("ERROR:\t%s\n", strerror(errnoVal));
}

static inline void ConvertIpToBin(const char* strAddr, in_addr_t* binAddr)
{
    int ret = 0;

    if (0 == strncmp(strAddr, LOCALHOST_ADDRESS, sizeof(LOCALHOST_ADDRESS)) ||
        '\0' == strAddr[0])
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
    // fprintf(stdout, "INFO:\tSending message: %s\n", buff);
    ret = sendto(sockfd, (const char *)buff, buffSize, 0,
            (const struct sockaddr *) &sendAddr, sizeof(sendAddr));
    if (ret < 0)
    {
        ret = errno;
        PrintErrnoMessage(ret);
        fprintf(stderr, "Sending error after %ld bytes\n", progress);
        return -1;
    }
    return ret;
}

static int RecvData(char* buff, size_t buffSize)
{
    ssize_t ret = -1;
    unsigned recvBytes = 0;

    ret = recvfrom(sockfd, buff, buffSize, MSG_WAITALL,
        (struct sockaddr *)&recvAddr, &recvBytes);

    if(ret < 0)
    {
        perror("Error during recvform :(");
        return -1;
    }
    buff[ret] = '\0';

    return ret;
}

static void SendACK()
{
    ssize_t ret = -1;
    const char message[] = ACK_MESSAGE;

    ret = sendto(sockfd, (const char *)message, sizeof(message), 0,
            (const struct sockaddr *) &recvAddr, sizeof(recvAddr));
    if (ret < 0)
    {
        ret = errno;
        PrintErrnoMessage(ret);
        fprintf(stderr, "Sending error while sending ACK\n");
        return;
    }
}

static int RecvACK()
{
    ssize_t ret = -1;
    unsigned recvBytes = 0;
    const char message[] = ACK_MESSAGE;
    char messBuff[sizeof(ACK_MESSAGE)] = {0};

    ret = recvfrom(sockfd, messBuff, sizeof(messBuff), MSG_WAITALL,
        (struct sockaddr *)&sendAddr, &recvBytes);

    if(ret < 0)
    {
        perror("Error during recvform :(");
        return -1;
    }
    messBuff[ret] = '\0';

    if (0 != strncmp(message, messBuff, sizeof(messBuff)))
    {
        fprintf(stderr, "ERROR:\tError while receiving ACK\n");
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
    unsigned long fileBytes = 0;
    unsigned sendNo = 0;

    if(ToS != NH_TOS_TRANSMITTER)
    {
        fprintf(stderr, "ERROR:\tCannot send files in receipient mode\n");
        return -1;
    }

    file2Send = InitFileHandler(fileName, FH_MODE_READ);
    if(file2Send != NULL)
    {
        fprintf(stdout, "INFO:\tInitialized FileHandler, filename: %s\n", fileName);
    }
    /* Check file size (we assume processing
       only text files so this tells file size in bytes) */
    fseek(file2Send, 0, SEEK_END);
    fileSize = ftell(file2Send);
    progressMax = fileSize;
    fseek(file2Send, 0, SEEK_SET);

    if (fileSize <= 0)
    {
        perror("ERROR:\tFile is empty or error occured during checking file size");
        return -1;
    }

    fprintf(stdout, "INFO:\tSending file size is: %ld bytes\n", fileSize);

    /* First transmission - send fileName */
    printf("Sent first message\n");
    strncpy(sendBuff, fileName, strlen(fileName));
    ret = SendData(sendBuff, sizeof(sendBuff));
    if (ret < 0) {return -1;}

    /* Second transmission, size of sending file */
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
        fileBytes = ReadTxtFile(sendBuff, sizeof(sendBuff), file2Send);
        if (0 > ret)
        {
            perror("ERROR:\tError while reading file");
            return -1;
        }

        ret = SendData(sendBuff, sizeof(sendBuff));
        if (ret < 0) {return -1;}

        // if(0 == (sendNo % 50))
        // {
        //     sleep(1);
        //     ret = RecvACK();
        //     if(0 > ret)
        //     {
        //         fprintf(stderr, "ERROR:\tFiled to get ack from receipient\n");
        //         return -1;
        //     }
        // }
        ++sendNo;

        progress += fileBytes;
        fprintf(stdout, "INFO:\tSent %lu/%lu bytes\n", progress, progressMax);
    } while (progress < progressMax);

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
    char filename[32] = {0};
    char recvBuff[READ_BUFF_SIZE] = {0};
    int ret = -1;
    unsigned long bytesToGo = 0;
    unsigned long writtenBytes = 0;
    unsigned recvNo = 0;

    if(ToS != NH_TOS_RECEIVER)
    {
        fprintf(stderr, "ERROR:\tCannot receive files in transmitter mode\n");
        return -1;
    }

    // First transmission, receive filename
    fprintf(stdout, "INFO:\tInitialized net handler, waiting for transmission...\n");
    ret = RecvData(recvBuff, sizeof(recvBuff));

    if (ret < 0)
    {
        fprintf(stderr, "ERROR:\tCannot receive filename, aborting...\n");
        return -1;
    }
    memcpy(filename, recvBuff, ret);

    // Second transmission, receive file size
    ret = RecvData(recvBuff, sizeof(recvBuff));
    maxRecvdBytes = atol(recvBuff);
    bytesToGo = 0;

    fprintf(stdout, "INFO:\tReceivedd transmission request, file: %s of size: %ld\n", filename, maxRecvdBytes);
    recvdFile = InitFileHandler(filename, FH_MODE_WRITE);

    while (maxRecvdBytes > bytesToGo)
    {
        ret = RecvData(recvBuff, sizeof(recvBuff));
        if (ret < 0)
        {
            fprintf(stderr, "ERROR:\tReceiving filed, aborting...\n");
            return -1;
        }

        writtenBytes = WriteTxtFile(recvBuff, ret, recvdFile);
        if (ret < 0)
        {
            fprintf(stderr, "ERROR:\tWriting file error, aborting...\n");
            return -1;
        }
        // Confirm that data was received
        // if (0 == (recvNo % 50))
            // SendACK();

        ++recvNo;
        bytesToGo += writtenBytes;
        fprintf(stdout, "Written %lu/%lu bytes\n", bytesToGo, maxRecvdBytes);
    }

    progressMax = maxRecvdBytes;
    Cleanup(recvdFile, sockfd);
    return 0;
}

unsigned long GetFinalNumOfBytesSent()
{
    return progressMax;
}
