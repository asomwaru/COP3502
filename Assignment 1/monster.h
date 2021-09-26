typedef struct monster {
    char *name;
    char *element;
    int population;
} monster;

typedef struct region {
    char *name;
    int nmonsters;
    int total_population;
    monster **monsters;
} region;

typedef struct itinerary {
    int nregions;
    region **regions;
    int captures;
} itinerary;

typedef struct trainer {
    char *name;
    itinerary *visits;

} trainer;

// Helper functions
int getTotalPop(monster** monsters, int nmonsters);
monster** getMonsters(monster** newList, char** monsterNames, monster** monsters, int mCount, int size);
region** getRegions(region** visitedRegions, char** rNames, region** regions, int visited, int length);
void freeMemory(monster** monsters, int mCount, region** regions, int rCount, trainer** trainers, int tCount);

// Monster create and read functions
monster* createMonster(monster* temp, char* name, char* element, int population);
monster** readMonsters(FILE* infile, monster** monsters, int monsterCount);
void freeMonsters(monster** m, int mCount);

// Region create and read functions
region* createRegion(region* r, char* name, int nmonsters, int totalPop, monster** monsters);
region** readRegion(FILE* infile, region** regions, int rCount, monster** monsters, int mCount);
void freeRegions(region** r, int rCount);

// Itinerary functions
itinerary* createItinerary(itinerary* it, int ncaptures, int regionsVis);

// Trainer function functions
trainer* createTrainer(trainer* t, char* name, itinerary* it);
trainer** readTrainer(FILE* input, trainer** trainers, int tCount, region** regions, int rCount);
void freeTrainers(trainer** trainers, int tCount);

// Gets the main output for the assignment
void calculateCatches(trainer** trainers, int tCount);