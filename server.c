#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/**
 * This method handles all of the contact
 * between the server and the client
 * @return void
**/
void clientOperations(int iSocketFileDescriptor) {
	// Declare the content length
	int iContentLength;
	// Declare the buffer
	char sBuffer[4096];
	// Reset the buffer
	bzero(sBuffer, 4096);
	// Read the socket connection data
	iContentLength = read(iSocketFileDescriptor, sBuffer, 4095);
	// Check for data
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to reading from socket.");
	}
	// Write to logs
	logMessage("/Users/tbrown/Documents/Applications/JodenWeb/logs/server.log", &sBuffer);
	// Write to the socket
	iContentLength = write(iSocketFileDescriptor, "I got your message", 18);
	// Check for write success
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to write to socket.");
	}
}
/**
 * This method handles the action of opening, 
 * writing and closing of log files
 **/
void logMessage(char *sFile, char *sMessage) {
	// Declare the file handle
	FILE *rFileHandle;
	// Open the file
	rFileHandle = fopen(sFile, "a");
	// Write the data
	fprintf(rFileHandle, sMessage);
	// Write a new line
	fprintf(rFileHandle, "\n");
	// Close the file
	fclose(rFileHandle);
}
/**
 * This method is our main constructor
 * it sets up and closes the connection
 **/
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
	// Declare the process id
	int iProcessId;
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
	while (1) {
		// Accept the incoming connection
		// and setup the new socket
		iNewSocketFileDescriptor = accept(iSocketFileDescriptor, (struct sockaddr *) &oClientAddress, &iClientLength);
		// Check for a connection
		if (iNewSocketFileDescriptor < 0) {
			// Throw a new error
			throwError("ERROR:  Unable to accept incoming connections.");
		}
		// Grab a new Process Id
		iProcessId = fork();
		// Make sure we have a valid pid
		if (iProcessId < 0) {
			// Throw a new error
			throwError("ERROR:  Unable to fork to the background.");
		}
		// Is the service still running
		if (iProcessId == 0) {
			// Kill the connection
			close(iSocketFileDescriptor);
			// Run the client operations
			clientOperations(iNewSocketFileDescriptor);
			// Terminate
			exit(0);
		} else {
			// Do nothing, but close the connection
			close(iNewSocketFileDescriptor);
		}
	}
	// Return
	return 0;
}
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
