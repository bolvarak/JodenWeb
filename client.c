#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/**
 * This is our error handler, it throws
 * errors to the client and terminated JodenWeb
 * @return void
**/
void throwError(char *sMessage) {
	// Set the error message
	perror(sMessage);
	// Terminate
	exit(1);
}

int main(int iArgC, char *sArgV[]) {
	// Declare the socket file descriptor
	int iSocketFileDescriptor;
	// Declare the port number
	int iPortNumber;
	// Declare the content length
	int iContentLength;
	// Declare the buffer size
	char sBuffer[4096];
	// Declare the server address
	struct sockaddr_in oServerAddress;
	// Declare the server container
	struct hostent *oServer;
	// Check for a defined hostname and port
	if (iArgC < 3) {
		// Show usage details
		fprintf(stderr, "USAGE:  %s host port", sArgV[0]);
		// Terminate
		exit(0);
	}
	// Set the port number
	iPortNumber           = atoi(sArgV[2]);
	// Set the socket connection
	iSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	// Check for successful connection
	if (iSocketFileDescriptor < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to open socket connection.");
	}
	// Set the server info
	oServer = gethostbyname(sArgV[1]);
	// Check for server data
	if (oSercer == NULL) {
		// Throw new error
		fprintf(stderr, "ERROR:  Unable to find host.");
		// Terminate
		exit(0);
	}
	// Set the buffer
	bzero((char *) &oServerAddress, sizeof(oServerAddress));
	// Set the server address interface type
	oServerAddress.sin_family = AF_INET;
	// Copy the server information
	bcopy((char *) oServer->h_addr, (char *) &oServerAddress.sin_addr.s_addr, oServer->h_length);
	// Set the server port number
	oServerAddress.sin_port   = htons(iPortNumber);
	// Try to connect
	if (connect(iSocketFileDescriptor, &oServerAddress, sizeof(oServerAddress)) < 0) {
		// Throw new error
		throwError("ERROR:  Unable to connect.");
	}
	// Prompt the user for a message
	printf("Please enter a message:  ");
	// Reset the buffer
	bzero(sBuffer, 4096);
	// Get the user input
	fgets(sBuffer, 4095, stdin);
	// Write the message to the socket
	iContentLength = write(iSocketFileDescriptor, sBuffer, strlen(sBuffer));
	// Check for successful write
	if (iContentLength < 0) {
		// Throw new error
		throwError("ERROR:  Unable to write connection data.");
	}
	// Reset the buffer
	bzero(sBuffer, 4096);
	// Read the connection data
	iContentLength = read(iSocketFileDescriptor, sBuffer, strlen(sBuffer));
	// Check for data
	if (iContentLength < 0) {
		// Throw new error
		throwError("ERROR:  Unable to read connection data.");
	}
	// Display the response
	printf("%s", sBuffer);
	// Return
	return 0;
}
