#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mem {
	int size;
	int start;
	char name[16];
	struct mem *next;
};
typedef struct mem mem;

struct bud {
    int size;
    int used;
    int start;
    char name[16];
    int empty;
    struct bud *left;
    struct bud *right;
    struct bud *parent;
};
typedef struct bud bud;

int buddy(bud **root, char name[16], int size) {
    bud *currNode = *root;
    if (currNode == NULL) {
        return 0;
    }
    if (currNode->empty) {
        if (currNode->size < size) {
            return 0;
        }
        else {
            while (currNode->size / 2 >= size) {
                bud *left = (bud*) malloc(sizeof(bud));
                left->empty = 0;
                left->size = currNode->size / 2;
                left->start = currNode->start;
                left->left = NULL;
                left->right = NULL;
                left->parent = currNode;

                bud *right = (bud*) malloc(sizeof(bud));
                right->empty = 1;
                right->size = currNode->size / 2;
                right->start = left->start + left->size;
                right->left = NULL;
                right->right = NULL;
                right->parent = currNode;

                currNode->empty = 0;
                currNode->left = left;
                currNode->right = right;

                currNode = currNode->left;
            }
            strncpy(currNode->name, name, 16);
            currNode->empty = 0;
            currNode->used = size;
            printf("ALLOCATED %s %d\n", name, currNode->start);
            return 1;
        }
    }
    else {
        int status = buddy(&currNode->left, name, size);
        if (status) {
            return status;
        }
        return buddy(&currNode->right, name, size);
    }
}

int firstFit(mem **head, char name[16], int size, int totalMem) {
	mem *currNode = *head;
	if (currNode == NULL) {
		if (size <= totalMem) {
			mem *new = (mem*) malloc(sizeof(mem));
			new->size = size;
			new->start = 0;
			strncpy(new->name, name, 16);
			new->next = NULL;
			*head = new;
			printf("ALLOCATED %s %d\n", name, 0);
            return 1;
		}
		else {
			printf("FAIL REQUEST %s %d\n", name, size);
            return 0;
		}
	}
	else {
		int curr = 0;
		mem *last = NULL;
		while (currNode != NULL) {
			if (currNode->start - curr >= size) {
				mem *new = (mem*) malloc(sizeof(mem));
				new->start = curr;
				new->size = size;
				strncpy(new->name, name, 16);
				if (last != NULL) {
					new->next = last->next;
					last->next = new;
				}
				else {
					new->next = *head;
					*head = new;
				}
				printf("ALLOCATED %s %d\n", name, new->start);
				return 1;
			}
			curr = currNode->start + currNode->size;
			last = currNode;
			currNode = currNode->next;
		}
		if (totalMem - curr >= size) {
			mem *new = (mem*) malloc(sizeof(mem));
			new->start = curr;
			new->size = size;
			strncpy(new->name, name, 16);
			if (last != NULL) {
				new->next = last->next;
				last->next = new;
			}
			printf("ALLOCATED %s %d\n", name, new->start);
            return 1;
		}
		else {
			printf("FAIL REQUEST %s %d\n", name, size);
            return 0;
		}
	}
}

void release(mem **head, char name[16]) {
	mem *last = NULL;
	mem *memory = *head;
	while (memory != NULL && strcmp(memory->name, name) != 0) {
		last = memory;
		memory = memory->next;
	}
	if (memory != NULL) {
		printf("FREE %s %d %d\n", name, memory->size, memory->start);
		if (last == NULL) {
			*head = memory->next;
		}
		else {
			last->next = memory->next;
		}
		free(memory);
	}
	else {
		printf("FAIL RELEASE %s\n", name);
	}
}

int releaseBuddy(bud **root, char name[16]) {
    bud *currNode = *root;
    int found = 0;
    if (currNode == NULL) {
        return found;
    }
    else if (currNode->empty) {
        return found;
    }
    else if (currNode->name != NULL && strcmp(currNode->name, name) == 0) {
        printf("FREE %s %d %d\n", name, currNode->size, currNode->start);
        currNode->empty = 1;
        currNode->used = 0;
        bud *parent = currNode->parent;
        while (parent->left->empty && parent->right->empty) {     
            free(parent->left);
            free(parent->right);
            parent->left = NULL;
            parent->right = NULL;
            parent->empty = 1;
            parent = parent->parent;
        }
        found = 1;
        return found;
    }
    else {
        found = releaseBuddy(&currNode->left, name);
        if (found) {
            return found;
        }
        found = releaseBuddy(&currNode->right, name);
        return found;
    }
}

