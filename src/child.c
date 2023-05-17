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

/*
	Traverse the directory recursively and search for pattern in files.
	@params:
		path - path to the directory
		pattern - pattern to be recusrively searched in the directory
		used_inos - an array of inodes already seen
		
	Note: Feel free to modify the function header if neccessary
	
*/
void dirTraverse(const char *path, char * pattern, ino_t *used_inos)
{
	DIR *dir = opendir(path);
	struct dirent *entry;

	while (entry = readdir(dir)) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		
		char entry_name[MAX_PATH_LENGTH] = {'\0'};
		snprintf(entry_name, sizeof(entry_name), "%s/%s", path, entry->d_name); // Path to current entry

		if (entry->d_type == DT_DIR) // Recursive call on sub directories
			dirTraverse(entry_name, pattern, used_inos);
		
		if (entry->d_type == DT_REG && notLink(entry_name, used_inos))
			searchPatternInFile(entry_name, pattern);
	}
}

int main(int argc, char** argv){

	if(argc !=3){
		fprintf(stderr,"Child process : %d recieved %d arguments, expected 3 \n",getpid(), argc);
		fprintf(stderr,"Usage child.o [Directory Path] [Pattern] \n");
		exit(EXIT_FAILURE);
	}
	
	char* path = argv[1];
	char* pattern = argv[2];

	// Array of seen inode numbers
	ino_t used_inos[MAX_INODE_LENGTH];
	memset(used_inos, 0, sizeof(used_inos)); // initalized to 0
	
	close(STDIN_FILENO);
	
	dirTraverse(path, pattern, used_inos);
	
	close(STDOUT_FILENO);
	
	exit(EXIT_SUCCESS);
}
