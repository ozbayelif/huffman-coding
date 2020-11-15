#include <stdio.h>
#include <stdlib.h>

typedef struct huffman_node_s *huffman_node;
typedef struct huffman_node_s {
    float data;
    huffman_node left;
    huffman_node right;
} huffman_node_t[1];

typedef struct huffman_tree_s *huffman_tree;
typedef struct huffman_tree_s {
    huffman_node root;
} huffman_tree_t[1];

huffman_tree huffman_tree_init();
huffman_node huffman_node_init(float data);
void hufmann_tree_print(huffman_node node, int i);
int filter_text(char *source_path, char *target_path);
float *get_prob(char *source_path);

huffman_tree huffman_tree_init() {
    huffman_tree tree = (huffman_tree)malloc(sizeof(huffman_tree_t));
    if(tree == NULL) {
        printf("[hufmann_tree_init] Error: Tree initialization failed!\n");
        exit(1);
    }
    tree->root = NULL;

    return tree;
}

huffman_node huffman_node_init(float data) {
    huffman_node node = (huffman_node)malloc(sizeof(huffman_node_t));
    if(node == NULL) {
        printf("[huffman_node_init] Error: Node initialization failed!\n");
        exit(1);
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void huffman_tree_print(huffman_node node, int i) {
    if(node != NULL) {
        i++;
        huffman_tree_print(node->right, i);
        for (int j = 0; j < i; j++) {
            printf("\t");
        }
        printf("%f\n", node->data);
        huffman_tree_print(node->left, i);
    }
}

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