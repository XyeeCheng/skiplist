#include<iostream>
#include<string>
#include<set>
#include<time.h>
#include<cstdint>

using namespace std;

// 定义一个模板结构体 Less，用于比较两个 T 类型的对象，返回 a < b 的结果
template<typename T>
struct Less
{
	// 重载函数调用运算符，使其成为一个函数对象（仿函数）
	bool operator()(const T& a, const T& b) const
	{
		return a < b;
	}
};

// 定义一个跳表类模板，K 为键的类型，V 为值的类型，Comp 为比较器类型，默认使用 Less<K>
template<typename K, typename V, typename Comp = Less<K>>
class skip_list
{
private:
	// 定义跳表节点结构体
	struct skip_list_node
	{
		int level;  // 节点所在的层级
		const K key;  // 节点的键，类型为 K，且为常量
		V value;  // 节点的值，类型为 V
		skip_list_node** forward;  // 指向后续节点的指针数组，用于在不同层级上连接其他节点

		// 默认构造函数，初始化键、值、层级为 0，forward 指针为 nullptr
		skip_list_node() : key{ 0 }, value{ 0 }, level{ 0 }, forward{ 0 }
		{
		}

		// 带参数的构造函数，用给定的键、值、层级初始化节点，并为 forward 数组分配内存，将每个元素初始化为 nxt
		skip_list_node(K k, V v, int l, skip_list_node* nxt = nullptr) : key(k), value(v), level(l)
		{
			forward = new skip_list_node* [level + 1];
			for (int i = 0; i <= level; ++i) forward[i] = nxt;
		}

		// 析构函数，释放 forward 数组的内存
		~skip_list_node()
		{
			delete[] forward;
		}
	};

	// 为 skip_list_node 定义一个别名 node，方便后续使用
	using node = skip_list_node;

	// 初始化跳表，设置随机种子，将当前层级和长度初始化为 0，为头节点的 forward 数组分配内存，并将每个元素指向尾节点
	void init()
	{
		srand((uint32_t)time(NULL));  // 设置随机种子
		level = length = 0;  // 初始化当前层级和长度为 0
		head->forward = new node* [MAXL + 1];  // 为头节点的 forward 数组分配内存
		for (int i = 0; i <= MAXL; i++)
			head->forward[i] = tail;  // 将每个元素指向尾节点
	}

	// 随机生成节点的层级，通过不断检查随机数是否小于 PS 来决定是否提升层级，最终返回不超过 MAXL 的层级
	int randomLevel()
	{
		int lv = 1;
		while ((rand() & S) < PS) ++lv;  // 不断检查随机数是否小于 PS，若小于则提升层级
		return MAXL > lv ? lv : MAXL;  // 返回不超过 MAXL 的层级
	}

	int level;  // 跳表当前的最大层级
	int length;  // 跳表中节点的数量
	static const int MAXL = 32;  // 跳表的最大层级，这里设为 32
	static const int P = 4;  // 用于随机层级计算的参数
	static const int S = 0xFFFF;  // 随机数掩码，值为 0xFFFF
	static const int PS = S / P;  // 随机层级计算的阈值，为 S / P
	static const int INVALID = INT_MAX;  // 无效值，设为 INT_MAX
	node* head, * tail;  // 跳表的头节点和尾节点指针
	Comp less;  // 比较器对象，用于比较键的大小

	// 在跳表中查找键为 key 的节点，并记录查找路径上每个层级的前驱节点到 update 数组中
	node* find(const K& key, node** update)
	{
		node* p = head;  // 从跳表的头节点开始查找
		for (int i = level; i >= 0; i--)  // 从最高层级开始，逐层向下查找
		{
			while (p->forward[i] != tail && less(p->forward[i]->key, key))  // 找到第一个键不小于 key 的节点
			{
				p = p->forward[i];
			}
			update[i] = p;  // 记录每个层级的前驱节点到 update 数组中
		}
		p = p->forward[0];  // 最后返回最低层级上找到的节点
		return p;
	}

public:
	// 跳表的迭代器结构体，用于遍历跳表中的节点
	struct Iter
	{
		node* p;  // 迭代器指向的节点指针

		// 默认构造函数，将指针初始化为 nullptr
		Iter() : p(nullptr)
		{
		};

		// 带参数的构造函数，用给定的节点指针初始化迭代器
		Iter(node* rhs) : p(rhs)
		{
		}

		// 重载箭头运算符，返回节点指针
		node* operator->() const
		{
			return (p);
		}

		// 重载解引用运算符，返回节点的引用
		node& operator*() const
		{
			return *p;
		}

		// 重载相等运算符，用于比较两个迭代器是否指向同一个节点
		bool operator==(const Iter& rhs)
		{
			return rhs.p == p;
		}

		// 重载不相等运算符，用于比较两个迭代器是否指向同一个节点
		bool operator!=(const Iter& rhs)
		{
			return !(rhs.p == p);
		}

		// 重载前置自增运算符，将迭代器指向下一个节点
		void operator++()
		{
			p = p->forward[0];
		}

		// 重载后置自增运算符，将迭代器指向下一个节点
		void operator++(int)
		{
			p = p->forward[0];
		}
	};

	// 默认构造函数，创建头节点和尾节点，使用默认的比较器，调用 init() 函数进行初始化
	skip_list() : head(new node()), tail(new node()), less{ Comp() }
	{
		init();
	}

	// 带比较器参数的构造函数，创建头节点和尾节点，使用给定的比较器，调用 init() 函数进行初始化
	skip_list(Comp _less) : head(new node()), tail(new node()), less{ _less }
	{
		init();
	}

