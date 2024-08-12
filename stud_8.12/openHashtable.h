#pragma once
#include<iostream>
#include<vector>

using namespace std;


namespace HashBucket
{
	template<class T>
	struct HashNode
	{
		HashNode<T>* _next; // 指向下一个节点的指针
		T _data; // 存储的数据

		HashNode(const T& data)
			:_next(nullptr) // 初始化下一个节点为nullptr
			, _data(data) // 初始化数据
		{}
	};

	// 前置声明哈希表
	template<class K, class T, class KeyOfT, class Hash>
	class HashTable;

	template<class K, class T, class Ref, class Ptr, class KeyOfT, class Hash>
	struct __HashIterator
	{
		typedef HashNode<T> Node; // 定义节点类型
		typedef HashTable<K, T, KeyOfT, Hash> HT; // 定义哈希表类型
		typedef __HashIterator<K, T, Ref, Ptr, KeyOfT, Hash> Self; // 自身类型

		typedef __HashIterator<K, T, T&, T*, KeyOfT, Hash> Iterator; // 可修改的迭代器类型

		Node* _node; // 当前节点
		const HT* _ht; // 指向哈希表的指针

		__HashIterator(Node* node, const HT* ht)
			:_node(node) // 初始化当前节点
			, _ht(ht) // 初始化哈希表指针
		{}

		__HashIterator(const Iterator& it)
			:_node(it._node) // 拷贝构造函数
			, _ht(it._ht)
		{}

		Ref operator*() // 解引用操作符
		{
			return _node->_data; // 返回当前节点的数据
		}

		Ptr operator->() // 指针操作符
		{
			return &_node->_data; // 返回当前节点数据的地址
		}

		bool operator!=(const Self& s) // 不等于操作符
		{
			return _node != s._node; // 比较当前节点与另一个迭代器的节点
		}

		// 前缀递增操作符
		Self& operator++()
		{
			if (_node->_next != nullptr) // 如果下一个节点不为空
			{
				_node = _node->_next; // 移动到下一个节点
			}
			else
			{
				// 找下一个不为空的桶
				KeyOfT kot;
				Hash hash;
				// 算出当前桶的位置
				size_t hashi = hash(kot(_node->_data)) % _ht->_tables.size();
				++hashi; // 移动到下一个桶
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi]) // 如果找到不为空的桶
					{
						_node = _ht->_tables[hashi]; // 更新当前节点
						break;
					}
					else
					{
						++hashi; // 继续查找
					}
				}

