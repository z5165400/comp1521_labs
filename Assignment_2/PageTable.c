// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017
// Completed by Andrew Walls (z5165400)

// This implementation of LRU and FIFO uses two double-linked lists;
// one stores the page table entries in order of access time for LRU,
// and the other in order of load time for FIFO.
// The linked lists are integrated with the page table (See the PTE definition).
// Four global pointers are used, for the head and tail of each list.

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

    // Next item in the paging list
    struct PTE *next;
    // Previous item in the paging list
    struct PTE *prev;

    // Store the current page's index in the page table
    // This is inefficient and not necessary for the program
    // to function, but is needed for one of the DBUG outputs
    // And hence may be required for autotesting
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

// First and last items in the paging list
static PTE *list_first;
static PTE *list_last;

// Forward refs for private functions

static PTE *findVictim(int);
static void updatePaging(PTE *);

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
    for (int i = 0; i < nPages; i++) {
        PTE *p = &PageTable[i];
        p->status = NOT_USED;
        p->modified = 0;
        p->frame = NONE;
        p->accessTime = NONE;
        p->loadTime = NONE;
        p->nPeeks = p->nPokes = 0;

        // Initialise the paging list with no items
        p->next = NULL;
        p->prev = NULL;

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
    // Any call to requestPage is an access to a page,
    // so update the LRU list for this page
    if(replacePolicy == REPL_LRU)
        updatePaging(p);
    int fno; // frame number
    switch (p->status) {
        case NOT_USED:
        case ON_DISK:
            countPageFault();
            // The FIFO list needs updating for this page
            // if it needs loading
            if(replacePolicy == REPL_FIFO)
                updatePaging(p);
            fno = findFreeFrame();
            if (fno == NONE) {
                PTE *victim = findVictim(time);
#ifdef DBUG
                printf("Evict page %d\n", victim->table_index);
#endif
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

            //printf("loadFrame(%d, %d, %d)\n", pno, fno, time);
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

// Update the paging list for an entry; move the given item to
// the end of the list and juggle pointers as necessary
// Add the item to the list if it isn't present
static void updatePaging(PTE *entry)
{
    if(list_first == NULL && list_last == NULL) {
        list_first = entry;
        list_last = entry;
    } else if(entry->next == NULL && entry->prev == NULL) {
        entry->prev = list_last;
        list_last->next = entry;
        list_last = entry;
    } else if(list_first == entry) {
        list_first = entry->next;
        entry->next->prev = NULL;
        entry->next = NULL;
        entry->prev = list_last;
        list_last->next = entry;
        list_last = entry;
    } else if(list_last != entry) {
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
        entry->next = NULL;
        entry->prev = list_last;
        list_last->next = entry;
        list_last = entry;
    }
}

// Remove an item from the paging list
static void removeFromPaging(PTE *entry)
{
    // Move the item to the end of the list
    // This does the majority of the removal work
    updatePaging(entry);
    // Remove the item from the end of the list
    if(entry == list_first)
        list_first = entry->next;
    if(entry->prev)
        entry->prev->next = NULL;
    list_last = entry->prev;
    entry->prev = NULL;
    entry->next = NULL;
}

// findVictim: find a page to be replaced
// uses the configured replacement policy

static PTE *findVictim(int time)
{
    PTE *victim = 0;
    switch (replacePolicy) {
        case REPL_LRU:
        case REPL_FIFO:
            // Select the first item in the paging list,
            // make it the victim and remove it from the list,
            // as it can no longer be evicted
            victim = list_first;
            removeFromPaging(victim);
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
