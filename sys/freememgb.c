/* freememgb.c - freememgb */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  freememgb  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL freememgb(struct mblock *block, unsigned size)
{
	STATWORD ps;
	struct	gcblock *p, *q;
	int	retVal = OK;

	disable(ps);
	// Free the garbage collection block
	p = (struct gcblock*)proctab[currpid].pblocks;
	q = (struct gcblock*)NULL;
	if (p == (struct gcblock*)NULL) {
		restore(ps);
		return SYSERR;
	} else {
		//kprintf2("block: %d\n", (WORD*)block);
		//kprintf2("p: %d\n", (WORD*)p->gcbase);
		while (p != (struct gcblock*)NULL && p->gcbase != (WORD*)block) {
			q = p;
			p = p->gcnext;
			//kprintf2("p: %d\n", (WORD*)p->gcbase);
		}
		if (p == (struct gcblock*)NULL) {
			//kprintf2("gcblock not found for freememgb call!!!!!!\n");
			restore(ps);
			return SYSERR;
		} else {
			if (q == (struct gcblock*)NULL) {
				proctab[currpid].pblocks = (WORD*)p->gcnext;
			} else {
				q->gcnext = p->gcnext;
			}
			if (p->gcsize != roundmb(size)) {
				restore(ps);
				return SYSERR;
			}
			if (freememinternal((struct mblock*)p, sizeof(struct gcblock)) == SYSERR) {
				restore(ps);
				return SYSERR;
			}
		}
	}

	// Free the actual block
	if (freememinternal(block, size) == SYSERR) {
		retVal = SYSERR;
	}

	restore(ps);
	return retVal;
}
int freememinternal(struct mblock *block, unsigned size)
{
	struct	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end))
		return(SYSERR);
	size = (unsigned)roundmb(size);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	return(OK);
}
