/*
    File: my_allocator.c

    Author: Devin Carr, Spencer Rawls
            Department of Computer Science
            Texas A&M University
    Date  : 10/11/15

    Modified:

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */

template <class T> T min(const T& a, const T& b) {
  return a<b ? a : b;
}


bool Allocator::init_allocator(unsigned int _basic_block_size, unsigned int _length) {
  basic_block_size = _basic_block_size;
  mem_pool_length = _length;
  mem_pool = (Addr)malloc(_length);

  num_tiers = round(log2(_length/_basic_block_size))+1;
  free_list = (Block**)malloc(num_tiers);
  memset(free_list, 0, num_tiers*sizeof(Block*));

  free_list[num_tiers-1] = (Block*)mem_pool;
  free_list[num_tiers-1]->tier = num_tiers-1;
  // std::cout << "Mem init check: " << (mem_pool != nullptr && free_list != nullptr) << std::endl;
  // print_free();

  return mem_pool != nullptr && free_list != nullptr;
}

int Allocator::release_allocator() {
  free(mem_pool);
  free(free_list);
  return mem_pool == NULL && free_list == NULL;
}

Addr Allocator::my_malloc(unsigned int _length) {
  int tier = round(ceil(log2((double)(_length+sizeof(Block))/basic_block_size)));
  if (tier < 0) tier = 0;
  //std::cout << "Tier: " << tier << " Num accessable tiers: " << num_tiers <<std::endl;
  if (tier >= num_tiers) {
    // the size is too large for the allocated memory
    std::cerr << "Size is too large: returning nullptr." << std::endl;
    return nullptr;
  }

  Block* free_node = free_list[tier];

  // Fetch an empty block
  if (free_node == nullptr) {
    free_node = (Block*)find_block(tier, 1);
  }
  // Pass block
  if (free_node != nullptr) {
    free_list[tier] = free_node->next;
    free_node->tier = tier;
    free_node->occupied = true;
    // print_mem();
    // print_free();
    //std::cout << free_node << std::endl;
    return (Addr)(free_node+1);
  } else {
    // Error: Not enough space for the block
    std::cerr << "Not Enough Space for block: returning nullptr." << std::endl;
    return nullptr;
  }
}

Addr Allocator::find_block(int original_tier, int increment_total) {
  int i = original_tier + increment_total;
  if (i >= num_tiers || i <= 0) {
    std::cerr << "Incorrect Space!\n num_tiers: " << num_tiers << " i: " << i << std::endl;
    return nullptr;
  } else {
    if (free_list[i] == nullptr) {
      // go up a tier to check for space
      return find_block(original_tier,increment_total+1);
    } else {
      // free block found!
      return split(original_tier, increment_total);
    }
  }
}

Addr Allocator::split(int start, int remaining) {
  int i = start + remaining;
  if (remaining < 1) {
    return (Addr)free_list[start];
  }
  // get the right half of the block
  Block* right = (Block*)((char*)free_list[i] + (basic_block_size<<(i-1)));
  // set it's next to the current free_list block
  right->next = free_list[i-1];
  right->tier = i-1;
  // set the tier-1 free_list to the right (half) block
  free_list[i-1] = right;
  // get the left half of the block
  Block* left = free_list[i];
  // set the current free_list node to the next free_block
  free_list[i] = free_list[i]->next;
  // set it's next to the current free_list block
  left->next = free_list[i-1];
  right->tier = i-1;
  // set the tier-1 free_list to the left (half) block
  free_list[i-1] = left;
  return split(start, remaining-1);
}

int Allocator::my_free(Addr _a) {
  Block* b = ((Block*)_a)-1;
  b->occupied = false;
  int tier = ((Block*)b)->tier;
  b->next = free_list[tier];
  free_list[tier] = b;
  merge(b);
  // std::cout << "freeing" << std::endl;
  // print_free();
  //print_mem();
  return 0;
}

void Allocator::merge(Block* a) {

  Block* b = (Block*)get_buddy(a);

  int tier = a->tier;
  if (!(a->occupied) && !(b->occupied) && tier == b->tier && tier < num_tiers) {

    // Remove a and b from current list tier
    if (free_list[tier] == a) {
      free_list[tier] = free_list[tier]->next;
    }
    if (free_list[tier] == b) {
      free_list[tier] = free_list[tier]->next;
    }
    for (Block* n = free_list[tier]; n != nullptr; n = n->next) {
      Block* next = n->next;
      if (next == a || next == b) {
        n->next = next->next;
      }
    }
    // Add new block [ab] to list tier+1
    Block* newblock = (Block*)min((long)a, (long)b);
    newblock->tier = newblock->tier+1;
    newblock->next = free_list[tier+1];
    free_list[tier+1] = newblock;
    merge(newblock);
  }
}

Addr Allocator::get_buddy(Addr a) {
  long rela = (long)a - (long)mem_pool;
  long thing = basic_block_size<<(((Block*)a)->tier);
  long buddy = rela^thing;
  Addr b = (Addr)(buddy + (long)mem_pool);
  return b;
}

void Allocator::print_mem() const {
  char* temp0 = (char*)mem_pool;
  std::cout << "mem_pool [";
  while ((long)temp0 < (long)mem_pool+mem_pool_length) {
    Block* temp = (Block*)temp0;
    int size = basic_block_size<<(temp->tier);
    int occupied = temp->occupied;
    std::cout << occupied << std::setfill('-') << std::setw(size/basic_block_size);
    temp0 = temp0 + size;
  }
  std::cout << "]" << std::endl;
}

void Allocator::print_free() const {
  std::cout << "free_list" << std::endl
            << "(tier): [xxxx] " << std::endl;
  for (int i = 0; i < num_tiers; i++) {
    std::cout << i << ": [ ";
    Block* temp=free_list[i];
    if (temp != nullptr) {
      std::cout << temp << " -> " << temp->next;
      temp = temp->next;
    } else {
      std::cout << "nil";
    }
    /*for (; temp != nullptr; temp = temp->next) {
      temp = free_list[i];
      if (temp != nullptr) {
        if (temp->next != nullptr) {
          while (temp != nullptr) {
            std::cout << temp->occupied;
            temp = temp->next;
          }
        } else {
          std::cout << temp->occupied;
        }
      } else {
        std::cout << "nil";
      }
      std:: cout << " -> " << temp;
    }*/
    std::cout << " ]" << std::endl;
  }
}
