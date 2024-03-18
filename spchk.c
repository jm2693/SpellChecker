#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>

#define DEBUG 0
#define DICT_LINES 10000                    // initial num of lines assumed to be 10000
#define WORD_LENGTH 200                     // initial word length assumed to be 199 chars + null terminator 

typedef struct Word {                       // creates a char* struct, as to not use a char[][] and instead use a word* for a string array
    char *word;
} Word;

Word* dict_arr (char* dict_file, int* word_num);                        // making dictionary into a string array
void file_search (char* filename, Word* dictionary, int word_num);      // determining if regular file or directory + recursive search
void check_spelling(char* txt_file, Word* dictionary, int word_num);    // checks every word in txt file against dictionary array
int return_error(char* txt_file, char* misspelled_word, int line, int column);  // returns error for misspelled word

// int is_punctuation(char c) {
//     return c == '.' || c == ',' || c == '!' || c == '?' || c == ':' || c == ';' || c == '(' || c == ')' 
//                     || c == '[' || c == ']' || c == '{' || c == '}' || c == '/' || c == '\'';
// }

int is_quote_brack(char c) {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '"' || c == '\'';
}

void remove_trailing_punctuation(char *str) {
    if (str == NULL) // Check for NULL pointer
        return;

    int length = 0;

    // Find the length of the string
    while ((str + length) != NULL) {
        length++;
    }

    // Decrement length to exclude null terminator
    length--;

    // Move backwards from the end of the string
    while (length >= 0 && ispunct(*(str + length))) {
        *(str + length) = '\0'; // Replace punctuation with null terminator
        length--; // Move to the previous character
    }
}

int compare_words (const void* first, const void* second) {   // for binary search 

    if (DEBUG) printf("check cmp 1\n");

    return strcmp ((char*)first, ((Word*)second)->word);
}     

int compare_strings (const void* first, const void* second) {   // for qsort for testing our dictionaries
    char *s1 = ((Word*)first)->word;
    char *s2 = ((Word*)second)->word;

    return strcmp (s1, s2);
}

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
                start = end+1;      // start now begins at the char after end
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

// function to recursively search for files in a directory 
void file_search (char* filename, Word* dictionary, int word_num) {     
    
    if (DEBUG) printf("check file 0\n");

    struct stat stats;
    stat(filename, &stats);

    if (S_ISDIR(stats.st_mode)) {

        if (DEBUG) printf("check file 1\n");

        DIR *dir = opendir(filename);            // creating a DIR* to open file           
        if (dir == NULL) return;                 // if file is NULL, function returns


        typedef struct dirent dirent;            
        dirent* entry;                    
        entry = readdir(dir);                      

        while (entry != NULL) {

            if (DEBUG) printf("check file 2\n");

            // checks if the file is a directory 
            if (entry->d_type == DT_DIR 
            && strcmp(entry->d_name, ".") != 0 
            && strcmp(entry->d_name, "..") != 0) {    

            if (DEBUG) printf("check file 3\n");

                char path[1024] = { '\0' };      // creates path for the file as a string
                strcat(path, filename);          // concatonates the currrent file to the path variable
                strcat(path, "/");               // '/' to indicate new dir
                strcat(path, entry->d_name);      
                if (DEBUG) printf("%s\n", path);
                file_search(path, dictionary, word_num);               // recursive search
            } else if (entry->d_type == DT_REG) {
                char path[1024] = { '\0' };      // creates path for the file as a string
                strcat(path, filename);          // concatonates the currrent file to the path variable
                strcat(path, "/");               // '/' to indicate new dir
                strcat(path, entry->d_name);      
                if (DEBUG) printf("%s\n", path);

                if (DEBUG) printf("check file 3.5\n");
                if (DEBUG) printf("%s, %s, %d\n", path, (char*)(dictionary), word_num);

                check_spelling(path, dictionary, word_num);
            }
            entry = readdir(dir);
        }
    closedir(dir);                           // close file when done 
    }

        if (S_ISREG(stats.st_mode)) {

            if (DEBUG) printf("check file 4\n");

            check_spelling(filename, dictionary, word_num);
        }

        if (DEBUG) printf("check file 5\n");

}

int case_word(char *word){
    for(int i = 1; i < sizeof(char*); i++){
        if(word[i] <=90 && word[i] >= 65){   // checks ascii value if it is a capitalized letter within word
            return 1;                        // returns 1 if capitalized letter
        }
    }
    return 0;                                // not a pronoun 
}


