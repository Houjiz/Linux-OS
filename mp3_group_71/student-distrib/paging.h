#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"

extern void setup_paging();
extern void enable_paging(int32_t* page_directory_addr);

#endif /*PAGING_H*/



