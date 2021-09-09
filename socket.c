/*
    Initialise Winsock
*/

//
// ComPrinter.c - This program reads text from a
//   serial port and prints it to the console.
//
// Written by Ted Burke - last updated 3-5-2012
//
// To compile with MinGW:
//
//      gcc -o socket.exe socket.c -lws2_32
//
// Example commands:
//
//      socket /port 4 /baudrate 9600
// 
//
// To stop the program, press Ctrl-C
//

#include<stdio.h>
#include<winsock2.h>
#include <windows.h>
#include <stdlib.h>
//#include < string.h >

//#define WINVER 0x0500
#define MESSAGE_LENGTH 200

#pragma comment(lib,"ws2_32.lib") //Winsock Library

// Declare variables and structures
HANDLE hSerial = INVALID_HANDLE_VALUE;
DCB dcbSerialParams = { 0 };
COMMTIMEOUTS timeouts = { 0 };
DWORD dwBytesWritten = 0;
char dev_name[MAX_PATH] = "";
int dev_number = -1;
int baudrate = 2400;
int scan_max = 30;
int scan_min = 1;
int simulate_keystrokes = 0;
int debug = 0; // print no info by default
int id = -1;

void CloseSerialPort()
{
	if (hSerial != INVALID_HANDLE_VALUE)
	{
		// Close serial port
		fprintf(stderr, "\nClosing serial port...");
		if (CloseHandle(hSerial) == 0)
			fprintf(stderr, "Error\n");
		else fprintf(stderr, "OK\n");
	}
}

void exit_message(const char* error_message, int error)
{
	// Print an error message
	fprintf(stderr, error_message);
	fprintf(stderr, "\n");

	// Exit the program
	exit(error);
}


int main(int argc, char* argv[])
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    char message[300];
    int n = 1;
    int state = 1;
    int i;
    char c;
    char* message_buffer;
    DWORD bytes_read;


    ///////////////////////////////////////////////////////////////////////READ COM PORT//////////////////////////////////////////////////////////////////////

    while (n < argc)
    {
        // Process next command line argument
        if (strcmp(argv[n], "/port") == 0)
        {
            if (++n >= argc) exit_message("Error: no device number specified", 1);

            // Set device number to specified value
            dev_number = atoi(argv[n]);
        }
        else if (strcmp(argv[n], "/baudrate") == 0)
        {
            if (++n >= argc) exit_message("Error: no baudrate value specified", 1);

            // Set baudrate to specified value
            baudrate = atoi(argv[n]);
        }
        else if (strcmp(argv[n], "/debug") == 0)
        {
            debug = 2;
        }
        else if (strcmp(argv[n], "/talk") == 0) //prints minimum info
        {
            debug = 1;
        }
        else
        {
            // Unknown command line argument
            if (debug) fprintf(stderr, "Unrecognised option: %s\n", argv[n]);
        }

        n++; // Increment command line argument counter
    }

    // Welcome message
    if (debug)
    {
        fprintf(stderr, "\nComPrinter.exe - written by Ted Burke\n");
        fprintf(stderr, "https://batchloaf.wordpress.com\n");
        fprintf(stderr, "This version: 3-5-2012\n\n");
    }

    // Debug messages
    if (debug > 1) fprintf(stderr, "dev_number = %d\n", dev_number);
    if (debug > 1) fprintf(stderr, "baudrate = %d\n\n", baudrate);

    // Register function to close serial port at exit time
    atexit(CloseSerialPort);

    if (dev_number != -1)
    {
        // Limit scan range to specified COM port number
        scan_max = dev_number;
        scan_min = dev_number;
    }

    // Scan for an available COM port in _descending_ order
    for (n = scan_max; n >= scan_min; --n)
    {
        // Try next device
        sprintf(dev_name, "\\\\.\\COM%d", n);
        if (debug > 1) fprintf(stderr, "Trying %s...", dev_name);
        hSerial = CreateFile(dev_name, GENERIC_READ | GENERIC_WRITE, 0, 0,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hSerial != INVALID_HANDLE_VALUE)
        {
            if (debug > 1) fprintf(stderr, "OK\n");
            dev_number = n;
            break; // stop scanning COM ports
        }
        else if (debug > 1) fprintf(stderr, "FAILED\n");
    }


    // Check in case no serial port could be opened
    if (hSerial == INVALID_HANDLE_VALUE)
        exit_message("Error: could not open serial port", 1);

    // If we get this far, a serial port was successfully opened
    fprintf(stderr, "Opening COM%d at %d baud\n\n", dev_number, baudrate);

    // Set device parameters:
    //  baudrate (default 2400), 1 start bit, 1 stop bit, no parity
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
        exit_message("Error getting device state", 1);
    dcbSerialParams.BaudRate = baudrate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (SetCommState(hSerial, &dcbSerialParams) == 0)
        exit_message("Error setting device parameters", 1);

    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hSerial, &timeouts) == 0)
        exit_message("Error setting timeouts", 1);

    // Read text and print to console (and maybe simulate keystrokes)

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Socket created.\n");


    server.sin_addr.s_addr = inet_addr("193.136.120.133");
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    //Connect to remote server
    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }

    puts("Connected");
    // No robot id specified - print everything to console
    
    message_buffer = (char*)malloc(200* sizeof(char));
    i = 0;
    int ldr1=-1, ldr2 = -1;
    while (1)
    {
       
        //int ldr1 = -1, ldr2 = -1;
        //while(1) {
            ReadFile(hSerial, &c, 1, &bytes_read, NULL);
            if (bytes_read == 1)
            {
                //printf("%c", c);
                message_buffer[i++] = c;
                //printf("%s", message_buffer);
                //if (strlen(message_buffer) == 62)
                //    break;

            }
        //}
       // printf("%s", message_buffer);
        //printf("\nstrlen: %d\n", strlen(message_buffer));
        //printf("  ldr1: %d |ldr2: %d\n", ldr1, ldr2);
        //sscanf(message_buffer, "\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>\n", &ldr1, &ldr2);

        if (sscanf(message_buffer, "\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>\n", &ldr1, &ldr2) == 2 && strlen(message_buffer) == 62) {
        //printf("%d %d", ldr1, ldr2);
        //if(ldr1 !=-1 && ldr2!=-1){
           // printf("%s\n", message_buffer);

            sprintf(message, "POST /~sad/ HTTP/1.1\r\nHost: 193.136.120.133\r\nContent-Type: application/xml\r\nContent-Length: %d\r\n\r\n\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>\n", 63, ldr1, ldr2);
            //printf("%s", message);
            //nr = sscanf(message_buffer, "\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>\n", ldr1, ldr2);
            if (send(s, message, strlen(message), 0) < 0)
            {
                puts("Send failed");
                //return 1;
            }


            printf("Data Sent\n");
            //printf("message:%s:", message);
            

            Sleep(1);
            ldr1 = ldr2 = -1;
            for (int j = 0; message_buffer[j];j++)
                message_buffer[j] = '\0';
            for (int j = 0; message[j]; j++)
                message[j] = '\0';
            i = 0;
            // } 
            //int nr = 0;

            //sprintf(message, "POST /~sad/ HTTP/1.1\r\nHost: 193.136.120.133\r\nContent-Type: application/xml\r\nContent-Length: %d\r\n\r\n%s", 5, "hello");

        }
    }
}

