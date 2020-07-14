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


### Simple mesh generation benchmark (no edge swap):
| Triangles    | ADF          |
| :----------: | :----------: |
|           29 |   7.22e-04 s |
|          106 |   2.88e-03 s |
|          464 |   6.56e-03 s |
|         1940 |   3.44e-02 s |
|        11396 |   1.48e-01 s |
|        45842 |   5.76e-01 s |
|       184070 |   2.90e+00 s |
|      1055218 |   2.03e+01 s |
|      4221754 |   9.13e+01 s |

