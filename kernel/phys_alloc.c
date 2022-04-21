#define SP_KERNEL_SRC

#include "phys_alloc.h"
#include "kmem.h"

struct phys_frame {
    uint32_t frame_data[1024];
};

uint32_t num_frames;
struct phys_frame * frames;
uint8_t is_alloced[8192];


phys_addr alloc_frame(){
    for(int i = 0; i < num_frames; i++){
        if(!is_alloced[i]){
            is_alloced[i] = true;
            return (phys_addr) &frames[i];
        }
    }

    if(km_is_init()){
        return (phys_addr) _km_page_alloc(1);
    }
    
    return 0;
}

void free_frame(phys_addr addr) {
    
    for(int i = 0; i< num_frames; i++){
        if(&frames[i] == (void *) addr){
            is_alloced[i] = false;
            break;
        }
    }
    
    if(km_is_init()){
        _km_page_free((void *) addr);
    }
}

void _phys_alloc_init(phys_addr addr, uint32_t num_f){
    frames = (struct phys_frame *)addr;
    num_frames = num_f;
}