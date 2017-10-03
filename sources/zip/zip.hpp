#ifndef PRIVATE_H
#define PRIVATE_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>

typedef struct CZipEntry_s {
    unsigned long fileNameLength;
    const unsigned char* fileName;
    unsigned short compressionMethod;
    unsigned int uncompressedSize;
    unsigned int compressedSize;
    const unsigned char* data;
    
    struct CZipEntry_s* next;
} CZipEntry;

typedef struct CZipFile_s
{
    const unsigned char *buf;
    ssize_t bufsize;

    // Central directory
    unsigned short  disknum;            //mDiskNumber;
    unsigned short  diskWithCentralDir; //mDiskWithCentralDir;
    unsigned short  entryCount;         //mNumEntries;
    unsigned short  totalEntryCount;    //mTotalNumEntries;
    unsigned int    centralDirSize;     //mCentralDirSize;
    unsigned int    centralDirOffest;  // offset from first disk  //mCentralDirOffset;
    unsigned short  commentLen;         //mCommentLen;
    const unsigned char*  comment;            //mComment;

    CZipEntry* entries;
} CZipFile;

int read_central_dir(CZipFile* file);

unsigned int read_le_int(const unsigned char* buf);
unsigned int read_le_short(const unsigned char* buf);

#endif // PRIVATE_H

