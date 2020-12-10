# 输出某个数的二进制1位数

```Go
// pc[i] is the population count of i.
var pc [256]byte // Pow(2, 8) = 256

func init() {
    for i := range pc {
        pc[i] = pc[i/2] + byte(i&1)
    }
}

// PopCount returns the population count (number of set bits) of x.
func PopCount(x uint64) int {
    return int(pc[byte(x>>(0*8))] +
        pc[byte(x>>(1*8))] +
        pc[byte(x>>(2*8))] +
        pc[byte(x>>(3*8))] +
        pc[byte(x>>(4*8))] +
        pc[byte(x>>(5*8))] +
        pc[byte(x>>(6*8))] +
        pc[byte(x>>(7*8))])
}
```
算法流程：
- byte --> uint8; 8-bit 2进制 --> [0, 256)的数，先提前记录好这256个数各自的 1-bit 位数
- 输入一个数, 8位8位地右移, 同时使用 `byte()` 做强制截断, 也即从小端8位8位地往前做计算处理