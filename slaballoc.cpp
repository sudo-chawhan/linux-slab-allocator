#include <bits/stdc++.h>
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

# define SZ 64000

# define SHEADER 500

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

void addSlab(struct slab* ptr, int bucketSize) {
	int fd;
	if ((fd = open("./1mfile", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
	if ((ptr = mmap(NULL, SZ, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		cout << "Memory can't be allocated." << endl;
		exit(1);
	}
	int objSize = (bucketSize + sizeof(struct slab*));
	int index = log2(bucketSize) - 2;
	ptr->buck = &bcktTable[index];
	ptr->slabHeader.totobj = (SZ - SHEADER) / objSize;
	// intially all objects are free
	ptr->slabHeader.freeobj = (SZ - SHEADER) / objSize;
	ptr->slabHeader.nxtSlab = NULL;
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
	int index;
	for(int i=0; i<11; i++) {
		if(bcktTable[i].objSize > size) {
			index = i;
			break;
		}
	}
	if(bcktTable[i].firstSlab == NULL) {
		// Add a slab to the beggining
		addSlab(bcktTable[i].firstSlab, bcktTable[i].objSize);
		bcktTable[i].firstSlab->slabHeader.bitmap[0] = 1;
		bcktTable[i].firstSlab->slabHeader.freeobj--;
		void*  ret_mem= (void *)bcktTable[i].firstSlab->slabHeader.nxtSlab + SHEADER ;
		struct object* alloca_mem = (struct object*) ret_mem;
		alloca_mem->data = i;
		alloca_mem->slbPtr = bcktTable[i].firstSlab;

		return  ret_mem + sizeof(struct slab*);

	} else {
		struct slab* traverse = bcktTable[i].firstSlab;
		while(traverse->slabHeader.nxtSlab && traverse->slabHeader.freeobj == 0) {
			 traverse = traverse->slabHeader.nxtSlab;
		}
		if(!traverse->slabHeader.nxtSlab) {
			// check if any free objects are available
			if(traverse->slabHeader.freeobj) {
				// return the required address using bitmap
				int idx;
				for(int i=0; i<traverse->slabHeader.totobj; i++) {
					if(traverse->slabHeader.bitmap[i] == 0) {
						idx = i;
						break;
					}
				}
				traverse->slabHeader.bitmap[idx] = 1;
				traverse->slabHeader.freeobj--;
				void* ret_mem = (void *)traverse->slabHeader + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*)) ;
				struct object* alloca_mem = (struct object*) ret_mem;
				alloca_mem->data = i;
				alloca_mem->slbPtr = traverse;

				return ret_mem + sizeof(struct slab*);

			} else {
				// Add a new slab at the end of the slab list
				addSlab(traverse->slabHeader.nxtSlab, bcktTable[i].objSize);
				traverse->slabHeader.bitmap[0] = 1;
				traverse->slabHeader.freeobj--;
				void* ret_mem = (void *)traverse->slabHeader.nxtSlab + SHEADER ;
				struct object* alloca_mem = (struct object*) ret_mem;
				alloca_mem->data = i;
				alloca_mem->slbPtr = traverse;

				return ret_mem + sizeof(struct slab*);
			}
		} else {
			// return the required address from the slab
			int idx;
			for(int i=0; i<traverse->slabHeader.totobj; i++) {
				if(traverse->slabHeader.bitmap[i] == 0) {
					idx = i;
					break;
				}
			}
			traverse->slabHeader.bitmap[idx] = 1;
			traverse->slabHeader.freeobj--;
			void* ret_mem = (void *)traverse->slabHeader + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*)) ;
			alloca_mem->data = i;
			alloca_mem->slbPtr = traverse;

			return ret_mem + sizeof(struct slab*);
		}
	}
}

void myfree(void *ptr){
	// struct slab* hdr = (struct slab*)ptr - 1;
	// // how to find the index
	// hdr->slabHeader.bitmap;

	//go to the ptr and use the slab* to go to the slab and update the bitmap
	// increment freeobj
	// if freeobj=totobj, free the slab
		// free using munmap(pointer to header,SZ) 

}

int main() {
	cout<<sizeof(header);
	for(int i=0; i<11; i++) {
		bcktTable[i].objSize = pow(2, i + 2);
		bcktTable[i].firstSlab = NULL;
	}
	int size;		// This is supposed to be given in bytes
	cin >> size;
	// void* p = mymalloc(8);
	// string inputS;
	// cin >> inputS;
	// if(sizeof(inputS) <= size) {	
	// 	// Write to the memory
	// 	void* mymalloc(size)
	// } else {
	// 	cout << "Input size inconsistent." << endl;
	// 	exit(1);
	// }
	void* toAllot = mymalloc(4);
	printf("%d\n",(int*) toAllot );

	// printf("%s\n",(char*)p);
	return 0;

	// sprintf() inputS on the memory location
}
