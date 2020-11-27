# 222.完全二叉树的节点个数

左子树不断遍历得出高度，二分查找+位运算得到最后一个叶结点的位置（自己想出来的，真是个小天才）；事实证明，自己就是得用点草稿纸才会写算法题。

> 明早再用Go做一遍
```python
# Definition for a binary tree node.
# class TreeNode:
#     def __init__(self, x):
#         self.val = x
#         self.left = None
#         self.right = None

class Solution:
    def countNodes(self, root: TreeNode) -> int:
        if root == None:
            return 0
        # 先找出层数
        tmp = root
        depth = 0  # 记录树的最高度
        while tmp != None:
            depth += 1
            tmp = tmp.left
        result = 2**(depth-1)-1 # 上层满树的情况
        # 下层非满树，二分查找
        left = 1
        right = 2**(depth-1)-1
        last = 0

        while left <= right:
            mid = int((left + right)/2)
            if self.hasNode(root, mid, depth):
                last = mid
                left = mid + 1
            else:
                right = mid - 1
        result += (last + 1)
        
        return result

    def hasNode(self, root, path, depth) -> bool:
        tmp = root
        for i in range(depth-2, -1, -1): # 比如高度为4，就要 4, 2, 1
            if 2**i & path == 0:
                tmp = tmp.left
            else:
                tmp = tmp.right
            if tmp == None:
                return False
        return True
```

```Go
/**
 * Definition for a binary tree node.
 * type TreeNode struct {
 *     Val int
 *     Left *TreeNode
 *     Right *TreeNode
 * }
 */
func countNodes(root *TreeNode) int {
	if root == nil {
		return 0
	} else if root.Left == nil && root.Right == nil {
		return 1
	}

	tmp := root
	depth := 0
	for tmp != nil {
		tmp = tmp.Left
		depth++
	}
	result := int(math.Pow(2, float64(depth-1)) - 1) // 上层满树的节点数

	// 二分查找
	left, right := 1, result // 正好right也是2^(d-1)-1
	ans := 0
	for left <= right {
		var mid int = (left + right) / 2
		if hasPath(root, mid, depth) {
			ans = mid
			left = mid + 1
		} else {
			right = mid - 1
		}
	}
	result += (ans + 1)
	return result
}

func hasPath(root *TreeNode, path, depth int) bool {
	tmp := root
	for i := depth-2; i >= 0; i-- {
		if int(math.Pow(2, float64(i)))&path == 0 {
			tmp = tmp.Left
		} else {
			tmp = tmp.Right
		}
		if tmp == nil {
			return false
		}
	}
	return true
}
```