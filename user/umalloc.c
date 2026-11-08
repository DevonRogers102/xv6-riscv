#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  if (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if (p == SBRK_ERROR)
    return 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}
//page size = 4096, max pages 256


static char *seglist[10];
static char *segpage[256];
static int   segclass[256];
static int   segnpages;

void *
block_alloc(int nbytes)
{
  if(nbytes <= 0 || nbytes > 4096) {
    return 0;
  }

  int size = 8, cls = 0;
  while(size < nbytes){
    size *= 2;
    cls++;
  }

  if(seglist[cls] == 0){
    if(segnpages == 256){
      return 0;
    }

    char *page = sbrk(4096);
    if(page == (char *)-1) {
      return 0;
    }

    uint64 misalign = (uint64)page % 4096;
    if(misalign){
      if(sbrk(4096 - misalign) == (char *)-1) {
        return 0;
      }
      page += 4096 - misalign;
    }

    segpage[segnpages] = page;
    segclass[segnpages] = cls;
    segnpages++;

    for(int off = 0; off < 4096; off += size){
      char *chunk = page + off;
      *(char **)chunk = seglist[cls];
      seglist[cls] = chunk;
    }
  }

  char *chunk = seglist[cls];
  seglist[cls] = *(char **)chunk;
  return chunk;
}

void
block_free(void *ptr)
{
  if(ptr == 0) {
    return;
}
  char *chunk = (char *)ptr;
  char *page = (char *)((uint64)chunk & ~(uint64)(4096 - 1));

  for(int i = 0; i < segnpages; i++){
    if(segpage[i] == page){
      *(char **)chunk = seglist[segclass[i]];
      seglist[segclass[i]] = chunk;
      return;
    }
  }
}

