#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * Prints the last nl lines of text.
 *
 * @param nl lines to print
 * @param text the text to print
 */
void printlines(int nl, char text[]) {
    int totallines = 0;
    int lines = 0;
    bool first = false;
    for (int i = 0; text[i] != 0; i++) {
        if (text[i] == '\n' || text[i] == '\0') {
            totallines++;
        }
    } //gets total lines in text
    int skip;
    if (nl > -1) {
        skip = totallines - nl - 1;
    } else {
        skip = totallines + nl - 1;
    }
    for (int i = 0; text[i] != 0; i++) {
        if (text[i] == '\n') {
            lines++;
        }
        if (lines > (skip)) {
            if (first) {
                write(STDOUT_FILENO, &text[i], 1);
            } else {
                first = true;
            }
        }
    } //iterates through text until skipped lines are passed
}

/**
 * Prints the last nb bytes of text.
 *
 * @param nb the number of bytes to print
 * @param text the text to print
 */
void printbytes(int nb, char text[]) {
    int bytes = 0;
    for (int i = 0; text[i] != 0; i++) {
        bytes++;
    } //gets total bytes
    int start = 0;
    if (nb > -1) {
        start = bytes - nb;
    } else {
        start = bytes + nb;
    }
    for (int i = start; text[i] != 0; i++) {
        write(STDOUT_FILENO, &text[i], 1);
    } //iterates through until only nb bytes are left, then prints
}

/**
 * Reads a file into a char array.
 *
 * @param filenum the file number of the file to read
 */
char * freed(int filenum) {

    char * content = malloc(8192);
    char * temp = malloc(8192);

    for (int i = 0; i < 8192; i++) {
        content[i] = 0;
        temp[i] = 0;
    }

    //int ctrack = 0;
    char c;
    int ct = 0;
    if (filenum == STDIN_FILENO) {
        while (((c = read(STDIN_FILENO, temp, 8192)) > 0)) {
            for (int i = 0; temp[i] != 0; i++) {
                content[ct] = temp[i];
                ct++;
            } //reads stdin into temp buffer
            for (int i = 0; i < 8192; i++) {
                temp[i] = 0;
            } //adds contents of temp buffer to final char array
        } 
    } 
    while ((c = read(filenum, content, 100)) != 0) {
    } //reads normal files into char array
    free(temp);  //frees memory for temp file
    return content;
}

/**
 * Repeats user input.
 *
 * @param n print last lines
 * @param b print last bytes
 * @param count number of lines/bytes to print
 */
void repeat(bool n, bool c, int count) {

    char buffer[8192];
    for (int i = 0; i < 8192; i++) {
        buffer[i] = 0;
    } //zero out buffer
    
    if (n) {
        int len = 0;
        while (len < count && (n = read(STDIN_FILENO, buffer, 8192)) > 0) {
            for (int i = 0; buffer[i] != 0; i++) {
                write(STDOUT_FILENO, &buffer[i], 1);
                if (buffer[i] == '\n') len++;
            }
        }
    } else if (c) {
        int bp = 0;
        while (bp < count && (n = read(STDIN_FILENO, buffer, 8192)) > 0) {
            for (int i = 0; buffer[i] != 0 && bp < count; i++) {
                write(STDOUT_FILENO, &buffer[i], 1);
                bp++;
            }
        }
    } else {
        while ((n = read(STDIN_FILENO, buffer, 8192)) > 0) {
        }
    }
}

/**
 * Parses user input and loops through files.
 */
int main(int argc, char * argv[]) {
    int nl = 10;
    int nb;
    bool dn = false;
    bool dc = false;
    bool justdash = false;
    char * text;
    char ** files;
    char * nofiles[] = {"-", NULL};
    int option;


    while((option = getopt(argc, argv,"n:c:")) != -1) {
        switch(option) {
            case 'n' :
                dn = true;
                nl = atoi(optarg);
                break;
            case 'c' :
                dc = true;
                nb = atoi(optarg);
                break;
            case '?' :
                //printf("whats going on \n");
                break;
        }
    } //parses input for options and option arguments
    files = (optind < argc ? (char **) &argv[optind] : nofiles); //creates array of remaining args
    int numfiles = argc - optind; //size of above array
    int fnum;

    if (numfiles == 0) {
        numfiles = 1;
    } //lets loop run if no args are passed

    for (int i = 0; i < numfiles; i++) {
        if (numfiles > 1) {
            char p1[] = "==> ";
            char p2[] = " <==\n";
            char * center;

            if(strcmp(files[i], "-") == 0) {
                center = "standard input";
            } else {
                center = files[i];
            }
            write(STDOUT_FILENO, p1, strlen(p1));
            write(STDOUT_FILENO, center, strlen(center));
            write(STDOUT_FILENO, p2, strlen(p2));
        }
        if (justdash || argc == 1 || numfiles == 0 || strcmp(files[i], "-") == 0) {
            if (dn) {
                text = freed(STDIN_FILENO);  
                printlines(nl, text);
            } else if (dc) {
                text = freed(STDIN_FILENO);  
                printbytes(nb, text);
            } else {
                text = freed(STDIN_FILENO);  
                printlines(nl, text);
            }
        } else if (dn) {
            fnum=open(files[i], O_RDONLY);
            if (fnum == -1) {
                char err[] = "head: cannot open '";
                char err2[] = "' for reading: No such file or directory\n";
                write(STDOUT_FILENO, err, strlen(err));
                write(STDOUT_FILENO, files[i], strlen(files[i]));
                write(STDOUT_FILENO, err2, strlen(err2));
                exit(EXIT_FAILURE);
            }
            text = freed(fnum);
            printlines(nl, text);
        } else if (dc) {
            fnum=open(files[i], O_RDONLY);
            if (fnum == -1) {
                char err[] = "tail: cannot open '";
                char err2[] = "' for reading: No such file or directory\n";
                write(STDOUT_FILENO, err, strlen(err));
                write(STDOUT_FILENO, files[i], strlen(files[i]));
                write(STDOUT_FILENO, err2, strlen(err2));
                exit(EXIT_FAILURE);
            }
            text = freed(fnum);
            printbytes(nb, text);
        } else {
            fnum=open(files[i], O_RDONLY);
            if (fnum == -1) {
                char err[] = "tail: cannot open '";
                char err2[] = "' for reading: No such file or directory\n";
                write(STDOUT_FILENO, err, strlen(err));
                write(STDOUT_FILENO, files[i], strlen(files[i]));
                write(STDOUT_FILENO, err2, strlen(err2));
                exit(EXIT_FAILURE);
            }
            text = freed(fnum);
            printlines(nl, text);
        }
        if (numfiles > 1 && i != numfiles - 1) {
            char newl[] = "\n";
            write(STDOUT_FILENO, newl, strlen(newl));
        } //seperates multiple files
        free(text); //frees allocated memory
    }
}

    
        