void check_spelling(char* txt_file, Word* dictionary, int word_num) {

    if (DEBUG) printf("check spelling 1\n");

    int fd = open(txt_file, O_RDONLY);
    if (fd < 0) {
        perror("Error: ");
        return;
    }

    int line_counter = 1;
    int column_counter = 0;

    ssize_t bytes_read;                 
    char buffer[WORD_LENGTH];           // buffer for reading in the txt file
    char word_buffer[WORD_LENGTH];      // buffer for constructing words from the txt file
    int word_index = 0;                 // index within each word
    // char valid_word[1024];              
    // int punctuation_start_count = 0;
    // int punctuation_middle_count = 0;
    // int punctuation_end_count = 0;
    // int punc_count = 0;


    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {

        if (DEBUG) printf("check spelling 2\n");

        for (ssize_t i = 0; i < bytes_read; i++) {

            if (DEBUG) printf("check spelling 3\n");

            char c = buffer[i];
            column_counter++;
            if (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '-') {

                if (DEBUG) printf("check spelling 4\n");

                if (c == '-') {
                    word_buffer[word_index] = '\0'; // null-terminate the word
                    Word key;
                    key.word = word_buffer;

                    Word *found = bsearch(key.word, dictionary, word_num, sizeof(Word), compare_words);
                    if (found == NULL) {
                        return_error(txt_file, key.word, line_counter, column_counter - word_index); // Adjust column for incomplete word
                    }
                    memset(word_buffer, 0, sizeof(word_buffer));
                    word_index = 0;

                    continue; // Skip processing hyphen as part of the word
                }

                if (word_index > 0) {
                    if (DEBUG) printf("check spelling 6\n");
                    word_buffer[word_index] = '\0'; // null-terminate the word
                    Word key;
                    key.word = word_buffer;

                    Word *found = bsearch(key.word, dictionary, word_num, sizeof(Word), compare_words);
                    // MISSING CASES FOR CAPITALIZED AND ALL CAPS
                    if (found == NULL) {

                        if (DEBUG) printf("check spelling 7\n");

                        return_error(txt_file, key.word, line_counter, column_counter - word_index);
                    }
                    memset(word_buffer, 0, sizeof(word_buffer));
                    word_index = 0;
                }

                if (c == '\n') {

                    if (DEBUG) printf("check spelling 5\n");

                    line_counter++;
                    column_counter = 0;
                }

            } 
            
            // else if (is_quote_brack(c) && word_index == 0) {
            //     punc_count++;
            //     continue;
            // } else if ((ispunct(c)) && word_index > 0) {
            //     punc_count++;
            //     continue;
            // }
            
            else {

                if (DEBUG) printf("check spelling 8\n");

                //word_index += punc_count;
                if (word_index < WORD_LENGTH-1) {

                    if (DEBUG) printf("check spelling 9\n");

                    word_buffer[(word_index++)] = c;
                }
            }

            if (DEBUG) printf("check spelling 10\n");

        }
    }

    if (word_index > 0) {
        column_counter++;
        word_buffer[word_index] = '\0'; // null-terminate the incomplete word
        Word key;
        key.word = word_buffer;

        Word *found = bsearch(key.word, dictionary, word_num, sizeof(Word), compare_words);
        if (found == NULL) {
            return_error(txt_file, key.word, line_counter, column_counter - word_index); // Adjust column for incomplete word
        }
    }

    if (DEBUG) printf("check spelling 11\n");

    close(fd);
}

int return_error(char* txt_file, char* misspelled_word, int line, int column) {

    if (DEBUG) printf("check error 1\n");

    printf("%s (%d,%d): %s\n", txt_file, line, column, misspelled_word);
    return EXIT_FAILURE;
}

int main (int argc, char** argv){
    if (argc < 3) {
        printf("Use of %s: <dictionary_path> <file1> <file2> ...", argv[0]);
        return EXIT_FAILURE;
    }

    int num_of_words;
    Word *official_dict_arr = dict_arr(argv[1], &num_of_words);
    if (official_dict_arr == NULL) {
        printf("Error: Failed to read dictionary file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    qsort(official_dict_arr, num_of_words, sizeof(Word), compare_strings);

    if (DEBUG) {
        printf("Dictionary words:\n");
        for (int i = 0; i < num_of_words; i++) {
            printf("%s\n", official_dict_arr[i].word);
        }
    }

    for (int i = 2; i < argc; i++) {

        if (DEBUG) printf("check arg 1\n");

        file_search((argv[i]), official_dict_arr, num_of_words);
    }

    for (int i = 0; i < num_of_words; i++) {
        free(official_dict_arr[i].word);
    }
    free(official_dict_arr);


    return EXIT_SUCCESS;
}