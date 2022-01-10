# Process Info

## Project Background
The myps project is drawn from the pool of warmup projects used in the Operating Systems (CS453/CS452) course. These warmup projects are assigned to help students identify weak areas in their C programming skillset so they can quickly get up to speed before the first OS programming project. I chose this particular project because it aligns particularly well with the content we've covered over the last four weeks.  

In this project you will write a simplified version of the ps command found on Linux/Unix based systems. The purpose of this command is to display the current processes on the system and some basic metadata including the process id number (PID) as well as the associated command (COMM/CMD). For your final project you will develop a simple program that loads information from the proc file system and displays it to the user with options provided to change the order that processes are displayed. For debugging/testing purposes an option will also be added to specify an alternate directory to load process data from.

### Learning objectives
The myps project aligns with the following learning objectives  

- Demonstrate knowledge of dynamic memory allocation
- Demonstrate knowledge of Create/Destroy design pattern
- Demonstrate knowledge of file stream processing
- Demonstrate knowledge of file system navigation
- Demonstrate knowledge of fundamental C language components: structs, arrays and pointers
- Demonstrate good coding style by following provided Style Guide
- Demonstrate good coding quality by producing code that has been well tested and is free of memory errors/warnings.


### Working with /proc file system
As we learned when we studied processes, the kernel is responsible for creating and managing processes within an operating system. On Linux, the kernel provides a window into its internal process structures with a virtual filesystem called /proc. This is mounted as a filesystem on Linux and can be navigated using the standard command line tools. 

**Example: ls /proc**  
<img src="etc/images/ls-proc.jpg" alt="ls /proc" width="900">


In addition to details about device drivers, memory usage and various other metrics, the /proc directory contains numbered subdirectories that correspond to the process id (PID) of currently running processes.  Within each directory is data for that particular process.  This directory structure is highly volatile as the PID directories will appear and disappear as processes are created and terminated.

**Example: ls /proc/1**  
<img src="etc/images/ls-proc-1.jpg" alt="ls /proc/1" width="900">

For this project we are particularly interested in the stat file located within each process directory. It contains a single line of space-delimited values that provide a variety of metrics for the corresponding process.


**Example: cat /proc/1/stat**  
<img src="etc/images/cat-proc-1-stat.jpg" alt="cat /proc/1/stat" width="900">


### Working with stat files
The man page for proc (**man 5 proc**) contains detailed information about the /proc file system. For this project we will only be extracting 7 data points from the stat file. The man page on the /proc file system is huge so we have copied the section relevant to this project here:
```
 /proc/[pid]/stat
              Status information about the process.  This is used by ps(1).
              It is defined in the kernel source file fs/proc/array.c.

              The fields, in order, with their proper scanf(3) format speci‐
              fiers, are listed below.  Whether or not certain of these
              fields display valid information is governed by a ptrace
              access mode PTRACE_MODE_READ_FSCREDS | PTRACE_MODE_NOAUDIT
              check (refer to ptrace(2)).  If the check denies access, then
              the field value is displayed as 0.  The affected fields are
              indicated with the marking [PT].

              (1) pid  %d
                        The process ID.

              (2) comm  %s
                        The filename of the executable, in parentheses.
                        This is visible whether or not the exe‐
                        cutable is swapped out.

              (3) state  %c
                        One of the following characters, indicating process
                        state:
                        R  Running
                        S  Sleeping in an interruptible wait
                        D  Waiting in uninterruptible disk sleep
                        Z  Zombie
                        T  Stopped (on a signal)
                        t  Tracing stop (Linux 2.6.33 onward)
                        W  Paging (only before Linux 2.6.0)
                        X  Dead (from Linux 2.6.0 onward)
                        x  Dead (Linux 2.6.33 to 3.13 only)
                        K  Wakekill (Linux 2.6.33 to 3.13 only)
                        W  Waking (Linux 2.6.33 to 3.13 only)
                        P  Parked (Linux 3.9 to 3.13 only)
              (4) ppid  %d
                        The PID of the parent of this process.
...
              (14) utime  %lu
                        Amount of time that this process has been scheduled
                        in user mode, measured in clock ticks (divide by
                        sysconf(_SC_CLK_TCK)).  This includes guest time,
                        guest_time (time spent running a virtual CPU, see
                        below), so that applications that are not aware of
                        the guest time field do not lose that time from
                        their calculations.
              (15) stime  %lu
                        Amount of time that this process has been scheduled
                        in kernel mode, measured in clock ticks (divide by
                        sysconf(_SC_CLK_TCK)).
...
              (20) num_threads  %ld
                        Number of threads in this process (since Linux 2.6).  

```

