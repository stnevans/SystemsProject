/**
** @file compat.h
**
** @author Warren R. Carithers
**
** Compatibility definitions for standard modules.
**
**      These definitions are here to simplify the integration
**      of some pre-written modules into the 452 baseline system.
**      This is used primarily for the 'kmem' and 'sio' modules.
*/

#ifndef COMPAT_H_
#define COMPAT_H_

/*
** Section 1:  sized integer types
**
** Internally, we use standard names for "sized" integer types for
** simplicity.  If those disagree with the names used in the rest of
** the system, we take the opportunity to define our names here.
**
** To enable these, uncomment them, and place the apropriate
** existing types in place of the '?' characters.
*/

// 20215 baseline uses these names already
// typedef ?   int8_t;
// typedef ?  uint8_t;
// typedef ?  int16_t;
// typedef ? uint16_t;
// typedef ?  int32_t;
// typedef ? uint32_t;
// typedef ?  int64_t;
// typedef ? uint64_t;
// typedef ?   bool_t;

/*
** Section 2:  other types
**
** Define types here as needed,
*/

// 20215 baseline uses this name already
// typedef ?    pcb_t;

/*
** Section 3:  function name aliases
**
** Include #define statements here as needed to define
** the names of functions used in these modules in
** terms of the names used in the rest of the baseline.
*/

// invoke the queue creation function
#define QCREATE(q)      do { q = _queue_create( NULL ); } while(0)

// invoke the queue "length" function
#define QLENGTH(q)    _queue_length( q )

// this macro expands into code that removes a value from
// 'q' and places it into 'd'
#define QDEQUE(q,d)     do { \
    status_t status =_queue_remove( q, (void **) &(d) ); \
    assert( status == E_SUCCESS ); \
} while(0)

// blocked queue for reading processes
#define READQ      _reading

// scheduler
#define SCHED      _schedule
#define DISPATCH   _dispatch

#endif
