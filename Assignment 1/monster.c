#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "monster.h"

int getTotalPop(monster** monsters, int nmonsters) {
    int total = 0;

    for (int i=0; i<nmonsters; i++) {
        total += monsters[i]->population;
    }

    return total;
}

monster** getMonsters(monster** newList, char** monsterNames, monster** monsters, int mCount, int size) {
    for (int i=0; i<mCount; i++) {
        for (int j=0; j<size; j++) {
            if (strcmp(monsterNames[j], monsters[i]->name) == 0) {
                newList[j] = monsters[i];
                break;
            }
        }
    }

    return newList;
}
 
region** getRegions(region** visitedRegions, char** rNames, region** regions, int visited, int length) {
    for (int i=0; i<length; i++) {
        for (int j=0; j<visited; j++) {
            if (strcmp(rNames[j], regions[i]->name) == 0) {
                visitedRegions[j] = regions[i];
                break;
            }
        }
    }

    return visitedRegions;
}


monster* createMonster(monster* temp, char* name, char* element, int population) {
    // copy the values for the passed monster*
    strcpy(temp->name, name);
    strcpy(temp->element, element);
    temp->population = population;

    return temp;
}

monster** readMonsters(FILE* infile, monster** monsters, int monsterCount) {
    char name[50], element[50];
    int population;

    // read in the details for every monster
    for (int i=0; i<monsterCount; i++) {
        fscanf(infile, "%s %s %d", name, element, &population);
        monsters[i] = createMonster(monsters[i], name, element, population);
    }

    return monsters;
}


region* createRegion(region* r, char* name, int nmonsters, int totalPop, monster** monsters) {    
    r->name = (char*)malloc(sizeof(char) * 51);
    strcpy(r->name, name);
    r->nmonsters = nmonsters;
    r->monsters = monsters;
    r->total_population = totalPop;

    return r;
}

region** readRegion(FILE* infile, region** regions, int rCount, monster** monsters, int mCount) {
    char temp[50]; // to read redundant strings

    int nmonsters, totalPop;
    monster** monsterList;
    char* name = (char*)malloc(sizeof(char) * 51);

    for (int i=0; i<rCount; i++) {
        fscanf(infile, "%s", name);
        fscanf(infile, "%d %s", &nmonsters, temp);

        // create a the list of names to filter for each region
        char** nameList = (char**)malloc(sizeof(char*) * nmonsters);

        // read in each name
        for (int j=0; j<nmonsters; j++) {
            nameList[j] = (char*)malloc(sizeof(char) * 51);
            fscanf(infile, "%s", nameList[j]);
        }

        // we now allocate and generate the list of monsters in that region
        monsterList = (monster**)malloc(sizeof(monster*) * nmonsters);
        monsterList = getMonsters(monsterList, nameList, monsters, mCount, nmonsters);

        // get total population
        totalPop = getTotalPop(monsterList, nmonsters);

        regions[i] = createRegion(regions[i], name, nmonsters, totalPop, monsterList);

        // free up local pointers
        for (int j=0; j<nmonsters; j++) {
            free(nameList[j]);   
        }
        
        free(nameList);
    }

    free(name);

    return regions;
}


itinerary* createItinerary(itinerary* it, int ncaptures, int regionsVis) {    
    it->captures = ncaptures;
    it->nregions = regionsVis;

    return it;
}


trainer** readTrainer(FILE* input, trainer** trainers, int tCount, region** regions, int rCount) {
    char temp[50];

    for (int i=0; i<tCount; i++) {
        // create local variables for trainer
        char* tName = (char*)malloc(sizeof(char) * 51);
        int captures, rVisited;


        // read in the file info 
        fscanf(input, "%s", tName);

        fscanf(input, "%d %s", &captures, temp);
        fscanf(input, "%d %s", &rVisited, temp);

        char** rNames = (char**)malloc(sizeof(char*) * rVisited);

        for (int j=0; j<rVisited; j++) {
            rNames[j] = (char*)malloc(sizeof(char) * 51);
            fscanf(input, "%s", rNames[j]);
        }

        // resize the regions list
        trainers[i]->visits->regions = (region**)realloc(trainers[i]->visits->regions, sizeof(region*) * rVisited);
        
        // filter the regions and get a list for the trainer
        trainers[i]->visits->regions = getRegions(trainers[i]->visits->regions, rNames, regions, rVisited, rCount);

        // copy the values for the itinerary
        trainers[i]->visits = createItinerary(trainers[i]->visits, captures, rVisited);

        strcpy(trainers[i]->name, tName);

        // free up local pointers
        for (int j=0; j<rVisited; j++) {
            free(rNames[j]);
        }
        
        free(rNames);
        free(tName);
    }

    fclose(input);

    return trainers;
}


void freeMemory(monster** monsters, int mCount, region** regions, int rCount, trainer** trainers, int tCount) {
    freeMonsters(monsters, mCount);
    
    freeRegions(regions, rCount);

    freeTrainers(trainers, tCount);
}

void freeMonsters(monster** m, int mCount) {
    // free name, element type, and the indexed monster
    for (int i=0; i<mCount; i++) {
        free(m[i]->name);
        free(m[i]->element);
        free(m[i]);
    }

    free(m);
}

void freeRegions(region** r, int rCount) {
    for (int i=0; i<rCount; i++) {
        free(r[i]->name);

        // since all monsters are freed, we can just free the indexes and its list
        for (int j=0; j<r[i]->nmonsters; j++) {
            free(r[i]->monsters[j]);
        }
        
        free(r[i]->monsters);
        free(r[i]);
    }

    free(r);
}

void freeTrainers(trainer** trainers, int tCount) {
    for (int i=0; i<tCount; i++) {
        free(trainers[i]->name);

        // we can free the regions since everything else is freed already
        for (int j=0; j<trainers[i]->visits->nregions; j++) {
            free(trainers[i]->visits->regions[j]);
        }

        free(trainers[i]->visits->regions);
        free(trainers[i]->visits);
        free(trainers[i]);
    }

    free(trainers);
}


void calculateCatches(trainer** trainers, int tCount) {
    FILE* output = fopen("out.txt", "w");

    for (int i=0; i<tCount; i++) { //trainer length
        fprintf(output, "%s\n", trainers[i]->name);

        for (int j=0; j<trainers[i]->visits->nregions; j++) { //region length
            fprintf(output, "%s\n", trainers[i]->visits->regions[j]->name);
            int totalPop = trainers[i]->visits->regions[j]->total_population;

            for (int k=0; k<trainers[i]->visits->regions[j]->nmonsters; k++) { //monsters length
                float percentage =  trainers[i]->visits->captures * (trainers[i]->visits->regions[j]->monsters[k]->population / (float)totalPop);

                int catches = round(percentage);

                // ensures that we print monsters that have been captured
                if (catches > 0) {
                    fprintf(output, "%d %s\n", catches, trainers[i]->visits->regions[j]->monsters[k]->name);
                }
            }
        }

        // makes sure we don't print out an extra newline
        if  (i != tCount - 1) {
            fprintf(output, "\n");
        } 
    }

    fclose(output);
}