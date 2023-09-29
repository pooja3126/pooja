

/*C program to implement a hashtable .The key and value stored are string values*/


#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Structure to represent a key-value pair
struct KeyValue {
    char *key;
    char *value;
    struct KeyValue *next;
};

// Structure to represent the hash table
struct HashTable {
    struct KeyValue *table[TABLE_SIZE];
};

// Function to create a new key-value pair
struct KeyValue *createKeyValue(const char *key, const char *value) {
    struct KeyValue *kv = (struct KeyValue *)malloc(sizeof(struct KeyValue));
    if (kv == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    kv->key = strdup(key);
    kv->value = strdup(value);
    kv->next = NULL;
    return kv;
}

// Function to calculate the hash value for a given key
unsigned int hash(const char *key) {
    unsigned int hashval = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hashval = key[i] + (hashval << 5) - hashval;
    }
    return hashval % TABLE_SIZE;
}

// Function to insert a key-value pair into the hash table
void insert(struct HashTable *ht, const char *key, const char *value) {
    unsigned int index = hash(key);
    struct KeyValue *newPair = createKeyValue(key, value);

    if (ht->table[index] == NULL) {
        // No collision, insert at the head of the list
        ht->table[index] = newPair;
    } else {
        // Collision detected, add to the end of the list
        struct KeyValue *current = ht->table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPair;
    }
}

// Function to search for a value using a key
char *search(struct HashTable *ht, const char *key) {
    unsigned int index = hash(key);
    struct KeyValue *current = ht->table[index];
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL; // Key not found
}

// Function to delete a key-value pair from the hash table
void deleteKey(struct HashTable *ht, const char *key) {
    unsigned int index = hash(key);
    struct KeyValue *current = ht->table[index];
    struct KeyValue *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Key found, delete the pair
            if (prev == NULL) {
                ht->table[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

int main() {
    struct HashTable ht;
    memset(ht.table, 0, sizeof(ht.table)); // Initialize the hash table

    insert(&ht, "name", "Pooja");
    insert(&ht, "age", "30");
    insert(&ht, "city", "Bangalore");

    // Delete the "age" key-value pair
    deleteKey(&ht, "age");

    // Search for values
    printf("Name: %s\n", search(&ht, "name"));
    printf("Age: %s\n", search(&ht, "age"));
    printf("City: %s\n", search(&ht, "city"));

    return 0;
}
