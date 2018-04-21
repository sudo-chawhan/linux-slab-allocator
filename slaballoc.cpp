#include <bits/stdc++.h>

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

struct slab {
	struct header slabHeader;
};

struct bucket {
	int objSize;
	struct slab* firstSlab;
};

bucket bcktTable[11];

void addSlab(struct slab* ptr, int bucketSize) {
	if ((ptr = mmap(NULL, SZ, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		cout << "Memory can't be allocated." << endl;
		exit(1);
	}
	int objSize = (bucketSize + sizeof(struct slab*));
	ptr->slabHeader.totobj = (SZ - SHEADER) / objSize;
	// intially all objects are free
	ptr->slabHeader.freeobj = (SZ - SHEADER) / objSize;
	ptr->slabHeader.nxtSlab = NULL;
}

void* mymalloc(int size){
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
				return (void *)traverse->slabHeader + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*)) + sizeof(struct slab*);
			} else {
				// Add a new slab at the end of the slab list
				addSlab(traverse->slabHeader.nxtSlab, bcktTable[i].objSize);
				traverse->slabHeader.bitmap[0] = 1;
				traverse->slabHeader.freeobj--;
				return (void *)traverse->slabHeader.nxtSlab + SHEADER + sizeof(struct slab*);
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
			return (void *)traverse->slabHeader + SHEADER + idx * (bcktTable[i].objSize + sizeof(struct slab*)) + sizeof(struct slab*);
		}
	}
}

void myfree(void *ptr){
	//go to the ptr and use the slab* to go the slab and update the bitmap
}

int main() {
	for(int i=0; i<11; i++) {
		bcktTable[i].objSize = pow(2, i + 2);
		bcktTable[i].firstSlab = NULL;
	}
	int size;		// This is supposed to be given in bytes
	cin >> size;
	void* p = mymalloc(size);
	string inputS;
	cin >> inputS;
	if(sizeof(inputS) == size) {
		// Write to the memory
	} else {
		cout << "Input size inconsistent." << endl;
	}
	// sprintf() inputS on the memory location
}
