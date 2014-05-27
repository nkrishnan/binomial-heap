
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "BINOMIAL_HEAP.h"

struct NUM {
  int number;
  BINOMIAL_HEAP::NODE * heap_ptr;
};

void reset_p( void * v, BINOMIAL_HEAP::NODE * new_node ) {
  struct NUM *n = *(struct NUM **) v;
  n->heap_ptr = new_node;
}

int compare_num( const void * n1, const void * n2 ) {
  struct NUM *v1 = *(struct NUM **) n1;
  struct NUM *v2 = *(struct NUM **) n2;
  if( v1->number == v2->number ) return 0;
  return (v1->number < v2->number) ? -1 : 1;
}

void print_num( void const * v ) {
  struct NUM *n = *(struct NUM **) v;
  printf( "%d ", n->number );
}

int main( int argc, char * argv[] ) {
  NUM * min_value = (NUM *) calloc(1, sizeof(NUM));
  min_value->number = -1;
  int start = (argc >= 2) ? atoi(argv[1]) : 0;
  int end = (argc >= 3) ? atoi(argv[2]) : 1000000;
  if( start > end ) {
    int tmp = start;
    start = end;
    end = tmp;
  }

  BINOMIAL_HEAP::HANDLE * bh = BINOMIAL_HEAP::init( compare_num, reset_p, &min_value, sizeof(NUM*) );
  printf("Inserting values in interval [%d, %d]\n", start, end);
  for( int i = start; i <= end; ++i ) {
    NUM * n = (NUM *) malloc(sizeof(NUM));
    n->number = i;
    BINOMIAL_HEAP::NODE * tmp = BINOMIAL_HEAP::insert( bh, &n );
    n->heap_ptr = tmp;
  }

  printf( "size: %lu\n", BINOMIAL_HEAP::size(bh) );

  size_t min_len = 0;
  NUM *min_heap_val = *(NUM **) BINOMIAL_HEAP::min( bh, min_len );
  printf( "min: %d\n", min_heap_val->number );

  printf( "roots:\n" );
  BINOMIAL_HEAP::print_roots( bh, print_num );

  BINOMIAL_HEAP::destroy( bh );
}
