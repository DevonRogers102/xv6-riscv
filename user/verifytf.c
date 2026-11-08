#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAGE 4096
#define MAXBLOCKS 512

char *blocks[MAXBLOCKS];
int
main(void)
{
  int n = 0;

  printf("Process page table has %d frames before allocation.\n\n", trackframes());

  printf("First Allocation:\n");
  blocks[n] = block_alloc(28);
  if(blocks[n] == 0){
    printf("block_alloc failed\n");
    exit(1);
  }
  printf("Allocated 32-byte chunk at %p (page %p)\n", blocks[n], (char *)((uint64)blocks[n] & ~(uint64)(PAGE - 1)));
  n++;

  int frames = trackframes();
  printf("Process page table has %d frames.\n\n", frames);

  int current = frames;
  while(current == frames && n < MAXBLOCKS){
    blocks[n] = block_alloc(28);
    if(blocks[n] == 0){
      printf("block_alloc failed\n");
      break;
    }
    printf("Allocated 32-byte chunk at %p (page %p)\n", blocks[n], (char *)((uint64)blocks[n] & ~(uint64)(PAGE - 1)));
    n++;

    current = trackframes();
    printf("Current page count: %d\n", current);
  }

  printf("\nBlocks allocated: %d\n", n);
  printf("\n================ Beginning block_free ================\n\n");

  for(int i = 0; i < n; i++){
    block_free(blocks[i]);
    printf("Freed 32-byte chunk at %p (page %p)\n",
           blocks[i], (char *)((uint64)blocks[i] & ~(uint64)(PAGE - 1)));
  }
  printf("All blocks freed.\n");
  exit(0);
}
