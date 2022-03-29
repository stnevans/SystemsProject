/**
** @file queues.c
**
** @author CSCI-452 class of 20215
**
** Queue module implementation
*/

#define SP_KERNEL_SRC

#include "common.h"

#include "queues.h"

/*
** PRIVATE DEFINITIONS
*/

// internal form of queue length call
#define QLEN(q)    ((q)->count)

// alternate version that actually invokes the function
// #define QLEN(q)    _queue_length(q)

/*
** PRIVATE DATA TYPES
*/

/*
** Queue organization
** ------------------
** Our queues are self-ordering, generic queues.  A queue can contain
** any type of data.  This is accomplished through the use of intermediate
** nodes called qnodes, which contain a void* data member, allowing them
** to point to any type of integral data (integers, pointers, etc.).
** The qnode list is doubly-linked for ease of traversal.
**
** Each queue has associated with it a comparison function, which may be
** NULL.  Insertions into a Queue are handled according to this function.
** If the function pointer is NULL, the queue is FIFO, and the insertion
** is always done at the end of the queue.  Otherwise, the insertion is
** ordered according to the results from the comparison function.
**
** Neither of these types are visible to the rest of the system.  The
** queue_t type is a pointer to the q_s struct.
*/

// queue nodes
typedef struct qn_s {
    struct qn_s *prev;  // link to previous node
    struct qn_s *next;  // link to next node
    void *data;         // what's in this entry
    key_t key;          // key to whatever's in this entry
} qnode_t;

// the queue itself is a pointer to this structure
typedef struct q_s {
    qnode_t *head;      // first element
    qnode_t *tail;      // last element
    uint_t count;       // current occupancy count
    int (*order)( const key_t, const key_t ); // how to compare entries
} qinfo_t;

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** The list of free qnodes.
**
** Organized as a singly-linked list using the 'next' pointer
** in the qnode structure.
*/
static qnode_t *_qnode_list;

/*
** The initial set of queues available in the system.
**
** If these are all allocated, _queue_alloc() will allocate
** another slice of memory as needed.
*/
static qinfo_t _queues[N_QUEUES];

/*
** The list of free queues.
**
** Organized as a singly-linked list using the 'head' pointer
** in the q_s structure.
*/
static qinfo_t *_queue_list;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** Qnode Functions
*/

// forward declarations for some internal functions
static void _qnode_free( qnode_t *qn );
void _queue_delete( queue_t q );

/**
** _qnode_new() - allocate a slice and carve it into qnodes
**
** @return the number of qnodes added to the free list
*/
static int _qnode_new( void ) {

    // start by carving off a slice of memory
    qnode_t *new = (qnode_t *) _km_slice_alloc();

    // if we couldn't get one, we're done
    if( new == NULL ) {
        return( 0 );
    }

    // clear out the allocated space
    __memclr( new, SZ_SLICE );

    // free the qnodes!
    for( int i = 0; i < (SZ_SLICE / sizeof(qnode_t)); ++i ) {
        // N.B.: if the slice size is not an integral multiple of
        // the qnode size, this will leave a small internal fragment
        // at the end of the allocated block of memory
        _qnode_free( &new[i] );
    }

    // all done
    return( SZ_SLICE / sizeof(qnode_t) );
}

/**
** _qnode_alloc() - allocate a qnode
**
** Removes the first qnode from the free list.
**
** @return A pointer to the allocated node, or NULL
*/
static qnode_t *_qnode_alloc( void ) {
    qnode_t *new;

    // see if there is an available node
    if( _qnode_list == NULL ) {

        // no - see if we can create some
        if( !_qnode_new() ) {
            // no!  let's just leave quietly
            return( NULL );
        }
    }

    // OK, we know that there is at least one free qnode;
    // just take the first one from the list

    new = _qnode_list;
    _qnode_list = new->next;

    // clear out the fields in this one just to be safe
    new->prev = new->next = new->data = NULL;
    new->key = 0;

    // pass it back to the caller
    return( new );
}

/**
** _qnode_free() - return a qnode to the free list
**
** Deallocates the supplied qnode
**
** @param qn   The qnode to be put on the free list
*/
static void _qnode_free( qnode_t *qn ) {
    assert( qn != NULL );

    // just stick this one at the front of the list
    qn->next = _qnode_list;
    _qnode_list = qn;
}

