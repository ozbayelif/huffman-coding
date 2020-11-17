#include <stdio.h>
#include <stdlib.h>

#define INF 0xFFFFFFFF
#define UNIT_LEN 8
#define ALPH_SIZE 27

typedef struct huffman_node_s *huffman_node;
typedef struct huffman_node_s {
    char letter;
    float prob;
    char *code;
    huffman_node left;
    huffman_node right;
    huffman_node parent;
} huffman_node_t[1];

typedef struct huffman_tree_s *huffman_tree;
typedef struct huffman_tree_s {
    huffman_node root;
    huffman_node *node_list;
    int height;
} huffman_tree_t[1];

huffman_tree huffman_tree_init();
huffman_node huffman_node_init(char letter, float data);
void huffman_tree_free_recursion(huffman_node node);
void huffman_tree_free(huffman_tree tree);
int huffman_tree_height_recursion(huffman_node node, int i, int max_i);
int huffman_tree_height(huffman_tree tree);
void huffman_tree_print_recursion(huffman_node node, int i);
void huffman_tree_print(huffman_tree tree);
void merge(huffman_node *A, int p, int q, int r);
void mergesort (huffman_node *A, int p, int r);
int filter_text(char *source_path, char *target_path);
huffman_node *get_nodes(char *source_path);
huffman_tree huffman_coding(char *source_path);
void write_code(FILE *fp, char *code, int height, char *msg, int *fullness);
int compress(char *source_path, char *target_path, huffman_tree tree);
int extract(char *source_path, char *target_path, huffman_tree tree);

huffman_tree huffman_tree_init() {
    huffman_tree tree = (huffman_tree)malloc(sizeof(huffman_tree_t));
    if(tree == NULL) {
        printf("[hufmann_tree_init] Error: Tree initialization failed!\n");
        exit(1);
    }
    tree->root = NULL;
    tree->node_list = NULL;

    return tree;
}

