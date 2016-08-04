#define bare_free_heap (*next - next - *(next+1) - 2)
#define free_heap (*next - next -(int)(*(next+1) * 1.1))
#define heap_size_left heap + hsize - next

void* halloc(void *heap, int s){
	void *next = *heap;
	int hsize= *(heap-1), size = s+2;
	for(;heap_size_left>0;next=*next){
		if(free_heap>=(int)(size*1.3)){
			void *alloc = next + (int)(*(next+1) * 1.1);
			*alloc = *next;
			*next = alloc++;
			*alloc = size;
			return(++alloc);
		}
	}
	int big = 0;
	void* free;
	for(next=*heap;heap_size_left>0;next=*next){
		if(bare_free_heap>big){
			big = bare_free_heap;
			free=next;
		}
	}
	if(big>size){
		void *alloc = (void*) (*next - size);
		*alloc = *next;
		*next = alloc++;
		*alloc = size;
		return(++alloc);
	}
	return NULL;
}
hfree(void *heap, void *alloc){
	
}
