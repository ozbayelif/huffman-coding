#include <stdio.h>

int filter_text(char *source_path, char *target_path) {
    FILE *fin = fopen(source_path, "r");
    FILE *fout = fopen(target_path, "w");

    if(fin == NULL) {
        printf("[filter_text] Error: File not found!\n");
        return -1;
    }
    if(fout == NULL) {
        printf("[filter_text] Error: File couldn't be created!\n");
        return -1;
    }

    char ch;
    int word_size = 0, last_space = 0;
    while(fscanf(fin, "%c", &ch) == 1) {
        if((int)ch == 32 || (int)ch == 10) {
            while((int)ch == 32 || (int)ch == 10) {
                if(fscanf(fin, "%c", &ch) != 1) {
                    break;
                }
            }
            if(!last_space) {
                fprintf(fout, "%c", ' ');
                last_space = 1;
            }
            word_size = 0;
        }
        if(((int)ch >= 65 && (int)ch <=90) || ((int)ch >= 97 && (int)ch <=122)) {
            fprintf(fout, "%c", ch);
            word_size++;
            last_space = 0;
        } else if((int)ch >= 128 || (int)ch < 0) {
            fseek(fout, -word_size, SEEK_CUR);
            while(fscanf(fin, "%c", &ch) == 1) {
                if((int)ch == 10 || (int)ch == 32) {
                    break;
                }
            }
            if(!last_space) {
                fprintf(fout, "%c", ' ');
                last_space = 1;
            }
            word_size = 0;
        }
    }
    fclose(fin);
    fclose(fout);

    return 1;
}



int main() {
    char *sample_path = "./texts/sample.txt";
    char *clean_sample_path = "./texts/clean_sample.txt";
    
    filter_text(sample_path, clean_sample_path);

    return 0;
}