huffman_node huffman_node_init(char letter, float prob) {
    huffman_node node = (huffman_node)malloc(sizeof(huffman_node_t));
    if(node == NULL) {
        printf("[huffman_node_init] Error: Node initialization failed!\n");
        exit(1);
    }
    node->letter = letter;
    node->prob = prob;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

void huffman_tree_free_recursion(huffman_node node) {
    if(node != NULL) {
        huffman_tree_free_recursion(node->right);
        huffman_tree_free_recursion(node->left);
        node->right = NULL;
        node->left = NULL;
        free(node);
    }
}

void huffman_tree_free(huffman_tree tree) {
    huffman_tree_free_recursion(tree->root);
    free(tree->node_list);
    free(tree);
}

int huffman_tree_height_recursion(huffman_node node, int i, int max_i) {
    if(node != NULL) {
        i++;
        max_i = huffman_tree_height_recursion(node->right, i, max_i);
        max_i = huffman_tree_height_recursion(node->left, i, max_i);
        if(i > max_i) {
            max_i = i;
        }
    }
    return max_i;
}

int huffman_tree_height(huffman_tree tree) {
    int height = huffman_tree_height_recursion(tree->root, 0, 0) - 1;

    return height;
}

void huffman_tree_print_recursion(huffman_node node, int i) {
    if(node != NULL) {
        i++;
        huffman_tree_print_recursion(node->right, i);
        for (int j = 0; j < i; j++) {
            printf("\t");
        }
        printf("%c(%.2f)\n", node->letter, node->prob);
        huffman_tree_print_recursion(node->left, i);
    }
}

void huffman_tree_print(huffman_tree tree) {
    int j;

    huffman_tree_print_recursion(tree->root, 0);
    for(int i = 0; i < ALPH_SIZE; i++) {
        printf("%c -> ", tree->node_list[i]->letter);
        j = 0;
        while(tree->node_list[i]->code[j] == '*') {
            j++;
        }
        for(; j < tree->height; j++) {
            printf("%c", tree->node_list[i]->code[j]);
        }
    }
}

void merge(huffman_node *A, int p, int q, int r) {
	int i, j, k, n1, n2;
    huffman_node *L, *R;

	n1 = q - p;
	n2 = r - q;
	L = (huffman_node *)malloc(sizeof(huffman_node) * (n1 + 1));
    for(i = 0; i < n1; i++) {
        L[i] = A[p+i];
    }
    L[n1] = huffman_node_init('*', INF);
	R = (huffman_node *)malloc(sizeof(huffman_node) * (n2 + 1));
    for(i = 0; i < n2; i++) {
        R[i] = A[q+i];
    }
    R[n2] = huffman_node_init('*', INF);

	i = j = 0;
	for(k = p; k < r; k++) {
		if(L[i]->prob <= R[j]->prob) {
            A[k] = L[i];
			i++;
		} else {
			A[k] = R[j];
			j++;
		}
	}
	free(L);
	free(R);
}

void mergesort (huffman_node *A, int p, int r) {
	int q;
	
	if(r > (p + 1)) {
		q = (p + r)/2;
		mergesort(A, p, q);
		mergesort(A, q, r);
		merge(A, p, q, r);
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

huffman_node *get_nodes(char *source_path) {
    FILE *fp = fopen(source_path, "r");
    char ch;
    int size = 0;

    if(fp == NULL) {
        printf("[get_nodes] Error: File not found!\n");
        return NULL;
    }

    huffman_node *nodes = (huffman_node *)malloc(sizeof(huffman_node) * ALPH_SIZE);
    for(int i = 0; i < ALPH_SIZE - 1; i++) {
        nodes[i] = huffman_node_init((char)(i + 97), 0.0);
    }
    nodes[ALPH_SIZE - 1] = huffman_node_init(' ', 0.0);
    while(fscanf(fp, "%c", &ch) == 1) {
        if((int)ch == 32) {
            nodes[ALPH_SIZE - 1]->prob++;
        } else {
            size++;
            if((int)ch < 97) {
                ch = ch + 32;
            }
            nodes[(ch - 97) % 26]->prob++;
        }
    }
    for(int i = 0; i < ALPH_SIZE; i++) {
        nodes[i]->prob = nodes[i]->prob * 100 / size;
    }
    fclose(fp);

    return nodes;
}

huffman_tree huffman_coding(char *source_path) {
    huffman_node *nodes, *alph, node, left, right, curr;
    huffman_node max_node = huffman_node_init('*', 101.0);
    int size = ALPH_SIZE, pos_l = 0, pos_r = 1, j, height;

    nodes = get_nodes(source_path);
    alph = (huffman_node *)malloc(sizeof(huffman_node) * ALPH_SIZE);
    for(int i = 0; i < ALPH_SIZE; i++) {
        alph[i] = nodes[i];
    }

    mergesort(nodes, 0, ALPH_SIZE);

    left = max_node;
    right = max_node;
    pos_l = 1;
    while(size > 1) {
        left = max_node;
        right = max_node;
        for(int i = 0; i < ALPH_SIZE; i++) {
            if(nodes[i] != NULL) {
                if(nodes[i]->prob < left->prob) {
                    if(nodes[pos_l] != NULL) {
                        right = nodes[pos_l];
                        pos_r = pos_l;
                    }
                    left = nodes[i];
                    pos_l = i;
                } else if(nodes[i]->prob < right->prob) {
                    right = nodes[i];
                    pos_r = i;
                }
            }
        }
        node = huffman_node_init('*', left->prob + right->prob);
        node->left = left;
        node->right = right;
        left->parent = node;
        right->parent = node;
        nodes[pos_l] = NULL;
        nodes[pos_r] = node;
        size--;
    }
    free(max_node);

    huffman_tree tree = huffman_tree_init();
    tree->root = node;
    height = huffman_tree_height(tree) + 1;
    tree->height = height;

    for(int i = 0; i < ALPH_SIZE; i++) {
        j = height - 2;
        node = alph[i];
        curr = alph[i];
        node->code = (char *)malloc(sizeof(char) * height);
        for(int k = 0; k < height; k++) {
            node->code[k] = '*';
        }
        while(curr != tree->root) {
            if(curr->parent->left == curr) {
                node->code[j] = '0';
                j--;
            } else if(curr->parent->right == curr) {
                node->code[j] = '1';
                j--;
            }
            curr = curr->parent;
        }
    }
    tree->node_list = alph;

    return tree;
}

void write_code(FILE *fp, char *code, int height, char *msg, int *fullness) {
    int i = 0;

    while(code[i] == '*') {
        i++;
    }
    while(i < height) {
        if(*fullness >= UNIT_LEN) {
            fwrite(msg, sizeof(char), 1, fp);
            *msg = 0;
            *fullness = 0;
        } else {
            *msg <<= 1;
            if(code[i] == '1') {
                *msg |= 1;
            }
            i++;
            (*fullness)++;
        }
    }
}

int compress(char *source_path, char *target_path, huffman_tree tree) {
    FILE *fin = fopen(source_path, "r");
    FILE *fout = fopen(target_path, "wb");
    char ch, *code;
    char msg = 0;
    int fullness = 0;

    if(fin == NULL) {
        printf("[compress] Error: File not found!\n");
        return -1;
    }
    if(fout == NULL) {
        printf("[compress] Error: File couldn't be created!\n");
        return -1;
    }

    while(fscanf(fin, "%c", &ch) == 1) {
        if(ch == ' ') {
            code[tree->height - 1] = '0';
            code = tree->node_list[ALPH_SIZE - 1]->code;    
        } else {
            if((int)ch < 97) {
                code = tree->node_list[((int)ch - 65) % (ALPH_SIZE - 1)]->code;
                code[tree->height - 1] = '1';
            } else {
                code = tree->node_list[((int)ch - 97) % (ALPH_SIZE - 1)]->code;
                code[tree->height - 1] = '0';
            }
        }
        write_code(fout, code, tree->height, &msg, &fullness);
    }
    if(fullness != 0) {
        fwrite(&msg, sizeof(char), 1, fout);
    }

    fclose(fin);
    fclose(fout);

    return 1;
}

int extract(char *source_path, char *target_path, huffman_tree tree) {
    FILE *fin = fopen(source_path, "rb");
    FILE *fout = fopen(target_path, "w");
    char msg = 0, new = 1;
    huffman_node node = tree->root;
    int sh;

    if(fin == NULL) {
        printf("[extract] Error: File not found!\n");
        return -1;
    }
    if(fout == NULL) {
        printf("[extract] Error: File couldn't be created!\n");
        return -1;
    }

    while(fread(&msg, sizeof(char), 1, fin) == 1) {
        if(new) {
            sh = UNIT_LEN - 1;
        }
        new = 1;
        while(sh >= 0) {
            if(node->left == NULL && node->right == NULL) {
                if((msg >> sh) & 0b00000001) {
                    fprintf(fout, "%c", node->letter - 32);
                } else {
                    fprintf(fout, "%c", node->letter);
                }
                node = tree->root;
                sh--;
            } else {
                if(((msg >> sh) & 0b00000001) == 0) {
                    node = node->left;
                } else {
                    node = node->right;
                }
                sh--;
            }
        }
        if(node->left == NULL && node->right == NULL) {
            fread(&msg, sizeof(char), 1, fin);
            sh = UNIT_LEN - 1;
            if((msg >> sh) & 0b00000001) {
                fprintf(fout, "%c", node->letter - 32);
            } else {
                fprintf(fout, "%c", node->letter);
            }
            node = tree->root;
            sh--;
            new = 0;
            fseek(fin, -sizeof(char), SEEK_CUR);
        }
    }

    fclose(fin);
    fclose(fout);

    return 1;
}

int main() {
    char *messy_sample_path = "./texts/messy_sample.txt";
    char *sample_path = "./texts/sample.txt";
    char *compressed_sample_path = "./texts/sample.ez";
    char *extracted__sample_path = "./texts/sample2.txt";
    
    filter_text(messy_sample_path, sample_path);
    huffman_tree tree = huffman_coding(sample_path);
    compress(sample_path, compressed_sample_path, tree);
    extract(compressed_sample_path, extracted__sample_path, tree);
    huffman_tree_free(tree);

    return 0;
}