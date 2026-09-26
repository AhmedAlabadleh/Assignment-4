# Assignment 4: Many Matrix Multiplications

At the core of machine learning is a simple operation-- matrix multiplication. In this assignment you will be implementing a simple matrix multiplication engine for the purpose of exploring how our computer systems respond to intensive computational pressure.


## Part 1: Implementing Matrix Multiplication

You will implement a simple matrix multiplication C program.

Matrix multiplication takes the dot product of a row of input matrix A and a column of input matrix B to calculate an entry in matrix C:

$$c_{ij} = a_{i1} b_{1j} + a_{i2} b_{2j} + \cdots + a_{in} b_{nj} = \sum_{k=1}^n a_{ik} b_{kj}$$


The C program should take in command line arguments as follows.
```
Usage: ./bench <rows1> <cols1> <cols2> <run forever? 0/1>
```

Your matrix multiplication program should implement four functions:

- `generate_random_matrix(int nrows, int ncols)` which generates a nrows x ncols matrix filled with random values using `rand()`.
- `multiply_matrices(int rows1, int cols1, int *matrix1, int rows2, int cols2, int *matrix2, int *result)` which generates a new output matrix by performing a matrix multiply between input matrix A and B.
- `void display_matrix(int rows, int cols, int *matrix)` prints the matrix to STDOUT for debugging purposes. 
- `int do_job(int rows1, int cols1, int cols2, int forever)` the main "engine" of the program. It should generate matrices of the specified size, multiply them, either one time if forever =0, or in a loop if forever=1.

```shell
$ ./bench 100 100 100 0
Generating Matrices...Matrix 1 done.
Matrix 2 done.
Thu May 21 11:22:53 2026 100x100 matrices: 0.003189 seconds
```

> [!IMPORTANT]
> **TASK:** Complete the code in matrix.c to perform a matrix multiplication between two randomly generated matrixes. 


## Part 2: Varying the Size of the Matrix

You will now run a small experiment on the execution time of your matrix multiplication program.

> [!IMPORTANT]
> **TASK:** Add the ability to run a timed matrix multiplication job. This job should randomly generate two NxN matrices and time the how long the multiplication takes.

Your timing output must include output that follows the EXACT format below:
```
100x100 matrices: 0.008961 seconds
```

To time some code in C:

```c
struct timespec t0, t1;

timespec_get(&t0, TIME_UTC);  // C11 feature

// WHAT YOU WANT TO TIME

timespec_get(&t1, TIME_UTC);  // C11 feature

// nano seconds elapsed converted to fractional seconds
float dns = (float)(t1.tv_nsec - t0.tv_nsec) / 1000000000;
// seconds elapsed
float ds = (float)(t1.tv_sec - t0.tv_sec);

float total_time = dns+ds;

```

You have been provided with bash script `bench-single.sh` which creates a small loop to invoke your matrix multiplication program on various size matrices and save the output in a file with redirection: `> data/EXPERIMENT_NAME/mm-i.out`.

It also saves the CPU percentage in a file as well: `> data/EXPERIMENT_NAME/mm-i-cpu.out`

Finally, you may use the provided `plot.py` to create a graph of your results.

To run the plotting script, use the following command:
```
python3 plot.py <results_folder> <output image path>
```

`results_folder` should match the corresponding experiment and be in the form: `data/EXPERIMENT_NAME`.

> [!IMPORTANT]
> **TASK:** Time and run matrix multiplication tests!
> - Run the bash testing script and python plotting programs. Inspect the result of the experiment, comparing the execution time and CPU usage percent of matrix multiplications ranging from 100x100 to 1000x1000. 
> - Save testing result output files in directory `data/bench-single`. Save these file as `plot-single.png` for each size.
> - Note: Depending on your physical machine, this test may take a long time to run. If it is taking FAR too long, change the sizes that `bench-single.sh` is using to attempt smaller matrix multiplications instead.


## Part 3: Varying the Number of Processes

Let's consider starting 10 matrix multiplications at the same time.

Your output plot for a single run of bench-multi.sh and plot.py should resemble something like the following:

<img src = "Figure_1.png"/>

