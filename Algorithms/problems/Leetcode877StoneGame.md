# Leetcode 877 - Stone Game

## 题目

Alex and Lee play a game with piles of stones.  There are an even number of piles arranged in a row, and each pile has a positive integer number of stones piles[i].

The objective of the game is to end with the most stones.  The total number of stones is odd, so there are no ties.

Alex and Lee take turns, with Alex starting first.  Each turn, a player takes the entire pile of stones from either the beginning or the end of the row.  This continues until there are no more piles left, at which point the person with the most stones wins.

Assuming Alex and Lee play optimally, return True if and only if Alex wins the game.


Example 1:
```
Input: [5,3,4,5]
Output: true

Explanation: 
Alex starts first, and can only take the first 5 or the last 5.
Say he takes the first 5, so that the row becomes [3, 4, 5].
If Lee takes 3, then the board is [4, 5], and Alex takes 5 to win with 10 points.
If Lee takes the last 5, then the board is [3, 4], and Alex takes 4 to win with 9 points.
This demonstrated that taking the first 5 was a winning move for Alex, so we return true.
```

Note:

- 2 <= piles.length <= 500
- piles.length is even.
- 1 <= piles[i] <= 500
- sum(piles) is odd.

## 题目分析&题解

[参阅](https://leetcode.com/problems/stone-game/discuss/154610/C%2B%2BJavaPython-DP-or-Just-return-true)

这道题的大概意思就是：有N(N为偶数)堆石头，每堆石头数量不同；有两个人开始轮流取堆，只能从当前剩下的首尾取，最后取到最多石头的一方胜利（因为石头总数是奇数所以不会有平手）。想问下给定一局，先手有没有“必胜序列”？

### 逻辑学？

首先是逻辑学的一个做法——可以证明无论如何先手都可以赢：因为石头的堆数是偶数，所以先手可以保证自己每轮都拿到奇数堆，而致使后手只能拿到偶数堆；同样先手也可以保证自己一直拿奇数堆。这样一来，先手的只要先判断是奇数堆的石头总数较多还是偶数堆的石头总数较多，然后采取相应策略就可以必胜。

实现方法很简单，如果先手要取全部奇数堆，就在第一轮取第一堆，然后无论后手取哪堆，先手在下一轮都取后手取的临近的那一堆；同理，先手要取全部偶数堆，只要在第一轮取最后一堆，然后同上步骤。

代码：
```c++
return true;
```

### 动态规划做法

当然这么简单做完的话，就没啥意思了。如果题目问的是：最多能赢多少个石头？这就不是前面那个做法能求出来的了。

思考两个对手对攻各自的思路，肯定是在每轮都会根据当前的情况求出最好的做法；因此我们可以定义状态 ```d[i][j]``` 为剩下石头堆为 i~j 堆的时候最多可以比对手多拿几个，由于下一轮马上就到对手采取最优策略，所以可以得：
- 假如拿```i```堆，```d[i][j] = pils[i] - d[i+1][j]```
- 假如拿```j```堆，```d[i][j] = pils[j] - d[i][j-1]```

也即状态转移方程为 ```d[i][j] = max{pils[i] - d[i+1][j], d[i][j] = pils[j] - d[i][j-1]}```


C++代码，复杂度为$O(N^2)$：
```c++
bool stoneGame(vector<int>& p) {
    int n = p.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) dp[i][i] = p[i];
    for (int d = 1; d < n; d++)
        for (int i = 0; i < n - d; i++)
            dp[i][i + d] = max(p[i] - dp[i + 1][i + d], p[i + d] - dp[i][i + d - 1]);
    return dp[0][n - 1] > 0;
}
```

助理解的注记：上面代码过程实际上可以理解为左下矩阵从最长的对角线开始，往左下收拢。代码中的`d`即为“剩余阵列宽度”，因此`d`从小到大即为题目描述的对弈的**逆向**过程。

### 优化动态规划

实际上不需要使用二维的空间，只需要一维的空间：

```c++
bool stoneGame(vector<int>& p) {
    vector<int> dp = p; // 复制了
    for (int d = 1; d < p.size(); d++)
        for (int i = 0; i < p.size() - d; i++)
            dp[i] = max(p[i] - dp[i + 1], p[i + d] - dp[i]);
    return dp[0] > 0;
}
```