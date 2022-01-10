/* 
 * Author: Luke Hindman
 * Date: Fri 13 Nov 2020 12:21:37 PM PST
 * Description: Starter code for final project (myps)
 */

#define _GNU_SOURCE

#include "ProcEntry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

ProcEntry ** myprocs = NULL;                // Array to store all process entries
DIR * procDirStruct = NULL;                 // Pointer to Process directory stream
struct dirent * validDirEntries = NULL;     // Array to store directory entries
struct dirent * dirEntryItr = NULL;         // Iterator to iterate through directory contents

static unsigned int procCount = 0;          // stores the number of process in proc directory
const char * PROC_DIR = "/proc";            //Stores the path of process directory

//Usage Message
const char * USAGE_HELP = " [-d <path>] [-p] [-c] [-z] [-h]\n"
        "-d <path> Directory containing proc entries (default: /proc)\n"
        "-p        Display proc entries sorted by pid (default)\n"
        "-c        Display proc entries sorted by command lexicographically\n"
        "-z        Display ONLY proc entries in the zombie state\n"
        "-h        Display this help message\n";


/**
 * @brief Comparator function for qsort
 * to sort relative to process id
 * @param a Pointer to array element
 * @param b Pointer to array element
 * @return int difference between pid of two process
 */
static int pidSort(const void *a, const void *b);

/**
 * @brief Comparator function for qsort
 * to sort relative to process command name
 * @param a Pointer to array elements
 * @param b Pointer to array elements
 * @return int difference between names of two process computed by strcmp()
 */
static int cmdSort(const void *a, const void *b);


/**
 * @brief Function to extract all valid proc 
 * directory entries from the proc directory
 * 
 */
void UpdateValidDirEntries(void);

/**
 * @brief Get and store all the Processes intp myprocs array
 * 
 */
void GetAllProcs(void);

/**
 * @brief Prints all the process entries if isZombie is 0
 * else prints the process in zombie state
 * 
 * @param isZombie flag stores 0 or 1 
 */
void PrintMyProcs(int isZombie);

/**
 * @brief Function to free up all the dynamically alloted memory  
 */
void CleanUp(void);

static int pidSort(const void *a, const void *b)
{
    //typecast the void pointers into process entry pointers 
    ProcEntry *f = *(ProcEntry **)a;
    ProcEntry *s = *(ProcEntry **)b;
    //If both entries are NULL they are equal
    if (f == NULL && s == NULL) {
        return 0;
    }
    //If either are NULL they should be sent to the back
    else if (f == NULL) {
        return 1;
    }
    else if (s == NULL) {
        return -1;
    }
    // return the pid difference
    int rval = f->pid - s->pid;
    return rval;
}

static int cmdSort(const void *a, const void *b)
{
    //typecast the void pointers into process entry pointers 
    ProcEntry *f = *(ProcEntry **)a;
    ProcEntry *s = *(ProcEntry **)b;
    if (f == NULL && s == NULL) {
        return 0;
    }
    else if (f == NULL || f->comm == NULL) {
        return 1;
    }
    else if (s == NULL || f->comm == NULL) {
        return -1;
    }
    // return the strcmp value of two non null strings
    int rval = strcmp(f->comm,s->comm);
    return rval;
}

void UpdateValidDirEntries() {

    struct stat dirEntStat;
    unsigned int dirItr = 0;
    char fullDirPath[1024];

    // check if directory fails to open 
    if ((procDirStruct = opendir(PROC_DIR)) == NULL) {
        fprintf(stderr,"%s :Directoy failed to open",PROC_DIR);
        exit(0);
    }

    while ((dirEntryItr = readdir(procDirStruct)) != NULL) {
        //Iterate throught the directory stream contents

        // construct the full directory pathof each entry    
        sprintf( fullDirPath, "%s/%s",PROC_DIR,dirEntryItr->d_name);

        //retrive directory info into dirEntStat 
        if (stat(fullDirPath, &dirEntStat) == -1) {
            continue ;
        }
        
        //check if the current entry is indeed a directory or a file
        if (( dirEntStat.st_mode & S_IFMT ) != S_IFDIR ) {
            continue;
        }
        
        //check whether directory name start with a number
        if (dirEntryItr->d_name[0] < '0' || dirEntryItr->d_name[0] > '9') {
            continue;
        }
        procCount++;
    }

    // reset the directory stream to beginning
    rewinddir(procDirStruct);

    //allocate appropriate space for the entries
    validDirEntries = (struct dirent *) malloc(sizeof(struct dirent)*procCount);

    while ((dirEntryItr = readdir(procDirStruct)) != NULL) {
        
        sprintf( fullDirPath, "%s/%s",PROC_DIR,dirEntryItr->d_name);

        if (stat(fullDirPath, &dirEntStat) == -1) {
            continue ;
        }
        if (( dirEntStat.st_mode & S_IFMT ) != S_IFDIR ) {
            continue;
        }
        if (dirEntryItr->d_name[0] < '0' || dirEntryItr->d_name[0] > '9') {
            continue;
        }
        
        //copy and store the directory entries
        memcpy(&validDirEntries[dirItr],dirEntryItr,sizeof(struct dirent));
        
        dirItr++;
    }
    
    closedir(procDirStruct);
}

