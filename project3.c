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
            return 0;
		}
		else {
			printf("FAIL REQUEST %s %d\n", name, size);
            return -1;
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
				return 0;
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
            return 0;
		}
		else {
			printf("FAIL REQUEST %s %d\n", name, size);
            return -1;
		}
	}
}

void buddy(mem **head, char name[16], int size, int totalMem) {

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

int main(int argc, char** argv) {
    // Pull out arguments
    int q = atoi(argv[1]);
    int p = atoi(argv[2]);
    int n = atoi(argv[3]);

    // Initialize the memory
    mem *memory = NULL;

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
                        if (status && deadlock[idx]) {
                            printf("DEADLOCK DETECTED");
                            return -1;
                        }
                        else if (status) {
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
                        buddy(&memory, name, size, n);
                    }
                }
                else if (strcmp(command, "RELEASE") == 0) {
                    char name[16];
                    sscanf(line, "%s %s\n", command, name);
                    //printf("RELEASE %s\n", name);
                    release(&memory, name);
                }
                else if (strcmp(command, "LIST") == 0) {
                    char command2[16];
                    sscanf(line, "%s %s\n", command, command2);
                    if (strcmp(command2, "AVAILABLE") == 0) {
                        //printf("LIST AVAILABLE\n");
                        listAvailable(&memory, n);
                    }
                    else if (strcmp(command2, "ASSIGNED") == 0) {
                        //printf("LIST ASSIGNED\n");
                        listAssigned(&memory);
                    }
                    else {
                        printf("Invalid command\n");
                        return -1;
                    }
                }
                else if (strcmp(command, "FIND") == 0) {
                    char name[16];
                    sscanf(line, "%s %s\n", command, name);
                    //printf("FIND %s\n", name);
                    find(&memory, name);
                }
                else {
                    printf("Invalid command\n");
                    return -1;
                }
                ++counter;
            }
            
        }
        // Go to the next file pointer circularly
        idx = (idx + 1) % p;
    }
}