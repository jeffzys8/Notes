package main

import (
	// "sort"
	"encoding/json"
	"fmt"
	"log"
	"sort"
)

func eventualSafeNodes(graph [][]int) []int {
	// 拓扑排序
	result := []int{}

	// 将图反向
	// 同时记录每个点的outDegree
	// 同时将outDegree为0的点加入队列
	rgraph := [][]int{}
	outDegrees := []int{}
	rq := []int{}
	for i := 0; i < len(graph); i++ {
		rgraph = append(rgraph, []int{})
	}
	for i := 0; i < len(graph); i++ {
		outDegrees = append(outDegrees, len(graph[i]))
		if len(graph[i]) == 0 {
			rq = append(rq, i)
		}
		for e := 0; e < len(graph[i]); e++ {
			v := graph[i][e]
			rgraph[v] = append(rgraph[v], i)
		}
	}

	// 逐个删除节点并加入结果队列
	for len(rq) != 0 {
		top := rq[0]
		for _, i := range rgraph[top] {
			outDegrees[i]--
			if outDegrees[i] == 0 {
				rq = append(rq, i)
			}
		}
		result = append(result, top)
		rq = rq[1:]
	}

	sort.Ints(result)
	return result
}

func main() {
	fmt.Println("hello")
	graphStr := `[[1,2],[2,3],[5],[0],[5],[],[]]`
	var graph [][]int
	if err := json.Unmarshal([]byte(graphStr), &graph); err != nil {
		log.Fatal(err)
	}
	fmt.Println(eventualSafeNodes(graph))
}
