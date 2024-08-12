#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <string>
#include "openHashtable.h"
#include <utility> // For std::pair
#include <ctime> // For clock_t, clock(), and CLOCKS_PER_SEC
using namespace std;

// 自定义键的哈希函数
struct HashFunc {
	size_t operator()(const string& key) const {
		return hash<string>()(key);
	}
};

// 自定义键的提取函数
struct KeyOfValue {
	const string& operator()(const pair<string, int>& kv) const {
		return kv.first; // 返回键
	}
};

void test_openhashtable1()
{
	HashBucket::HashTable<string, pair<string, int>, KeyOfValue, HashFunc> ht;

	// 插入数据
	ht.Insert(make_pair("apple", 1));
	ht.Insert(make_pair("banana", 2));
	ht.Insert(make_pair("orange", 3));
	ht.Insert(make_pair("grape", 4));
	ht.Insert(make_pair("pear", 5));

	// 查找数据
	auto it = ht.Find("banana");
	if (it != ht.end()) {
		cout << "Found: " << it->first << " -> " << it->second << endl;
	}
	else {
		cout << "Not found: banana" << endl;
	}

	// 删除数据
	if (ht.Erase("orange")) {
		cout << "Deleted: orange" << endl;
	}
	else {
		cout << "Failed to delete: orange" << endl;
	}

	// 打印所有数据
	cout << "All items in the hash table:" << endl;
	for (auto it = ht.begin(); it != ht.end(); ++it) {
		cout << it->first << " -> " << it->second << endl;
	}

	// 打印最大桶大小
	cout << "Max bucket size: " << ht.MaxBucketSize() << endl;


}


//void testHashBucket(int numElements)
//{
//	HashBucket::HashTable<std::string, std::pair<std::string, int>, KeyOfValue, HashFunc> ht(10000);
//	// Record start time
//	clock_t start = clock();
//
//	// Insert a large number of data elements
//	for (int i = 0; i < numElements; ++i) {
//		std::string key = "key_" + std::to_string(i); // Create a key
//		std::string valueStr = "value_" + std::to_string(i); // Create a value string
//		int valueInt = i; // Create a value integer
//		ht.insert(std::make_pair(key, std::make_pair(valueStr, valueInt)));
//	}
//
//	// Record end time
//	clock_t end = clock();
//	double duration = double(end - start) / CLOCKS_PER_SEC;
//	std::cout << "插入 " << numElements << " 个元素耗时: " << duration << " 秒" << std::endl;
//}

int main() 
{
	
	test_openhashtable1();
	//testHashBucket(10000);
	return 0;
}




