#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * Prints the first n lines of text.
 *
 * @param nl lines to print
 * @param text the text to print
 */
void printlines(int nl, char text[]) {
    int lp = 0;
    if (nl > -1) { // prints first nl lines
        for (int i = 0; lp < nl && text[i] != 0; i++) {
            if (text[i] == '\n') {
                lp++;
            } 
            write(STDOUT_FILENO, &text[i], 1);
        }
    } else { //prints everything but last nl lines
        int totallines = 0;
        for (int i = 0; text[i] != 0; i++) {
            if (text[i] == '\n') {
                totallines++;
            }
        } //gets total number of lines
        for (int i = 0; lp < totallines + nl && text[i] != 0; i++) {
            if (text[i] == '\n') {
                lp++;
            }
            write(STDOUT_FILENO, &text[i], 1);
        }
    }
}

/**
 * Prints the first nb bytes of text.
 *
 * @param nb bytes to print
 * @param text text to print
 */
void printbytes(int nb, char text[]) {
    for (int i = 0; i < nb && text[i] != 0; i++) {
        write(STDOUT_FILENO, &text[i], 1);
    }
}

/**
 * Reads a file into a char array.
 *
 * @param file number
 */
char * freed(int filenum) {

    char * content = malloc(8192);

    for (int i = 0; i < 8192; i++) {
        content[i] = 0;
    } //zero out buffer 

    char c;
    if (filenum == STDIN_FILENO) {
         
    } else {
        while ((c = read(filenum, content, 100)) != 0) {
        }
    } //read file 100 bytes at a time into content until EOF
    return content;
}

/**
 * Repeats input to user.
 *
 * @param n repeat by lines
 * @param b repeat by bytes
 * @param count how much to repeat
 */
void repeat(bool n, bool c, int count) {

    char buffer[8192];
    for (int i = 0; i < 8192; i++) {
        buffer[i] = 0;
    }
    // repeats input until \n count = nl    
    if (n) {
        int len = 0;
        while (len <= count && (n = read(STDIN_FILENO, buffer, 8192)) > 0) {
            for (int i = 0; buffer[i] != 0; i++) {
                write(STDOUT_FILENO, &buffer[i], 1);
                if (buffer[i] == '\n') len++;
            }
            //if (write(STDOUT_FILENO, buffer, eofsig) != eofsig) perror("write");
        }
    } else if (c) { //repeats input for first c chars
        int bp = 0;
        while (bp < count && (n = read(STDIN_FILENO, buffer, 8192)) > 0) {
            for (int i = 0; buffer[i] != 0 && bp < count; i++) {
                write(STDOUT_FILENO, &buffer[i], 1);
                bp++;
            }
            //if (write (STDOUT_FILENO, buffer, eofsig) != eofsig) perror("write");
        }
    } else {
        while ((n = read(STDIN_FILENO, buffer, 8192)) > 0) {
            //write(STDOUT_FILENO, buffer, eofsig);
        }
    }
}

/**
 * Parses inputs and opens files
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

    //printf("started \n");

    while((option = getopt(argc, argv,"n:c:")) != -1) {
        //printf("started input check \n");
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
    } //parses options and option args
    files = (optind < argc ? (char **) &argv[optind] : nofiles); //creates array of non-option arguments
    int numfiles = argc - optind; //gets size of above array
    int fnum;

    if (numfiles == 0) {
        numfiles = 1;
    }

    bool hadopts;
    for (int i = 0; i < numfiles; i++) {
        hadopts = true;
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
            hadopts = false;
            if (dn) {
                repeat(true, false, nl);
            } else if (dc) {
                repeat(false, true, nb);
            } else {
                repeat(false, false, 0);
            } //takes care of reading from stdin
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
                char err[] = "head: cannot open '";
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
                char err[] = "head: cannot open '";
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
        } //adds new line before multiple files
        if (hadopts) {
            free(text);
        } //frees allocated memory if memory was allocated
    }
}

    
        

