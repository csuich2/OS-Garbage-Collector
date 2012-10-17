/* getmemgb.c - getmemgb */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getmemgb  --  allocate heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD *getmemgb(unsigned nbytes)
{
	STATWORD ps;
	struct	gcblock *gcptr;
	WORD*	block;

	disable(ps);

	// Allocate a garbage collection block
	gcptr = (struct gcblock *)getmeminternal(sizeof(struct gcblock));
	if (gcptr == (struct gcblock*)SYSERR) {
		restore(ps);
		return (WORD *)SYSERR;
	}
	
	// Allocate the actual block
	block = (WORD*)getmeminternal(nbytes);
	if (block == (WORD*)SYSERR) {
		freememinternal((struct mblock*)gcptr, sizeof(struct gcblock));
		restore(ps);
		return (WORD *)SYSERR;
	}
	
	// Set the values of the garbage collection block
	gcptr->gcbase = block;
	gcptr->gcsize = nbytes;
	gcptr->gcnext = NULL;

	// Insert this garbage collection block into the processes
	// linked list of allocated memory blocks
	//
	// If the list is empty, set the pointer to this block
	if (proctab[currpid].pblocks == (WORD*)NULL) {
		proctab[currpid].pblocks = (WORD *)gcptr;
	// Otherwise, add it to the end
	} else {
		struct gcblock *next = (struct gcblock*)proctab[currpid].pblocks;
		while (next->gcnext != (struct gcblock*)NULL) {
			next = next->gcnext;
		}
		next->gcnext = gcptr;
	}

	restore(ps);
	return block;
}

WORD *getmeminternal(unsigned nbytes)
{
	struct	mblock	*p, *q, *leftover;

	if (nbytes==0 || memlist.mnext== (struct mblock *) NULL) {
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= &memlist,p=memlist.mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			return( (WORD *)p );
		} else if ( p->mlen > nbytes ) {
			kprintf2("q: %d\n", q);
			kprintf2("p: %d\n", p);
			kprintf2("nbytes: %d\n", nbytes);
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			kprintf2("leftover: %d\n", leftover);
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			return( (WORD *)p );
		}
	return( (WORD *)SYSERR );
}
