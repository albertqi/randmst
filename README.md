# CS 124: Random Minimum Spanning Trees
## February 22, 2023
## Albert Qi and Steve Dalla

## Table of Contents
1. Pruning Thresholds
2. Results

## 1. Pruning Thresholds

Unif(0, 1), averaged over 16 trials

| $n$         | Max Edge Weight |
| ----------- | --------------- |
| 128         | 0.0436          |
| 256         | 0.0253          |
| 512         | 0.0140          |
| 1024        | 0.00782         |
| 2048        | 0.00401         |
| 4096        | 0.00224         |

3/(x^0.86)



DIM = 2, averaged over 16 trials

| $n$         | Max Edge Weight |
| ----------- | --------------- |
| 128         | 0.148           |
| 256         | 0.109           |
| 512         | 0.0798          |
| 1024        | 0.0565          |
| 2048        | 0.0419          |
| 4096        | 0.0295          |





DIM = 3, averaged over 16 trials

| $n$         | Max Edge Weight |
| ----------- | --------------- |
| 128         | 0.279           |
| 256         | 0.224           |
| 512         | 0.183           |
| 1024        | 0.147           |
| 2048        | 0.116           |
| 4096        | 0.0961          |





DIM = 4, averaged over 16 trials

| $n$         | Max Edge Weight |
| ----------- | --------------- |
| 128         | 0.392           |
| 256         | 0.340           |
| 512         | 0.288           |
| 1024        | 0.243           |
| 2048        | 0.213           |
| 4096        | 0.181           |


$\left(\frac{\log_2 n}{\pi n}\right)^{1/d}$


## 2. Results

Unif(0, 1), averaged over 16 trials, prune for $n\geq 16384$

| $n$         | MST Weight      | Runtime for Graph Construction | Runtime for Prim's |
| ----------- | --------------- | ------------------------------ | ------------------ |
| 128         | 1.201           |                                |                    |
| 256         | 1.208           |                                |                    |
| 512         | 1.212           |                                |                    |
| 1024        | 1.204           |                                |                    |
| 2048        | 1.218           |                                |                    |
| 4096        |                 |                                |                    |
| 8192        |                 |                                |                    |
| 16384       |                 |                                |                    |
| 32768       |                 |                                |                    |
| 65536       |                 |                                |                    |
| 131072      |                 |                                |                    |
| 262144      |                 |                                |                    |