void listAvailable(mem **head, int totalMem) {
	int curr = 0;
	int end = totalMem;
	int full = 1;
	mem *memory = *head;
	while (memory != NULL) {
		if (curr < memory->start) {
			printf("(%d, %d) ", memory->start - curr, curr);
			full = 0;
		}
		curr = memory->start + memory->size;
		memory = memory->next;
	}
	if (curr < end) {
		printf("(%d, %d) ", end - curr, curr);
		full = 0;
	}
	if (full) {
		printf("FULL\n");
	}
	else {
		printf("\n");
	}
}

int listAvailableBuddy(bud **root) {
    int full = 1;
    bud *currNode = *root;
    if (currNode == NULL) {
        return full;
    }
    else if (currNode->empty) {
        printf("(%d, %d) ", currNode->size, currNode->start);
        full = 0;
        return full;
    }
    else {
        int status1 = listAvailableBuddy(&currNode->left);
        int status2 = listAvailableBuddy(&currNode->right);
        if (!status1 || !status2) {
            full = 0;
        }
        return full;
    }
}

void listAssigned(mem **head) {
	mem *memory = *head;
	if (memory == NULL) {
		printf("NONE\n");
	}
	else {
		while (memory != NULL) {
			printf("(%s, %d, %d) ", memory->name, memory->size, memory->start);
			memory = memory->next;
		}
		printf("\n");
	}
}

void listAssignedBuddy(bud **root) {
    bud *currNode = *root;
    if (currNode == NULL) {
        return;
    }
    if (currNode->empty) {
        return;
    }
    else if (!currNode->empty && currNode->left == NULL && currNode->right == NULL) {
        printf("(%s, %d, %d) ", currNode->name, currNode->size, currNode->start);
        return;
    }
    else {
        listAssignedBuddy(&currNode->left);
        listAssignedBuddy(&currNode->right);
    }
}

void find(mem **head, char name[16]) {
	int found = 0;
	mem *memory = *head;
	while (memory != NULL) {
		if (strcmp(memory->name, name) == 0) {
			printf("(%s, %d, %d)\n", name, memory->size, memory->start);
			found = 1;
			break;
		}
		memory = memory->next;
	}
	if (found == 0) {
		printf("FAULT\n");
	}
}

int findBuddy(bud **root, char name[16]) {
    int found = 0;
    bud *currNode = *root;
    if (currNode == NULL) {
        return found;
    }
    else if (currNode->empty) {
        return found;
    }
    else if (currNode->name != NULL && strcmp(currNode->name, name) == 0) {
        printf("(%s, %d, %d)\n", name, currNode->used, currNode->start);
        found = 1;
        return found;
    }
    else {
        found = findBuddy(&currNode->left, name);
        if (found) {
            return found;
        }
        found = findBuddy(&currNode->right, name);
        return found;
    }
}

void closeAll(int *closed, FILE **files, int n) {
    for (int i = 0; i < n; ++i) {
        if (!closed[i]) {
            fclose(files[i]);
        }
    }
}