> [!IMPORTANT]
> **TASK:** Use the provided python plotting program `plot.py` and bash tester script `bench-multi.sh` that times the matrix multiplication and CPU usage percentage with 10 concurrently running matrix multiplications. Run this experiment with 500x500 sized matrices. Save this file as `plot-multi.png`.
> - In `questions.txt` answer the following question: What do you notice about the output compared to `plot-single.png`? Label your answer `(1)`.
> - Note: Depending on your physical machine, this test may take a long time to run. If it is taking FAR too long, change the sizes/number of processes that `bench-multi.sh` is using to attempt smaller matrix multiplications instead.

## Part 3: Varying the CPU Resource

The `taskset` command allows us to restrict which CPU the process can be scheduled on.

For example, the following command:
```
taskset -c 0 ./bench 100 100 100 0
```
Forces the 100x100 matrix multiplication to run on CPU 0.

> [!IMPORTANT]
> **TASK:** Modify the bash tester script, creating `bench-taskset.sh`, such that the different processes are all scheduled on the same CPU. Plot the time the matrix multiplication and CPU usage percentage as the number of processes increases. `plot-taskset.png`
> - In `questions.txt` answer the following question: What do you notice about the output compared to `plot-multi.png`? Label your answer `(2)`.
> - Note: Depending on your physical machine, this test may take a long time to run. If it is taking FAR too long, change the sizes/number of processes that `bench-taskset.sh` is using to attempt smaller matrix multiplications instead.

## Part 4: Varying the Scheduling Priorities

The `nice` command allows you to schedule different processes with different priorities. 

For example:
```
nice -n 10 ./bench 100 100 100 0
```

Schedules a 100x100 matrix multiplication with "niceness" 10. Niceness ranges from -20 to 19 with the default being 0. A process that is "nicer" will step aside and allow other "less nice" processes to run more frequently.

> [!IMPORTANT]
> **TASK:** Modify your bash tester script, creating `bench-nice.sh`, to schedule some matrix multiplication tasks as low-priority (high niceness) and some as high-priority (low niceness). Save this plot as `plot-nice.png`. 
> - In `questions.txt` answer the following question: What do you notice about the output compared to `plot-multi.png`? Label your answer `(3)`.
> - Note: Depending on your physical machine, this test may take a long time to run. If it is taking FAR too long, change the sizes/number of processes that `bench-nice.sh` is using to attempt smaller matrix multiplications instead.


## Part 5: Interactive Job Dispatching

Now, we will create an “interactive” version of the matrix multiplication engine. The program accepts a list of matrix multiplication jobs and runs them using a selected scheduling policy.

A **job of size `n`** consists of generating two random `n × n` matrices and multiplying them together. Therefore, each number in the comma-separated list represents one complete matrix multiplication job.

We will implement two scheduling policies:

- **FIFO (first-in, first-out):** Runs the jobs in the order in which they were provided.
- **SJF (shortest job first):** Runs the jobs in ascending order by matrix size.

Matrix multiplication has a useful property for this exercise: larger matrices generally require more time to multiply. Therefore, the matrix dimension can be used as an estimate of a job’s execution time.

The program will accept command-line arguments in the following format:

```text
Usage: ./interactive <FIFO/SJF> <job_sizes_comma_separated>
```

For example:

```bash
./interactive SJF 100,200,50
```

This command specifies three jobs:

1. Multiply two `100 × 100` matrices.
2. Multiply two `200 × 200` matrices.
3. Multiply two `50 × 50` matrices.

With FIFO scheduling, these jobs would run in the order `100, 200, 50`. With SJF scheduling, they would run in the order `50, 100, 200`.

> [!IMPORTANT]
> **TASK:** Create `interactive.c` which allows users to input in new job requests via the command line. Implement a simple shortest-job-first scheduling policy in your interactive matrix multiplication program.

At this point you should pass all tests `source test.sh`.

## Submitting on Gradescope

To submit on Gradescope, submit all the files in this directory to the assignment upload.

You do not need to upload the `data/` subdirectory.

**DO NOT upload a zip.** Use Shift to select all the files in your assignment directory instead.

Note: To download files from google colab, navigate to the `Assignment-4` directory that should be saved in your **Google Drive**. (Assuming you did all your work in `/content/drive/MyDrive/Assignment-4`). Clicking the three vertical dots shows a "download" option that will download all files to your local computer for upload to gradescope.

You should see the autograder run and report a score. Ensure that you are happy with this score! Feel free to resubmit as many times as you wish before the deadline.
