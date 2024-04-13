#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COP3502C-24Spring 0025
   4/13/2024
   This program is written by: Devon Villalona */

#define MAXLEN 19
#define TABLESIZE 300007

// Structure for storing item details
typedef struct item {
    char name[MAXLEN + 1];
    int quantity;
    int saleprice;
} item;

// Node structure for separate chaining in hash table
typedef struct node {
    item* iPtr;
    struct node* next;
} node;

// Hash table structure
typedef struct hashtable {
    node** lists;
    int size;
} hashtable;

// Global variables for tracking total complexity and total cash
int totalComplexity = 0;
int totalCash = 100000;  // Starting with $100,000

// Hash function to compute the index for a given item name
int hashfunc(char* word, int size) {
    int len = strlen(word);
    int res = 0;
    for (int i = 0; i < len; i++)
        res = (1151 * res + (word[i] - 'a')) % size;
    return res;
}

// Function to initialize the hash table
hashtable* initializeHashTable(int size) {
    hashtable* ht = malloc(sizeof(hashtable));
    ht->size = size;
    ht->lists = malloc(sizeof(node*) * size);
    for (int i = 0; i < size; i++) {
        ht->lists[i] = NULL;
    }
    return ht;
}

// Function to find an existing item or create a new one if not found
item* findOrCreateItem(hashtable* ht, char* name, int* complexity) {
    int index = hashfunc(name, ht->size);
    node* head = ht->lists[index];
    int pos = 1;

    while (head != NULL) {
        if (strcmp(head->iPtr->name, name) == 0) {
            *complexity += pos;
            return head->iPtr;
        }
        head = head->next;
        pos++;
    }

    // Creating a new item if not found
    item* newItem = malloc(sizeof(item));
    strcpy(newItem->name, name);
    newItem->quantity = 0;
    newItem->saleprice = 0;

    node* newNode = malloc(sizeof(node));
    newNode->iPtr = newItem;
    newNode->next = ht->lists[index];
    ht->lists[index] = newNode;

    *complexity += pos;
    return newItem;
}

// Function to process a buy command
void processBuy(hashtable* ht, char* name, int quantity, int totalprice) {
    int complexity = 0;
    item* it = findOrCreateItem(ht, name, &complexity);
    it->quantity += quantity;
    totalCash -= totalprice;
    totalComplexity += complexity;
    printf("%s %d %d\n", name, it->quantity, totalCash);
}

// Function to process a sell command
void processSell(hashtable* ht, char* name, int quantity) {
    int complexity = 0;
    item* it = findOrCreateItem(ht, name, &complexity);
    int sellQuantity = (quantity > it->quantity) ? it->quantity : quantity;
    it->quantity -= sellQuantity;
    totalCash += sellQuantity * it->saleprice;
    totalComplexity += complexity;
    printf("%s %d %d\n", name, it->quantity, totalCash);
}

// Function to process a change_price command
void processChangePrice(hashtable* ht, char* name, int new_price) {
    int complexity = 0;
    item* it = findOrCreateItem(ht, name, &complexity);
    it->saleprice = new_price;
    totalComplexity += complexity;
}

// Function to free a linked list in the hash table
void freeLinkedList(node* head) {
    while (head != NULL) {
        node* temp = head;
        head = head->next;
        free(temp->iPtr);  // Free the item
        free(temp);        // Free the node
    }
}

// Function to free the entire hash table
void freeHashTable(hashtable* ht) {
    for (int i = 0; i < ht->size; i++) {
        freeLinkedList(ht->lists[i]);
    }
    free(ht->lists);  // Free the array of list heads
    free(ht);         // Free the hash table structure
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open input file %s\n", argv[1]);
        return 1;
    }

    int n;
    fscanf(file, "%d", &n);

    hashtable* ht = initializeHashTable(TABLESIZE);

    char command[20], item_name[MAXLEN + 1];
    int quantity, price;

    // Processing each command
    for (int i = 0; i < n; i++) {
        fscanf(file, "%s", command);

        if (strcmp(command, "buy") == 0) {
            fscanf(file, "%s %d %d", item_name, &quantity, &price);
            processBuy(ht, item_name, quantity, price);
        } else if (strcmp(command, "sell") == 0) {
            fscanf(file, "%s %d", item_name, &quantity);
            processSell(ht, item_name, quantity);
        } else if (strcmp(command, "change_price") == 0) {
            fscanf(file, "%s %d", item_name, &price);
            processChangePrice(ht, item_name, price);
        }
    }

    // Outputting final results
    printf("%d\n%d\n", totalCash, totalComplexity);

    // Close the file
    fclose(file);

    // Free memory and clean up
    freeHashTable(ht);
    return 0;
}
