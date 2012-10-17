/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <mem.h>

void halt();
int main2();
unsigned int availablemem();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	kprintf("sizeof(struct gcblock): %d\n", sizeof(struct gcblock));
	kprintf("available memory: %d\n", availablemem());
	kprintf("main getmem\n");
	char *ptr = getmemgb(sizeof(int)*10);
	kprintf("&ptr: %d\n", &ptr);
	kprintf("main got mem\n");
	resume(create(main2, 2000, 50, "main2", 1, 0));
	kprintf("main getmem2\n");
	char *ptr2 = getmemgb(sizeof(int)*10);
	kprintf("&ptr2: %d\n", &ptr2);
	kprintf("main got mem2\n");
	freememgb((struct mblock*)ptr,sizeof(int)*10);
	freememgb((struct mblock*)ptr2,sizeof(int)*10);
	kprintf("available memory: %d\n", availablemem());
	return 0;
}

int main2()
{
	kprintf("main2 getmem\n");
	char *ptr = getmemgb(sizeof(int)*100);
	kprintf("&ptr: %d\n", &ptr);
	kprintf("main2 got mem\n");
	freememgb((struct mblock*)ptr,sizeof(int)*100);
}

unsigned int availablemem()
{
	unsigned int left = 0;
	struct	mblock	*p;
	for(p=memlist.mnext; p!=(struct mblock*)NULL; p=p->mnext) {
		left += p->mlen;
	}

	return left;
}
