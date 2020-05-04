# LeetCode 338 - Counting Bits

## 题目及思路

给定`N`，求`[0,N]`之间每个整数的二进制表示中1的位数之和。

例：
```
Input: 5
Output: [0,1,1,2,1,2]
```

小小的动态规划题，$O(N)$复杂度，核心就在于:

```
f[i] = f[i/2] + i%2
```

这里除2的操作还用到了位运算`>>`，这是个常见优化。

## 代码

```c++
vector<int> countBits(int num) {
    vector<int> result;
    result.push_back(0);
    for(int i = 1; i <= num; ++i){
        result.push_back(result[i>>1] + (i & 1));
    }
    return result;
}
```