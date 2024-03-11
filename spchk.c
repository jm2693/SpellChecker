#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEBUG=1

int main (int argc, char** argv){
    char *fname = argc > 1 ? argv[1] : “text.txt”;
	int fd = open(fname, O_RDONLY); 

}