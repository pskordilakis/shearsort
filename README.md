SHEARSORT ALGORITHM
===================

Algorithm Description
---------------------

Parallel sorting algorithm, designed to be implemented in array of processors.
The algorithm has two phases that are being executed alternately until the array
is sorted. 

In the first phase all the rows are sorted. Even rows are sorted to the left and
odd rows are sorted to the right.

In the second phase all columns are sorted. Columns are sorted upward.

Shearsort needs log N + 1 phases to sort the array. Numbers of array are sorted
in snakelike order.

Odd-Even Transposition
----------------------

Shearsort algorithm uses another sorting algorithm to sort rows and columns.
This algorithm is odd-even transposition.

Odd-even transposition is used to sort numbers in a vector. This algorithm also
has two phases that are executed alternately until the vector is sorted.

In the first phase compare and exchange operations are executed for all even 
indexed i and i+1 numbers.

In the second phase same operations are executed for the odd indexed j and j+1
numbers.

Odd-even transposition has worst-case time O(N)

0-1 Sorting Lemma
-----------------

To show that the algorithm produce correct results for every input, 0-1 sorting
lemma is used.

Lemma: If an oblivius comparison-exchange algorithm sorts all inputs sets 
consisting of 0s and 1s, then it sorts all input set with arbitrary values.

Implementation
--------------

The algorithm is implemented in C++ and OpenCL.

Dependencies

1. C++ compiler
2. cmake
3. OpenCL 1.1 capable GPU
4. OpenCL 1.1 compatible GPU drivers
