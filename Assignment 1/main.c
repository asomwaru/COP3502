#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monster.h"
#include "leak_detector_c.h"

int main() {
    atexit(report_mem_leak);

    FILE* input;
    int mCount, rCount, tCount;
    char temp[50]; // used to read the unneeded strings

    input = fopen("in.txt", "r");

    fscanf(input, "%d %s", &mCount, temp); // read in  mCount

    // allocate memory for list of monsters
    monster** monsters = (monster**)calloc(mCount, sizeof(monster*));
    for (int i=0; i<mCount; i++) {
        monsters[i] = (monster*)malloc(sizeof(monster));
        monsters[i]->name = (char*)malloc(sizeof(char) * 51);
        monsters[i]->element = (char*)malloc(sizeof(char) * 51);
    }

    monsters = readMonsters(input, monsters, mCount);

    // Read in the number of regions and allocate memory for those regions
    fscanf(input, "%d %s", &rCount, temp);

    region** regions = (region**)calloc(rCount, sizeof(region*));
    for (int i=0; i<rCount; i++) {
        regions[i] = (region*)malloc(sizeof(region));
    }

    regions = readRegion(input, regions, rCount, monsters, mCount);

    // Same thing as before but for trainers
    fscanf(input, "%d %s", &tCount, temp);

    trainer** trainers = (trainer**)malloc(sizeof(trainer*) * tCount);
    for (int i=0; i<tCount; i++) {
        trainers[i] = (trainer*)malloc(sizeof(trainer*));
        trainers[i]->name = (char*)malloc(sizeof(char) * 51);

        trainers[i]->visits = (itinerary*)malloc(sizeof(itinerary));

        // Temporarily give the regions a size of rCount but resize it while reading the trainer info
        trainers[i]->visits->regions = (region**)malloc(sizeof(region*) * rCount);
    }

    trainers = readTrainer(input, trainers, tCount, regions, rCount);

    calculateCatches(trainers, tCount); // get the needed output and next we free the memory

    freeMemory(monsters, mCount, regions, rCount, trainers, tCount);

    return 0;
}
