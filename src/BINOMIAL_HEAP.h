
#ifndef _BINOMIAL_HEAP_
#define _BINOMIAL_HEAP_

namespace BINOMIAL_HEAP {

  struct NODE {
    void * data;
    struct NODE * child;
    struct NODE * sibling;
    struct NODE * parent;
    int degree;
  };

  struct HANDLE;

  struct HANDLE * init( int (*compare) ( void const * node1, void const * node2 ), void (*reset_p) ( void * v, BINOMIAL_HEAP::NODE * new_node ), void * min_data, size_t data_len );

  void destroy( struct HANDLE * &h );

  struct NODE * insert( struct HANDLE * h, void * data );

  void remove( struct HANDLE * h, struct NODE * node );

  void decrease_key( struct HANDLE * h, struct NODE * node, void * data );

  void * min( struct HANDLE * h, size_t & data_len );

  void extract_min( struct HANDLE * h, void * buf = NULL );

  struct HANDLE * heap_union( struct HANDLE * h1, struct HANDLE * h2 );

  size_t size( struct HANDLE * h );

  void print_roots( struct HANDLE * h, void (*print_data) ( void const *) );

};

#endif
