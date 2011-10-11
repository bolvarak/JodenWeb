////////////////////////////////////////////////////////////////////////
/// Libraries /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include "socket.h"
#include <exception>
////////////////////////////////////////////////////////////////////////
/// Namespace /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
using namespace std;
////////////////////////////////////////////////////////////////////////
/// Constants /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const int FALSE                   = 0;
const int JODEN_BUFFER_SIZE       = 8192;
const char JODEN_DEFAULT_RESPONSE = "Your gibson has just been hacked ...";
const int JODEN_LISTEN_TIME       = 5;
const int JODEN_PORT_NUMBER       = 1988;
const int TRUE                    = 1;
////////////////////////////////////////////////////////////////////////
/// Definitions ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void clientOperations(int iSocket);
int logClient(char *sFile, char *sMessage);
int throwError(char *sMessage);
////////////////////////////////////////////////////////////////////////
/// Client Handler ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void clientOperations(int iSocket) {
	// Declare the content length
	int iContentLength;
	// Declare the buffer
	char sBuffer[JODEN_BUFFER_SIZE];
	// Reset the buffer
	bzero(sBuffer, JODEN_BUFFER_SIZE);
	// Read the socket data
	iContentLength = read(iSocket, sBuffer, JODEN_BUFFER_SIZE);
	// Check for data
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to open socket connection for sending.");
	}
	// Log the client
	logClient("access.log", &sBuffer);
	// Send a response to the client
	iContentLength = write(iSocket, JODEN_DEFAULT_RESPONSE, strlen(JODEN_DEFAULT_RESPONSE));
	// Make sure the response was successful
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to write to socket connection.");
	}
}
////////////////////////////////////////////////////////////////////////
/// Client Logger /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int logClient(char *sFile, char *sMessage) {
	// Declare the file handle
	FILE *rHandle;
	// Open the file
	fopen(sFile, "a");
	// Write the data
	fprintf(rHandle, sMessage);
	// Write a new line
	fprintf(rHandle, "\n\n");
	// Close the file
	fclose(rHandle);
	// We're done
	return FALSE;
}
////////////////////////////////////////////////////////////////////////
/// Constructor ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main (int iArgC, char *sArgV[]) {
	// Declare the server and 
	// client socket descriptor
	int iServerSocket, iClientSocket;
	// Declare the port number
	int iPortNumber;
	// Declare the client 
	// address length
	int iClientAddressLength;
	// Declare the content length
	int iContentLength;
	// Declare the procces id
	int iProcessId;
	// Declare the buffer
	char sBuffer[JODEN_BUFFER_SIZE];
	// Declare the server and client
	struct sockaddr_in oServer, oClient;
	// See if the caller provided a custom port number
	if (iArgC < 2) {
		// No custom port number was
		// provided, we will use the default
		iPortNumber = JODEN_PORT_NUMBER;
	} else {
		// Set the port number to the 
		// caller specified port
		iPortNumber = atoi(sArgV[1]);
	}
	// Create the server socket
	iServerSocket = socket(AF_INET, SOCK_STREAM, FALSE);
	// Make sure the socket was created
	if (iServerSocket < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to create server socket.");
	}
	// Reset the server
	bzero((char *) &oServer, sizeof(oServer));
	// Set the server interface
	oServer.sin_port        = htons(iPortNumber);
	// Set the server address
	oServer.sin_addr.s_addr = INADDR_ANY;
	// Try to bind to the socket
	if (bind(iServerSocket, (struct sockaddr *) &oServer, sizeof(oServer)) < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to bind to socket.");
	}
	// Add the listener
	listen(iServerSocket, JODEN_LISTEN_TIME);
	// Set the client length
	iClientAddressLength = sizeof(oClient);
	// Open the request
	while (TRUE) {
		// Accept incoming connections and
		// create the client socket
		iClientSocket = accept(iServerSocket, (struct sockaddr *) &oClient, &iClientAddressLength);
		// Check for a successful connection
		if (iClientSocket < 0) {
			// Throw a new error
			throwError("ERROR:  Unable to accept incoming connections.");
		}
		// Create a new process id
		iProcessId = fork();
		// Make sure we have a valid pid
		if (iProcessId < 0) {
			// Throw a new error
			throwError("ERROR:  Unable to fork to the background.");
		}
		// Check the running connection
		if (iProcessId == FALSE) {
			// Close the server connection
			close(iServerSocket);
			// Handle the client
			clientOperations(iClientSocket);
			// We're done
			exit(FALSE);
		} else {
			// Close the client connection
			close(iClientSocket);
		}
	}
	// We're done
	return FALSE;
}
////////////////////////////////////////////////////////////////////////
/// Error Handler /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int throwError(char *sMessage) {
	// Set the error message
	perror(sMessage);
	// Terminate
	exit(TRUE);
	// We're done
	return FALSE;
}
