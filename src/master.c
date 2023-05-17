/*  test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int main(int argc, char** argv){

	if(argc != 3){	 
		fprintf(stderr,"Usage ./a.out [Path to Directory] [Pattern to search] \n");
		exit(EXIT_FAILURE);
	}
	char* path = argv[1];
	char* pattern = argv[2];
	
	// Declare any other neccessary variables
	int numOfChildren = 0;
	int pipe_fds[MAX_ROOT_SUBDIRS]; // Array of pipe file descriptors to read from
	ino_t used_inos[MAX_INODE_LENGTH]; // Array of seen inode numbers
	memset(used_inos, 0, sizeof(used_inos)); // initalized to 0
		
	// Open root directory
	DIR *dir = opendir(path);
	struct dirent *entry;

	while (entry = readdir(dir)) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		
		char entry_name[MAX_PATH_LENGTH] = {'\0'};
		snprintf(entry_name, sizeof(entry_name), "%s/%s", path, entry->d_name); // Path to current entry

		// Iterate through root dir and spawn children as neccessary
		if (entry->d_type == DT_DIR) {
			int fds[2];
			if (pipe(fds)) { // On success pipe evaluates to false
				fprintf(stderr, "ERROR: Failed to create pipe\n");
				exit(EXIT_FAILURE);
			}
			// Create new process to handle parallel traversal of sub directories
			pid_t pid;
			if (pid = fork()){
				if (pid == -1) {
					fprintf(stderr, "ERROR: Failed to create new process\n");
					exit(EXIT_FAILURE);
				}
				// Parent process
				pipe_fds[numOfChildren++] = fds[0]; // Store pipe file descriptors
				close(fds[1]); // Close file descriptor of the unused end
			}
			else {
				// Children process
				dup2(fds[1], STDOUT_FILENO);
				close(fds[0]); close(fds[1]); // Close unused end and redundant file descriptor
				
				execl("child", "child", entry_name, pattern, NULL);

				fprintf(stderr, "ERROR: Failed to execute program \"child\"\n");
				exit(EXIT_FAILURE);
			}
		}

		if (entry->d_type == DT_REG && notLink(entry_name, used_inos))
			searchPatternInFile(entry_name, pattern);
	}

	// Waits for all the children processes to finish before outputting the results
	int i;
	for (i = 0; i < numOfChildren; i++) wait(NULL); 
	
	char *read_buff = malloc(sizeof(char) * MAX_READ_LENGTH);
	int bytes_read;
	
	for (i = 0; i < numOfChildren; i++) {
		bytes_read = read(pipe_fds[i], read_buff, MAX_READ_LENGTH); // Reads from each pipe
		if (bytes_read < 0) {
			fprintf(stderr, "ERROR: Failed to read from pipe [%d]\n", i);
			exit(EXIT_FAILURE);
		}
		write(STDOUT_FILENO, read_buff, bytes_read); // Outputs the string read in from the pipe
		close(pipe_fds[i]); // Close the file descriptor after outputting
	}

	free(read_buff);

	return 0;
}
