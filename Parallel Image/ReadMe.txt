Project: Gaussian Blur – Sequential vs Multithreaded (Parallel/Concurrent)

This project applies a Gaussian blur filter to an image using two C programs:

1. blur_seq.c = Sequential version  
2. blur_mt.c = Multithreaded version using pthreads (Parallel + Concurrent)

Each version processes the same image and records the execution time.

How to Compile:
gcc blur_seq.c -o blur_seq  
gcc blur_mt.c -o blur_mt -pthread

How to Run:
./blur_seq  
./blur_mt data/input.ppm [num_threads]    Example: ./blur_mt data/input.ppm 4

Outputs:
- Blurred images saved in /data/ as JPG files  
- Timing results saved in: results.csv  
- Average summary saved in: summary.csv  

Environment:
- Must be run on a Linux environment (e.g. Ubuntu)  
- GCC compiler required  
- ImageMagick must be installed (for image conversion with `convert`)

Note:
- The multithreaded version demonstrates both parallel and concurrent processing.  
- The sequential version runs line by line without threading.