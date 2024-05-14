#ifndef ROFILE_H
#define ROFILE_H

#include "x86_desc.h"
#include "syscall.h"
#include "lib.h"
#include "types.h"


extern void setup_files();

extern int32_t read_dirEntry_by_name(const uint8_t * name, dir_entry_t * readDirEntry);
extern int32_t read_dirEntry_by_index(uint32_t index, dir_entry_t * readDirEntry);

extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buffer, uint32_t length);

extern int32_t fileOpen(const uint8_t * name);
extern int32_t fileClose(int32_t fd);
extern int32_t dirEntryOpen(const uint8_t * name);
extern int32_t dirEntryClose(int32_t fd);

extern int32_t fileRead(int32_t fd, void * buffer, int32_t nbytes);
extern int32_t fileWrite(int32_t fd, void * buffer, int32_t nbytes);
extern int32_t dirEntryRead(int32_t fd, void * buffer, int32_t nbytes);
extern int32_t dirEntryWrite(int32_t fd, void * buffer, int32_t nbytes);

/*  Pointers that Help with FileSystem  */
extern uint32_t fileSystem;
extern bootBlock_t * bootBlock;
extern inode_t * inodeHead;
extern dataBlock_t * dataHead;
extern dir_entry_t * dirEntryHead;


#endif /*ROFILE_H*/