/**
** _queue_new() - allocate a slice and carve it into queues
**
** @return number of queues added to the free list
*/
static int _queue_new( void ) {
    struct q_s *slice;

    // start by carving off a slice of memory
    slice = (struct q_s *) _km_slice_alloc();

    // NULL slice is a problem
    if( slice == NULL ) {
        return( 0 );
    }

    // clear out the allocated space
    __memclr( slice, SZ_SLICE );

    // 
    for( int i = 0; i < (SZ_SLICE / sizeof(struct q_s)); ++i ) {
        _queue_delete( slice + i );
    }

    // all done!
    return( SZ_SLICE / sizeof(struct q_s) );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** _queue_create() - allocate a queue
**
** Allocates a queue structure and returns it to the caller.
**
** @param order   The ordering function to be used, or NULL
**
** @return a pointer to the allocated queue, or NULL
*/
queue_t _queue_create( int (*order)(const key_t,const key_t) ) {
    queue_t new;

    // see if there is an available node
    if( _queue_list == NULL ) {

        // no - see if we can create some
        if( !_queue_new() ) {
            // no!  let's just leave quietly
            return( NULL );
        }
    }

    // OK, we know that there is at least one free qnode;
    // just take the first one from the list

    new = _queue_list;
    _queue_list = (queue_t) new->head;

    // sanity check!
    assert1( new != NULL );

    // clear out the fields in this one just to be safe
    new->head = new->tail = NULL;
    new->count = 0;
    new->order = order;

    // pass it back to the caller
    return( new );
}

/**
** _queue_delete() - return a queue to the free list
**
** Deallocates the supplied queue
**
** @param q   The queue to be put on the free list
*/
void _queue_delete( queue_t q ) {

    // sanity check!
    assert1( q != NULL );
    assert1( q->count == 0 );

    // just stick this one at the front of the list
    q->head = (qnode_t *) _queue_list;
    _queue_list = q;
}

/**
** _queue_length() - return the count of elements in a queue
**
** @param q   The queue to be checked
*/
uint_t _queue_length( queue_t q ) {

    // sanity check!
    assert1( q != NULL );

    // this one's easy
    return( q->count );
}

/**
** _queue_add() - add an element to a queue
**
** @param q     The queue to be manipulated
** @param data  The data to be added
** @param key   The key value to be used when ordering the queue
**
** @return the status of the insertion attempt
*/
status_t _queue_add( queue_t q, void *data, key_t key ) {

    // sanity check!
    assert1( q != NULL );

    // need to use a qnode
    qnode_t *qn = _qnode_alloc();
    if( qn == NULL ) {
        return( E_NO_MEM );
    }

    // fill in the node
    qn->data = data;
    qn->key = key;

    /*
    ** Insert the data.
    **
    ** The simplest case is insertion into an empty queue.
    */

    if( QLEN(q) == 0 ) {
        // first, last, and only element
        q->head = q->tail = qn;
        q->count = 1;
        return( E_SUCCESS );
    }

    /*
    ** next simplest is an un-ordered queue
    */

    if( q->order == NULL ) {
        // just add at the end
        qn->prev = q->tail;     // predecessor is (old) last node
        q->tail->next = qn;     // new is successor to (old) last node
        q->tail = qn;           // new is now the last node
        q->count += 1;         // one more in the list
        return( E_SUCCESS );
    }

    /*
    ** Insertion into a non-empty, ordered list.
    **
    ** Start by traversing the list looking for the node
    ** that will come after the node we're inserting.
    */

    qnode_t *curr = q->head;

    while( curr != NULL && q->order(key,curr->key) >= 0 ) {
        curr = curr->next;
    }

    /*
    ** We now know the successor of the node we're inserting.
    **
    ** CURR == NULL:  add at end
    **         else:  add before curr
    */

    qn->next = curr;    // correct even if curr is NULL

    if( curr == NULL ) {

        // if curr is NULL, we're adding at the end
        q->tail->next = qn;     // new is successor to (old) last node
        qn->prev = q->tail;     // predecessor is (old) last node
        q->tail = qn;           // new is now the last node

    } else {

        // adding before the end; set the predecessor pointer
        qn->prev = curr->prev;

        // if curr is the first node, this is the new head node
        if( curr->prev == NULL ) {
            // new first node in the list
            q->head = qn;
        } else {
            // adding to the middle of the list
            curr->prev->next = qn;
        }

        // finally, point our successor back to us
        curr->prev = qn;
    }

    q->count += 1;

    return( E_SUCCESS );
}

/**
** _queue_remove() - remove an element from a queue
**
** @param q     The queue to be manipulated
** @param data  (output) The data removed from the queue
**
** @return the status of the removal attempt
*/
status_t _queue_remove( queue_t q, void **data ) {

    // sanity check!
    assert1( q != NULL );
    assert1( data != NULL );

    // can't remove anything from an empty queue
    if( QLEN(q) == 0 ) {
        return( E_EMPTY );
    }

    // OK, we have something to return; take it from the queue
    qnode_t *qn = q->head;

    // save the data value
    *data = qn->data;

    // unlink the qnode from the list
    q->head = qn->next;

    // was this the only node in the list?
    if( q->head == NULL ) {
        // yes, so now there's also no last element
        q->tail = NULL;
    } else {
        // no - unlink the (old) successor from this node
        q->head->prev = NULL;
    }

    // update the occupancy count
    q->count -= 1;

    // return the qnode for later re-use
    _qnode_free( qn );

    // send the result back to the caller
    return( E_SUCCESS );
}

/**
** _queue_remove_specific() - remove an element from a queue
**
** @param q     The queue to be manipulated
** @param data  The data to be removed from the queue
**
** @return the removed data, or NULL
*/
void *_queue_remove_specific( queue_t q, void *data ) {

    // sanity check!
    assert1( q != NULL );

    // can't remove anything from an empty queue
    if( QLEN(q) == 0 ) {
        return( NULL );
    }

    // search the queue looking for that specific value
    qnode_t *qn = q->head;

    while( qn != NULL && qn->data != data ) {
        qn = qn->next;
    }

    // did we find it?
    if( qn == NULL ) {
        // no!
        return( NULL );
    }

    // no need to save the 'data' field, because it
    // is equal to our 'data' parameter

    // unlink this qnode from its predecessor
    if( qn->prev == NULL ) {
        // first node in thelist
        q->head = qn->next;
    } else {
        qn->prev->next = qn->next;
    }

    // now, unlink from the successor
    if( qn->next == NULL ) {
        // last node in the list
        q->tail = qn->prev;
    } else {
        qn->next->prev = qn->prev;
    }
 
    // update the occupancy count
    q->count -= 1;

    // return the qnode for later re-use
    _qnode_free( qn );

    // send the result back to the caller
    return( data );
}

/**
** _queue_peek() - peek at the first element in a queue
**
** @param q   The queue to be checked
**
** @return the data pointer from the first node in the queue, or NULL
**         if the queue is empty (note: an empty queue is not
**         distinguishable from one whose first entry has a data
**         value of 0 using this function!).
*/
void *_queue_peek( queue_t q ) {

    // sanity check!
    assert1( q != NULL );

    // if there is a node, return its data pointer
    if( QLEN(q) > 0 ) {
        return( q->head->data );
    }

    // otherwise, return NULL
    return( NULL );
}

/**
** _queue_kpeek() - peek at the first element in a queue
**
** @param q   The queue to be checked
**
** @return the key from the first node in the queue, or 0
**         if the queue is empty (note: an empty queue is
**         not distinguishable from one whose first entry
**         has a key value of 0 using this function!).
*/
key_t _queue_kpeek( queue_t q ) {

    // sanity check!
    assert1( q != NULL );

    // if there is a node, return its key value
    if( QLEN(q) > 0 ) {
        return( q->head->key );
    }

    // otherwise, return 0
    return( 0 );
}

/*
** Debugging/tracing routines
*/

/**
** _queue_dump(msg,que)
**
** dump the contents of the specified queue to the console
**
** @param msg  Optional message to print
** @param q    Queue to dump
*/
void _queue_dump( const char *msg, queue_t q ) {

    // report on this queue
    __cio_printf( "%s: ", msg );
    if( q == NULL ) {
        __cio_puts( "NULL???\n" );
        return;
    }

    // first, the basic data
    __cio_printf( "head %08x tail %08x %d count",
                  (uint32_t) q->head, (uint32_t) q->tail, q->count );

    // next, how the queue is ordered
    if( q->order ) {
        __cio_printf( " order %08x\n", (uint32_t) q->order );
    } else {
        __cio_puts( " FIFO\n" );
    }

    // if there are members in the queue, dump the first nodes
    if( q->count > 0 ) {
        __cio_puts( " data: " );
        qnode_t *tmp;
        int i = 0;
        for( tmp = q->head; i < 5 && tmp != NULL; ++i, tmp = tmp->next ) {
            __cio_printf( " [%x,%08x]", tmp->key, (uint32_t) tmp->data );
        }

        if( tmp != NULL ) {
            __cio_puts( " ..." );
        }

        __cio_putchar( '\n' );
    }
}

/*
** Module initialization
*/

/**
** _queue_init() - initialize the queue module
**
** Allocates the initial set of qnodes and set of queues.
**
** Dependencies:
**    Cannot be called before kmem is initialized
**    Must be called before any queue manipulation can be done
*/
void _queue_init( void ) {

    __cio_puts( " Queue:" );

    // first, "allocate" our static set of queues
    for( int i = 0; i < N_QUEUES; ++i ) {
        _queue_delete( &_queues[i] );
    }

    // next, the qnodes
    _qnode_list = NULL;
    if( _qnode_new() < 1 ) {
        PANIC( 0, "_qnode_new failed" );
    }

    // all done!
    __cio_puts( " done" );
}