	// 在跳表中插入键值对 (key, value)
	void insert(const K& key, const V& value)
	{
		node* update[MAXL + 1];  // 用于记录查找路径上每个层级的前驱节点
		node* p = find(key, update);  // 调用 find 函数查找键为 key 的节点，并记录查找路径
		if (p->key == key)  // 如果键已存在，则更新该节点的值
		{
			p->value = value;
			return;
		}
		int lv = randomLevel();  // 随机生成新节点的层级
		if (lv > level)  // 如果新节点的层级大于当前跳表的最大层级，则更新跳表的最大层级，并更新 update 数组
		{
			lv = ++level;
			update[lv] = head;
		}
		node* newNode = new node(key, value, lv);  // 创建新节点
		for (int i = lv; i >= 0; --i)  // 将新节点插入到相应的位置
		{
			p = update[i];
			newNode->forward[i] = p->forward[i];
			p->forward[i] = newNode;
		}
		++length;  // 增加跳表的长度
	}

	// 从跳表中删除键为 key 的节点
	bool erase(const K& key)
	{
		node* update[MAXL + 1];  // 用于记录查找路径上每个层级的前驱节点
		node* p = find(key, update);  // 调用 find 函数查找键为 key 的节点，并记录查找路径
		if (p->key != key) return false;  // 如果键不存在，则返回 false
		for (int i = 0; i <= p->level; ++i)  // 更新查找路径上每个层级的前驱节点的 forward 指针，跳过要删除的节点
		{
			update[i]->forward[i] = p->forward[i];
		}
		delete p;  // 释放要删除节点的内存
		while (level > 0 && head->forward[level] == tail) --level;  // 如果删除节点后，最高层级为空，则降低跳表的最大层级
		--length;  // 减少跳表的长度
		return true;  // 返回 true 表示删除成功
	}

	// 查找键为 key 的节点，并返回指向该节点的迭代器
	Iter find(const K& key)
	{
		node* update[MAXL + 1];  // 用于记录查找路径上每个层级的前驱节点
		node* p = find(key, update);  // 调用 find 函数查找键为 key 的节点
		if (p == tail) return Iter(tail);  // 如果未找到（节点为尾节点），则返回指向尾节点的迭代器
		if (p->key != key) return Iter(tail);  // 如果键不匹配，则返回指向尾节点的迭代器
		return Iter(p);  // 否则，返回指向该节点的迭代器
	}

	// 检查键为 key 的节点是否存在于跳表中
	bool count(const K& key)
	{
		node* update[MAXL + 1];  // 用于记录查找路径上每个层级的前驱节点
		node* p = find(key, update);  // 调用 find 函数查找键为 key 的节点
		if (p == tail) return false;  // 如果未找到（节点为尾节点），则返回 false
		return key == p->key;  // 否则，检查键是否匹配，返回匹配结果
	}

	// 返回指向跳表尾节点的迭代器，表示迭代的结束位置
	Iter end()
	{
		return Iter(tail);
	}

	// 返回指向跳表第一个节点的迭代器，表示迭代的起始位置
	Iter begin()
	{
		return Iter(head->forward[0]);
	}
};

int main()
{
	{
		// 使用 lambda 表达式定义一个比较器 cmp，根据字符串长度比较大小
		auto cmp = [](const string& a, const string& b)
		{ return a.length() < b.length(); };
		// 创建一个 skip_list 对象，使用该比较器
		skip_list<string, int, decltype(cmp)> list(cmp);
		list.insert("aab", 1321);  // 插入键值对 ("aab", 1321)
		list.insert("hello", 54342);  // 插入键值对 ("hello", 54342)
		list.insert("world", 544);  // 插入键值对 ("world", 544)
		// 使用迭代器遍历跳表并输出每个节点的键和值
		for (auto it = list.begin(); it != list.end(); it++)
		{
			cout << it->key << " " << it->value << endl;
		}
	}

	cout << "==================================" << endl;

	{
		// 定义一个仿函数 cmp，根据整数大小降序比较
		struct cmp
		{
			bool operator()(int a, int b)
			{
				return a > b;
			}
		};
		// 创建一个 skip_list 对象，使用该仿函数
		skip_list<int, int, cmp> list{};
		for (int i = 1; i <= 10; i++) list.insert(rand() % 20, rand());  // 插入 10 个随机键值对
		// 查找键为 10 的节点，并从该节点开始遍历跳表并输出
		for (auto it = list.find(10); it != list.end(); it++)
		{
			cout << it->key << " " << it->value << endl;
		}
	}

	cout << "==================================" << endl;

	{
		// 创建一个 skip_list 对象，使用默认的比较器（小于号）
		skip_list<int, int> list;
		list.insert(1, 3);  // 插入键值对 (1, 3)
		list.insert(1, 3);  // 插入键值对 (1, 3)
		list.insert(4, 3);  // 插入键值对 (4, 3)
		list.insert(5, 3);  // 插入键值对 (5, 3)
		list.insert(1, 3);  // 插入键值对 (1, 3)
		list.insert(4, 3);  // 插入键值对 (4, 3)
		// 使用迭代器遍历跳表并输出每个节点的键和值
		for (auto it = list.begin(); it != list.end(); it++)
		{
			cout << it->key << " " << it->value << endl;
		}
	}

	{
		// 注释中提到可以添加移动语义和重载 [] 运算符来进一步完善跳表类
	}

	return 0;
}