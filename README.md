# 跳表实现代码说明

## 一、概述
本代码实现了一个跳表（Skip List）数据结构，跳表是一种随机化的数据结构，它允许在 $O(\log n)$ 的平均时间复杂度内完成插入、删除和查找操作。代码使用 C++ 模板实现，支持不同类型的键和值，并且可以自定义比较器。

## 二、代码结构

### 1. 比较器结构体 `Less`
```cpp
template<typename T>
struct Less
{
    bool operator()(const T& a, const T& b) const
    {
        return a < b;
    }
};
```
该结构体定义了一个默认的比较器，用于比较两个 `T` 类型的对象，返回 `a < b` 的结果。

### 2. 跳表类模板 `skip_list`
```cpp
template<typename K, typename V, typename Comp = Less<K>>
class skip_list
{
    // ...
};
```
- **模板参数**：
  - `K`：键的类型。
  - `V`：值的类型。
  - `Comp`：比较器类型，默认使用 `Less<K>`。

- **内部结构体 `skip_list_node`**：定义了跳表的节点结构，包含键、值、层级和指向下一个节点的指针数组。

- **主要成员函数**：
  - `init()`：初始化跳表，设置随机种子，初始化头节点和尾节点。
  - `randomLevel()`：随机生成节点的层级。
  - `find(const K& key, node** update)`：查找键为 `key` 的节点，并记录查找路径上每个层级的前驱节点。
  - `insert(const K& key, const V& value)`：在跳表中插入键值对 `(key, value)`。
  - `erase(const K& key)`：从跳表中删除键为 `key` 的节点。
  - `find(const K& key)`：查找键为 `key` 的节点，并返回指向该节点的迭代器。
  - `count(const K& key)`：检查键为 `key` 的节点是否存在于跳表中。
  - `begin()`：返回指向跳表第一个节点的迭代器。
  - `end()`：返回指向跳表尾节点的迭代器。

### 3. 迭代器结构体 `Iter`
用于遍历跳表中的节点，重载了箭头运算符、解引用运算符、相等运算符、不相等运算符和自增运算符。

## 三、使用示例

### 1. 使用 lambda 表达式作为比较器
```cpp
auto cmp = [](const string& a, const string& b)
{ return a.length() < b.length(); };
skip_list<string, int, decltype(cmp)> list(cmp);
list.insert("aab", 1321);
list.insert("hello", 54342);
list.insert("world", 544);
for (auto it = list.begin(); it != list.end(); it++)
{
    cout << it->key << " " << it->value << endl;
}
```

### 2. 使用仿函数作为比较器
```cpp
struct cmp
{
    bool operator()(int a, int b)
    {
        return a > b;
    }
};
skip_list<int, int, cmp> list{};
for (int i = 1; i <= 10; i++) list.insert(rand() % 20, rand());
for (auto it = list.find(10); it != list.end(); it++)
{
    cout << it->key << " " << it->value << endl;
}
```

### 3. 使用默认比较器
```cpp
skip_list<int, int> list;
list.insert(1, 3);
list.insert(1, 3);
list.insert(4, 3);
list.insert(5, 3);
list.insert(1, 3);
list.insert(4, 3);
for (auto it = list.begin(); it != list.end(); it++)
{
    cout << it->key << " " << it->value << endl;
}
```

## 四、编译和运行
将上述代码保存为一个 `.cpp` 文件，例如 `skip_list.cpp`，然后使用以下命令进行编译和运行：

```sh
g++ -std=c++17 skip_list.cpp -o skip_list
./skip_list
```
