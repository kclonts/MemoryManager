#include <cassert>
#include <iostream>
#include "dlUtils.h"
#include "MemoryManager.h"

MemoryManager::MemoryManager(unsigned int memtotal): memsize(memtotal)
{
   baseptr = new unsigned char[memsize];
   blockdata dummyBlock(0,false,0);
   blockdata originalBlock(memsize,true,baseptr);
   header = new dlNode<blockdata>(dummyBlock,nullptr,nullptr);
   trailer = new dlNode<blockdata>(dummyBlock,nullptr,nullptr);
   header->next = new dlNode<blockdata>(originalBlock,header,trailer);
   trailer->prev = header->next;
}

MemoryManager::~MemoryManager()
{
  delete [] baseptr;
  clearList(header);
}

void MemoryManager::showBlockList()
{
  printDlList(header,trailer,"->");
}

void MemoryManager::splitBlock(dlNode<blockdata> *p, unsigned int chunksize) {
    // Complete the code for this method
    assert(chunksize < p->info.blocksize);

/*
    int memoff = 0;
    dlNode<blockdata>* cursor = header->next;
    while(cursor != p) {
        memoff += cursor->info.blocksize;
        cursor = cursor->next;
    }
*/

    blockdata block = blockdata(p->info.blocksize-chunksize, true, NULL);
    block.blocksize = chunksize;
    block.blockptr = (unsigned char*)&block;

    //nsertAfter(trailer, p, block);

    dlNode<blockdata>* temp = new dlNode<blockdata>(block, p, p->next);
    p->next = temp;
    temp->next->prev = temp;
}

unsigned char * MemoryManager::malloc(unsigned int request) {
    // Complete the code for this method

    dlNode<blockdata>* cursor = header;

    while(++cursor != trailer) {
        if(cursor->info.blocksize >= request) {
            if(cursor->info.blocksize > request) splitBlock(cursor, request);
            cursor->info.free = false;
            return cursor->info.blockptr;
        }
    }

    return 0;
}

void MemoryManager::mergeForward(dlNode<blockdata> *p) {
    // Complete the code for this method
    assert(p->next != trailer && p->info.free && p->next->info.free);

    dlNode<blockdata>* temp = p->next;
    p->next = temp->next;
    temp->next->prev = p;
    p->info.blocksize += temp->info.blocksize;

    delete temp;
}

void MemoryManager::mergeBackward(dlNode<blockdata> *p)
{
    // Complete the code for this method
    assert(p->prev != header && p->info.free && p->prev->info.free);

    dlNode<blockdata>* temp = p->prev;
    p->prev = p->prev->prev;
    temp->prev->next = p;
    p->info.blocksize += temp->info.blocksize;

    delete temp;
}

void MemoryManager::free(unsigned char *ptr2block)
{
    // Complete the code for this method
    dlNode<blockdata>* temp = header;
    while(++temp != trailer) if(temp->info.blockptr == ptr2block) {
        temp->info.free = true;
        if(temp->next->info.free) mergeForward(temp);
        if(temp->prev->info.free) mergeBackward(temp);
    }
}
