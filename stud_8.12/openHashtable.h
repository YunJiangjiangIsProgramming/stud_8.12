#pragma once
#include<iostream>
#include<vector>

using namespace std;


namespace HashBucket
{
	template<class T>
	struct HashNode
	{
		HashNode<T>* _next; // ָ����һ���ڵ��ָ��
		T _data; // �洢������

		HashNode(const T& data)
			:_next(nullptr) // ��ʼ����һ���ڵ�Ϊnullptr
			, _data(data) // ��ʼ������
		{}
	};

	// ǰ��������ϣ��
	template<class K, class T, class KeyOfT, class Hash>
	class HashTable;

	template<class K, class T, class Ref, class Ptr, class KeyOfT, class Hash>
	struct __HashIterator
	{
		typedef HashNode<T> Node; // ����ڵ�����
		typedef HashTable<K, T, KeyOfT, Hash> HT; // �����ϣ������
		typedef __HashIterator<K, T, Ref, Ptr, KeyOfT, Hash> Self; // ��������

		typedef __HashIterator<K, T, T&, T*, KeyOfT, Hash> Iterator; // ���޸ĵĵ���������

		Node* _node; // ��ǰ�ڵ�
		const HT* _ht; // ָ���ϣ���ָ��

		__HashIterator(Node* node, const HT* ht)
			:_node(node) // ��ʼ����ǰ�ڵ�
			, _ht(ht) // ��ʼ����ϣ��ָ��
		{}

		__HashIterator(const Iterator& it)
			:_node(it._node) // �������캯��
			, _ht(it._ht)
		{}

		Ref operator*() // �����ò�����
		{
			return _node->_data; // ���ص�ǰ�ڵ������
		}

		Ptr operator->() // ָ�������
		{
			return &_node->_data; // ���ص�ǰ�ڵ����ݵĵ�ַ
		}

		bool operator!=(const Self& s) // �����ڲ�����
		{
			return _node != s._node; // �Ƚϵ�ǰ�ڵ�����һ���������Ľڵ�
		}

		// ǰ׺����������
		Self& operator++()
		{
			if (_node->_next != nullptr) // �����һ���ڵ㲻Ϊ��
			{
				_node = _node->_next; // �ƶ�����һ���ڵ�
			}
			else
			{
				// ����һ����Ϊ�յ�Ͱ
				KeyOfT kot;
				Hash hash;
				// �����ǰͰ��λ��
				size_t hashi = hash(kot(_node->_data)) % _ht->_tables.size();
				++hashi; // �ƶ�����һ��Ͱ
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi]) // ����ҵ���Ϊ�յ�Ͱ
					{
						_node = _ht->_tables[hashi]; // ���µ�ǰ�ڵ�
						break;
					}
					else
					{
						++hashi; // ��������
					}
				}

