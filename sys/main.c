/* main.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <mem.h>

void halt();
extern SYSCALL freememgb(struct mblock* block, unsigned size);
extern WORD* getmemgb(unsigned nbytes);

// Function prototypes
// Utility methods
unsigned get_free_memsize();
void assert(Bool condition, char* message);

// Single process test cases
void single_allocate_and_free_test();
void single_allocate_and_not_free_test();
void single_free_without_allocating_test();
void single_allocate_and_suicide_test();
void single_allocate_and_free_and_suicide_test();
void allocate_and_free_invalid_address_length_test();
void allocate_n_times_and_free_m_test();

// Multiple process allocate/free
void allocate_in_main_free_in_another();

// Allocation/freeing scenarios
void allocate_and_free(char *name, int numblocks);
void allocate_and_not_free(char *name, int numblocks);
void free_without_allocating(char *name, int numblocks);
void allocate_and_suicide(char *name, int numblocks);
void allocate_and_free_and_suicide(char *name, int numblocks);
void allocate_and_free_invalid_address(char *name, int numblocks, int twoallocations);
void allocate_and_free_invalid_length(char *name, int numblocks, int twoallocations);
void free_mem(char* name, int * ptr, int numblocks);
void allocate_n_times_and_free_m(char* name, int total_blks, int blks_to_free, int blk_size);


/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
        single_allocate_and_free_test();
        single_allocate_and_not_free_test();
        single_free_without_allocating_test();
        single_allocate_and_suicide_test();
        single_allocate_and_free_and_suicide_test();

	allocate_and_free_invalid_address_length_test();
	allocate_in_main_free_in_another();
	allocate_n_times_and_free_m_test();

	return 0;
}

// Single process test cases
void single_allocate_and_free_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : single_allocate_and_free\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));

        resume(proc = create(allocate_and_free, 2000, 20, "A", 2, 
                                                        "A", 50));
        
        kill(proc);
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0,
                "Test failed\n");
}

void single_allocate_and_not_free_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : single_allocate_and_not_free\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));

        resume(proc = create(allocate_and_not_free, 2000, 20, "A", 2, 
                                                        "A", 50));

        kill(proc);        
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0,
                "Test failed\n");
}

void single_free_without_allocating_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : single_free_witout_allocating\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(free_without_allocating, 2000, 20, "A", 2, 
                                                        "A", 50));

        kill(proc);
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "Test failed\n");
}

void single_allocate_and_suicide_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : single_allocate_and_suicide\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_suicide, 2000, 20, "A", 2, 
                                                        "A", 50));
        
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "Test failed\n");
}

void single_allocate_and_free_and_suicide_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : single_allocate_and_free_and_suicide\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_free_and_suicide, 2000, 20, "A", 2, 
                                                        "A", 50));
        
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "Test failed\n");
}

void allocate_and_free_invalid_address_length_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : allocate_and_free_invalid_address_length_test\n");
        kprintf("\tTest1 : Free invalid address \n");
        kprintf("\tBefore termination : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_free_invalid_address, 2000, 20, "A", 3, 
                                                        "A", 50, FALSE));
        
        kprintf("\tAfter termination : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "\tTest failed\n");


        kprintf("\n\tTest2 : Free invalid length \n");
	kprintf("\tBefore termination : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_free_invalid_length, 2000, 20, "A", 3, 
                                                        "A", 50, FALSE));
        
        kprintf("\tAfter termination : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "\tTest failed\n");

        kprintf("\tTest3 : Free invalid address (with two allocations)\n");
        kprintf("\tBefore termination : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_free_invalid_address, 2000, 20, "A", 3, 
                                                        "A", 50, TRUE));
        
        kprintf("\tAfter termination : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "\tTest failed\n");


        kprintf("\n\tTest4 : Free invalid length (with two allocations)\n");
	kprintf("\tBefore termination : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));
        
        resume(proc = create(allocate_and_free_invalid_length, 2000, 20, "A", 3, 
                                                        "A", 50, TRUE));
        
        kprintf("\tAfter termination : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0, 
                "\tTest failed\n");
}

void allocate_in_main_free_in_another()
{
	int proc;
	int* ptr;
        unsigned pre_memsize, post_memsize;

        kprintf("\nTest : allocate_in_main_free_in_another\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));

	ptr = (int *)getmemgb(50*sizeof(int));

        resume(proc = create(free_mem, 2000, 20, "A", 3, 
                                         	"A", ptr, 50));

        freememgb(ptr, 50*sizeof(int));        
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0,
                "Test failed\n");

}

void allocate_n_times_and_free_m_test()
{
        int proc;
        unsigned pre_memsize, post_memsize;
	int n = 1000;
	int m = 21;
	int blk_size = 500;
	//int blk_size = 10000; 	// NOTE: Try with large block size also

        kprintf("\nTest : allocate_n_times_and_free_m_test\n");
        kprintf("Before : Free mem size = %ld\n", 
                (pre_memsize = get_free_memsize()));

        resume(proc = create(allocate_n_times_and_free_m, 2000, 20, "A", 4, 
                                                        "A", n, m, blk_size));
       
	sleep(7); 
        kill(proc);
        kprintf("After : Free mem size = %ld\n", 
                (post_memsize = get_free_memsize()));

        assert((post_memsize - pre_memsize) == 0,
                "Test failed\n");

}

// Memory allocation and freeing scenarios
void allocate_and_free(char* name, int numblocks)
{
        int* ptr;
        kprintf("Process %s\n", name);

        ptr = (int *)getmemgb(numblocks*sizeof(int));

        freememgb(ptr, numblocks*sizeof(int));
}

void allocate_and_not_free(char* name, int numblocks)
{
        int* ptr;

        kprintf("Process %s\n", name);

        ptr = (int *)getmemgb(numblocks*sizeof(int));
}

void free_without_allocating(char* name, int numblocks)
{
        int* ptr;

        kprintf("Process %s\n", name);

        // TODO: What happens when free is called with a size greater than that 
        //      originally allocated to ptr?
        freememgb(ptr, numblocks*sizeof(int));
}

void allocate_and_suicide(char *name, int numblocks)
{
        int* ptr;

        kprintf("Process %s\n", name);

        ptr = (int *)getmemgb(numblocks*sizeof(int));

        kill(currpid);
}

void allocate_and_free_and_suicide(char *name, int numblocks)
{
        int* ptr;

        kprintf("Process %s\n", name);

        ptr = (int *)getmemgb(numblocks*sizeof(int));

        freememgb(ptr, numblocks*sizeof(int));

        kill(currpid);
}


void allocate_and_free_invalid_address(char *name, int numblocks, int twoallocations)
{
        int* ptr;
        kprintf("\tProcess %s\n", name);
	if (twoallocations) {
		getmemgb(numblocks*sizeof(int));
	}
        ptr = (int *)getmemgb(numblocks*sizeof(int));
        if(freememgb(ptr+1, numblocks*sizeof(int)) == SYSERR) {
        
		kprintf("\tCorrect Behavior on passing invalid address to freememgb. Got SYSERR. Expected SYSERR. \n");
	} else {

		kprintf("\tIncorrect Behavior on passing invalid address to freememgb. Got OK. Expected SYSERR. \n");
	}
}

void allocate_and_free_invalid_length(char *name, int numblocks, int twoallocations)
{
        int* ptr;
        kprintf("\tProcess %s\n", name);
	if (twoallocations) {
		getmemgb(numblocks*sizeof(int));
	}
        ptr = (int *)getmemgb(numblocks*sizeof(int));
        if(freememgb(ptr, numblocks*sizeof(int) - 1) == SYSERR) {
        
		kprintf("\tCorrect Behavior on passing invalid address to freememgb. Got SYSERR. Expected SYSERR. \n");
	} else {

		kprintf("\tIncorrect Behavior on passing invalid address to freememgb. Got OK. Expected SYSERR. \n");
	}
}

void free_mem(char* name, int * ptr, int numblocks)
{
        kprintf("Process %s\n", name);
        if (freememgb(ptr, numblocks*sizeof(int)) == SYSERR) {
		
		kprintf("Correct Behavior. Tried deleting block belonging to another process. Got SYSERR. Expected SYSERR. \n");
	} else {
		
		kprintf("Incorrect Behavior. Tried deleting block belonging to another process. Got OK. Expected SYSERR. \n");
	}
}

void allocate_n_times_and_free_m(char* name, int total_blks, int blks_to_free, int blk_size)
{
	int i;
	int* ptr;
        kprintf("Process %s \t Block size: %d\n", name, blk_size);
	
	int ** free_block_arr = getmemgb(blks_to_free * sizeof(int *));
	for (i=0; i<blks_to_free; i++) 
	{
		*(free_block_arr + i) = 0;
	}
	
	for (i=0; i<total_blks; i++) 
	{
		*(free_block_arr + (i%blks_to_free)) = (int *) getmemgb(blk_size * sizeof(int));
		if (*(free_block_arr + (i%blks_to_free)) == SYSERR) {
			*(free_block_arr + (i%blks_to_free)) = 0;
		}
	}

	for (i=0; i<blks_to_free; i++) 
	{
		if (freememgb(*(free_block_arr + i), blk_size*sizeof(int)) == SYSERR 
			&& *(free_block_arr + i) != 0) { 
			kprintf("\t Error! Could not free valid block ! \n");
		} else {
			if(*(free_block_arr + i) != 0)
				kprintf("\t Block Freed ! \n");
			else 
				kprintf("\t Null Block. Not Freed ! \n");
		}
	}
	kprintf("In allocate_n_times_and_free_m : Free mem size = %ld\n", 
                get_free_memsize());

}

// Utility methods
unsigned get_free_memsize()
{
        struct mblock *mptr;
        unsigned size = 0;

        for(mptr = memlist.mnext; 
                mptr != (struct mblock*)NULL; 
                mptr = mptr->mnext)
        {
                size += mptr->mlen;
        }

        return size;
}

void assert(Bool condition, char* message)
{
        if(condition == FALSE)
        {
                kprintf("%s\n", message);
        }
}
