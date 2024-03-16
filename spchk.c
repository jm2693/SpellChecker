#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#define DEBUG 1

typedef struct word {                       // creates a char* struct, as to not use a char[][] and instead use a word* for a string array
    char *word;
} word;

word* dict_arr (char* dict_file, int* word_num) {   // dict_arr will take in the dictionary pathname and return an pointer to a word array (array of strings)

    int fd = open(dict_file, O_RDONLY);     // opens the dictionary file using its pathname 
    if (fd < 0) {
        perror("Error: ");                  // prints error using msg and errno 
        return EXIT_FAILURE;                // if open fails, returns error
    }
    word *dictionary_start = NULL;          // a pointer to the word array (what the function will return)
    *word_num = 0;


    close(fd);                              // anything opened must be closed when done with use
}




void file_search (char* filename) {          // function to recursively search for files in a directory 
    DIR *dir = opendir(filename);            // creating a DIR* to open file           
    if (dir == NULL) return;                 // if file is NULL, function returns

    typedef struct dirent dirent;            
    dirent* entity;                          // creating dirent* called entity 
    entity = readdir(dir);                      

    while (entity != NULL) {
        if (entity->d_name[0] == '.') {
            continue;                        // skips hidden files or directories
        }

        if (entity->d_type == DT_DIR) {      // checks if the file is a directory 
            char path[200] = { '\0' };       // creates path for the file as a string
            strcat(path, filename);          // concatonates the currrent file to the path variable
            strcat(path, '/');               // '/' to indicate new dir
            strcat(path, entity->d_name);    // 
            file_search(path);               // recursive search
        }
    }

    closedir(dir);                           // close file when done 
}




void check_for_word() {

}



void return_error() {

}


int main (int argc, char** argv){

    if (argc < 3) {
        printf("Use of %s: <dictionary_path> <file1> <file2> ...");
        return EXIT_FAILURE;
    }

    int num_of_words;
    dict_arr(argv[1], &num_of_words);



}