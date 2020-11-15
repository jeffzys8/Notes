package main

import "fmt"

func main() {
	list := []int{}
	var num int
	fmt.Scanln(&num)

	for i := 0; i < num; i++ {
		var tmp int
		fmt.Scan(&tmp)
		list = append(list, tmp)
	}

	fmt.Println(list)

	partition(list)

	fmt.Println(list)
}

func partition(list []int) {
	if len(list) <= 1 {
		return
	}

	l := 1
	r := len(list) - 1
	pivot := list[0] // 以list首为轴
	for {
		fmt.Println(l, r)
		for list[l] < pivot {
			if l == len(list)-1 {
				break
			}
			l++
		}
		for pivot < list[r] {
			if r == 0 {
				break
			}
			r--
		}
		if l >= r {
			break
		}
		list[l], list[r] = list[r], list[l]
	}
	list[0], list[r] = list[r], list[0]
	partition(list[0:r])
	partition(list[r+1:])
}
