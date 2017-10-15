// Stats.h ... interface to VM stats
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

#ifndef STATS_H

void initStats(void);
void countPeekRequest(void);
void countPokeRequest(void);
void countPageLoad(void);
void countPageSave(void);
void countPageFault(void);
void countPageHit(void);
void showSummaryStats(void);

#endif
