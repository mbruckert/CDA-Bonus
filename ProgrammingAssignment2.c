// Mark Bruckert
// Cache Implementation for CDA 3103

// Necessary Imports
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CACHE_SIZE 32 // Number of lines in the cache
#define CACHE_LINES 8 // Number of lines in each set

// A struct which stores a line for a cache
typedef struct
{
    unsigned int tag;
    int valid;
    int counter;
} CacheLine;

// This function updates the Least Recently Used counter for the given cache line
void updateLRU(CacheLine cache[][CACHE_LINES], int set, int line)
{
    // Increase the counter for all lines in the set except for the current one
    for (int i = 0; i < CACHE_LINES; i++)
    {
        if (i != line)
        {
            cache[set][i].counter++;
        }
        else
        {
            cache[set][i].counter = 0;
        }
    }
}

// This function finds the cache line with the highest Least Recently Used counter
int findLRU(CacheLine cache[][CACHE_LINES], int set)
{
    int max = -1;
    int line = -1;
    for (int i = 0; i < CACHE_LINES; i++)
    {
        if (cache[set][i].counter > max)
        {
            max = cache[set][i].counter;
            line = i;
        }
    }
    return line;
}

int main()
{
    // Read in the file
    FILE *file = fopen("traces.txt", "r");
    // Check if the file was opened successfully
    if (file == NULL)
    {
        printf("Failed to open the traces.txt file\n");
        return 1;
    }

    // Initialize variables
    int accessCount = 0;
    int hits[4] = {0, 0, 0, 0};
    unsigned int address;
    srand(time(NULL));

    // Initialize the caches
    CacheLine directMappedCache[CACHE_SIZE];
    CacheLine twoWaySetAssocCache[CACHE_SIZE / 2][2];
    CacheLine fourWaySetAssocCache[CACHE_SIZE / 4][4];
    CacheLine fullyAssocCache[1][CACHE_SIZE];

    // Initialize the caches to be empty
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        directMappedCache[i].valid = 0;
        // Initialize the 2-way set associative cache to be empty
        if (i < CACHE_SIZE / 2)
        {
            twoWaySetAssocCache[i / 2][i % 2].valid = 0;
        }
        // Initialize the 4-way set associative cache to be empty
        if (i < CACHE_SIZE / 4)
        {
            fourWaySetAssocCache[i / 4][i % 4].valid = 0;
        }
        fullyAssocCache[0][i].valid = 0;
    }

    // Read in the addresses
    while (fscanf(file, "%x", &address) != EOF)
    {
        accessCount++;

        unsigned int tag;
        int set;
        int hit;

        // Direct-mapped cache
        set = (address / 4) % CACHE_SIZE;
        tag = address / (4 * CACHE_SIZE);
        // Check if there was a hit
        if (directMappedCache[set].valid && directMappedCache[set].tag == tag)
        {
            hits[0]++;
        }
        else
        {
            directMappedCache[set].tag = tag;
            directMappedCache[set].valid = 1;
        }

        // 2-way set associative cache
        set = (address / 4) % (CACHE_SIZE / 2);
        tag = address / (4 * CACHE_SIZE / 2);
        hit = 0;
        // Check if there was a hit
        for (int i = 0; i < 2; i++)
        {
            if (twoWaySetAssocCache[set][i].valid && twoWaySetAssocCache[set][i].tag == tag)
            {
                hits[1]++;
                updateLRU(twoWaySetAssocCache, set, i);
                hit = 1;
                break;
            }
        }
        // If there was no hit, replace the LRU line
        if (!hit)
        {
            int line = findLRU(twoWaySetAssocCache, set);
            twoWaySetAssocCache[set][line].tag = tag;
            twoWaySetAssocCache[set][line].valid = 1;
            updateLRU(twoWaySetAssocCache, set, line);
        }

        // 4-way set associative cache
        set = (address / 4) % (CACHE_SIZE / 4);
        tag = address / (4 * CACHE_SIZE / 4);
        hit = 0;
        // Check if there was a hit
        for (int i = 0; i < 4; i++)
        {
            if (fourWaySetAssocCache[set][i].valid && fourWaySetAssocCache[set][i].tag == tag)
            {
                hits[2]++;
                updateLRU(fourWaySetAssocCache, set, i);
                hit = 1;
                break;
            }
        }
        // If there was no hit, replace the LRU line
        if (!hit)
        {
            int line = findLRU(fourWaySetAssocCache, set);
            fourWaySetAssocCache[set][line].tag = tag;
            fourWaySetAssocCache[set][line].valid = 1;
            updateLRU(fourWaySetAssocCache, set, line);
        }

        // Fully associative cache
        set = 0;
        tag = address / 4;
        hit = 0;
        for (int i = 0; i < CACHE_SIZE; i++)
        {
            // Check if there was a hit
            if (fullyAssocCache[set][i].valid && fullyAssocCache[set][i].tag == tag)
            {
                hits[3]++;
                updateLRU(fullyAssocCache, set, i);
                hit = 1;
                break;
            }
        }
        // If there was no hit, replace the LRU line
        if (!hit)
        {
            int line = findLRU(fullyAssocCache, set);
            fullyAssocCache[set][line].tag = tag;
            fullyAssocCache[set][line].valid = 1;
            updateLRU(fullyAssocCache, set, line);
        }
    }

    // Close the file
    fclose(file);

    // Print the results
    printf("Direct-mapped: Hits: %d, Total Accesses: %d, Hit Rate: %f\n", hits[0], accessCount, (float)hits[0] / accessCount);
    printf("2-way set associative: Hits: %d, Total Accesses: %d, Hit Rate: %f\n", hits[1], accessCount, (float)hits[1] / accessCount);
    printf("4-way set associative: Hits: %d, Total Accesses: %d, Hit Rate: %f\n", hits[2], accessCount, (float)hits[2] / accessCount);
    printf("Fully associative: Hits: %d, Total Accesses: %d, Hit Rate: %f\n", hits[3], accessCount, (float)hits[3] / accessCount);

    return 0;
}
