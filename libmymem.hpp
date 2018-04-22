# define pragma once

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

# define SZ 64*1024

# define SHEADER 2024

struct header {
    int totobj;
    int freeobj;
    bitset<16000> bitmap;
    struct bucket* buck;
    struct slab* nxtSlab;
};

struct object{
    struct slab* slbPtr;
    int data;
};

struct slab {
    struct header slabHeader;
};

struct bucket {
    int objSize;
    struct slab* firstSlab;
};

bucket bcktTable[11];

struct slab* addSlab(int bucketSize);

void* mymalloc(int size);

void myfree(void *ptr);

