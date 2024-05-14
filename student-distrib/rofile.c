#include "rofile.h"
#include "x86_desc.h"
#include "syscall.h"

/*  defining pointers that were declared in rofile.h */
uint32_t fileSystem;
bootBlock_t * bootBlock;
inode_t * inodeHead;
dataBlock_t * dataHead;
dir_entry_t * dirEntryHead;
/* void setup_files(void)
 * Inputs: void
 * Return Value: void
 * Function: maps all our pointers to the proper location in the file system */
void setup_files() {

    bootBlock = (bootBlock_t *)(fileSystem);
    dirEntryHead = bootBlock->dir_entries;
    inodeHead = (inode_t *)(bootBlock + 1);
    dataHead = (dataBlock_t *)(inodeHead + bootBlock->N);

}

/* int32_t read_dirEntry_by_name(const uint8_t * name, dir_entry_t * readDirEntry)
 * Inputs: fileName (string), pointer to a DirectEntry
 * Return Value: 0 on success, -1 on fail
 * Function: searches through the bootblocks list of dir_entry_t structs to find one with a matching filename */
int32_t read_dirEntry_by_name(const uint8_t * name, dir_entry_t * readDirEntry) {

    uint32_t i = 0;

    for(i = 0; i < NUMDIRENTRY; i++) {

        uint8_t * curName = (uint8_t *)(bootBlock->dir_entries[i].FILENAME);

        if (strncmp((int8_t*)name, (int8_t*)curName, FILENAMESIZE) == 0) {
    
          strncpy((int8_t*)readDirEntry->FILENAME, (int8_t*)(bootBlock->dir_entries[i]).FILENAME, strlen((int8_t*)(bootBlock->dir_entries[i]).FILENAME));
          readDirEntry->FILETYPE = (bootBlock->dir_entries[i]).FILETYPE;
          readDirEntry->INODENUM =(bootBlock->dir_entries[i]).INODENUM;
          return 0;

        }

    }

    /*  file not found  */
    return -1;

}

/* int32_t read_dirEntry_by_index(uint32_t index, dir_entry_t * readDirEntry)
 * Inputs: index to dirEntry (unsigned integer), pointer to a DirectEntry
 * Return Value: 0 on success, -1 on fail
 * Function: grabs dirEntry at given index */
int32_t read_dirEntry_by_index(uint32_t index, dir_entry_t * readDirEntry) {

    /*  bad value   */
    if(index >= NUMDIRENTRY) {

        return -1;
    
    }



    if((int8_t*)(bootBlock->dir_entries[index].FILENAME)==NULL){
        printf("no file");
        return -1;
    }

    /*  copy values into our pointer*/
    strncpy((int8_t*)readDirEntry->FILENAME, (int8_t*)(bootBlock->dir_entries[index]).FILENAME, strlen((int8_t*)(bootBlock->dir_entries[index]).FILENAME));
    readDirEntry->FILETYPE = (bootBlock->dir_entries[index]).FILETYPE;
    readDirEntry->INODENUM =(bootBlock->dir_entries[index]).INODENUM;

    return 0;

}

/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buffer, uint32_t length)
 * Inputs: inode- index to inode related to the file, offset- offset in file to begin writing, buffer- buffer to write data to, length- number of bits to read
 * Return Value: -1 on fail, bits read on success
 * Function: reads a given file and copies its data into the buffer */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buffer, uint32_t length) {

    /* grabbing information we need */
    
    /*  is inode valid? */
    if(inode > bootBlock->N) {

        return 0;

    }

    /*  grab inode with file    */
    inode_t * curNode = inodeHead + inode;
    /*  file size   */
    uint32_t fileLength = curNode->LENGTH;

    if(offset > curNode->LENGTH) {

        /*  invalid bounds  */
        return 0;

    }

    /*  keep the length tbc in bounds*/
    uint32_t toBeCopied = length;

    if(length + offset > fileLength) {

        toBeCopied = fileLength - offset;

    }

    /*  Based on offset, which datablock?  */
    uint8_t * curLocation;
    uint32_t dataBlockIndex = offset / 4096;
    uint32_t inDataBlock;

    inDataBlock = curNode->DATABLOCKS[dataBlockIndex];
    curLocation = (uint8_t *)(dataHead + inDataBlock);

    /*  where in the datablock are we?  */
    uint32_t offsetDataBlock = offset % 4096;

    curLocation += offsetDataBlock;

    /*  functions both as an interator variable + tracks bits read  */
    uint32_t bitsRead = 0;

     while(bitsRead < toBeCopied) {
        /*  everything is in THIS data block, whatever is left to read is within the bounds of this block. */
        if((length - bitsRead) <= (SIZEOFBLOCK - offsetDataBlock)) {

            memcpy(buffer + bitsRead, curLocation, length - bitsRead);
            bitsRead = length;

        } else {

            memcpy(buffer + bitsRead, curLocation, SIZEOFBLOCK - offsetDataBlock);
            dataBlockIndex++;
            inDataBlock = curNode->DATABLOCKS[dataBlockIndex];
            curLocation = (uint8_t *)(dataHead + inDataBlock);
            bitsRead += (SIZEOFBLOCK - offsetDataBlock);
            offsetDataBlock = 0;

        }

    }
    // printf("%d", bitsRead);
        return bitsRead;

}

