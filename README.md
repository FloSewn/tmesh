# TMesh
------------------------------------------------------------

<img src="doc/images/qtree.png" alt="Background-Qtree" width="500"/>

## Profiling
* Compile with `-pg` (default in debugging mode)
* Run profiling with: `gprof test gmon.out > prof_output`

### Qtree search benchmark:
| Nodes        | Qtree        | Brute Force  |
| :----------: | :----------: | :----------: |
|        10000 |        0.0 s |        0.6 s |
|        20000 |        0.1 s |        2.6 s |
|        30000 |        0.3 s |        8.2 s |
|        50000 |        0.8 s |       38.4 s |
|       100000 |        3.5 s |      371.7 s | 


### Simple mesh generation benchmark:
| Triangles    | ADF          |
| :----------: | :----------: |
|           46 |   2.25e-03 s |
|          534 |   1.12e-01 s |
|         2164 |   2.01e+00 s |
|         8592 |   4.14e+01 s |

