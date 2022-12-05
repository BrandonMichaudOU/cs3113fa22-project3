# Brandon Michaud

# How to Build and Run
Use `make all` to build the executable program. After this has been done, the program can be run by issuing the following command: `./project3 q p N ALGO`, where q is the size of the quantum used for round-robin scheduling, p is the number of processes that are available to run, N is the total memory allocation, and ALGO is the desired placement algorithm (FIRSTFIT or BUDDY).

# Assumptions
The program operates under the assumption that the executable call will be formatted as described in the project description and that there will be p input files following the format 0.ins, 1.ins, ..., (p-1).ins. It also assumes that multiple processes can have the same name. When this is the case and release or find is called with the process name, it uses the first process in memory with the name.

# Functions
The program begins in the main function and then calls other functions depending on the input line from the files. There are fittingly named functions for both first fit and buddy which perform the job of request, release, list available, list assigned, and find. 

# Bugs
There are no known bugs in this program.

# Libraries/Sources
No external libraries or web sources were used in this program.
