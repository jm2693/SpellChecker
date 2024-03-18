#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define DEBUG 1
#define DICT_LINES 10000                    // initial num of lines assumed to be 10000
#define WORD_LENGTH 200                     // initial word length assumed to be 199 chars + null terminator 

typedef struct Word {                       // creates a char* struct, as to not use a char[][] and instead use a word* for a string array
    char *word;
} Word;

// dict_arr will take in the dictionary pathname and return an pointer to a word array (array of strings)
Word* dict_arr (char* dict_file, int* word_num) {   
    int fd = open(dict_file, O_RDONLY);     // opens the dictionary file using its pathname 
    if (fd < 0) {                           // in case of error, prints error message 
        perror("Error: ");                  
        return NULL;                        
    }

    int lines_num = DICT_LINES;                  // initial assumed number of lines
    char word_buffer[WORD_LENGTH];               // essentially a blank string. acts as our buffer
    *word_num = 0;                               // the current numbers of words in the dictionary is 0

    Word *dictionary_start = (Word*)malloc(lines_num * sizeof(Word));   // a pointer to the word array (what the function will return)
    if (dictionary_start == NULL) {                                     // in case allocation fails
        close(fd);
        perror("Error: ");
        return NULL;
    }
    ssize_t bytes_read; 

    // reads in WORD_LENGTH bytes at a time into the word_buffer                    
    while((bytes_read = read(fd, word_buffer, sizeof(word_buffer))) > 0) {  
        char* start = word_buffer;          // string to be copied into dictionary_start array
        char* end = NULL;                   // used to locate the end of the word

        for (ssize_t i = 0; i < bytes_read; i++) {
            if (word_buffer[i] == '\n') {   // will replace newline char with null terminator to separate words
                end = &word_buffer[i];
                *end = '\0';

                dictionary_start[*word_num].word = strdup(start);   // copy word into dictionary array (strdup is more memory safe than strcpy)
                if (dictionary_start == NULL) {                                     // in case allocation fails
                    for (int j = 0; j < *word_num; j++) {
                        free(dictionary_start[j].word);
                    }
                    free(dictionary_start);
                    close(fd);
                    perror("Error: ");
                    return NULL;
                }
                (*word_num)++;

                 // resizing if more words in dictionary than initial assumption
                if ((*word_num) >= lines_num) {                       
                    lines_num *= 2;
                    Word *bigger_dictionary = realloc(dictionary_start, lines_num * sizeof(Word));
                    if (bigger_dictionary == NULL) {                // in case reallocation fails
                        for (int j = 0; j < *word_num; j++) {
                            free(dictionary_start[j].word);
                        }
                        free(dictionary_start);
                        close(fd);
                        perror("Error: ");
                        return NULL;
                    }
                    dictionary_start = bigger_dictionary;
                }
                start = end + 1;
            }

            // in case last word does not have newlines at the end
            if ((i == (bytes_read-1)) && (word_buffer[i] != '\n')) {    
                word_buffer[bytes_read] = '\0';
                dictionary_start[*word_num].word = strdup(start);
                if (dictionary_start == NULL) {                     // in case allocation fails
                    for (int j = 0; j < *word_num; j++) {
                        free(dictionary_start[j].word);
                    }
                    free(dictionary_start);
                    close(fd);
                    perror("Error: ");
                    return NULL;
                }
                (*word_num)++;
            }
        }
        // former do while   
    }
    close(fd);                              // anything opened must be closed when done with use
    return dictionary_start;
}

void file_search (char* filename, Word* dictionary) {          // function to recursively search for files in a directory 
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
            strcat(path, "/");               // '/' to indicate new dir
            strcat(path, entity->d_name);    // 
            file_search(path, dictionary);               // recursive search
        }
    }

    closedir(dir);                           // close file when done 
}


int case_word(char *word){
    for(int i = 1; i < sizeof(char*); i++){

        if(word[i] <=90 && word[i] >= 65){   // checks ascii value if it is a capitalized letter within word
            return 2;                        // returns 1 if capitalized letter
        }
    }
    return 1;                                // not a pronoun 
}

void check_for_word(char* word, Word* dictionary) {
    char ex[12]; 
    switch(case_word(ex)){
        case 1:                     // case for regular words
        
        case 2:                     // case for pronouns/ words that have capitalization in middle
    }

}

void return_error() {

}

int main (int argc, char** argv){
    if (argc < 2) {
        printf("Use of %s: <dictionary_path> <file1> <file2> ...", argv[0]);
        return EXIT_FAILURE;
    }

    int num_of_words;
    Word *official_dict_arr = dict_arr(argv[1], &num_of_words);
    if (official_dict_arr == NULL) {
        printf("Error: Failed to read dictionary file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (DEBUG) {
        printf("Dictionary words:\n");
        for (int i = 0; i < num_of_words; i++) {
            printf("%s\n", official_dict_arr[i].word);
        }
    }

    for (int i = 2; i < argc; i++) {
        file_search((argv[i]), official_dict_arr);
    }

    for (int i = 0; i < num_of_words; i++) {
        free(official_dict_arr[i].word);
    }
    free(official_dict_arr);

    
    return EXIT_SUCCESS;
}


// former do while:
// do {
        //     printf("here step 5\n");
        //     if (*end == '\n') {             // once end locates the newline operator
        //         *end = '\0';                // it replaces it with null terminator to denote the end of a word
        //         printf("here step 9\n");

        //         dictionary_start[*word_num].word = strdup(start);   // word then gets copied into dictionary_start (using strdup over strcpy because it is more memory safe)
        //         printf("word: %s\n", dictionary_start[*word_num].word);
        //         if (dictionary_start[*word_num].word == NULL) {     // in case allocation fails
 
        //             for (int i = 0; i < *word_num; i++) {
        //                 free(dictionary_start[i].word);
        //             }
        //             free(dictionary_start);
        //             close(fd);
        //             perror("Error: ");
        //             return NULL;
        //         }

        //         (*word_num)++;                                      // number of words in dict goes up
        //         num_of_words++;

        //         if ((*word_num) >= lines_num) {                     // in case there are more words than initial assumption
        //             lines_num *= 2;

        //             Word *bigger_dict_arr = realloc(dictionary_start, lines_num * sizeof(Word));    // reallocates space for more lines
        //             if (bigger_dict_arr == NULL) {                                                  // in case reallocation fails
        //                 for (int i = 0; i < *word_num; i++) {
        //                     free(dictionary_start[i].word);
        //                 }
        //                 free(dictionary_start);
        //                 close(fd);
        //                 perror("Error: ");
        //                 return NULL;
        //             }
        //             dictionary_start = bigger_dict_arr;
        //         }
        //         start = end + 1;
        //     }
            
        //     end++;

        //     bytes_read -= (word_buffer - start);            // in case something goes wrong and 
        //     if (bytes_read > 0) {
        //         memmove(word_buffer, start, bytes_read);
        //     }

        // } while ((*end != '\n') && (end != NULL));    
        // printf("here step 6\n"); 