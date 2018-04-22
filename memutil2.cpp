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

# define SZ 64000

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



struct slab* addSlab(int bucketSize) {
    int fd;
    struct slab* ptr;

    // printf("5\n");
    if ((fd = open("./1mfile", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    // printf("6\n");
    // cout << "--------------" << ptr << endl;
    if ((ptr = (struct slab* )mmap(NULL, SZ, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        cout << "Memory can't be allocated." << endl;
        exit(1);
    }
    // cout << ptr << "--------------" << endl;

    // printf("7\n");
    int objSize = (bucketSize + sizeof(struct slab*));
    int index = log2(bucketSize) - 2;
    ptr->slabHeader.buck = &bcktTable[index];

    // printf("b%d\n",bucketSize);
    ptr->slabHeader.totobj = (SZ - SHEADER) / objSize;
    // intially all objects are free

    // printf("8\n");
    ptr->slabHeader.freeobj = (SZ - SHEADER) / objSize;
    ptr->slabHeader.nxtSlab = NULL;

    // printf("9\n");
    // cout << ptr << "--------------" << endl;

    return ptr;
}

void* mymalloc(int size){
    // add slab *
    // find the bucket to put
    // check the first slab for free objects
        // update the bitmap and
        // then return the address of the first free object available
    // else go to the next slab
    // if no slab has free objects then create a new slab and then return the free space
    // and update the bitmap
    int i;
    for(int index=0; index<11; index++) {
        if(bcktTable[index].objSize >= size) {
            i = index;
            break;
        }
    }

    // printf("2 inside myalloc\n");
    // printf("i = %d\n", i);
    if(bcktTable[i].firstSlab == NULL) {

        // printf("3\n");
        // Add a slab to the beggining
        // cout << bcktTable[i].firstSlab << endl;
        bcktTable[i].firstSlab = addSlab(bcktTable[i].objSize);
        // cout << "Returned value " << addSlab(bcktTable[i].objSize) << endl;
        // cout << bcktTable[i].firstSlab << endl;

        bcktTable[i].firstSlab->slabHeader.bitmap[0] = 1;
        
        bcktTable[i].firstSlab->slabHeader.freeobj--;

        void* ret_mem = (void *)((char *)bcktTable[i].firstSlab + SHEADER );
        // cout << ret_mem << endl;
        struct object* alloca_mem = (struct object*) ret_mem;
        alloca_mem->data = 0;
        alloca_mem->slbPtr = bcktTable[i].firstSlab;

        // cout << (char *)(ret_mem + sizeof(struct slab*)) << endl;
        return (void*)((char *)ret_mem + sizeof(struct slab*));

    } else {
        struct slab* traverse = bcktTable[i].firstSlab;
        while(traverse->slabHeader.nxtSlab && traverse->slabHeader.freeobj == 0) {
             traverse = traverse->slabHeader.nxtSlab;
        }
        if(!traverse->slabHeader.nxtSlab) {
            // check if any free objects are available
            if(traverse->slabHeader.freeobj) {
                // return the required address using bitmap
                int idx = 0;
                for(int j=0; j<traverse->slabHeader.totobj; j++) {
                    if(traverse->slabHeader.bitmap[j] == 0) {
                        idx = j;
                        break;
                    }
                }
                traverse->slabHeader.bitmap[idx] = 1;
                traverse->slabHeader.freeobj--;
                void* ret_mem = (void *)((char*)traverse + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*))) ;
                struct object* alloca_mem = (struct object*) ret_mem;
                alloca_mem->data = idx;
                alloca_mem->slbPtr = traverse;

                return (void *)((char*)ret_mem + sizeof(struct slab*));

            } else {
                // Add a new slab at the end of the slab list
                traverse->slabHeader.nxtSlab = addSlab( bcktTable[i].objSize);
                traverse->slabHeader.bitmap[0] = 1;
                traverse->slabHeader.freeobj--;
                void* ret_mem = (void *)((char *)traverse->slabHeader.nxtSlab + SHEADER) ;
                struct object* alloca_mem = (struct object*) ret_mem;
                alloca_mem->data = 0;
                alloca_mem->slbPtr = traverse;

                return (void *)((char *)ret_mem + sizeof(struct slab*));
            }
        } else {
            // return the required address from the slab
            int idx = 0;
            for(int j=0; j<traverse->slabHeader.totobj; j++) {
                if(traverse->slabHeader.bitmap[j] == 0) {
                    idx = j;
                    break;
                }
            }
            traverse->slabHeader.bitmap[idx] = 1;
            traverse->slabHeader.freeobj--;
            void* ret_mem = (void *)((char *)traverse + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*))) ;
            struct object* alloca_mem = (struct object*) ret_mem;
            alloca_mem->data = idx;
            alloca_mem->slbPtr = traverse;

            return (void *)((char *)ret_mem + sizeof(struct slab*));
        }
    }
}

void myfree(void *ptr){
    // cout << "0" << endl;
    struct object* hdr = (struct object*)((char *)ptr - sizeof(struct slab*));
    // how to find the index
    struct slab* parentSlab = hdr->slbPtr;
    // cout << "1" << endl;
    // cout << "data: " << hdr->data << endl;
    // cout << "ptr:" << ptr << endl;
    long int x = ((char *)ptr - sizeof(struct slab*) - (char *)(void *)parentSlab + SHEADER);
    struct bucket* parentBuck = parentSlab->slabHeader.buck; 
    // cout << "x = " << x << endl;
    // cout << "Size of object = \n" << parentSlab->slabHeader.buck->objSize << endl;
    int idx = x / (parentSlab->slabHeader.buck->objSize + sizeof(struct slab*));
    // cout << "idx = " << idx << endl;
    // cout << "2" << endl;
    parentSlab->slabHeader.bitmap[idx]=0;
    // cout << "3" << endl;
    parentSlab->slabHeader.freeobj++;
    hdr->data = -1;
    // cout << "4" << endl;

    if(parentSlab->slabHeader.freeobj == parentSlab->slabHeader.totobj){
        struct slab* iter = parentBuck->firstSlab;
        
        if(iter->slabHeader.nxtSlab == NULL) {
            parentBuck->firstSlab = NULL;
        } else {
            while(iter->slabHeader.nxtSlab != parentSlab){
                iter = iter->slabHeader.nxtSlab;
            }
            iter->slabHeader.nxtSlab = parentSlab->slabHeader.nxtSlab;
        }

        munmap(parentSlab,SZ);
    }
    


    //go to the ptr and use the slab* to go to the slab and update the bitmap
    // increment freeobj
    // if freeobj=totobj, free the slab
        // free using munmap(pointer to header,SZ) 

} 

/*--  --*/

std::mutex mtx;

void threadMain(int threadId, int niterations) {
    // cout << "thread id" << threadId << endl;
    int objList[11];
    for(int i=0; i<11; i++) objList[i] = pow(2, i + 2);

    srand(time(NULL));
    for(int i=0; i<niterations; i++) {
        int size = objList[rand() % 11];

        mtx.lock();

        // cout << "1" << endl;
        // cout << "size  = " << size << endl;

        struct object* toAllot = (struct object*)((char *)mymalloc(size) - sizeof(struct slab*));

        cout << toAllot->data << endl;

        // cout << "2" << endl;

        mtx.unlock();

        // Sleep for small random time
        int sleepTime = rand()%1000 + 1;
        this_thread::sleep_for(chrono::milliseconds(sleepTime));

        myfree((void*)((char *)toAllot + sizeof(struct slab*)));

    }

}

int main(int argc, char const *argv[])
{
    int nthreads, niterations;
    cin >> nthreads >> niterations;
    thread th[nthreads];

    for(int i=0; i<11; i++) {
        bcktTable[i].objSize = pow(2, i + 2);
        bcktTable[i].firstSlab = NULL;
    }

    for(int i=0; i<nthreads; i++) {
        th[i] = std::thread(threadMain,i, niterations);
    }

    for(int i=0; i<nthreads; i++) {
        th[i].join();
    }
    return 0;
}