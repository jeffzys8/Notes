/*
爬虫并发实践：https://tour.go-zh.org/concurrency/10
*/

package main

import (
	"fmt"
	"sync"
)

type Fetcher interface {
	// Fetch 返回 URL 的 body 内容，并且将在这个页面上找到的 URL 放到一个 slice 中。
	Fetch(url string) (body string, urls []string, err error)
}

// Crawl 使用 fetcher 从某个 URL 开始递归的爬取页面，直到达到最大深度。
func Crawl(url string, depth int, fetcher Fetcher, ch chan bool) {
	// TODO: 并行的抓取 URL。
	// TODO: 不重复抓取页面。
	// 下面并没有实现上面两种情况：

	defer func() { ch <- true }() // 提示这层深搜完成
	if depth <= 0 {
		return
	}

	mux.Lock()
	if _, ok := m[url]; ok {
		mux.Unlock()
		return
	}
	mux.Unlock()

	body, urls, err := fetcher.Fetch(url)

	// 即使是报错也标记为已搜寻
	mux.Lock()
	m[url] = true
	mux.Unlock()

	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("found: %s %q\n", url, body)
	nCh := make(chan bool, len(urls)) // 作为结束信号
	for _, u := range urls {
		go Crawl(u, depth-1, fetcher, nCh)
	}

	for range urls {
		<-nCh // 等待len(urls)个结果
	}

	return
}

func main() {
	ch := make(chan bool)
	go Crawl("https://golang.org/", 4, fetcher, ch)
	<-ch
}

// fakeFetcher 是返回若干结果的 Fetcher。
type fakeFetcher map[string]*fakeResult

type fakeResult struct {
	body string
	urls []string
}

func (f fakeFetcher) Fetch(url string) (string, []string, error) {
	if res, ok := f[url]; ok {
		return res.body, res.urls, nil
	}
	return "", nil, fmt.Errorf("not found: %s", url) // mark: fmt.Errorf
}

// fetcher 是填充后的 fakeFetcher。
var fetcher = fakeFetcher{
	"https://golang.org/": &fakeResult{
		"The Go Programming Language",
		[]string{
			"https://golang.org/pkg/",
			"https://golang.org/cmd/",
		},
	},
	"https://golang.org/pkg/": &fakeResult{
		"Packages",
		[]string{
			"https://golang.org/",
			"https://golang.org/cmd/",
			"https://golang.org/pkg/fmt/",
			"https://golang.org/pkg/os/",
		},
	},
	"https://golang.org/pkg/fmt/": &fakeResult{
		"Package fmt",
		[]string{
			"https://golang.org/",
			"https://golang.org/pkg/",
		},
	},
	"https://golang.org/pkg/os/": &fakeResult{
		"Package os",
		[]string{
			"https://golang.org/",
			"https://golang.org/pkg/",
		},
	},
}

var m = map[string]bool{}

var mux = sync.Mutex{}
