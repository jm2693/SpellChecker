#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#define DEBUG 1

void dict_arr () {


}




void list_files (char* filename) {
    DIR *dir = opendir(filename);
    if (dir == NULL) return;

    typedef struct dirent dirent;
    dirent* entity;
    entity = readdir(dir);

    while (entity != NULL) {
        if (entity->d_type == DT_DIR) {
            char path[200] = { '\0' };
            strcat(path, filename);
            strcat(path, '/');
            strcat(path, entity->d_name);
            list_files(path);
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