### In A Nutshell
The myps tool navigates to each PID directory in /proc (or other specified directory), opens the stat file, and extracts the required fields to build a ProcEntry struct. A pointer to this ProcEntry struct will be stored in an array of ProcEntry struct pointers.  Once all the PID directories have been processed and the associated ProcEntry structs have been created with pointers added to the array, the array will be sorted based upon user specified criteria and displayed in the console. 

The code to output both the array column headers as well as displaying individual ProcEntry structs has been provided and must not be changed. A portion of the grade for this project will depend upon exact output matching.


## Implementation Guide (part 1)
The myps tool will collect the following information on each process from the /proc file system and store the data in a ProcEntry struct. 

    - pid - The process id number of the process
    - ppid - The process id number of the parent process
    - comm - The filename of the executable
    - state - The state of the process (Running, Sleeping, etc)
    - utime - The amount of time that the process has been scheduled in user mode
    - stime - The amount of time that the process has been scheduled in kernel mode
    - num_threads - The number of threads used by by this process

The only field that is extra is the char *path field. This field is used to store the file path to the stat file that you loaded. Normally this will be /proc/[pid]/stat unless the user uses the -d flag (described below) to load a different directory.

Carefully study the provided ProcEntry.h file including both the provided ProcEntry struct and the documentation for each support function.  Each ProcEntry will represent a single process on the system. Implement the specified support functions in ProcEntry.c.  Do not modify the provided ProcEntry.h file as the provided struct definition and function declarations will be used to test this portion of your project.  

You must use the function below to display the ProcEntry structs to stdout.  Please copy this function verbatim into ProcEntry.c
```
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
```


**HINT:** Lab10 will be a great reference for this portion of the project.  It demonstrates the Create/Destroy design pattern as well as how to read data from files and load it into a struct.  It isn't an exact match, but a solid understanding of the concepts presented in Lab10 will be incredibly helpful here.

**TESTING:** Add test cases to mytests.c as you implement the functions declared in ProcEntry.h. As you write the tests, look for ways to exercise all the code in your functions.  It isn't practical to go for 100% code coverage, but 80 to 90% should be doable.  I will be running my own set of unit tests against your projects as part of the grading process so it would be a good idea to test the functions to ensure they handle expected and unexpected conditions as specified in the comments provided in ProcEntry.h