				// û���ҵ���Ϊ�յ�Ͱ
				if (hashi == _ht->_tables.size())
				{
					_node = nullptr; // ���õ�ǰ�ڵ�Ϊnullptr
				}
			}

			return *this; // ��������
		}
	};

	template<class K, class T, class KeyOfT, class Hash>
	class HashTable
	{
		template<class K, class T, class Ref, class Ptr, class KeyOfT, class Hash>
		friend struct __HashIterator; // ��Ԫ����

		typedef HashNode<T> Node; // ����ڵ�����
	public:
		typedef __HashIterator<K, T, T&, T*, KeyOfT, Hash> iterator; // ���޸ĵĵ���������
		typedef __HashIterator<K, T, const T&, const T*, KeyOfT, Hash> const_iterator; // ��������������

		iterator begin() // ���ص������Ŀ�ʼ
		{
			Node* cur = nullptr;
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				cur = _tables[i]; // ����ÿ��Ͱ
				if (cur) // �ҵ���һ����Ϊ�յ�Ͱ
				{
					break;
				}
			}

			return iterator(cur, this); // ���ص�����
		}

		iterator end() // ���ص������Ľ���
		{
			return iterator(nullptr, this); // ����nullptr�ĵ�����
		}

		const_iterator begin() const // �����������Ŀ�ʼ
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

			return const_iterator(cur, this); // ���س���������
		}

		const_iterator end() const // �����������Ľ���
		{
			return const_iterator(nullptr, this); // ����nullptr�ĳ���������
		}

		~HashTable() // ��������
		{
			for (auto& cur : _tables) // ����ÿ��Ͱ
			{
				while (cur) // ɾ��ÿ���ڵ�
				{
					Node* next = cur->_next; // ������һ���ڵ�
					delete cur; // ɾ����ǰ�ڵ�
					cur = next; // �ƶ�����һ���ڵ�
				}

				cur = nullptr; // ���õ�ǰͰΪnullptr
			}
		}

		iterator Find(const K& key) // ���Ҽ�
		{
			if (_tables.size() == 0) // �����Ϊ��
				return end(); // ���ؽ���������

			KeyOfT kot;
			Hash hash;
			size_t hashi = hash(key) % _tables.size(); // �����ϣֵ
			Node* cur = _tables[hashi]; // ��ȡ��ӦͰ�Ľڵ�
			while (cur) // ��������
			{
				if (kot(cur->_data) == key) // ����ҵ�ƥ��ļ�
				{
					return iterator(cur, this); // �����ҵ��ĵ�����
				}

				cur = cur->_next; // �ƶ�����һ���ڵ�
			}

			return end(); // ���ؽ���������
		}

		bool Erase(const K& key) // ɾ����
		{
			Hash hash;
			KeyOfT kot;
			size_t hashi = hash(key) % _tables.size(); // �����ϣֵ
			Node* prev = nullptr; // ǰһ���ڵ�
			Node* cur = _tables[hashi]; // ��ǰ�ڵ�
			while (cur) // ��������
			{
				if (kot(cur->_data) == key) // ����ҵ�ƥ��ļ�
				{
					if (prev == nullptr) // ����ǵ�һ���ڵ�
					{
						_tables[hashi] = cur->_next; // ����Ͱ��ͷָ��
					}
					else
					{
						prev->_next = cur->_next; // ����ǰһ���ڵ��ָ��
					}
					delete cur; // ɾ����ǰ�ڵ�

					return true; // ���سɹ�
				}
				else
				{
					prev = cur; // ����ǰһ���ڵ�
					cur = cur->_next; // �ƶ�����һ���ڵ�
				}
			}

			return false; // ����ʧ��
		}
		// ��ȡ��һ������
		size_t GetNextPrime(size_t prime)
		{
			static const int __stl_num_primes = 28; // ��������
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
				if (__stl_prime_list[i] > prime) // �ҵ���һ������
					return __stl_prime_list[i];
			}

			return __stl_prime_list[i]; // �������һ������
		}
		

		pair<iterator, bool> Insert(const T& data) // ��������
		{
			KeyOfT kot;
			iterator it = Find(kot(data)); // ��������
			if (it != end()) // ����Ѵ���
			{
				return make_pair(it, false); // �����Ѵ��ڵĵ�����
			}

			Hash hash;

			// ��������==1ʱ����
			if (_n == _tables.size()) // ����������Ӵﵽ1
			{
				size_t newsize = GetNextPrime(_tables.size()); // ��ȡ��һ������
				vector<Node*> newtables(newsize, nullptr); // �����±�
				for (auto& cur : _tables) // �����ɱ�
				{
					while (cur) // ���²���ڵ�
					{
						Node* next = cur->_next; // ������һ���ڵ�

						size_t hashi = hash(kot(cur->_data)) % newtables.size(); // ������Ͱλ��

						// ͷ�嵽�±�
						cur->_next = newtables[hashi]; // ������Ͱ��ͷָ��
						newtables[hashi] = cur; // ����ǰ�ڵ������Ͱ

						cur = next; // �ƶ�����һ���ڵ�
					}
				}

				_tables.swap(newtables); // �����ɱ���±�
			}

			Node* newnode = new Node(data); // �����½ڵ�
			size_t hashi = hash(kot(data)) % _tables.size(); // ����Ͱλ��
			newnode->_next = _tables[hashi]; // �����½ڵ�
			_tables[hashi] = newnode; // ����Ͱ��ͷָ��

			++_n; // ���ӽڵ����
			return make_pair(iterator(newnode, this), true); // �����²���ĵ�����
		}

		size_t MaxBucketSize() // ��ȡ���Ͱ��С
		{
			size_t max = 0; // ��ʼ�����ֵ
			for (size_t i = 0; i < _tables.size(); ++i) // ����ÿ��Ͱ
			{
				auto cur = _tables[i];
				size_t size = 0; // ��ǰͰ�Ĵ�С
				while (cur) // ���㵱ǰͰ�Ĵ�С
				{
					++size; // ���Ӵ�С����
					cur = cur->_next; // �ƶ�����һ���ڵ�
				}

				if (size > max) // �������ֵ
				{
					max = size;
				}
			}

			return max; // �������Ͱ��С
		}
	private:
		vector<Node*> _tables; // ָ�����飬�洢Ͱ
		size_t _n = 0; // �洢��Ч���ݸ���
	};
}
