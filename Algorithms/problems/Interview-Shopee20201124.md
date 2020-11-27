# 虾皮的两道“简单”题

> 我先总结一下问题，然后用python和Go各做一遍

## 题目概述、思路、总结

1. 求出字符串中首次重复的字符

比如 `abbac` 从左到右首次重复的字符 就是 `b`

2. 求出字符串中所有重复字符里最左的重复字符

比如`abbac` 重复字符有`a`和`b`，其中`a`最先出现，所以答案是`b`

**值得注意的是，两道题都限定了出现的字符全都是`a-z`**

1. **过于着急**，在解第一题的时候一开始忽略关键题目信息：`a-z`，因此一开始选用`map`去做，然后还扯了一大堆什么哈希和红黑树之类的，时间浪费。
2. 其实仔细看这两道题目，要做出来不难，但是要寻求最优解的时候（特别是第二题）还是有一些思考难度的，所以当时也是**一开始轻敌，随后慌张而没有用草稿去做题**，思路当然就很涣散。
3. 对Go语法的不熟，**没有本地用VSCode搭一个lint环境**，导致频繁run耽误了时间；但其实从这几天刷题的角度来讲，Go还是相当好用的，特别是树、图这些操作，字符串也不错。所以下次面试，还是可以**Go、Python二选一，两个环境都搭一下**
4. ps：这胖面试官着实有点脸黑（不是找借口，事实上就是受到了影响）

## 第一题题解

Go:
```Go
func firstRepeatedLetter(input string) rune {
	var mark int = 0
	for _, v := range input {
		if mark&(1<<uint(v-'a')) > 0 {
			return v
		}
		mark |= (1 << uint(v-'a'))
	}
	return '0'
}
```

Python:
```python
def first_repeated_letter(input: str)->str:
    mark = 0
    for i in range(len(input)):
        tmp = ord(input[i]) - ord('a') # 0~25
        if mark & (1 << tmp) != 0:
            return input[i]
        mark |= (1 << tmp)
    return '0'
```

## 第二题题解

Go:
```Go
func leftMostRepeatedLetter(input string) rune {
	mark := [26]int{}
	for i := 0; i < 26; i++ {
		mark[i] = -1
	}
	min := len(input)
	for i, v := range input {
		if mark[v-'a'] == -1 {
			mark[v-'a'] = i
		} else if mark[v-'a'] < min {
			min = mark[v-'a']
		}
	}
	if min < len(input) {
		return rune(input[min])
	}
	return '0'
}
```

```python
def left_most_repeated_letter(input: str)->str:
    mark = [-1] * 26
    min_result = len(input)
    for i in range(len(input)):
        tmp = ord(input[i]) - ord('a') # 0~25
        if mark[tmp] == -1:
            mark[tmp] = i
        else:
            min_result = min(min_result, mark[tmp])
    if min_result == len(input):
        return '0'
    return input[min_result]
```