When testing, be certain to check the test cases with valgrind. The provided makefile includes a **memtest-mytests** rule to assist with this testing.
```
make memtest-mytests 
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes ./mytests
==72756== Memcheck, a memory error detector
==72756== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==72756== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==72756== Command: ./mytests
==72756== 
Create/Destroy Test passed
CreateFromFile/Destroy Test passed
            999     S     0     0    1 (gmain)                   test_proc/999/stat  
CreateFromFile/Print/Destroy Test passed
CreateFromFile NULL Test passed
CreateProcEntryFromFile: No such file or directory
CreateFromFile DoesNotExist Test passed
CreateFromFile InvalidFormat Test passed
==72756== 
==72756== HEAP SUMMARY:
==72756==     in use at exit: 0 bytes in 0 blocks
==72756==   total heap usage: 16 allocs, 16 frees, 15,478 bytes allocated
==72756== 
==72756== All heap blocks were freed -- no leaks are possible
==72756== 
==72756== For lists of detected and suppressed errors, rerun with: -s
==72756== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Implementation Guide (part 2)
Begin the implementation of myps itself by writing the code to process the following command line options:
```
Usage: ./myps [-d <path>] [-p] [-c] [-z] [-h]
        -d <path> Directory containing proc entries (default: /proc)
        -p        Display proc entries sorted by pid (default)
        -c        Display proc entries sorted by command lexicographically
        -z        Display ONLY proc entries in the zombie state 
        -h        Display this help message
 ```
 
By default, myps should process the /proc directory, however the user may specify another directory with the -d option.  This will be incredibly useful for testing as the /proc file system changes quite frequently.  Stub out the -p, -c and -z options as they will be implemented in part 3.  
 
Once the command line options have been processed, build a filtered array of dirent structs that contains only PID directories. A filter for PID directories will need to check if the dirent type is a directory and if the first character of the name is a number.  For debugging purposes, go ahead and write the directory entries to the console to confirm the correct set of directories is being processed.

**HINT:** Lab11 will be a great reference for this part of the project as well as the manpage for readdir(). 

**TESTING:** The core testing here will be to ensure the correct set of directories is being matched by the filter.  This can be performed manually.  Once again, be sure to run these tests with valgrind to ensure no memory leaks creep into the codebase. For basic testing, use the **memtest-myps** rule in the provided makefile.

## Implementation Guide (part 3)
Complete the myps implementation by dynamically creating an array of ProcEntry* items.  The number of items is determined by the number of dirents found in part 2. 

```
ProcEntry ** myprocs = (ProcEntry **) (malloc(sizeof(ProcEntry *) * n));
```

Iterate through the array of dirents created in part 2, use string concatenation to build the full file path to the stat file located in each PID directory, then use it to create a new ProcEntry struct with the CreateProcEntryFromFile() function defined in part 1 and add it to the ProcEntry* array.

Use qsort() to order items in the ProcEntry* array. The following comparison function can be used to sort ProcEntry items by process id. The myps tool should sort by pid if the -p option is specified or if no sorting option is specified on the command line. You will need to implement a second comparison function to enable sorting by command (comm) as specified by the -c command line option.
```
static int pidSort(const void *a, const void *b)
{
     ProcEntry *f = *(ProcEntry **)a;
     ProcEntry *s = *(ProcEntry **)b;
     int rval = f->pid - s->pid;
     return rval;
}
```

The last step is to display the sorted items in the ProcEntry* array.  This is also where myps will implement the -z option to display only ProcEntry items that are in the zombie (Z) state. Use the following code, verbatim, to display column headers that align with the output of the PrintProcEntry() function provided above:
```
fprintf(stdout,"%7s %7s %5s %5s %5s %7s %-25s %-20s\n","PID","PPID","STATE","UTIME","STIME","THREADS","CMD","STAT_FILE");
```

**HINT:** Lab12, particularly mysort, will be an excellent reference for this part of the project.

**TESTING:** Start with a few basic smoke tests to make sure the basics are working correctly. Please extract the test_data.tgz package into your project directory to complete this testing. Then use the proc datasets provided in test_data to more fully test the functionality of your myps implementation.

```
tar -xzf test_data.tgz
```

**Test Sort By PID (-p)**
```
./myps -d test_data/onyx_proc -p | head -5
    PID    PPID STATE UTIME STIME THREADS CMD                       STAT_FILE           
      1       0     S     1     3       1 (systemd)                 test_data/onyx_proc/1/stat
      2       0     S     0     0       1 (kthreadd)                test_data/onyx_proc/2/stat
      4       2     S     0     0       1 (kworker/0:0H)            test_data/onyx_proc/4/stat
      6       2     S     0     0       1 (ksoftirqd/0)             test_data/onyx_proc/6/stat 
```
**Test Sort By Command (-c)**
```
./myps -d test_data/onyx_proc -c | head -5
    PID    PPID STATE UTIME STIME THREADS CMD                       STAT_FILE           
   1623       1     S     0     0       3 (NetworkManager)          test_data/onyx_proc/1623/stat
  19089       1     S     0     0       3 (abrt-dbus)               test_data/onyx_proc/19089/stat
   1539       1     S     0     0       1 (abrt-watch-log)          test_data/onyx_proc/1539/stat
   1488       1     S     0     0       1 (abrtd)                   test_data/onyx_proc/1488/stat
