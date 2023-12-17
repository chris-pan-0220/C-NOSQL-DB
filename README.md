# HW4: JSON Storage
* Author: 潘兆新
* Student ID: 111504506
* Department: 資電二
## Command line interface 
```bash 
./main
```
```txt
***************************************
        Author: chris-pan
        Version: 2.0
        A simple Nosql db
***************************************
DataBase > help
Usage: 
A simplest database with dict and doubly-linked list.
 set    [KEY] [VALUE]           - store Key-Value pair in DB. If key have existed in DB, update Value in DB.
 get    [key]                   - use Key to get Value in DB.
 del    [key]                   - delete key-Value pair in DB.
 lpush  [KEY] [VALUE]           - push Value from left to a doubly-linked list in DB.
 lpop   [KEY]                   - pop Value by left from a doubly-linked list in DB.
 rpush  [KEY] [VALUE]           - push Value from right to a doubly-linked list in DB.
 rpop   [KEY]                   - pop Value by right from a doubly-linked list in DB.
 llen   [KEY]                   - show length of a doubly-linked list in DB.
 lrange [KEY] [LEFT] [RIGHT]    - show elements from [LEFT] to [RIGHT] of a doubly-linked list.
 help                           - list command usage.
 exit                           - exit and close DB.
DataBase >
```
## Compile Dynamic Shared Library Using Makefile

compile: 

```bash
make
```

It generates following lines: 

```bash
gcc -fPIC -g -c status.c -o status.o
gcc -fPIC -g -c db.c -o db.o
gcc -fPIC -g -c dbobj.c -o dbobj.o
gcc -fPIC -g -c dict.c -o dict.o
gcc -fPIC -g -c murmurhash.c -o murmurhash.o
gcc -fPIC -g -c dlist.c -o dlist.o
gcc -fPIC -g -c str.c -o str.o
gcc -shared -g -o libdatabase.so status.o db.o dbobj.o dict.o murmurhash.o dlist.o str.o
```

clean: 

```bash
make clean
```

It generates following lines: 

```bash
rm -f status.o db.o dbobj.o dict.o murmurhash.o dlist.o str.o libdatabase.so
```

## Compile Main 
```bash
gcc -o main main.c -L. -ldatabase -Wl,-rpath=.
```
## Compile Memory Leak Check
```bash
gcc -o memoryleak memoryleak.c -L. -ldatabase -Wl,-rpath=.
```
## Compile Benchmark for my database
```bash
gcc -o benchmark benchmark.c -L. -ldatabase -Wl,-rpath=.
```
## Compile Benchmark Test for hiredis
```bash
gcc redistest.c -o redistest -I /usr/local/include/hiredis -lhiredis
```
## Run Benchmark Test
Run `benchmark` & `redistest` for comparison

### Execution time 
**benchmark**
```bash
./test
```
```txt
hash table info:
Buckets:        1572869
Data:           0
Insert  1000000 data    Total 575.527000 ms     average 575.527000 ns
Get     1000000 data    Total 421.570000 ms     average 421.570000 ns
Delete  1000000 data    Total 439.720000 ms     average 439.720000 ns
```

**redistest**
```bash
./redistest
```
```txt
Insert  1000000 data    Total 72656.282000 ms   average 72656.282000 ns
Get     1000000 data    Total 70829.693000 ms   average 70829.693000 ns
Delete  1000000 data    Total 70829.693000 ms   average 70829.693000 ns
```

### Memory Usage & Memory Leak Check
**test**
heap memory usage: 211.61 MB (including dlist test)

```bash
valgrind --leak-check=full --show-reachable=yes -s ./benchmark
```
```txt
==1350== Memcheck, a memory error detector
==1350== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==1350== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==1350== Command: ./benchmark
==1350== 
Insert  1000000 data    Total 9.735902 s        average 9.735902 us
Get     1000000 data    Total 3.800144 s        average 3.800144 us
Delete  1000000 data    Total 5.702249 s        average 5.702249 us

lpush   1000000 data    Total 9.404397 s        average 9.404397 us
rpush   1000000 data    Total 9.596898 s        average 9.596898 us
lpop    1000000 data    Total 5.868238 s        average 5.868238 us
rpop    1000000 data    Total 5.728346 s        average 5.728346 us
==1350==
==1350== HEAP SUMMARY:
==1350==     in use at exit: 0 bytes in 0 blocks
==1350==   total heap usage: 14,145,785 allocs, 14,145,785 frees, 221,889,196 bytes allocated
==1350==
==1350== All heap blocks were freed -- no leaks are possible
==1350==
==1350== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

**redistest**
heap memory usage: 869.44 MB (only set, get, del operation)

```bash
valgrind --leak-check=full --show-reachable=yes ./redistest
```
```txt
==5016== Memcheck, a memory error detector
==5016== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==5016== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==5016== Command: ./redistest
==5016==
Insert  1000000 data    Total 157915.297000 ms    avarage 157915.297000 ns
Get     1000000 data    Total 147801.114000 ms    average 147801.114000 ns
Delete  1000000 data    Total 145200.367000 ms    average 145200.367000 ns
==5016==
==5016== HEAP SUMMARY:
==5016==     in use at exit: 0 bytes in 0 blocks
==5016==   total heap usage: 46,000,026 allocs, 46,000,026 frees, 911,672,858 bytes allocated
==5016==
==5016== All heap blocks were freed -- no leaks are possible
==5016==
==5016== For lists of detected and suppressed errors, rerun with: -s
==5016== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/hiq_Rwwc)