				// 没有找到不为空的桶
				if (hashi == _ht->_tables.size())
				{
					_node = nullptr; // 设置当前节点为nullptr
				}
			}

			return *this; // 返回自身
		}
	};

	template<class K, class T, class KeyOfT, class Hash>
	class HashTable
	{
		template<class K, class T, class Ref, class Ptr, class KeyOfT, class Hash>
		friend struct __HashIterator; // 友元声明

		typedef HashNode<T> Node; // 定义节点类型
	public:
		typedef __HashIterator<K, T, T&, T*, KeyOfT, Hash> iterator; // 可修改的迭代器类型
		typedef __HashIterator<K, T, const T&, const T*, KeyOfT, Hash> const_iterator; // 常量迭代器类型

		iterator begin() // 返回迭代器的开始
		{
			Node* cur = nullptr;
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				cur = _tables[i]; // 遍历每个桶
				if (cur) // 找到第一个不为空的桶
				{
					break;
				}
			}

			return iterator(cur, this); // 返回迭代器
		}

		iterator end() // 返回迭代器的结束
		{
			return iterator(nullptr, this); // 返回nullptr的迭代器
		}

		const_iterator begin() const // 常量迭代器的开始
		{
			Node* cur = nullptr;
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				cur = _tables[i];
				if (cur)
				{
					break;
				}
			}

			return const_iterator(cur, this); // 返回常量迭代器
		}

		const_iterator end() const // 常量迭代器的结束
		{
			return const_iterator(nullptr, this); // 返回nullptr的常量迭代器
		}

		~HashTable() // 析构函数
		{
			for (auto& cur : _tables) // 遍历每个桶
			{
				while (cur) // 删除每个节点
				{
					Node* next = cur->_next; // 保存下一个节点
					delete cur; // 删除当前节点
					cur = next; // 移动到下一个节点
				}

				cur = nullptr; // 设置当前桶为nullptr
			}
		}

		iterator Find(const K& key) // 查找键
		{
			if (_tables.size() == 0) // 如果表为空
				return end(); // 返回结束迭代器

			KeyOfT kot;
			Hash hash;
			size_t hashi = hash(key) % _tables.size(); // 计算哈希值
			Node* cur = _tables[hashi]; // 获取对应桶的节点
			while (cur) // 遍历链表
			{
				if (kot(cur->_data) == key) // 如果找到匹配的键
				{
					return iterator(cur, this); // 返回找到的迭代器
				}

				cur = cur->_next; // 移动到下一个节点
			}

			return end(); // 返回结束迭代器
		}

		bool Erase(const K& key) // 删除键
		{
			Hash hash;
			KeyOfT kot;
			size_t hashi = hash(key) % _tables.size(); // 计算哈希值
			Node* prev = nullptr; // 前一个节点
			Node* cur = _tables[hashi]; // 当前节点
			while (cur) // 遍历链表
			{
				if (kot(cur->_data) == key) // 如果找到匹配的键
				{
					if (prev == nullptr) // 如果是第一个节点
					{
						_tables[hashi] = cur->_next; // 更新桶的头指针
					}
					else
					{
						prev->_next = cur->_next; // 更新前一个节点的指针
					}
					delete cur; // 删除当前节点

					return true; // 返回成功
				}
				else
				{
					prev = cur; // 更新前一个节点
					cur = cur->_next; // 移动到下一个节点
				}
			}

			return false; // 返回失败
		}
		// 获取下一个素数
		size_t GetNextPrime(size_t prime)
		{
			static const int __stl_num_primes = 28; // 素数数量
			static const unsigned long __stl_prime_list[__stl_num_primes] =
			{
				53, 97, 193, 389, 769,
				1543, 3079, 6151, 12289, 24593,
				49157, 98317, 196613, 393241, 786433,
				1572869, 3145739, 6291469, 12582917, 25165843,
				50331653, 100663319, 201326611, 402653189, 805306457,
				1610612741, 3221225473, 4294967291
			};

			size_t i = 0;
			for (; i < __stl_num_primes; ++i)
			{
				if (__stl_prime_list[i] > prime) // 找到下一个素数
					return __stl_prime_list[i];
			}

			return __stl_prime_list[i]; // 返回最后一个素数
		}
		

		pair<iterator, bool> Insert(const T& data) // 插入数据
		{
			KeyOfT kot;
			iterator it = Find(kot(data)); // 查找数据
			if (it != end()) // 如果已存在
			{
				return make_pair(it, false); // 返回已存在的迭代器
			}

			Hash hash;

			// 负载因子==1时扩容
			if (_n == _tables.size()) // 如果负载因子达到1
			{
				size_t newsize = GetNextPrime(_tables.size()); // 获取下一个素数
				vector<Node*> newtables(newsize, nullptr); // 创建新表
				for (auto& cur : _tables) // 遍历旧表
				{
					while (cur) // 重新插入节点
					{
						Node* next = cur->_next; // 保存下一个节点

						size_t hashi = hash(kot(cur->_data)) % newtables.size(); // 计算新桶位置

						// 头插到新表
						cur->_next = newtables[hashi]; // 更新新桶的头指针
						newtables[hashi] = cur; // 将当前节点插入新桶

						cur = next; // 移动到下一个节点
					}
				}

				_tables.swap(newtables); // 交换旧表和新表
			}

			Node* newnode = new Node(data); // 创建新节点
			size_t hashi = hash(kot(data)) % _tables.size(); // 计算桶位置
			newnode->_next = _tables[hashi]; // 插入新节点
			_tables[hashi] = newnode; // 更新桶的头指针

			++_n; // 增加节点计数
			return make_pair(iterator(newnode, this), true); // 返回新插入的迭代器
		}

		size_t MaxBucketSize() // 获取最大桶大小
		{
			size_t max = 0; // 初始化最大值
			for (size_t i = 0; i < _tables.size(); ++i) // 遍历每个桶
			{
				auto cur = _tables[i];
				size_t size = 0; // 当前桶的大小
				while (cur) // 计算当前桶的大小
				{
					++size; // 增加大小计数
					cur = cur->_next; // 移动到下一个节点
				}

				if (size > max) // 更新最大值
				{
					max = size;
				}
			}

			return max; // 返回最大桶大小
		}
	private:
		vector<Node*> _tables; // 指针数组，存储桶
		size_t _n = 0; // 存储有效数据个数
	};
}
