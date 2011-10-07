#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lib/mongo/src/mongo.h"
// Definitions
#define TRUE  1
#define FALSE 0
// MongoDB Server Data
#define MONGO_LOG_SERVER_HOST "127.0.0.1"
#define MONGO_LOG_SERVER_PASS ""
#define MONGO_LOG_SERVER_PORT 27017
#define MONGO_LOG_SERVER_USER ""
// MySQL Server Data
#define MYSQL_LOG_SERVER_HOST "127.0.0.1"
#define MYSQL_LOG_SERVER_PASS ""
#define MYSQL_LOG_SERVER_PORT 3306
#define MYSQL_LOG_SERVER_USER ""
// File Paths
#define JODEN_CONFIG_FILE "/Users/tbrown/Documents/Applications/JodenWeb/conf.d/joden.conf"
#define JODEN_LOG_FILE    "/Users/tbrown/Documents/Applications/JodenWeb/logs/server.log"
/**
 * This method handles all of the contact
 * between the server and the client
 * @param int iSocketFileDescriptor
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
	logMessage(JODEN_LOG_FILE, &sBuffer);
	// Write to the socket
	iContentLength = write(iSocketFileDescriptor, "I got your message", 18);
	// Check for write success
	if (iContentLength < 0) {
		// Throw a new error
		throwError("ERROR:  Unable to write to socket.");
	}
}
char getConfig() {
	// Declare the array variable
	const char *aConfig[][2];
	// Declare the buffer
	char sBufferLine[256];
	// Declare the line number
	int iLineNumber = 0;
	// Read the lines
	while (fgets(sBufferLine, 256, JODEN_CONFIG_FILE) != NULL) {
		
	}
	// Return the config
	return aConfig;
}
/**
 * This method logs messages to a 
 * database to be read by other apps
 * @param char sMessage
 * @return void
 **/
void logMessageToDatabase(char *sMessage) {
	// Declare the MongoDB connection
	mongo oConnection[1];
	// Set the connection status
	iConnectionStatus = mongo_connect(oConnection, MONGO_LOG_SERVER_HOST, MONGO_LOG_SERVER_PORT);
	// Declare the current time in milliseconds
	time_t iTimeStamp = (time(NULL) * 1000);
	// Make sure we have a good status
	if (iConnectionStatus != MONGO_OK) {
		// Check for success
		if (oConnection->err == MONGO_CONN_SUCCESS) {
			// Declare the BSON object
			bson oLogData[1];
			// Initialize the BSON object
			bson_init(oLogData);
			// Give the object an ID
			bson_append_new_oid(oLogData, "iLogMessageId");
			// Set the message
			bson_append_string(oLogData, "sMessage", sMessage);
			// Set the date and time
			bson_append_int(oLogData, "iTimeStamp", iTimeStamp);
			// Finalize the BSON object
			bson_finish(oLogData);
			// Insert the data
			mongo_insert(oConnection, "JodenWeb.Logs", oLogData);
			// Destroy the BSON object
			bson_destroy(oLogData);
		} else {
			// Parse the error
			switch (oConnection->err) {
				case MONGO_CONN_BAD_ARG:
					// Throw a new error
					throwError("DB-ERROR:  Bad arguments.");
					// Return
					return 1;
				case MONGO_CONN_NO_SOCKET:
					// Throw a new error
					throwError("DB-ERROR:  Unable to connect to socket.");
					// Return
					return 1;
				case MONGO_CONN_FAIL:
					// Throw a new error
					throwError("DB-ERROR:  Unable to connect.");
					// Return
					return 1;
				case MONGO_CONN_NOT_MASTER:
					// Throw a new error
					throwError("DB-ERROR:  Not master server.");
					// Return
					return 1;
			}
		}
	}
	// Desttroy the connection
	mongo_destroy(oConnection);
}
/**
 * This method handles the action of opening, 
 * writing and closing of log files
 * @param char sFile
 * @param char sMessage
 * @return void
 **/
void logMessageToFile(char *sFile, char *sMessage) {
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
 * @param int iArgC
 * @param char sArgV
 * @return int
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
	while (TRUE) {
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
 * @param char sMessage
 * @return void
**/
void throwError(char *sMessage) {
	// Set the error message
	perror(sMessage);
	// Terminate
	exit(0);
}
