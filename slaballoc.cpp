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
using namespace std;
#define slabSize 32

struct bucket
{
    int objectSize;
    struct slab* firstSlab;
};

struct bucket buckets[11];

struct object
{
    struct slab* slabptr;
    int data;
};

struct slab
{
    int totalObj;
    int freeObj;
    bitset <16000> bitmap;
    struct bucket* bucketPtr;
    struct slab* nextSlab;
    void* offset;
};


struct slab* slabCreator(unsigned size)
{
    int fd;
    int SZ = size;
    if ((fd = open("./1mfile", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
    void* ptr1;
    if ((ptr1 = mmap(NULL, SZ, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    void* ptrTraverse = ptr1;
    struct slab* ptr = (struct slab*)ptr1;

    int sizeheader = sizeof(ptr->totalObj) + sizeof(ptr->freeObj) + sizeof(ptr->bitmap) + sizeof(ptr->bucketPtr) + sizeof(ptr->nextSlab) 
                        + sizeof(ptr->offset);

    cout << "The header" << sizeheader << endl;

    ptr -> totalObj = ((4096-sizeheader)/(8+size));

    ptrTraverse += sizeof(ptr->totalObj);
    ptr -> freeObj = ptr -> totalObj;
    ptrTraverse += sizeof(ptr->freeObj);

    for(int i = 0; i < ptr -> totalObj; i++)
        ptr -> bitmap[i] = 0;

    ptrTraverse += sizeof(ptr->bitmap);
    ptrTraverse += sizeof(ptr->bucketPtr);
    ptrTraverse += sizeof(ptr->nextSlab) + sizeof(ptr->offset);

    ptr -> nextSlab = NULL;

    ptr-> offset = ptrTraverse;

    return ptr;
}

void* mymalloc(unsigned size)
{
    int index = log2(size) - 2;
    int check = 1;
    if(buckets[index].firstSlab == NULL)
    {
        buckets[index].firstSlab = slabCreator(size);
    }
    struct slab* current = buckets[index].firstSlab;
    do
    {
        int i = 0;
        void* allocationPoint;
        while(current -> bitmap[i] == 1)
            i++;
        cout << "postion" << i << endl;
        cout << "current" << current -> totalObj - 1 << endl;
        if(i == current -> totalObj)
        {
            if(current -> nextSlab == NULL)
            {
                current -> nextSlab = slabCreator(size);
            }
                current = current -> nextSlab;
        }
        else
        {
            current -> bitmap[i] = 1;
            current -> freeObj--;
            check = 0;
            allocationPoint = current->offset + i * (sizeof(struct slab*)+size);
            struct object newObj;
            newObj.data = i;
            newObj.slabptr = current;
            cout << " i = " << i << endl;
            void* temp = allocationPoint;
            
            cout << temp << endl;
            sprintf((char*)temp, "%d", i);
            cout << "hi"<< endl;
            //current -> objects[i].data = i;
            return (temp);
        }

    }while(check == 1);
}
int main()
{


    for(int i = 0; i < 11; i++)
    {
        buckets[i].objectSize = pow(2, i + 2);
        buckets[i].firstSlab = NULL;
    }
    while(true)
    {
        int size;
        cin >> size;
        printf("%s hey",(char*)mymalloc(size));
        //struct object* ptr = (struct object*)mymalloc(size);
        //printf("%s   ", (char*)mymalloc(size));
        //cout << ptr -> data;
    }
    return 0;
}