void GetAllProcs() {
    
    //variable to store fullpath of stat file
    char fullStatPath[1024];
    
    //allocate space to hold pointers 
    myprocs = (ProcEntry **) malloc(sizeof(ProcEntry*)*procCount);
    
    if(myprocs==NULL) {
        return;
    }

    for(unsigned int itr = 0; itr < procCount; itr++) {
        //for every directory entry construct the full path of stat file
        sprintf( fullStatPath, "%s/%s/stat",PROC_DIR,validDirEntries[itr].d_name);
        // populate the myprocs element
        myprocs[itr] = CreateProcEntryFromFile(fullStatPath);
    }

}

void PrintMyProcs(int isZombie) {
    //Print the column names
    fprintf(stdout,"%7s %7s %5s %5s %5s %7s %-25s %-20s\n","PID","PPID","STATE","UTIME","STIME","THREADS","CMD","STAT_FILE");
    
    for(unsigned int itr = 0; itr < procCount; itr++) {
        if(myprocs[itr]==NULL) {
            //skip null entries
            continue;
        }
        if(isZombie && myprocs[itr]->state == 'Z') {
            //if zombies is true print only zombie processes
            PrintProcEntry(myprocs[itr]);
        }
        else if (!isZombie) {
            // else print all processes
            PrintProcEntry(myprocs[itr]);
        }
    }
}

void CleanUp() {

    for(unsigned int itr=0; itr<procCount;itr++) {
        if(myprocs[itr]!=NULL) {
            //Destroy all the non NULL entries in myprocs array
            DestroyProcEntry(myprocs[itr]);
        } 
    }
    free(myprocs); //free the myprocs pointer array
    free(validDirEntries); //free the directory entry array
    
    //set pointer values to NULL
    myprocs=NULL;
    validDirEntries = NULL;
    procDirStruct = NULL;
    dirEntryItr = NULL;
}

int main (int argc, char * argv[]) {
    
    int argItr = 1;             // Iterator to iterate over command line arguments
    int isPidSort = 1;          // Flag to check presence of -p 
    int isCmdSort = 0;          // Flag to check present of -c 
    int isZombie = 0;           // Flag to check present of -z
    struct stat procDirStat;    // stat struct to check validate process directory
    
    if (argc>1 && strcmp(argv[1],"-h") == 0) {
        // if -h flag is present print out the USAGE message
        printf("Usage: %s%s",argv[0],USAGE_HELP);
        return 0;
    }
    
    while (argItr < argc){
        // Iterate over each command line argument 
        if (strcmp(argv[argItr],"-d") == 0){
            argItr++;

            if (argItr>= argc) {
                fprintf(stderr,"Directory path is not provided.\n\n");
                printf("Usage: %s%s",argv[0],USAGE_HELP);
                return 0;
            }
            //Fill the stat struct with relevant path info
            stat(argv[argItr], &procDirStat);

            if (S_ISDIR(procDirStat.st_mode)) {
                // Check if provided path leads to an existing directory
                PROC_DIR = (const char*)argv[argItr];
            }
            else {
                fprintf(stderr,"%s\tNo such directory\n",argv[argItr]);
                return 0;
            }
        }
        else if (strcmp(argv[argItr],"-p") == 0){
            isPidSort = 1;
        }
        else if (strcmp(argv[argItr],"-c") == 0){
            isCmdSort = 1;  //Switch the default sorting flag
            isPidSort = 0;
        }
        else if (strcmp(argv[argItr],"-z") == 0){
            isZombie = 1;
        }
        else if (strcmp(argv[argItr],"-h") == 0){
            ; //If -h flag is present later ignore it 
        }
        else {
            fprintf(stderr,"Unknown option argument: %s\n\n",argv[argItr]);
            printf("Usage: %s%s",argv[0],USAGE_HELP);
            return 0;
        }
        argItr++;
    }

    //Iterate through all directory entries 
    //and store the entries corresponding to a valid process 
    UpdateValidDirEntries();

    //Populate the myProcs array with the stat info of 
    //every valid directory entry stored in validDirEntries
    GetAllProcs();


    if (isPidSort) {
        //sort by process ID
        qsort((void*)myprocs,procCount,sizeof(myprocs[0]),pidSort);
    }
    else if(isCmdSort) {
        //sort by command name
        qsort((void*)myprocs,procCount,sizeof(myprocs[0]),cmdSort);
    }

    //Print all the procs if zombie is false
    //Else print only the zombie procs
    PrintMyProcs(isZombie);

    //Release all the dynamically allocated memory
    CleanUp();

    return 0;    
}
  