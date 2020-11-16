#include <stdio.h>
#include <stdlib.h>

#define INF 30000

typedef struct huffman_node_s *huffman_node;
typedef struct huffman_node_s {
    char letter;
    float prob;
    huffman_node left;
    huffman_node right;
} huffman_node_t[1];

typedef struct huffman_tree_s *huffman_tree;
typedef struct huffman_tree_s {
    huffman_node root;
} huffman_tree_t[1];

huffman_tree huffman_tree_init();
huffman_node huffman_node_init(char letter, float data);
void *huffman_tree_free_recursion(huffman_node node);
void huffman_tree_free(huffman_tree tree);
void huffman_tree_print(huffman_node node, int i);
void merge(huffman_node *A, int p, int q, int r);
void mergesort (huffman_node *A, int p, int r);
int filter_text(char *source_path, char *target_path);
huffman_node *get_nodes(char *source_path);

huffman_tree huffman_tree_init() {
    huffman_tree tree = (huffman_tree)malloc(sizeof(huffman_tree_t));
    if(tree == NULL) {
        printf("[hufmann_tree_init] Error: Tree initialization failed!\n");
        exit(1);
    }
    tree->root = NULL;

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

    return node;
}

void *huffman_tree_free_recursion(huffman_node node) {
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
    free(tree);
}

void huffman_tree_print(huffman_node node, int i) {
    if(node != NULL) {
        i++;
        huffman_tree_print(node->right, i);
        for (int j = 0; j < i; j++) {
            printf("\t");
        }
        printf("%c(%.2f)\n", node->letter, node->prob);
        huffman_tree_print(node->left, i);
    }
}

void merge(huffman_node *A, int p, int q, int r) {
	int i, j, k, n1, n2;
    huffman_node *L, *R;

	n1 = q - p;
	n2 = r - q;
	L = (huffman_node *)malloc(sizeof(huffman_node) * (n1 + 1));
    for(i = 0; i < n1; i++) {
        L[i] = huffman_node_init(A[p+i]->letter, A[p+i]->prob);
    }
    L[n1] = huffman_node_init('*', INF);
	R = (huffman_node *)malloc(sizeof(huffman_node) * (n2 + 1));
    for(i = 0; i < n2; i++) {
        R[i] = huffman_node_init(A[q+i]->letter, A[q+i]->prob);
    }
    R[n2] = huffman_node_init('*', INF);

	i = j = 0;
	for(k = p; k < r; k++) {
		if(L[i]->prob <= R[j]->prob) {
			A[k]->letter = L[i]->letter;
            A[k]->prob = L[i]->prob;
			i++;
		} else {
			A[k]->letter = R[j]->letter;
            A[k]->prob = R[j]->prob;
			j++;
		}
	}
    for(i = 0; i < n1 + 1; i++) {
        free(L[i]);
    }
	free(L);
    for(i = 0; i < n2 + 1; i++) {
        free(R[i]);
    }
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
        return -1;
    }

    huffman_node *nodes = (huffman_node *)malloc(sizeof(huffman_node) * 26);
    for(int i = 0; i < 26; i++) {
        nodes[i] = huffman_node_init((char)(i + 97), 0.0);
    }
    while(fscanf(fp, "%c", &ch) == 1) {
        if((int)ch == 32) {
            continue;
        }
        size++;
        if((int)ch < 97) {
            ch = ch + 32;
        }
        probs[(ch - 97) % 26][1]++;
    }
    for(int i = 0; i < 26; i++) {
        probs[i][1] = probs[i][1] * 100 / size;
    }
    fclose(fp);

    return probs;
}

int huffman_coding(char *source_path) {
    huffman_node *nodes;
    huffman_node node, left, right;
    int size = 26, pos_l = 0, pos_r = 1;

    nodes = get_nodes(source_path);
    mergesort(nodes, 0, 26);
    // for(int i = 0; i < 26; i++) {
    //     printf("%c(%.2f)\n", nodes[i]->letter, nodes[i]->prob);
    // }
    // for(int i = 0; i < 26; i++) {
    //     if(nodes[i] == NULL) {
    //         printf("NULL ");
    //     } else {
    //         printf("%c(%.2f) ", nodes[i]->letter, nodes[i]->prob);
    //     }
    // }
    // printf("\n\n");

    while(size > 1) {
        for(int i = 0; i < 25; i++) {
            if(nodes[i] != NULL) {
                left = nodes[i];
                pos_l = i;
                for(int j = i + 1; j < 26; j++) {
                    if(nodes[j] != NULL) {
                        right = nodes[j];
                        pos_r = j;
                        i = j = 27;
                    }
                }
            }
        }
        // printf("l: %c r: %c\n", left->letter, right->letter);
        for(int i = 0; i < 25; i++) {
            if(nodes[i] != NULL) {
                for(int j = i + 1; j < 26; j++) {
                    if(nodes[j] != NULL) {
                        if(nodes[i]->prob + nodes[j]->prob < left->prob + right->prob) {
                            left = nodes[i];
                            right = nodes[j];
                            pos_l = i;
                            pos_r = j;
                        }
                    }
                }
            }
        }
        node = huffman_node_init(' ', left->prob + right->prob);
        node->left = left;
        node->right = right;
        nodes[pos_l] = NULL;
        nodes[pos_r] = node;
        size--;
        for(int i = 0; i < 26; i++) {
            if(nodes[i] == NULL) {
                printf("NULL ");
            } else {
                printf("%c(%.2f) ", nodes[i]->letter, nodes[i]->prob);
            }
        }
        printf("\n\n");
    }
    huffman_tree tree = huffman_tree_init();
    tree->root = node;

    huffman_tree_print(tree->root, 0);

    huffman_tree_free(tree);
}

int main() {
    char *sample_path = "./texts/sample.txt";
    char *clean_sample_path = "./texts/clean_sample.txt";
    
    filter_text(sample_path, clean_sample_path);

    return 0;
}