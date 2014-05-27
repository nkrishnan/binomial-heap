
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "BINOMIAL_HEAP.h"

namespace BINOMIAL_HEAP {

  struct BHEAP {
    struct NODE * roots;
  };

  struct HANDLE {
    int (*compare) ( void const * node1, void const * node2 );
    void (*reset_p) ( void * v, BINOMIAL_HEAP::NODE * new_node );
    size_t num_items;
    void * min_data;
    size_t data_len;
    struct BHEAP * heap;
  };

  struct BHEAP * init_heap( ) {
    struct BHEAP * bh = (struct BHEAP *) calloc( 1, sizeof(struct BHEAP) );
    return bh;
  }

  void destroy_heap( struct BHEAP * &bh ) {
    free( bh );
    bh = NULL;
  }

  struct HANDLE * init(	int (*compare) ( void const * node1, void const * node2 ),
			void (*reset_p) ( void * v, BINOMIAL_HEAP::NODE * new_node ),
			void * min_data, size_t data_len ) {
    struct HANDLE * h = (struct HANDLE *) calloc( 1, sizeof(struct HANDLE) + data_len );
    h->compare = compare;
    h->reset_p = reset_p;
    h->num_items = 0;
    h->min_data = (void *) (h+1);
    memcpy( h->min_data, min_data, data_len );
    h->data_len = data_len;
    h->heap = init_heap( h->ba );
    return h;
  }

  void destroy( struct HANDLE * &h ) {
    destroy_heap( h->heap, h->ba );
    free( h );
    h = NULL;
  }

  struct NODE * find_min_root( struct HANDLE * h ) {
    struct NODE * curr = h->heap->roots;
    struct NODE * min_node = curr;
    while( curr ) {
      int cmp = h->compare( curr->data, min_node->data );
      if( cmp < 0 ) {
	min_node = curr;
      }
      curr = curr->sibling;
    }
    return min_node;
  }

  void remove_min_from_roots( struct HANDLE * h, struct NODE * min_node ) {
    if( h->heap->roots == min_node ) {
      h->heap->roots = min_node->sibling;
    }
    else {
      struct NODE * curr = h->heap->roots->sibling;
      struct NODE * prev = h->heap->roots;
      while( curr ) {
	if( curr == min_node ) {
	  prev->sibling = curr->sibling;
	  break;
	}
	prev = curr;
	curr = curr->sibling;
      }
    }
  }

  struct NODE * reverse_list( struct NODE * head ) {
    if(!head) return NULL;
    struct NODE * prev = NULL;
    struct NODE * curr = head;
    struct NODE * next = curr->sibling;
    while( next ) {
      curr->sibling = prev;
      prev = curr;
      curr = next;
      next = next->sibling;
    }
    curr->sibling = prev;
    head = curr;
    return head;
  }

  /**********************************************************************
   * link - link two binomial trees, rooted at y and z, each of order k *
   * It makes z the parent of y, resulting in a single binomial tree of *
   * order (k+1) rooted at z                                            *
   **********************************************************************/
  void link( struct NODE * y, struct NODE * z ) {
    assert(y->degree == z->degree);
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree += 1;
  }

  /**********************************************************************
   * merge - merge root lists of two binomial heaps                     *
   * After the merge, the roots are in monotonically increasing degree  *
   * order                                                              *
   **********************************************************************/
  void merge( struct HANDLE * h, struct BHEAP * h1, struct BHEAP * h2 ) {
    struct NODE * h1_roots = h1->roots;
    struct NODE * h2_roots = h2->roots;
    struct BHEAP * merged = init_heap( h->ba );
    struct NODE * merged_roots = merged->roots;
    struct NODE * merged_prev = NULL;
    while( h1_roots && h2_roots ) {
      if( h1_roots->degree <= h2_roots->degree ) {
	merged_roots = h1_roots;
	h1_roots = h1_roots->sibling;
      }
      else {
	merged_roots = h2_roots;
	h2_roots = h2_roots->sibling;
      }
      if( merged_prev ) {
	merged_prev->sibling = merged_roots;
      }
      else {
	merged->roots = merged_roots;
      }
      merged_prev = merged_roots;
      merged_roots = merged_roots->sibling;
    }
    // append the remaining list
    if(h2_roots) h1_roots = h2_roots;
    merged_roots = h1_roots;
    if( merged_prev == NULL )
      merged->roots = merged_roots;
    else
      merged_prev->sibling = merged_roots;
    destroy_heap(h1, h->ba);
    destroy_heap(h2, h->ba);
    h->heap = merged;
  }

