# 编辑距离

编辑距离指的是

```
Input: two strings x and y.
		x: SNOWY
		y: SUNNY
Output: minimum number of edit operations to transform x into y.

Edit operations: insertion, deletion, substitution.
```

动态规划的解法通常分为两步骤：
1. 定义状态的表示方法
2. 确定状态转移方程

首先是状态的定义: 假设有字符串`x[0...n]`, `y[0...m]`;<br>
则设`E(i,j)`为子串`x[0...i]`, `y[0...j]`的编辑距离（E就是 **状态**) 

然后对状态转移方程进行定义: 
- `i=0` 时, `E(i,j) = j`
- `j=0` 时, `E(i,j) = i`
- 由上可有 `E(0,0) = 0`

> 上面三行把二维数组的第一行和第一列都定义好了

- 其余情况下:
    ```
    E(i, j) = min 
        {   
            1 + E(i-1, j),
            1 + E(i, j-1), 
            diff(i, j) + E(i–1, j-1)
        }
    其中，当x[i]=y[j]时， diff(i, j)=0；否则diff(i, j)=1.
    ```

> 编辑距离的dp算法的正确性该如何证明？中间每一步都像是贪心