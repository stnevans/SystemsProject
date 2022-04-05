#define SP_KERNEL_SRC

#include "phys_alloc.h"


struct phys_frame {
    uint32_t frame_data[1024];
};

#define num_frames 8192
struct phys_frame frames[num_frames] __attribute__ ((aligned (4096)));
uint8_t is_alloced[num_frames];




phys_addr alloc_frame(){
    for(int i = 0; i < num_frames; i++){
        if(!is_alloced[i]){
            is_alloced[i] = true;
            return (phys_addr) &frames[i];
        }
    }
    return 0;
}

void free_frame(phys_addr addr) {
    for(int i = 0; i< num_frames; i++){
        if(&frames[i] == addr){
            is_alloced[i] = false;
            break;
        }
    }
}