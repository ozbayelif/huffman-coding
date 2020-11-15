#include <stdio.h>

int filter_text(char *source_path, char *target_path) {
    FILE *fin = fopen(source_path, "r");
    FILE *fout = fopen(target_path, "w");

    if(fin == NULL) {
        printf("Error: File to be filtered not found!\n");
        return -1;
    }
    if(fout == NULL) {
        printf("Error: Target file for the filtered text couldn't be created!\n");
        return -1;
    }

    char ch;
    while(fscanf(fin, "%c", &ch) == 1) {
        if((int)ch == 32 || (int)ch == 10) {
            while((int)ch == 32 || (int)ch == 10) {
                fscanf(fin, "%c", &ch);
            }
            fprintf(fout, "%c", ' ');
        }
        if((int)ch >= 65 && (int)ch <=90) {
            fprintf(fout, "%c", (int)ch + 32);
        } else if((int)ch >= 97 && (int)ch <=122) {
            fprintf(fout, "%c", ch);
        }
    }

    fclose(fin);
    fclose(fout);

    return 1;
}

int main() {
    return 0;
}