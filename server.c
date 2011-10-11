////////////////////////////////////////////////////////////////////////
/// Libraries /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
////////////////////////////////////////////////////////////////////////
/// Constants /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define FALSE             0
#define JODEN_BUFFER_SIZE 8192
#define JODEN_PORT        "1988"
#define JODEN_QUEUE       25
#define TRUE              1
#define UNDEF            -1
////////////////////////////////////////////////////////////////////////
/// Zombie Handler ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void sigChildHandler(int iNumber) {
	// Kill the child
	while (waitpid(-1, NULL, WNOHANG) > 0);
}
////////////////////////////////////////////////////////////////////////
/// Inet Address Handler //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void *getInternetAddress(struct sockaddr *oSocketAddress) {
	// Check for IPv4
	if (oSocketAddress->sa_family == AF_INET) {
		// Return the IPv4 address
		return &(((struct sockaddr_in *) oSocketAddress)->sin_addr);
	}
	// Return the IPv6 address
	return &(((struct sockaddr_in6 *) oSocketAddress)->sin6_addr);
}
////////////////////////////////////////////////////////////////////////
/// Constructor ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int iArgC, char *sArgV[]) {
	// Declare the server and client socket
	int iServer, iClient;
	// Declare address infor containers
	struct addrinfo oHints, *oServerInfo, *oProtocol;
	// Declare the client address information container
	struct sockaddr_storage oClientAddress;
	// Declare socket internet address size
	socklen_t iSocketAddressSize;
	// Declare the signal action
	struct sigaction oSignalAction;
	// Declare the buffer
	char sClientAddress[INET6_ADDRSTRLEN];
	// Declare a yes operator
	int iYes = 1;
	// Declare the error code placeholder
	int iAddressStatus;
	// Allocate our memeory
	memset(&oHints, FALSE, sizeof oHints);
	// Set our interface type
	oHints.ai_family   = AF_UNSPEC;
	// Set our socket type
	oHints.ai_socktype = SOCK_STREAM;
	// Set our interface flags
	oHints.ai_flags    = AI_PASSIVE;
	// See if we are able to get 
	// the server address info
	if ((iAddressStatus = getaddrinfo(NULL, JODEN_PORT, &oHints, &oServerInfo)) != FALSE) {
		// Throw a new error
		fprintf(stderr, "ERROR:  Unable to get server address info.  Got code %s.\n", gai_strerror(iAddressStatus));
		// Nothing more we can do
		return 1;
	}
	// Loop through all of the options and
	// bind to the first one we can
	for (oProtocol = oServerInfo; oProtocol != NULL; oProtocol = oProtocol->ai_next) {
		// Try to create a new socket
		if ((iServer = socket(oProtocol->ai_family, oProtocol->ai_socktype, oProtocol->ai_protocol)) == UNDEF) {
			// Show status
			perror("SERVER:  Socket initialized\n");
			// Continue processing
			continue;
		}
		// Try to set the socket options
		if (setsockopt(iServer, SOL_SOCKET, SO_REUSEADDR, &iYes, sizeof(int)) == UNDEF) {
			// Show status
			perror("SERVER:  Options set\n");
			// Continue processing
			continue;
		}
		// Try to bind the socket
		if (bind(iServer, oProtocol->ai_addr, oProtocol->ai_addrlen) == UNDEF) {
			// Close the socket
			close(iServer);
			// Show status
			perror("SERVER:  Bound\n");
			// Continue processing
			continue;
		}
		// End the loop
		break;
	}
	// Check for a valid protocol structure
	if (oProtocol == NULL) {
		// Throw an error
		fprintf(stderr, "ERROR:  Failed to connect.\n");
		// Nothing more we can do
		return 2;
	}
	// Free up address resources
	freeaddrinfo(oServerInfo);
	// Try to setup the listener
	if (listen(iServer, JODEN_QUEUE) == UNDEF) {
		// Show the status
		perror("SERVER:  Listening\n");
		// Everything is done, terminate
		exit(1);
	}
	// Reap what we have sown
	oSignalAction.sa_handler = sigChildHandler;
	// Empty the set
	sigemptyset(&oSignalAction.sa_mask);
	// Check for a sigaction
	if (sigaction(SIGCHLD, &oSignalAction, NULL) == UNDEF) {
		// Show status
		perror("SERVER:  Executed SigAction\n");
		// Nothing more to do
		exit(1);
	}
	// Show server status
	printf("SERVER:  Awaiting connections\n");
	// Accept loop
	while (1) {
		// Get the client's address size
		iSocketAddressSize = sizeof oClientAddress;
		// Create the client socket connection
		iClient            = accept(iServer, (struct sockaddr *) &oClientAddress, &iSocketAddressSize);
		// Check for data
		if (iClient == UNDEF) {
			// Show status
			perror("SERVER:  Reading connection\n");
			// Continue processing
			continue;
		}
		// Grab the client's address
		inet_ntop(oClientAddress.ss_family, getInternetAddress((struct sockaddr *) &oClientAddress), sClientAddress, sizeof sClientAddress);
		// Show the status
		printf("SERVER:  Host %s successfully connected\n", sClientAddress);
		// Try to fork this process
		// to the background
		if (!fork()) {
			// We are dealing with the child process, 
			// so we close the listener as it is no
			// longer needed by this process
			close(iServer);
			// Try to send a response
			if (send(iClient, "Hello World!", 13, FALSE) == UNDEF) {
				// Show status
				perror("SERVER:  Sending response\n");
			}
			// Close the socket
			close(iClient);
			// Nothing more to do
			exit(0);
		}
		// Close the socket
		close(iClient);
	}
	// We're done
	return 0;
}
