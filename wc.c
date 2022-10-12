#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * Gets the number of whitespace seperated words.
 *
 * @param text the string to scan for number of words
 */
int getWords(char text[]) {
    int nw = 0;
    bool isspace = true;
    for(int i = 0; text[i] != 0; i++) {
        if (text[i] == '\n' || text[i] == ' ' || text[i] == '\0' || text[i] == '\t') {
            isspace = true;
        } else if(isspace) {
            nw++;
            isspace=false;
        } 
    } //goes through file, increments word count whenever transition from whitespace to !whitespace occurs
    return nw;
}

/**
 * Gets the number of lines in a string.
 *
 * @param text the string to scan for number of lines
 */
int getLines(char text[]) {
    int nl = 0;
    for(int i = 0; text[i] != 0; i++) {
        if (text[i] == '\n' || text[i] == '\0') {
            nl++;
        }
    } //counts newlines and EOF to get totallines
    return nl;
}

/**
 * Gets the number of bytes in a string.
 *
 * @param text the string to scan for number of bytes
 */
int getBytes(char text[]) {
    int nb = 0;
    for(int i = 0; text[i] != 0; i++) {
        nb++;
    }
    return nb;
}

/**
 * Reads a file into a char array.
 *
 * @param filenum the file number to read into the array.
 */
char * freed(int filenum) {

    char * content = malloc(8192);
    char * temp = malloc(8192);

    for (int i = 0; i < 8192; i++) {
        content[i] = 0;
        temp[i] = 0;
    }

    char c;
    int ct = 0;
    if (filenum == STDIN_FILENO) {
        while (((c = read(STDIN_FILENO, temp, 8192)) > 0)) {
            for (int i = 0; temp[i] != 0; i++) {
                content[ct] = temp[i];
                ct++;
            } //reads stdin into temp buffer, copies temp to final content array
            for (int i = 0; i < 8192; i++) {
                temp[i] = 0;
            } //resets temp buffer
        } 
    } 
    while ((c = read(filenum, content, 100)) != 0) {
    }
    free(temp); //frees temp buffer memory
    return content;
}

/**
 * Parses user input and loops through files.
 */
int main(int argc, char * argv[]) {
    setbuf(stdout, NULL); //lets printf be used
    bool dl = false;
    bool dc = false;
    bool dw = false;
    int totaldl = 0;
    int totaldc = 0;
    int totaldw = 0;
    char * text;
    char ** files;
    char * nofiles[] = {"-", NULL};
    int option;


    while((option = getopt(argc, argv,"clw")) != -1) {
        switch(option) {
            case 'c' :
                dc = true;
                break;
            case 'l' :
                dl = true;
                break;
            case 'w' :
                dw = true;
                break;
            case '?' :
                break;
        }
    } //parses input for options
    if (dl == false && dc == false && dw == false) {
        dl = true;
        dc = true;
        dw = true;
    } //default case if no options
    files = (optind < argc ? (char **) &argv[optind] : nofiles); //turns remaining arguments into file array
    int numfiles = argc - optind; //size of above array
    int fnum;

    if (numfiles == 0) {
        numfiles = 1;
    } //lets loop run on no files
    for (int i = 0; i < numfiles; i++) {
        if (argc == 1 || strcmp(files[i], "-") == 0) {
            fnum = STDIN_FILENO;
        } else {
            fnum = open(files[i], O_RDONLY);
        }
        if (fnum == -1) {
            char err[] = "wc: cannot open '";
            char err2[] = "' for reading: No such file or directory\n";
            write(STDOUT_FILENO, err, strlen(err));
            write(STDOUT_FILENO, files[i], strlen(files[i]));
            write(STDOUT_FILENO, err2, strlen(err2));
            exit(EXIT_FAILURE);
        }
        text = freed(fnum);
        int values[3] = {getBytes(text), getLines(text), getWords(text)};
        //array of bytes, lines, words in input
        totaldl = totaldl + values[1];
        totaldw = totaldw + values[2];
        totaldc = totaldc + values[0];
        // totals up values for multiple files
        if (dl) {
            printf("%d ", values[1]);
        }
        if (dw) {
            printf("%d ", values[2]);
        }
        if (dc) {
            printf("%d ", values[0]);
        }
        printf(files[i]); //prints filename
        printf("\n");
        free(text); //frees allocated memory;
    }
    if (numfiles > 1) {
        if (dl) {
            printf(" %d", totaldl);
        }
        if (dw) {
            printf(" %d", totaldw);
        }
        if (dc) {
            printf(" %d", totaldc);
        }
        printf(" total\n");
    } //prints totals
}

    
        