```
**Test Show Zombies (-z)**
```
./myps -d test_data/onyx_proc -z | head -5
    PID    PPID STATE UTIME STIME THREADS CMD                       STAT_FILE           
   1368       2     Z     0     0       1 (kworker/37:2)            test_data/onyx_proc/1368/stat
   2159       2     Z     0     0       1 (nfsd)                    test_data/onyx_proc/2159/stat
   2224       2     Z     0     0       1 (nfsd)                    test_data/onyx_proc/2224/stat
  19068       2     Z     0     0       1 (kworker/32:0)            test_data/onyx_proc/19068/stat
```
**Check For Memory Errors**
```
make memtest-myps 
valgrind --tool=memcheck --leak-check=yes --show-reachable=yes ./myps > /dev/null
==218067== Memcheck, a memory error detector
==218067== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==218067== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==218067== Command: ./myps
==218067== 
==218067== 
==218067== HEAP SUMMARY:
==218067==     in use at exit: 0 bytes in 0 blocks
==218067==   total heap usage: 1,871 allocs, 1,871 frees, 547,332 bytes allocated
==218067== 
==218067== All heap blocks were freed -- no leaks are possible
==218067== 
==218067== For lists of detected and suppressed errors, rerun with: -s
==218067== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Final Project Evaluation
There are 150 points allocated to this project, the equivilant of five labs. The purpose of this project is to assess your mastery of the learning objectives described above. The following rubric will be used to evaluate project submissions
### Rubric
- **Coding Style (25 points)** - This score will reflect how closely you follow the [CS253 Style Guide](https://docs.google.com/document/d/1zKIpNfkiPpDHEvbx8XSkZbUEUlpt8rnZjkhCSvM-_3A/edit?usp=sharing) we've been using in the labs this semester.
- **Code Quality (25 points)** - This score will reflect any compilation warnings, run-time warnings or errors as well as memory issues reported by valgrind.  At program exit, no heap memory should be in use as indicated in the valgrind output above.
- **Unit Testing (50 points)** - This score will reflect how closely your implementation of the ProcEntry support functions in ProcEntry.c follow the described behavior documented in ProcEntry.h It is highly recommended that you develop a number of unit tests in mytests.c to confirm that these functions behave as expected. Unit tests must run without memory errors to pass.
- **Integration Testing (50 points)** - This score will check the output of your myps implementation to stdout against the expected output. The only output your program should generate on stdout will come from the supplied fprintf() header statement and the PrintProcEntry() function.  All other output, except the help/usage output, must be written to stderr or to a file.


### Integration Testing

**Compare output for pid sort of onyx test data to expected using diff**
```
./myps -d test_data/onyx_proc -p > myps-pid_sort.out
diff myps-pid_sort.out test_data/onyx_proc_expected/myps-pid_sort.out
<< No output means success! >>
echo $?
0
```

**Compare output for command sort of onyx test data to expected using diff**  
**NOTE:** Due to duplicate command data and no requirement for secondary sorting of PIDs, we must strip unneeded fields and only focus on the commands themself in this validation test.
```
./myps -d test_data/onyx_proc -c | awk '{print $7}'> myps-cmd_sort-stripped.out
cat test_data/onyx_proc_expected/myps-cmd_sort.out | awk '{print $7}'> test_data/onyx_proc_expected/myps-cmd_sort-stripped.out
diff myps-cmd_sort-stripped.out test_data/onyx_proc_expected/myps-cmd_sort-stripped.out
<< No output means success! >>
echo $?
0
```

**Compare output for zombie only listing of onyx test data to expected using diff**
```
./myps -d test_data/onyx_proc -z > myps-zombie_only.out
diff myps-zombie_only.out test_data/onyx_proc_expected/myps-zombie_only.out
<< No output means success! >>
echo $?
0
```