int main(int argc, char** argv) {
    // Pull out arguments
    int q = atoi(argv[1]);
    int p = atoi(argv[2]);
    int n = atoi(argv[3]);

    // Initialize the memory
    mem *memory = NULL;

    // Initialize buddy system memory
    bud *buddyMem = (bud*) malloc(sizeof(bud));
    buddyMem->empty = 1;
    buddyMem->size = n;
    buddyMem->start = 0;
    buddyMem->left = NULL;
    buddyMem->right = NULL;
    buddyMem->parent = NULL;

    // Find the placement algorithm
    int type;
    if (strcmp(argv[4], "FIRSTFIT") == 0) {
        type = 0;
    }
    else if (strcmp(argv[4], "BUDDY") == 0) {
        type = 1;
    }
    else {
        printf("Invalid placement algorithm");
        return -1;
    }

    // Open all the file pointers
    FILE *files[p];
    for (int i = 0; i < p; ++i) {
        char fileName[16];
        sprintf(fileName, "%d.ins", i);
        files[i] = fopen(fileName, "r");
    }
    
    // Create an array to represent whether a file is closed or not
    int closed[p];
    for (int i = 0; i < p; ++i) {
        closed[i] = 0;
    }
    int numClosed = 0;

    // Create an array to represent whether a file is in deadlock or not
    int deadlock[p];
    for (int i = 0; i < p; ++i) {
        deadlock[i] = 0;
    }

    // Create variables to use for looping over the files
    int idx = 0;
    char line[64];

    // Loop over the files while there is still at least one open
    while (numClosed < p) {
        // Count how many instructions have been read
        int counter = 0;

        // Read from the file while it is open and has not reached quantum
        while (!closed[idx] && counter < q) {
            // If the end of file is reached, close it
            if (fgets(line, 64, files[idx]) == NULL) {
                ++numClosed;
                closed[idx] = 1;
                fclose(files[idx]);
            }
            // If the file is still open, perform the action specified
            else {
                if (line[0] == '#') {
                    continue;
                }

                char command[16];
                sscanf(line, "%s ", command); 
                if (strcmp(command, "REQUEST") == 0) {
                    char name[16];
                    int size;
                    sscanf(line, "%s %s %d\n", command, name, &size);
                    //printf("REQUEST %s %d\n", name, size);
                    if (type == 0) {
                        int status = firstFit(&memory, name, size, n);
                        if (!status && deadlock[idx]) {
                            printf("DEADLOCK DETECTED\n");
                            closeAll(closed, files, p);
                            return -1;
                        }
                        else if (!status) {
                            deadlock[idx] = 1;
                            int offset = 0;
                            for (int i = 0; i < 64; ++i) {
                                if (line[i] == '\0') {
                                    break;
                                }
                                ++offset;
                            }
                            fseek(files[idx], -offset, SEEK_CUR);
                            counter = q;
                        }
                        else {
                            deadlock[idx] = 0;
                        }
                    }
                    else {
                        int status = buddy(&buddyMem, name, size);
                        if (!status && deadlock[idx]) {
                            printf("FAIL REQUEST %s %d\n", name, size);
                            printf("DEADLOCK DETECTED\n");
                            closeAll(closed, files, p);
                            return -1;
                        }
                        else if (!status) {
                            printf("FAIL REQUEST %s %d\n", name, size);
                            deadlock[idx] = 1;
                            int offset = 0;
                            for (int i = 0; i < 64; ++i) {
                                if (line[i] == '\0') {
                                    break;
                                }
                                ++offset;
                            }
                            fseek(files[idx], -offset, SEEK_CUR);
                            counter = q;
                        }
                        else {
                            deadlock[idx] = 0;
                        }
                    }
                }
                else if (strcmp(command, "RELEASE") == 0) {
                    char name[16];
                    sscanf(line, "%s %s\n", command, name);
                    //printf("RELEASE %s\n", name);
                    if (type == 0) {
                        release(&memory, name);
                    }
                    else {
                        int found = releaseBuddy(&buddyMem, name);
                        if (!found) {
                            printf("FAIL RELEASE %s\n", name);
                        }
                    }
                }
                else if (strcmp(command, "LIST") == 0) {
                    char command2[16];
                    sscanf(line, "%s %s\n", command, command2);
                    if (strcmp(command2, "AVAILABLE") == 0) {
                        //printf("LIST AVAILABLE\n");
                        if (type == 0) {
                            listAvailable(&memory, n);
                        }
                        else {
                            int full = listAvailableBuddy(&buddyMem);
                            printf("\n");
                            if (full) {
                                printf("FULL\n");
                            }
                        }
                    }
                    else if (strcmp(command2, "ASSIGNED") == 0) {
                        //printf("LIST ASSIGNED\n");
                        if (type == 0) {
                            listAssigned(&memory);
                        }
                        else {
                            listAssignedBuddy(&buddyMem);
                            printf("\n");
                        }
                    }
                    else {
                        printf("Invalid command\n");
                        closeAll(closed, files, p);
                        return -1;
                    }
                }
                else if (strcmp(command, "FIND") == 0) {
                    char name[16];
                    sscanf(line, "%s %s\n", command, name);
                    //printf("FIND %s\n", name);
                    if (type == 0) {
                        find(&memory, name);
                    }
                    else {
                        int found = findBuddy(&buddyMem, name);
                        if (!found) {
                            printf("FAULT\n");
                        }
                    }
                }
                else {
                    printf("Invalid command\n");
                    closeAll(closed, files, p);
                    return -1;
                }
                ++counter;
            }
            
        }
        // Go to the next file pointer circularly
        idx = (idx + 1) % p;
    }
    closeAll(closed, files, p);
    return 0;
}