/* int32_t fileOpen(const uint8_t * name)
 * Inputs: name- name of file to open
 * Return Value: 0 for now, can't open files yet
 * Function: nothing yet */
int32_t fileOpen(const uint8_t * name) {

    return 0;

}
/* int32_t fileClose(uint32_t fd)
 * Inputs: fd- index of directory to close
 * Return Value: 0 for now, can't close files yet
 * Function: nothing yet */
int32_t fileClose(int32_t fd) {

    return 0;

}
/* int32_t dirEntryOpen(const uint8_t * name)
 * Inputs: name- name of file to open
 * Return Value: 0 for now, can't open dirEntries yet
 * Function: nothing yet */
int32_t dirEntryOpen(const uint8_t * name) {

    return 0;

}
/* int32_t dirEntryClose(uint32_t fd)
 * Inputs: fd- index of directory to close
 * Return Value: 0 for now, can't close dirEntries yet
 * Function: nothing yet */
int32_t dirEntryClose(int32_t fd) {

    return 0;

}
/*  just keeping track of which file we are currently reading in the directory. Eventually will be replaced by a filedirectory system   */
uint32_t curDirEntry = 0;
//int curDirEntry_ = -1;
/* int32_t fileRead(uint32_t fd, uint8_t * buffer, uint32_t nbytes)
 * Inputs: fd- index of directory, currently not used. buffer - buffer to copy data into. nbytes - bytes to read
 * Return Value: -1 on fail, bitsRead on success.
 * Function: calls into read_data to... read data... */
int32_t fileRead(int32_t fd, void *buffer, int32_t nbytes) {
    
    /*  not a file  */
    // if((strlen((int8_t*)bootBlock->dir_entries[curDirEntry-1].FILENAME) < 1)) {

    //     return -1;

    // }
    // uint32_t toRead = (uint32_t)nbytes;

    // if(toRead > (inodeHead + bootBlock->dir_entries[curDirEntry-1].INODENUM)->LENGTH) {

    //     toRead = (inodeHead + bootBlock->dir_entries[curDirEntry-1].INODENUM)->LENGTH;

    // }
    

    // return read_data(bootBlock->dir_entries[curDirEntry-1].INODENUM, 0,(uint8_t *)buffer, toRead);


    pcb_t* pcb_file_read = get_current_pcb(current_pid);

    if(fd <= 1){
        return -1;
    }

    if(fd >= FILE_ARRAY_SIZE){
        return -1;
    }

    if(buffer ==NULL){
        return -1;
    }

    if(pcb_file_read->file_array[fd].flag ==0){
        return -1;
    }

    int32_t data_file_read = read_data(pcb_file_read->file_array[fd].inode, pcb_file_read->file_array[fd].file_pos,(uint8_t *)buffer, nbytes);

    if(data_file_read!=-1){
        pcb_file_read->file_array[fd].file_pos +=data_file_read;
        if(data_file_read == 0) {
            pcb_file_read->file_array[fd].file_pos = 0;
        }
        return data_file_read;
    }
    
    return -1;

    

}
/* int32_t fileWrite(uint32_t fd, uint8_t * buffer, uint32_t nbytes)
 * Inputs: fd- index of directory, currently not used. buffer - buffer to copy data from. nbytes - bytes to write
 * Return Value: -1, this system is read only.
 * Function: nothing*/
int32_t fileWrite(int32_t fd, void * buffer, int32_t nbytes) {

    /*  read only, always fail  */
    return -1;

}
/* int32_t dirEntryRead(uint32_t fd, uint8_t * buffer, uint32_t nbytes)
 * Inputs: fd- index of directory, currently not used. buffer - buffer to copy filename into. nbytes - bytes to read
 * Return Value: -1 on fail, bitsRead on success.
 * Function: calls into read_data to... read data... */
int32_t dirEntryRead(int32_t fd, void * buffer, int32_t nbytes) {
    uint32_t lenRead = strlen((int8_t*)bootBlock->dir_entries[curDirEntry].FILENAME);
    if(curDirEntry >= bootBlock->NDE) {
        curDirEntry = 0;
    }


    if(lenRead >= FILENAMESIZE) {

        lenRead = FILENAMESIZE;

    }
    
    strncpy((int8_t*)buffer, (int8_t*)bootBlock->dir_entries[curDirEntry].FILENAME, lenRead);

    curDirEntry++;

    return lenRead;

}
/* int32_t dirEntryWrite(uint32_t fd, uint8_t * buffer, uint32_t nbytes)
 * Inputs: fd- index of directory, currently not used. buffer - buffer to copy data from. nbytes - bytes to write
 * Return Value: -1, this system is read only.
 * Function: nothing*/
int32_t dirEntryWrite(int32_t fd, void * buffer, int32_t nbytes) {

    /*  read only, always fail  */
    return -1;

}


