# 单例模式 (C++线程安全)

## 懒汉模式

第一次调用该类实例的时候才产生一个新的该类实例，并在以后仅返回此实例。

多个线程可能进入判断是否已经存在实例的if语句，从而线程不安全，使用double-check来保证thread safety.但是如果处理大量数据时，该锁才成为严重的性能瓶颈。

```c++
class Singleton
{
private:
	static Singleton* m_instance;
	Singleton(){}
public:
	static Singleton* getInstance();
};
  
Singleton* Singleton::getInstance()
{
	if(NULL == m_instance)
	{
		Lock();	//借用其它类来实现，如boost
		if(NULL == m_instance)
		{
			m_instance = new Singleton;
		}
		UnLock();
	}
	return m_instance;
}
```

C++0X以后，要求编译器保证内部静态变量的线程安全性，可以不加锁。但C++ 0X以前，仍需要加锁

```c++
class SingletonInside
{
private:
	SingletonInside(){}
public:
	static SingletonInside* getInstance()
	{
		Lock(); // not needed after C++0x
		static SingletonInside instance;
		UnLock(); // not needed after C++0x
		return instance; 
	}
};
```

## 饿汉模式

在程序开始时就会产生一个该类的实例，并在以后仅返回此实例。

由静态初始化实例保证其线程安全性，因为静态实例初始化在程序开始时进入主函数之前就由主线程以单线程方式完成了初始化，不必担心多线程问题。

故在性能需求较高时，应使用这种模式，避免频繁的锁争夺。

```c++
class SingletonStatic
{
private:
	static const SingletonStatic* m_instance;
	SingletonStatic(){}
public:
	static SingletonStatic* getInstance()
	{
		return m_instance;
	}
};

//外部初始化 before invoke main
const SingletonStatic* SingletonStatic::m_instance = new SingletonStatic;
```


问题是：m_pInstance指向的空间什么时候释放呢？更严重的问题是，该实例的析构函数什么时候执行？

我们需要一种方法，正常的删除该实例。一个妥善的方法是让这个类自己知道在合适的时候把自己删除，或者说把删除自己的操作挂在操作系统中的某个合适的点上，使其在恰当的时候被自动执行。

我们知道，程序在结束的时候，系统会自动析构所有的全局变量。事实上，**系统也会析构所有的类的静态成员变量**，就像这些静态成员也是全局变量一样。利用这个特征，我们可以在单例类中定义一个这样的静态成员变量，而它的唯一工作就是在析构函数中删除单例类的实例。

```c++
class CSingleton
{
public:
	static CSingleton* GetInstance();
private:
    CSingleton(){};
    static CSingleton * m_pInstance;
	class CGarbo 	//它的唯一工作就是在析构函数中删除CSingleton的实例
	{
	public:
		~CGarbo()
		{
			if( CSingleton::m_pInstance )
				delete CSingleton::m_pInstance;
		}
	}
	static CGabor Garbo; //定义一个静态成员，程序结束时，系统会自动调用它的析构函数
};

//外部初始化 before invoke main
const CSingleton* CSingleton::m_pInstance = new CSingleton;
```