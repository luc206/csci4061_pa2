<!--test machine: csel-kh4250-03.cselabs.umn.edu (ssh)
*   group number: G19
*   name: Timothy Anderson, Luc Rubin, Hank Hanson
*   x500: and09985, rubin206, hans7186 -->

# Group Number 19

## Names and x500 addresses
Timothy Anderson - and09985 \
Luc Rubin - rubin206 \
Hank Hanson - hans7186 

## Individual contributions
We split up the work by the 3 source files. Luc Rubin created master.c, Tim Anderson created child.c, and Hank Hanson created the utils.c.
After our individual work was done, we collaborated to find bugs, cleanup the code, and made sure everything ran as expected.

## Assumptions
*	inode numbers start from 1, 0 is used as a sentinel value by the system and is also used to find the end of the stored inode values when testing for hardlinks.
*	read() only needs to be called once per pipe, the individual pipes are no longer being written too at this point as all the child processes have been terminated.
*	The searchPatternInFile() function in utils.c writes to the pipe by outputting to stdout, in the case where master.c calls this function, this will output directly to the terminal.

## Compile
	> make clean
	> make

## Execution
	> make run1
	> make run2 
	...
	> make run8
