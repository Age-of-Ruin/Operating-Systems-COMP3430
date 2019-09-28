#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	
	// Declarations
	DIR *dir;
	struct dirent *dirStruct;

	//Initialize
	dir = opendir("/");
	
	while((dirStruct = readdir(dir)) != NULL){
		
		printf("FileName: %s - Inode Number: %d\n", dirStruct->d_name, dirStruct->d_ino);
		
	} /* End of While */
		
	printf("Program Ends...\n");
	
}