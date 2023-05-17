/*  test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

/*
	Tests to make sure a given file isn't a hard or soft link

	@params:
		path : Path to the file to be tested
		used_inos : An array of seen inode numbers
*/
char notLink(const char *path, ino_t *used_inos) {
	struct stat* st = (struct stat*)malloc(sizeof(struct stat));
	lstat(path, st);

	for (; *used_inos; used_inos++) // Loops till value at used_ino == 0 (end of seen inodes)
		if (st->st_ino == *used_inos) return 0; // inode found, this is a hard link to a file already seen
	
	*used_inos = st->st_ino;
	return !S_ISLNK(st->st_mode); // Returns a check if its a soft link
}

/*
	Searches for pattern in file and writes the entire line containing the pattern to STDOUT
	Format for print: [path to file] : Line in the file
	@Params:
		path - path to the file
		pattern : pattern to be searched
*/
void searchPatternInFile(char* path, char* pattern){

	FILE * fp;

	fp = fopen(path, "r");
	if (fp == NULL){
		fprintf(stderr,"Error opening file: %s in utils.c\n",path);
		return;
	}

	char *line = NULL; // Pointer to line string (memory allocation is handled by getline)
	size_t len = 0; // Stores current length of line
	ssize_t read;

	char write_buff[MAX_WRITE_LENGTH] = {'\0'}; // Buffer string for formatted output
	ssize_t bytes_write;
	while ((read = getline(&line, &len, fp)) != -1){
		if (strstr(line, pattern) != NULL){
			bytes_write = sprintf(write_buff, "%s: %s", path, line); // Formatted string with output
			write(STDOUT_FILENO, write_buff, bytes_write); // Writes to stdout, in the child process this will write to the pipe, otherwise output to the terminal.
		}	
	}
		
	free(line);
	fclose(fp);
}
