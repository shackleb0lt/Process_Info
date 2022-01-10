#include "ProcEntry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Create a Proc Entry struct and
 * initialize it to default values
 * 
 * @return ProcEntry* newly created struct, NULL on failure
 */
ProcEntry * CreateProcEntry() {
    //allocate memory for the pointer
    ProcEntry * newProcEntry = (ProcEntry *)malloc(sizeof(ProcEntry));
    
    // return NULL on failure
    if(newProcEntry==NULL) {
        return NULL;
    }

    newProcEntry->pid = 0;
    newProcEntry->ppid = 0;
    newProcEntry->comm = (char *)malloc(sizeof(char)*32);
    newProcEntry->state = '\0';
    newProcEntry->utime = 0;
    newProcEntry->stime = 0;
    newProcEntry->num_threads = 0;
    newProcEntry->path = NULL;

    return newProcEntry;
}

/**
 * @brief Create a Process Entry from staFile path
 * parse the stat file to fill the struct values 
 * 
 * @param statFile fullpath to statfile
 * @return ProcEntry*  pointer tonewly created and 
 * filled process entry, NULL on failure
 */
ProcEntry * CreateProcEntryFromFile(const char statFile[]) {
    
    int itr = 4;
    char tempStat[16];
    FILE * statFilePtr;
    
    // Get new entry with default values 
    ProcEntry * newEntry = CreateProcEntry();

    if (statFile == NULL || newEntry==NULL) {
        return NULL;
    }

    statFilePtr = fopen(statFile,"r");
    
    if ( statFilePtr == NULL) {
        //if stat file doen't open return NULLl
        DestroyProcEntry(newEntry);
        return NULL;
    }

    //scanf first four entries from from stat file
    if (fscanf(statFilePtr,"%d %s %c %d", 
                &newEntry->pid,newEntry->comm, 
                &newEntry->state, 
                &newEntry->ppid) < 4) {
        DestroyProcEntry(newEntry);
        fclose(statFilePtr);
        return NULL;
    }
    
    while (itr < 21)
    {
        //scan the utime stime and numthreads attribute
        if (itr == 14) {
            if (fscanf(statFilePtr,"%lu", &newEntry->utime)==0) {
                DestroyProcEntry(newEntry);
                fclose(statFilePtr);
                return NULL;
            }
        }
        else if (itr == 15) {
            if (fscanf(statFilePtr,"%lu", &newEntry->stime)==0) {
                DestroyProcEntry(newEntry);
                fclose(statFilePtr);
                return NULL;
            }
            
        }
        else if (itr == 20) {
            if (fscanf(statFilePtr,"%ld", &newEntry->num_threads)==0) {
                DestroyProcEntry(newEntry);
                fclose(statFilePtr);
                return NULL;
            }
        }
        else {
            fscanf(statFilePtr,"%s ", tempStat);
        }
        itr++;
    }
    
    //duplicate the path onto heap 
    newEntry->path = (char *) malloc(strlen(statFile)+1);
    strcpy(newEntry->path,statFile);

    if (newEntry->path == NULL) {
        DestroyProcEntry(newEntry);
        fclose(statFilePtr);
        return NULL;
    }
    fclose(statFilePtr);
    return newEntry;
}

/**
 * @brief Function to release all dynamically alloted memory
 * 
 * @param entry Process entry to be freed
 */
void DestroyProcEntry(ProcEntry * entry) {
    if(entry==NULL) {
        return;
    }
    if(entry->comm!=NULL) {
        free(entry->comm);
    }
    if(entry->path!=NULL) {
        free(entry->path);
    }
    entry->comm = NULL;
    entry->path = NULL;
    free(entry);
    entry = NULL; 
}
/**
 * @brief Function to print the process entry 
 * 
 * @param entry process entry pointer to be printed
 */
void PrintProcEntry(ProcEntry *entry)
{
    unsigned long int utime = entry->utime / sysconf(_SC_CLK_TCK);
    unsigned long int stime = entry->stime / sysconf(_SC_CLK_TCK);
    fprintf(stdout, "%7d %7d %5c %5lu %5lu %7ld %-25s %-20s\n",
            entry->pid,
            entry->ppid,
            entry->state,
            utime,
            stime,
            entry->num_threads,
            entry->comm,
            entry->path);
}