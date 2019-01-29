<h2>关联容器</h2>

|按关键字有序保存元素|类型|
|:-:|:-:|
|map|关联数组，保存key-value对|
|set|key即为value，即只保存关键字的容器|
|multimap|关键字可以反复出现的map|
|multiset|关键字可以反复出现的set|
|**无序集合**||
|unordered_map|用hash函数组织的map|
|unordered_set|用hash函数组织的set|
|unordered_multimap|用hash函数组织的map，关键字可以反复出现|
|unordered_multiset|用hash函数组织的set，关键字可以反复出现|

```cpp
    map<string, size_t> word_count;
    set<string> exclude = {"The","But","And","Or"};
    string word;
    while (cin >> word)
        if(exclude.find(word) == exclude.end())//只统计不在exclude中的单词
            ++word_count[word];//未在map内就会额外创建一个新元素，key=word，val=0
    for (const auto &w : word_count)
    {
        cout << w.first << " occurs " << w.second
             << ((w.second > 1) ? " times" : "time") << endl;
    }
```

关联容器不支持顺序容器的位置相关操作，如push\_back或push\_front，也不支持构造函数或插入操作这些接收一个元素值和一个数量值的操作。

构造相关容器：

初始化时，map必须同时提供key和value，并且用`{key, value}`表示

```cpp
map<string, string> authors = {{"Joyce",   "James"},
                               {"Austen",  "Jane"},
                               {"Dickens", "Charles"}};
```

标准库用关键词类型的"<"运算符来比较两个关键词，我们可以重载元素的"<"，提供的操作必须要严格弱序("≤")

自定义的操作也是容器类型的一部分，指定使用自定义的操作，必须在定义关联容器类型时提供此操作的类型，而且必须在尖括号中紧接着元素类型给出。

```cpp
bool compareISBN(const Sale_data &lhs, const Sale_data &rhs)
{
    return lhs.isbn() < rhs.isbn();
}

int main()
{
    //compareISBN这个函数指针作为自定义操作传入到容器中，在向bookstore添加元素时，调用函数指针为这些元素排序
    //decltype指定出自定义操作的类型
    multiset<Sale_data, decltype(compareISBN) *> bookstore(compareISBN);
}
```

例题：定义一个map，将单词与行号关联，list保存单词出现的行号：

```cpp
    ifstream in(argv[1]);
    map<string, list<int>> word_lineno;
    string line, word;
    int lineno = 0;
    while (getline(in, line))
    {
        lineno++;
        istringstream l_in(line);//构造字符串流读取单词
        while (l_in >> word)
            word_lineno[word].push_back(lineno);//添加行号
    }
```

pair类型：定义于\<utility>中，保存两个public的数据成员，命名为first和second



