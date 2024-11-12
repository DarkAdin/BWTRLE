#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<omp.h>

// Set this buffer to accomodate for the number of characters per row
#define BUF 25000

// This was necessary
// https://stackoverflow.com/questions/39560773/different-declarations-of-qsort-r-on-mac-and-linux
void qsort_r(
    void *base, int nmemb, int size,
    unsigned short (*compar)(const void *, const void *, void *),
    void *arg
);

unsigned short compare_r(const void *a, const void *b, void *p) {
    unsigned short *row_a = *(unsigned short **)a;
    unsigned short *row_b = *(unsigned short **)b;
    short cols = *(short *)p;
    for (int i = 0; i< cols; i++) {
        if (row_a[i] < row_b[i]) return -1;
        else if (row_a[i] > row_b[i]) return 1;
    }
    return 0;
}

// The alphabet decides the order
char *alphabet = "¡¿!$'+,.-:;?^_|[]()ABCDEFGHIJKLMNÑOPQRSTUVWXYZabcdefghijklmnñopqrstuvwxyz";

unsigned short to_number (char letter, char *alphabet) {
    unsigned short len = strlen(alphabet);
    for (unsigned short i = 0; i < len; i++) {
        if (letter == alphabet[i]) return i;
    }
}

// BURROWS-WHEELER TRANSFORM + RUN-LENGTH ENCODING
void bwt_rle (char *text, char *alphabet, FILE *out) {
    /********************************
     * 1. BURROWS-WHEELER TRANSFORM *
     ********************************/
    unsigned int l = strlen(text);

    // Create unsorted rotations matrix.
    //      Only the first row is computed,
    //      the rest are generated from it.
    unsigned short **rotations = malloc(sizeof(unsigned short*) * l);
    rotations[0] = malloc(sizeof(unsigned short) * l);

    // First row first
    #pragma omp parallel for
    for (unsigned int i = 0; i < l; i++) rotations[0][i] = to_number(text[i], alphabet);

    // Then the rest of the rows
    #pragma omp parallel for
    for (unsigned int i = 1; i < l; i++) {
        rotations[i] = malloc(sizeof(unsigned short*) * l);
        #pragma omp parallel for
        for (unsigned int j = 0; j < l; j++) rotations[i][j] = rotations[0][(j + i) % l];
    }

    // Sort them
    qsort_r(rotations, l, sizeof(short*), compare_r, &l);

    // This will be the transformed string
    char *string = malloc(sizeof(char) * l);

    #pragma omp parallel for
    for (unsigned int i = 0; i < l; i++) {
        string[i] = alphabet[rotations[i][l-1]];
        free(rotations[i]);
    }
    free(rotations);

    // Null-terminate the string
    string[l] = '\0';

    /**************************
     * 2. RUN-LENGTH ENCODING *
     **************************/
    unsigned int count = 1;

    for (unsigned int i = 0; i < l; i++) {
        if (string[i+1] == string[i]) {
            count++;
            continue;
        }
        fprintf(out, "%c%d", string[i], count);
        count = 1;
    }
    fprintf(out, "%c", '\n');

    free(string);
}

// INVERSE RUN-LENGTH ENCODING + BURROWS-WHEELER TRANSFORM 
void inv_bwt_rle (char *text, char *alphabet, FILE *out) {

    /******************
     * 1. INVERSE RLE *
     ******************/
    // Character count of the original string
    unsigned int intcount = 0;

    char *inv_rle = malloc(sizeof(char) * BUF);

    while (*text) {
        char letter;
        int val;
        if (isdigit(*text) || ( (*text == '-' || *text == '+') && isdigit(*(text+1)) )) {
            val = strtol(text, &text, 10);
            intcount += val;
            #pragma omp parallel for
            for (unsigned int i = intcount-val; i < intcount; i++) inv_rle[i] = letter;
        }
        else {
            letter = text[0];
            text++;
        }
    }

    inv_rle[intcount] = '\0';

    /******************
     * 2. INVERSE BWT *
     ******************/
    int len = strlen(inv_rle);

    unsigned short **transformations = calloc(len, sizeof(short*));
    #pragma omp parallel for
    for (int i = 0; i < len; i++) transformations[i] = calloc(len, sizeof(short));

    // TODO: Optimize this very, very slow process
    int sortings = 0;
    for (int i = len-1; i >= 0; i--) {
        #pragma omp parallel for
        for (int j = 0; j < len; j++) transformations[j][i] = to_number(inv_rle[j], alphabet);
        qsort_r(transformations, len, sizeof(short*), compare_r, &len);
        printf("SORTING NUMBER %d/%d\n", sortings++, len-1);
    }

    for (int i = 0; i < len; i++) {
        // We found the ending of the original string, this is our row
        if (alphabet[transformations[i][len-1]] == '$') {
            for (int j = 0; j < len; j++) fprintf(out, "%c", alphabet[transformations[i][j]]);
            fprintf(out, "%c", '\n');
            break;
        }
    }

    // Free memory
    for (int i = 0; i < len; i++) free(transformations[i]);
    free(transformations);
    free(inv_rle);
}

// MAIN
int main (int argc, char **argv) {

    if (argc != 3) {
        printf("Burrows-Wheeler Transform + Run-Length Encoding of text/FASTA files.\nUsage: ./BWT (c)ompress/(d)ecompress text/FASTA_file\n");
        return 1;
    }

    FILE *file = fopen(argv[2], "r");

    if (file == NULL) {
        printf("Could not open file %s.\n", argv[2]);
        return 1;
    }

    //clock_t begin = clock();

    // 1. COMPRESS
    if (argv[1][0] == 'c') {

        char *outfile_name = "compression_results.txt";

        FILE *out = fopen(outfile_name, "w");

        char *line = malloc(sizeof(char) * BUF);

        while (fgets(line, BUF, file) != NULL) {
            if (line[0] == '>') {
		        printf("Name: %s\n", line);
                fprintf(out, "%s", line);
                continue;
            }
            int l = strlen(line);

            // Null-terminate the string
            if (line[l-1] == '\n') line[l-1] = '\0';
            else line[l] = '\0';

	     printf("Compressing string of %d characters.\n", l);

            // BWT+RLE
            bwt_rle(line, alphabet, out);
        }

        free(line);
        fclose(out);

        printf("Compression results written to %s.\n", outfile_name);
    }

    // 2. DECOMPRESS
    else if (argv[1][0] == 'd') {
        char *outfile_name = "decompression_results.txt";

        FILE *out = fopen(outfile_name, "w");

        char *line = malloc(sizeof(char) * BUF);

        while (fgets(line, BUF, file) != NULL) {
            if (line[0] == '>') {
                fprintf(out, "%s", line);
                continue;
            }
            int l = strlen(line);
            // Null-terminate the string
            if (line[l-1] == '\n') line[l-1] = '\0';
            else line[l] = '\0';
            // Inverse BWT+RLE
            inv_bwt_rle (line, alphabet, out);
        }

        free(line);
        fclose(out);

        printf("Decompression results written to %s.\n", outfile_name);
    }

    // No good option provided
    else {
        printf("Burrows-Wheeler Transform + Run-Length Encoding of text/FASTA files.\nUsage: ./BWT (c)ompress/(d)ecompress text/FASTA_file\n");
        return 1;
    }

    fclose(file);

    //clock_t end = clock();
    //float time = (float)(end-begin) / CLOCKS_PER_SEC;

    //printf("Done processing file %s in %.6f seconds.\n", argv[2], time);
    printf("Done processing file %s.\n", argv[2]);

    return 0;
}
