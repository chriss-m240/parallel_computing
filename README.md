# UNAL-2020-I Parallel Computing Course

## Assignments

**Pi calculation, nThreads vs time graph**

Run script.sh as a normal bash script, in case it can't be executed may a problem of permissions, allow the file to be executed doing:

> chmod +x script.sh

Then just wait, _script.sh_ is a bash script that compiles and run the _openmp_pi.c_ file with different number of treads as parameter, calculate the execution time with the **time** command, stores the result in the _out.csv_ file and then _plot.py_  graph them using mathplotlib.
