# Parallel Image Processing Gaussian Blur

Sequential and multithreaded Gaussian blur implementation developed using C and POSIX threads.

## Overview

This project compares sequential and parallel approaches for image processing using a Gaussian blur filter.

Two implementations were developed:

- Single threaded implementation
- Multithreaded implementation using POSIX threads

The goal was to analyse performance improvements achieved through parallel execution.

---

## Features

- Gaussian blur image processing
- Sequential implementation
- Parallel implementation
- POSIX threads
- Performance comparison
- Runtime analysis
- CSV result generation
- Image output generation

---

## Technologies Used

- C
- POSIX Threads (Pthreads)
- Linux
- WSL
- CSV performance logging

---

## Files Included

```text
blur_seq.c
blur_mt.c
results.csv
summary.csv
data/
```

---

## Performance Focus

The project evaluates:

- Execution speed
- Thread efficiency
- Resource usage
- Reliability
- Scalability

---

## Academic Context

Developed for the Parallel and Concurrent Programming module as part of the BSc Computer Science programme at the University of South Wales.

---

## Author

Emad Ezzadeen  
BSc Computer Science Graduate  
University of South Wales
