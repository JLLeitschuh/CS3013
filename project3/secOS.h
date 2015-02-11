#ifndef SECOS_H
#define SECOS_H


typedef enum {
  TOP_SECRET,
  SECRET,
  UNCLASSIFIED
} SecurityLevel;

typedef struct job
{
  SecurityLevel p;  
  int id; // thread id
  int stay; // amt of time to process
} job;

#endif