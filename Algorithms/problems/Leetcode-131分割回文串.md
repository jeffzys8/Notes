# 131. 分割回文串

https://leetcode-cn.com/problems/palindrome-partitioning/

递归+动归做法

```Go
func partition(s string) [][]string {
	save := make([][][][]string, len(s)) // 保存已知的子串的回文序列, save[l][r] 表示s[l:r+1]的回文串组合列表
	for i := 0; i < len(s); i++ {
		save[i] = make([][][]string, len(s))
		for e := 0; e < len(s); e++ {
			save[i][e] = [][]string{}
		}
	}
	for i := 0; i < len(s); i++ {
		save[i][i] = append(save[i][i], []string{string(s[i])}) // 确定了单个字符可构成回文
	}
	calculate(s, 0, len(s)-1, save)
	return save[0][len(s)-1]
}

func calculate(s string, l, r int, save [][][][]string) {
	if l > r || len(save[l][r]) > 0 {
		return
	}
	for i := l; i <= r; i++ {
		if judge(s[l : i+1]) {
			tmp := []string{s[l : i+1]}
			calculate(s, i+1, r, save)
			if i == r {
				save[l][r] = append(save[l][r], tmp)
				continue
			}
			for _, k := range save[i+1][r] {
				// k:[]string
				save[l][r] = append(save[l][r], append(tmp, k...))
			}
		}
	}
}

// 判断是否回文
func judge(s string) bool {
	for i, e := 0, len(s)-1; i <= e; {
		if s[i] != s[e] {
			return false
		}
		i++
		e--
	}
	return true
}
```

TODO: 最长回文子串，[马拉车算法](https://zhuanlan.zhihu.com/p/70532099) 