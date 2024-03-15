#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#define DEBUG 1

void dict_arr () {


}




void file_search (char* filename) {          // function to recursively search for files in a directory 
    DIR *dir = opendir(filename);            // creating a DIR* to open file           
    if (dir == NULL) return;                 // if file is NULL, function returns

    typedef struct dirent dirent;            
    dirent* entity;                          // creating dirent* called entity 
    entity = readdir(dir);                      

    while (entity != NULL) {
        if (entity->d_type == DT_DIR) {      // checks if the file is a directory 
            char path[200] = { '\0' };       // creates path for the file as a string
            strcat(path, filename);          // concatonates the currrent file to the path variable
            strcat(path, '/');               // '/' to indicate new dir
            strcat(path, entity->d_name);    // 
            file_search(path);               // recursive search
        }
    }

    closedir(dir);
}




void check_for_word() {

}


int main (int argc, char** argv){
    char *fname = argv[1];
	int fd = open(fname, O_RDONLY); 

    

}