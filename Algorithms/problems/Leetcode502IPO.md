# LeetCode 502 - IPO

## 题目

**S**uppose LeetCode will start its IPO soon. In order to sell a good price of its shares to Venture Capital, LeetCode would like to work on some projects to increase its capital before the IPO. Since it has limited resources, it can only finish at most k distinct projects before the IPO. Help LeetCode design the best way to maximize its total capital after finishing at most k distinct projects.

**Y**ou are given several projects. For each project i, it has a pure profit Pi and a minimum capital of Ci is needed to start the corresponding project. Initially, you have W capital. When you finish a project, you will obtain its pure profit and the profit will be added to your total capital.

**T**o sum up, pick a list of at most k distinct projects from given projects to maximize your final capital, and output your final maximized capital.

Example 1:
```Shell
Input: k=2, W=0, Profits=[1,2,3], Capital=[0,1,1].

Output: 4

Explanation:

Since your initial capital is 0, you can only start the project indexed 0.
After finishing it you will obtain profit 1 and your capital becomes 1.
With capital 1, you can either start the project indexed 1 or the project indexed 2.
Since you can choose at most 2 projects, you need to finish the project indexed 2 to get the maximum capital.
Therefore, output the final maximized capital, which is 0 + 1 + 3 = 4.
```
Note:
- You may assume all numbers in the input are non-negative integers.
- The length of Profits array and Capital array will not exceed 50,000.
- The answer is guaranteed to fit in a 32-bit signed integer.

## 思路

显然这是一道使用贪心算法的题目：每轮应该可选的项目里挑利润最大的（每个项目只能选一次），这样所带来的```accumulative capital```才是最大的，而```capital```越大，之后能选择的范围也就越大

由于不存在```capital```增加以后，原来可选的项目反而变得不能选了，因此可以“贪心”地每次都选择可选范围内利润最大的。

首先能想到的就是**暴力解法**，在每轮筛选中直接遍历整个容器，对每一个项目都判断是否可选以及是否是本轮最大值。这样的复杂度是$O(N^2)$。当然，要记得每轮把选择过的项目剔除，在`k`较大时可以一定程度优化循环。

当然上述并不是好的解法，我们重复判断了那些在中间过程中已经被判断为“可选”的项目，这时我们可以想到**用额外的空间去存储当前可选项目**。

首先我们对`Profits`和`Capital`数组进行合并，并按`Capital`排序。随后我们定义一个游标从该合并数组开始，将所有当前可行的项目逐个取出并排序（**插入排序**）。这样我们每轮都可以在取出动作结束后直接获得利润最大值并加到`result`中。并在第二轮的项目选取中，我们可以直接从上次游标停下的地方继续搜索合并数组，将新的可行项目取出并排序。

这里需要注意的是，不仅`k`是程序停止的信号，排序项目中是否仍有可行项目也需要纳入考虑。另外，对于这个排序项目容器的需求，我们可以直接使用**优先级队列**来实现。

## 代码

```c++
#include <algorithm>
#include <queue>

using namespace std;

bool compare(pair<int, int>a, pair<int, int> b){
    return (a.first < b.first);
}

class Solution {
public:
    

    int findMaximizedCapital(int k, int W, vector<int>& Profits, vector<int>& Capital) {
        int result = W;
        
        // 合并两个vector
        vector< pair<int, int> > store;
        vector<int>::iterator pIter = Profits.begin();
        vector<int>::iterator cIter = Capital.begin();    
        while(pIter != Profits.end()){
            store.push_back(make_pair(*cIter, *pIter));
            cIter ++;
            pIter ++;
        }
        // 对合并vector基于Capital从小到大排序
        sort(store.begin(), store.end(), compare);
        
        vector< pair<int, int> >::iterator iter = store.begin();
        priority_queue<int> profit_queue;
        for(int i = 0; i < k; ++i){
            while(iter != store.end() && iter->first <= result){
                profit_queue.push(iter->second);
                ++iter;
            }

            if(!profit_queue.empty()){
                result += profit_queue.top();
                profit_queue.pop();
            }
        }
        return result;
    }
};
```
