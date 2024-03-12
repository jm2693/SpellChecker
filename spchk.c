#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEBUG 1

int main (int argc, char** argv){
    char *fname = argv[1];
	int fd = open(fname, O_RDONLY); 

    

}