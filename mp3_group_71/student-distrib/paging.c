#include "paging.h"

/* void setup_paging(void)
 * Inputs: void
 * Return Value: void
 * Function: maps memory to virtual memory, calls pagingHelp.S as well */
void setup_paging() {

    /*  Iterator Variable   */
    uint32_t i = 0;

    /*  Begin by emptying out the directory */
    /*  This means setting all values to 0  */
    /*  AVL is just dummy padding and we don't care to set it   */
    for(i = 0; i < NUMPAGES; i++) {

        page_directory[i].P = 0;
        page_directory[i].RW = 0;
        page_directory[i].US = 0;
        page_directory[i].PWT = 0;
        page_directory[i].PCD = 0;
        page_directory[i].A = 0;
        page_directory[i].PS = 0;
        page_directory[i].ADDR = i; /*  We set addr to the index as a dummy value for now   */

    }

    /*  Do the same for PTE */
    for(i = 0; i < NUMPAGES; i++) {

        page_table[i].P = 0;
        page_table[i].RW = 0;
        page_table[i].US = 0;
        page_table[i].PWT = 0;
        page_table[i].PCD = 0;
        page_table[i].A = 0;
        page_table[i].D = 0;
        page_table[i].PAT = 0;
        page_table[i].G = 0;
        page_table[i].ADDR = i; /*  We set addr to the index as a dummy value for now   */

    }
     /*  Do the same for video mem PTE */
    for(i = 0; i < NUMPAGES; i++) {

        vidmap_page_table[i].P = 0;
        vidmap_page_table[i].RW = 1;
        vidmap_page_table[i].US = 0;
        vidmap_page_table[i].PWT = 0;
        vidmap_page_table[i].PCD = 0;
        vidmap_page_table[i].A = 0;
        vidmap_page_table[i].D = 0;
        vidmap_page_table[i].PAT = 0;
        vidmap_page_table[i].G = 0;
        vidmap_page_table[i].ADDR = i; /*  We set addr to the index as a dummy value for now   */

    }

    /*  set up the video memory */
    page_table[PVIDEOLOCATION].P = 1;
    page_table[PVIDEOLOCATION].RW = 1;
    page_table[PVIDEOLOCATION].ADDR = PVIDEOLOCATION;
    /*  set up the video map */
    // vidmap_page_table[PVIDEOLOCATION].P = 1;
    // vidmap_page_table[PVIDEOLOCATION].RW = 1;
    // vidmap_page_table[PVIDEOLOCATION].US = 1;
    // vidmap_page_table[PVIDEOLOCATION].ADDR = PVIDEOLOCATION;

    /*  add to directory    */
    page_directory[0].P = 1;
    page_directory[0].RW = 1;
    page_directory[0].PS = 0;   //set up 4kb pages
    page_directory[0].ADDR = (uint32_t)page_table >> PAGINGOFFSET;

    /*  now set up kernel   */
    page_directory[1].P = 1;
    page_directory[1].RW = 1;
    page_directory[1].PS = 1;   //set up 4mb pages
    page_directory[1].ADDR = 0x400000 >> PAGINGOFFSET;

    //inline assembly for actually enabling the paging system.
    //I either suck at inline assembly or it just causes issues.
    enable_paging((int32_t*) page_directory);

}