  void heap_union( struct HANDLE * h, struct BHEAP * h1, struct BHEAP * h2 ) {
    merge( h, h1, h2 );
    if( h->heap->roots == NULL ) return;
    struct NODE * prev_x = NULL;
    struct NODE * x = h->heap->roots;
    struct NODE * next_x = x->sibling;
    while( next_x != NULL ) {
      if( (x->degree != next_x->degree) || (next_x->sibling != NULL && next_x->sibling->degree == x->degree) ) {
	prev_x = x;
	x = next_x;
      }
      else {
	if( h->compare(x->data, next_x->data) <= 0 ) {
	  x->sibling = next_x->sibling;
	  link( next_x, x );	  
	}
	else {
	  if(prev_x == NULL) {
	    h->heap->roots = next_x;
	  }
	  else {
	    prev_x->sibling = next_x;
	  }
	  link( x, next_x );
	  x = next_x;
	}
      }
      next_x = x->sibling;
    }
  }

  struct NODE * insert( struct HANDLE * h, void * data ) {
    struct BHEAP * temp = init_heap( h->ba );
    struct NODE * node = (struct NODE *) calloc( 1, sizeof(struct NODE) + h->data_len );
    node->data = node+1;
    memcpy( node->data, data, h->data_len );
    temp->roots = node;
    heap_union( h, h->heap, temp );
    return node;
  }

  void remove( struct HANDLE * h, struct NODE * node ) {
    if(!h || !node) return;
    decrease_key( h, node, h->min_data );
    extract_min( h );
  }

  void decrease_key( struct HANDLE * h, struct NODE * node, void * data ) {
    int cmp = h->compare( node->data, data );
    // if new data has higher value, i.e. if cmp < 0, we have a  problem
    if(cmp < 0) abort();
    memcpy( node->data, data, h->data_len );
    struct NODE * y = node;
    struct NODE * z = y->parent;
    void * temp = calloc( 1, h->data_len );
    while( z != NULL && h->compare( y->data, z->data ) < 0 ) {
      memcpy( temp, y->data, h->data_len );
      memcpy( y->data, z->data, h->data_len );
      memcpy( z->data, temp, h->data_len );
      h->reset_p( z->data, z );
      h->reset_p( y->data, y );
      y = z;
      z = y->parent;
    }
    free( temp );
  }

  void * min( struct HANDLE * h, size_t & data_len ) {
    data_len = 0;
    if( h->heap->roots == NULL ) return NULL;
    struct NODE * min_root = find_min_root( h );
    data_len = h->data_len;
    return min_root->data;
  }

  void extract_min( struct HANDLE * h, void * buf ) {
    struct NODE * min_root = find_min_root( h );
    remove_min_from_roots( h, min_root );
    struct BHEAP * temp = init_heap( h->ba );
    min_root->child = reverse_list( min_root->child );
    for( struct NODE * tmp = min_root->child; tmp != NULL; tmp = tmp->sibling ) {
      assert(tmp->parent == min_root);
      tmp->parent = NULL;
    }
    temp->roots = min_root->child;
    heap_union( h, h->heap, temp );
    if(buf) memcpy( buf, min_root->data, h->data_len );
    free( min_root );
  }

  size_t size( struct HANDLE * h ) {
    return h->num_items;
  }

  void print_roots( struct HANDLE * h, void (*print_data) ( void const *) ) {
    struct NODE * curr = h->heap->roots;
    while( curr ) {
      printf( "degree: %d ", curr->degree );
      if( print_data ) print_data( curr->data );
      printf( "\n" );
      curr = curr->sibling;
    }
  }
}
