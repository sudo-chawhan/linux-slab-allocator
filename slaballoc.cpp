struct header{
	int totobj;
	int freeobj;
	struct slab* buck;
	struct slab* nxt_slab;

};

struct object{
	slab* parent_slab;
	void 
}

slab *bucket[11];

void* mymalloc(unsigned int size){
	// find the bucket to put
	// check the first slab for free objects
		// update the bitmap and
		// then return the address of the first free object available
	// else go to the next slab
	// if no slab has free objects then create a new slab and then return the free space
	// and update the bitmap
}

void myfree(void *ptr){
	//go to the ptr and use the slab* to go the slab and update the bitmap
}