/*
    File: my_allocator.h

    Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified: 10/11/15
							Devin Carr, Spencer Rawls

*/

#ifndef _my_allocator_h_                   // include file only once
#define _my_allocator_h_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include <cmath>
#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef void * Addr;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MODULE   MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/*
 * Created a struct that allows the memory to be prefaced by this header.
 * This allows the memory to know what blocks to merge back together
 * and generate a free_list with much more ease as the free blocks are chainable.
*/
struct Block {
	Block* next = nullptr;
	unsigned int tier;
	bool occupied = false;
	Block() : next(nullptr), occupied(false) {}
};
/*
 * Also the conversion from c -> c++ allows us to move the
 * relaese into the deconstructor
 */
class Allocator {
	unsigned int basic_block_size;
	unsigned int num_tiers;
	unsigned int mem_pool_length;
	Addr mem_pool = nullptr;
	Block** free_list = nullptr;
public:
	Allocator() {}
	Allocator(unsigned int b, unsigned int l) { init_allocator(b, l); }
	~Allocator() { release_allocator(); }

	bool init_allocator(unsigned int _basic_block_size,
				    unsigned int _length);
	/* This function initializes the memory allocator and makes a portion of
	   ’_length’ bytes available. The allocator uses a ’_basic_block_size’ as
	   its minimal unit of allocation. The function returns the amount of
	   memory made available to the allocator. If an error occurred,
	   it returns 0.
	*/

	int release_allocator();
	/* This function returns any allocated memory to the operating system.
	   After this function is called, any allocation fails.
	*/

	Addr my_malloc(unsigned int _length);
	/* Allocate _length number of bytes of free memory and returns the
	   address of the allocated portion. Returns 0 when out of memory. */

	int my_free(Addr _a);
	/* Frees the section of physical memory previously allocated
	   using ’my_malloc’. Returns 0 if everything ok. */

	Addr find_block(int original_tier, int increment_total);
	Addr split(int start, int remaining);
	void merge(Block* a);
	Addr get_buddy(Addr a);
	void print_mem() const;
	void print_free() const;
};



#endif
