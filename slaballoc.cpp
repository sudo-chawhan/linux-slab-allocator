#include <bits/stdc++.h>

using namespace std;

struct slab {
	int totobj;
	int freeobj;
	struct object
	struct slab* buck;
	struct slab* nxtSlab;
};

struct bucket {
	int objSize;
	struct slab* firstSlab;
};

struct object{
	struct slab* parentSlab;
	int userData;		// will comment it later
}

bucket bcktTable[11];

void addSlab(struct slab* ptr) {
	struct slab 
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
	if(bcktTable.firstSlab == NULL) {
		// Add a slab to the beggining
		addSlab(bcktTable.firstSlab);
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
	struct object* obj = (struct object*)mymalloc(size);
}
