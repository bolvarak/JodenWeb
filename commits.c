#include <stdio.h>
#include <jansson.h>

#define URL_FORMAT "http://github.com/api/v2/json/commits/list/%s/%s/master"
#define URL_SIZE   256

int main(int iArgC, char *sArgV[]) {
    // Declare a size
    size_t iText;
    // Declare the response container
    char *sResponse;
    // Declare the URL
    char sUrl[URL_SIZE];
    // Declare the JSON object
    json_t *oJson;
    // Declare the JSON error container
    json_error_t oError;
    // Declare the JSON commits key
    json_t *aCommits;
    // Check for a username and repository
    if (iArgC != 3) {
    	// Display an error
    	fprintf(stderr, "USAGE:  %s user repository\n", sArgV[0]);
    	// Return
    	return 2;
    }
    // Build the url
    snprintf(sUrl, URL_SIZE, URL_FORMAT, sArgV[1], sArgV[2]);
    // Make the request
    sResponse = request(sUrl);
    // Check for a response
    if (!sResponse) {
    	// Return
    	return 1;
    }
    // Decode the JSON
    oJson = json_loads(sResponse, 0, &oError);
    // Free up the response container
    free(sResponse);
    // Check for data
    if (!oJson) {
    	// Display an error
    	fprintf(stderr, "ERROR:  Line %d:  %s\n", oError.line, oError.text);
    	// Return
    	return 1;
    }
    // Get the commits from the JSON object
    aCommits = json_object_get(oJson, "commits");
    // Make sure the commits are in array format
    if (!json_is_array(aCommits)) {
    	// Display an error
    	fprintf(stderr, "ERROR:  No commits available.\n");
    	// Return
    	return 1;
    }
    // Declare the commit increment
    int iCommit;
    // Loop through the commits
    for (iCommit = 0; iCommit < json_array_size(aCommits); iCommit ++) {
    	// Declare the individual commit object
    	json_t *oCommit;
    	// Declare the commit ID
    	json_t *sCommitId;
    	// Declare the commit message
    	json_t *sMessage;
    	// Declare the message text
    	const char *sMessageText;
    	// Grab the commit
    	oCommit = json_array_get(aCommits, iCommit);
    	// Make sure the commit is an object
    	if (!json_is_object(oCommit)) {
    		// Display an error
    		fprintf(stderr, "ERROR:  Commit %d is not a valid commit.\n", (iCommit + 1));
    		// Return
    		return 1;
    	}
    	// Grab the commit ID
    	sCommitId = json_object_get(oCommit, "id");
    	// Make sure the ID is valid
    	if (!json_is_string(sCommitId)) {
    		// Display an error
    		fprintf(stderr, "ERROR:  Commit %d does not have a valid ID.\n", (iCommit + 1));
    		// Return 
    		return 1;
    	}
    	// Grab the message
    	sMessage = json_object_get(oCommit, "message");
    	// Make sure the message is valid
    	if (!json_is_string(sMessage)) {
    		// Display an error
            fprintf(stderr, "ERROR:  Commit %d does not have a valid message.\n", (iCommit + 1));
            // Return
            return 1;
    	}
        // Grab the message text
        sMessageText = json_string_value(sMessage);
        // Display the message
        printf("%s - %.s\n", json_string_value(sCommitId), sMessageText);
        // Free up our resources
        json_decref(oJson);
        // Return 
        return 0;
    }
}
