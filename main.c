#include <stdio.h>
#include <stdlib.h>

int filter_text(char *source_path, char *target_path);
float *get_prob(char *source_path);

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

float *get_prob(char *source_path) {
    FILE *fp = fopen(source_path, "r");
    float *probs = (float *)malloc(sizeof(float) * 26);
    char ch;
    int size = 0;

    if(fp == NULL) {
        printf("[get_prob] Error: File not found!\n");
        return -1;
    }

    for(int i = 0; i < 26; i++) {
        probs[i] = 0.0;
    }
    while(fscanf(fp, "%c", &ch) == 1) {
        if((int)ch == 32) {
            continue;
        }
        size++;
        if((int)ch < 97) {
            ch = ch + 32;
        }
        probs[(ch - 97) % 26]++;
    }
    for(int i = 0; i < 26; i++) {
        probs[i] = probs[i] * 100 / size;
    }
    fclose(fp);

    return probs;
}

int main() {
    char *sample_path = "./texts/sample.txt";
    char *clean_sample_path = "./texts/clean_sample.txt";
    
    filter_text(sample_path, clean_sample_path);

    return 0;
}