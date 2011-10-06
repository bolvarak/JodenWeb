#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/**
 * This is our error handler, it throws
 * errors to the client and terminated JodenWeb
 * @return void
**/
void throwError(char *sMessage) {
	// Set the error message
	perror(sMessage);
	// Terminate
	exit(0);
}

int main(int iArgC, char *sArgV[]) {
	// Declare the socket file descriptor
	int iSocketFileDescriptor;
	// Declare the new socket file descriptor
	int iNewSocketFileDescriptor;
	// Declare the port number
	int iPortNumber;
	// Declare the client address length
	int iClientLength;
	// Declare the content length
	int iContentLength;
	// Declare the buffer size
	char sBuffer[4096];
	// Declare the server address
	struct sockaddr_in oServerAddress;
	// Declare the client address
	struct sockaddr_in oClientAddress;
	// Make sure we have a port number
	if (iArgC < 2) {
		// Throw an error
		fprintf(stderr, "ERROR:  No port number provided.");
		// Terminate
		exit(0);
	}
	// Create the new socket
	iSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	// Check for successful socket creation
	if (iSocketFileDescriptor < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to open socket.");
	}
	// Set the server address to zero
	bzero((char *) &oServerAddress, sizeof(oServerAddress));
	// Set the port number
	iPortNumber = atoi(sArgV[1]);
	// Set the server address interface type
	oServerAddress.sin_family      = AF_INET;
	// Set the server address port number
	oServerAddress.sin_port        = htons(iPortNumber);
	// Set the IP of the server address
	oServerAddress.sin_addr.s_addr = INADDR_ANY;
	// Try to bind to the socket
	if (bind(iSocketFileDescriptor, (struct sockaddr *) &oServerAddress, sizeof(oServerAddress)) < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to bind to address.");
	}
	// Add a listener
	listen(iSocketFileDescriptor, 5);
	// Set the client length
	iClientLength            = sizeof(oClientAddress);
	// Accept the incoming connection
	// and setup the new socket
	iNewSocketFileDescriptor = accept(iSocketFileDescriptor, (struct sockaddr *) &oClientAddress, &iClientLength);
	// Check for a connection
	if (iNewSocketFileDescriptor < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to accept incoming connections.");
	}
	// Set the new buffer
	bzero(sBuffer, 4096);
	// Read the content
	iContentLength = read(iNewSocketFileDescriptor, sBuffer, 4096);
	// Make sure we have data
	if (iContentLength < 0) {
		// Throw new error
		throwError("ERROR:  Unable to read connection data.");
	}
	// Display the recieved data
	printf("Here is the message:  %s", sBuffer);
	// Write to the socket, confirming
	// that data was recieved
	iContentLength = write(iNewSocketFileDescriptor, "I got your message", 18);
	// Make sure the data was
	// successfully written
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to write connection data.");
	}
	// Return
	return 0;
}
