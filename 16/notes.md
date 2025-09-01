# Day 16 Part 2

This took me quite a while to figure out.
Part 1 was easy, but I could not see how to simplify part 2 so that it is still computable.

I realized that the described task is basically a matrix multiplication.
For example for the 8 element example:

$$
\begin{bmatrix}
1 & 0 & -1 & 0 & 1 & 0  & -1 & 0 \\
0 & 1 & 1  & 0 & 0 & -1 & -1 & 0 \\
0 & 0 & 1  & 1 & 1 & 0  & 0  & 0 \\
0 & 0 & 0  & 1 & 1 & 1  & 1  & 0 \\
0 & 0 & 0  & 0 & 1 & 1  & 1  & 1 \\
0 & 0 & 0  & 0 & 0 & 1  & 1  & 1 \\
0 & 0 & 0  & 0 & 0 & 0  & 1  & 1 \\
0 & 0 & 0  & 0 & 0 & 0  & 0  & 1 \\
\end{bmatrix}
\cdot
\begin{bmatrix}
1 \\
2 \\
3 \\
4 \\
5 \\
6 \\
7 \\
8 \\
\end{bmatrix}
\Rightarrow\
\begin{bmatrix}
4 \\
8 \\
2 \\
2 \\
6 \\
1 \\
5 \\
8 \\
\end{bmatrix}
$$

Notice that we only keep the last digits of the result, since we only need that.

The key insight that makes the large repeating pattern computable is that the bottom right part of the matrix is
entirely composed of ones and zeros, and the offset in the problem points to well beyond the half of the elements.

If we look at only the bottom right part:

$$
\begin{bmatrix}
1 & 1  & 1  & 1 \\
0 & 1  & 1  & 1 \\
0 & 0  & 1  & 1 \\
0 & 0  & 0  & 1 \\
\end{bmatrix}
\cdot
\begin{bmatrix}
5 \\
6 \\
7 \\
8 \\
\end{bmatrix}
\Rightarrow\
\begin{bmatrix}
6 \\
1 \\
5 \\
8 \\
\end{bmatrix}
$$

We can see that the submatrix computes the relevant section of the output.
In fact, any part of the result depends only on the elements of the matrix that are lower than itself (+ itself).
This is true, because the entire pattern forms an upper triangle matrix.

The lower submatrix is even more special, since every element of the upper triangle is $1$.

In general this means that

$$
\begin{bmatrix}
1 & \cdots & 1 \\
\vdots & \ddots & \vdots \\
0 & \cdots & 1 \\
\end{bmatrix}
\cdot
\begin{bmatrix}
a_n \\
\vdots \\
a_0 \\
\end{bmatrix}
\Rightarrow\
\begin{bmatrix}
b_n \\
\vdots \\
b_0 \\
\end{bmatrix}
$$

From this follows that 
$$
b_i = \sum_{0}^{i} {a_i}
$$

For the code we can simplify it even further

$$
b_i = a_i + b_{i-1}
$$

With this formula we can even compute the new values in the array in-place like so:

``` C
for (int i = 1; i < len; i++) {
    arr[i] = arr[i-1] + arr[i];
    arr[i] %= 10;
}
```

All in all in my case I had to compute about 500k elements and repeat it 100 times, which ran in roughly 125 ms on my machine.
