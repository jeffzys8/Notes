# LRU缓存

使用一个map和一个手动实现的双向链表，这里的诀窍在于设置两个冗余的Head和Tail，就省去了双向链表更新时的很多边界条件的考虑。

```Go
type Node struct {
	Prev  *Node
	Next  *Node
	Key   int
	Value int
}

type LRUCache struct {
	Index map[int]*Node
	Head  *Node
	Tail  *Node
	Max   int
}

func Constructor(capacity int) LRUCache {

	cache := LRUCache{Index: make(map[int]*Node), Max: capacity}
	cache.Head = &Node{}
	cache.Tail = &Node{}
	cache.Head.Next, cache.Tail.Prev = cache.Tail, cache.Head
	return cache
}

func (this *LRUCache) Get(key int) int {
	// not-exist
	node, ok := this.Index[key]
	if !ok {
		return -1
	}
	// already first
	if this.Head.Next == node {
		return node.Value
	}
	// delete from middle
	node.Prev.Next = node.Next
	node.Next.Prev = node.Prev

	// get to the front
	node.Prev = this.Head
	node.Next = this.Head.Next
	this.Head.Next.Prev = node
	this.Head.Next = node

	return node.Value
}

func (this *LRUCache) Put(key int, value int) {
	// exist: update & switch head(not head)
	node, ok := this.Index[key]
	if ok {
		node.Value = value
		this.Get(key)
		return
	}
	// new head & delete tail(if max)
	node = &Node{Key: key, Value: value}
	if len(this.Index) == this.Max {
		// delete tail
		// suppose Max != 0 and Max != 1 (should be considered also)
		last := this.Tail.Prev
		last.Next.Prev = last.Prev
		last.Prev.Next = last.Next
		delete(this.Index, last.Key)
	}
	// add to head
	node.Prev = this.Head
	node.Next = this.Head.Next
	this.Head.Next.Prev = node
	this.Head.Next = node
	this.Index[key] = node
}
```