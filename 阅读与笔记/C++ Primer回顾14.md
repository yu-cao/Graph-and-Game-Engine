<h2>标准库特殊设施</h2>

**C++11** 

tuple类型

是类似`pair`的模板。`pair`每次都有两个成员，而`tuple`可以有任意数量的成员。

|命令|含义|
|:-:|:-:|
|`tuple<T1,T2,...,Tn> t(v1,v2,...vn);`|t是一个`tuple`，成员类型是T1...Tn，每个成员用对应的v1...vn进行初始化，这个构造是显式构造的|
|`make_tuple(v1,v2,...,vn)`|返回一个用于给定初始值初始化的tuple，类型由初始值推断得到|
|`t1 == t2`或`t1 != t2`或比较|两个具有相同数量成员且成员对应相等时，两个tuple才相等，只有每对成员的`<`运算符都合法，比较才合法|
|`get<i>(t)`|返回t的第i个数据成员的引用；如果t是左值，结果是左值引用；否则返回右值引用|
|`tuple_size<tupleType>::value`|一个类模板，可通过一个tuple类型进行初始化。有一个名为value的public constexpr static成员，类型size_t，表示给定tuple类型中成员的数量|
|`tuple_element<i,tupleType>::type`|一个类模板，可以通过一个整型常量和一个tuple类型进行初始化，有一个type的public成员，表示给定tuple类型中指定成员的的类型|

```cpp
tuple<size_t, size_t, size_t> threeD;//三个成员都默认初始化为0
tuple<string, vector<double>, int, list<int> > someVal("constants",{3.14,2.718},42,{0,1,2,3,4});//构造函数是显式的，必须直接初始化语法
auto item = make_tuple("0-999-78345-X",3, 20.00);//类型make_pair直接生成
```

对比pair，因为严格规定了只有两个成员，所以可以提供命名为`.first`和`.second`，而tuple类型就没有限制，这导致了我们必须给定显式模板实参告诉编译器我们要访问第几个成员，返回的是指定成员的引用

```cpp
auto item = make_tuple("0-999-78345-X",3, 20.00);
auto isbn = get<0>(item);
auto price = get<2>(item);
get<2>(item) *= 0.8;
```

如果不知道tuple准确类型，就可以使用`tuple_size`和`tuple_element`进行查询

```cpp
typedef decltype(item) trans;
size_t sz = tuple_size<trans>::value;
tuple_element<1,trans>::type cnt = get<1>(item);
```



