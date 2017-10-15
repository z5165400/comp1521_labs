// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#include <stdlib.h>
#include <stdio.h>
#include "Memory.h"
#include "Stats.h"
#include "PageTable.h"

// Symbolic constants

#define NOT_USED 0
#define IN_MEMORY 1
#define ON_DISK 2

// PTE = Page Table Entry

typedef struct PTE {
    char status;      // NOT_USED, IN_MEMORY, ON_DISK
    char modified;    // boolean: changed since loaded
    int  frame;       // memory frame holding this page
    int  accessTime;  // clock tick for last access
    int  loadTime;    // clock tick for last time loaded
    int  nPeeks;      // total number times this page read
    int  nPokes;      // total number times this page modified
    // TODO: add more fields here, if needed ...
    struct PTE *lru_next;
    struct PTE *lru_prev;
    struct PTE *fifo_next;
    struct PTE *fifo_prev;

    // TODO: Delete after testing
    int table_index;
} PTE;

// The virtual address space of the process is managed
//  by an array of Page Table Entries (PTEs)
// The Page Table is not directly accessible outside
//  this file (hence the static declaration)

static PTE *PageTable;      // array of page table entries
static int  nPages;         // # entries in page table
static int  replacePolicy;  // how to do page replacement
static int  fifoList;       // index of first PTE in FIFO list
static int  fifoLast;       // index of last PTE in FIFO list

static PTE *lru_first;
static PTE *lru_last;
static PTE *fifo_first;
static PTE *fifo_last;

// Forward refs for private functions

static PTE *findVictim(int);
static void updateLRU(PTE *);
static void updateFIFO(PTE *);

// initPageTable: create/initialise Page Table data structures

void initPageTable(int policy, int np)
{
    PageTable = malloc((unsigned long)np * sizeof(PTE));
    if (PageTable == NULL) {
        fprintf(stderr, "Can't initialise Memory\n");
        exit(EXIT_FAILURE);
    }
    replacePolicy = policy;
    nPages = np;
    fifoList = 0;
    fifoLast = nPages-1;
    //PTE *prev = NULL;
    for (int i = 0; i < nPages; i++) {
        PTE *p = &PageTable[i];
        p->status = NOT_USED;
        p->modified = 0;
        p->frame = NONE;
        p->accessTime = NONE;
        p->loadTime = NONE;
        p->nPeeks = p->nPokes = 0;

        p->fifo_next = NULL;
        p->lru_next = NULL;
        p->fifo_prev = NULL;
        p->lru_prev = NULL;

#if 0
        p->fifo_prev = prev;
        p->lru_prev = prev;

        if(i == 0) {
            lru_first = p;
            fifo_first = p;
        }
        if (i == nPages - 1) {
            lru_last = p;
            fifo_last = p;
        }

        if(prev) {
            prev->fifo_next = p;
            prev->lru_next = p;
        }
        prev = p;
#endif

        p->table_index = i;
    }
}

// requestPage: request access to page pno in mode
// returns memory frame holding this page
// page may have to be loaded
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

int requestPage(int pno, char mode, int time)
{
    if (pno < 0 || pno >= nPages) {
        fprintf(stderr,"Invalid page reference\n");
        exit(EXIT_FAILURE);
    }
    PTE *p = &PageTable[pno];
    updateLRU(p);
    int fno; // frame number
    switch (p->status) {
        case NOT_USED:
        case ON_DISK:
            // TODO: add stats collection
            countPageFault();
            updateFIFO(p);
            fno = findFreeFrame();
            if (fno == NONE) {
                PTE *victim = findVictim(time);
#ifdef DBUG
                printf("Evict page %d\n", victim->table_index);
#endif
                // TODO:
                // if victim page modified, save its frame
                // collect frame# (fno) for victim page
                // update PTE for victim page
                // - new status
                // - no longer modified
                // - no frame mapping
                // - not accessed, not loaded
                fno = victim->frame;
                if(victim->modified == 1) {
                    saveFrame(fno);
                }
                victim->status = ON_DISK;
                victim->modified = 0;
                victim->frame = NONE;
                victim->accessTime = NONE;
                victim->loadTime = NONE;

            }
            // TODO:
            // load page pno into frame fno
            // update PTE for page
            // - new status
            // - not yet modified
            // - associated with frame fno
            // - just loaded
            printf("loadFrame(%d, %d, %d)\n", pno, fno, time);
            loadFrame(fno, pno, time);
            p->status = IN_MEMORY;
            p->modified = 0;
            p->frame = fno;
            p->loadTime = time;
            break;
        case IN_MEMORY:
            countPageHit();
            break;
        default:
            fprintf(stderr,"Invalid page status\n");
            exit(EXIT_FAILURE);
    }
    if (mode == 'r')
        p->nPeeks++;
    else if (mode == 'w') {
        p->nPokes++;
        p->modified = 1;
    }
    p->accessTime = time;
    return p->frame;
}

// findVictim: find a page to be replaced
// uses the configured replacement policy

static void updateLRU(PTE *entry) {
    if(lru_last != entry) {
        if(entry->lru_prev)
            entry->lru_prev->lru_next = entry->lru_next;
        if(entry->lru_next)
            entry->lru_next->lru_prev = entry->lru_prev;
        if(lru_first == entry)
            lru_first = entry->lru_next;
        if(lru_last)
            lru_last->lru_next = entry;
        entry->lru_prev = lru_last;
        lru_last = entry;
        if(lru_first == NULL)
            lru_first = entry;
    }
}

static void updateFIFO(PTE *entry) {
    if(fifo_last != entry) {
        if(entry->fifo_prev)
            entry->fifo_prev->fifo_next = entry->fifo_next;
        if(entry->fifo_next)
            entry->fifo_next->fifo_prev = entry->fifo_prev;
        if(fifo_first == entry)
            fifo_first = entry->fifo_next;
        if(fifo_last)
            fifo_last->fifo_next = entry;
        entry->fifo_prev = fifo_last;
        fifo_last = entry;
        if(fifo_first == NULL)
            fifo_first = entry;
    }
}

static PTE *findVictim(int time)
{
    PTE *victim = 0;
    PTE *entry;
    switch (replacePolicy) {
        case REPL_LRU:
            entry = lru_first;
            updateLRU(entry);
            victim = entry;
            break;
        case REPL_FIFO:
            entry = fifo_first;
            updateFIFO(entry);
            victim = entry;
            break;
        case REPL_CLOCK:
            return NULL;
    }
    return victim;
}

// showPageTableStatus: dump page table
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

void showPageTableStatus(void)
{
    char *s;
    printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
            "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
    for (int i = 0; i < nPages; i++) {
        PTE *p = &PageTable[i];
        printf("[%02d]", i);
        switch (p->status) {
            case NOT_USED:  s = "-"; break;
            case IN_MEMORY: s = "mem"; break;
            case ON_DISK:   s = "disk"; break;
            default:        s = ""; break;
        }
        printf(" %6s", s);
        printf(" %4s", p->modified ? "yes" : "no");
        if (p->frame == NONE)
            printf(" %6s", "-");
        else
            printf(" %6d", p->frame);
        if (p->accessTime == NONE)
            printf(" %7s", "-");
        else
            printf(" %7d", p->accessTime);
        if (p->loadTime == NONE)
            printf(" %7s", "-");
        else
            printf(" %7d", p->loadTime);
        printf(" %7d", p->nPeeks);
        printf(" %7d", p->nPokes);
        printf("\n");
    }
}
