/*
 * File: ProcEntry.h
 * Author(s): Luke Hindman & Shane Panter
 * Date: Fri 09 Apr 2021 08:08:30 AM MDT
 * Description: Definition of ProcEntry struct and declaration
 *   of support functions used in the myps project.
 */
#ifndef __PROC_ENTRY_H
#define __PROC_ENTRY_H

struct proc_entry {
     int pid;
     int ppid;
     char *comm;
     char state;
     unsigned long int utime;
     unsigned long int stime;
     long num_threads;
     char *path;
};
typedef struct proc_entry ProcEntry;

/* CreateProcEntry: Allocate a ProcEntry struct in the heap using malloc.
 *   All fields will be initialized to zero or NULL based upon data type.
 * 
 * returns - Pointer to ProcEntry allocated on the heap, NULL on error
 */
ProcEntry * CreateProcEntry(void);

/* CreateProcEntryFromFile: Allocate a ProcEntry struct in the heap 
 *    using malloc and initialize the fields with data provided from
 *    the specified statFile. The command field (comm) and path field 
 *    will be allocated on the heap using malloc. 
 * 
 * The statFile parameter must reference a stat file, formatted as 
 *    described in the /proc manpage entry. The statFile will be
 *    opened and the required fields extract to properly initialize
 *    the ProcEntry struct.
 *
 * If the statFile parameter is NULL or if the file does not exist, 
 *    return NULL. 
 * 
 * If the statFile exists, but an error occurs while attempting to 
 *    extract the field data because is not a properly formatted
 *    stat file, return NULL.
 *  
 * returns - Pointer to ProcEntry allocated on the heap, NULL on error
 */
ProcEntry * CreateProcEntryFromFile(const char statFile[]);

/* DestroyProcEntry: Release memory allocated by malloc in the
 *    CreateProcEntry or CreateProcEntryFromFile functions.  
 *    Does nothing if thisNode is NULL.
 *   
 * entry - Pointer to ProcEntry object to be freed.
 */
void DestroyProcEntry(ProcEntry * entry);

/* PrintProcEntry: Write the fields of the specified ProcEntry
 *    to stdout (console) using the exact format specified
 *    in the project guide.
 * 
 * entry - Pointer to ProcEntry object to be displayed
 */
void PrintProcEntry(ProcEntry * entry);


#endif /* __PROC_ENTRY_H */
