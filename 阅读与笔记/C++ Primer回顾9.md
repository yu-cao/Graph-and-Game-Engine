动态内存与智能指针

**C++11**
shared_ptr:绝大部分雷同C的指针，比如p指向一个对象，p在条件判断中为true，*p得到它指向的对象，p->mem等于(\*p).mem

|命令|意义|
|:-:|:-:|
|shared\_ptr<T> sp <br>unique\_ptr<T> up|空智能指针，可以指向类型为T的对象|
|p.get()|返回p中保存的指针，但是如果智能指针释放了其对象，返回的指针对象也会消失|
|swap(p,q)<br>p.swap(q)|交换p和q的指针|
|**以下为shared_ptr独有**|
|make_shared<T> (args)|返回一个shared_ptr，指向一个动态分配的类型为T的对象，用args初始化该对象，args必须匹配T中合适的构造函数进行构造|
|shared_ptr<T> p(q)|p是shared_ptr q的拷贝，这个操作会递增q中的计数器，q中的指针必须能被转换成T*|
|p = q|p和q都是shared_ptr，保存的指针能相互转化，会递减p的引用计数，递增q的引用计数，若p引用计数=0，则将其管理的原内存释放|
|p.unique()|p.use_count = 1，返回ture；否则返回false|
|p.use_count()|返回与p共享对象的智能指针数量，很慢，主要用于调试|

```cpp
auto p1 = make_shared<vector<string>>();//p1类型为shared_ptr<vector<string>>
```

每当shared\_ptr进行拷贝或者赋值时，都会记录有多少个其他的shared\_ptr指向相同对象，当shared\_ptr初始化另一个shared\_ptr，或作为参数传递进函数，或作为函数返回值，计数器会递增；当shared\_ptr失效的时候（被赋值或离开作用域等），计数器会递减；一旦shared\_ptr计数器为0，就自动释放内存（使用T对象的析构函数将T析构）

```cpp
class StrBlob
{
public:
    using size_type = std::vector<std::string>::size_type;

    StrBlob() : data(make_shared<vector<string>>())
    {};

    //实参数量未知，可变参数模板
    StrBlob(std::initializer_list<std::string> il) : data(make_shared<vector<string>>())
    {};

    size_type size() const
    { return data->size(); }

    bool empty() const
    { return data->empty(); }

    //增删元素
    void push_back(const std::string &t)
    { data->push_back(t); }

    void pop_back()
    {
        check(0, "pop_back empty StrBlob");
        return data->pop_back();
    }

    //元素访问
    std::string &front()
    {
        check(0, "front on empty StrBlob");
        return data->front();
    }

    std::string &back()
    {
        check(0, "back on empty StrBlob");
        return data->back();
    }

private:
    std::shared_ptr<std::vector<std::string>> data;

    void check(size_type i, const std::string &msg) const//data[i]不合法就抛出异常
    {
        if (i >= data->size())
            throw out_of_range(msg);
    }
};
```

初始化：

提供了一个括号包围的初始化器，就可以使用auto，从这个初始化器来推断我们想要分配的类型（当且仅当只有一个初始化器）

```cpp
auto p1 = new auto(obj);//p指向一个与obj类型相同的对象，该对象用obj进行初始化
auto p2 = new auto{a,b,c};//ERROR，括号内只允许单个初始化器
```

内存耗尽：new表达式失败，抛出bad_alloc的异常；及时使用delete表达式归还动态申请的内存，销毁对象，但是**delete一块非new分配的内存或相同指针值重复释放的结果是未定义的**

```cpp
int *p1 = new int(30);//p1指向的对象的值为30，内存耗尽就抛出异常
int *p2 = new (nothrow) int;//内存耗尽就返回给p2空指针，为定位new
delete p1;//p1必须指向一个动态分配的对象或者空指针
```

**程序员对new出来的对象负责，必须显式释放**：由内置指针（而非智能指针）管理的动态内存在被显示释放前一直存在。所以，坚持只使用智能指针可以避免这些问题

以下两个是典型错误：

```cpp
StrBlob* factory(string &s)
{
    return new StrBlob({s});
}

void use_factory(string &s)
{
    auto p = factory(s);
    //忘记了delete...
}//离开了作用域，p失效，但是指向的内存却还在，发生泄漏
```

```cpp
int *p(new int(42));
auto q = p;
delete p;//p与q同时无效！对q操作很可能出现高危操作
p = nullptr;
```

智能指针替代内置指针提高动态内存安全性：

```cpp
vector<int> *new_vector(void)
{
    return new(nothrow) vector<int>;
}

//使用智能指针方式
shared_ptr<vector<int>> new_share_vector(void)
{
    return make_shared<vector<int>>();
}

void read_ints(vector<int> *pv)//使用智能指针方式(shared_ptr<vector<int>> pv)
{
    int v;

    while (cin >> v)
        pv->push_back(v);
}

void print_int(vector<int> *pv)//使用智能指针方式(shared_ptr<vector<int>> pv)
{
    for (const auto &item : *pv)
        cout << v << " ";
    cout << endl;
}

int main(int argc, char* argv[])
{
    vector<int> *pv = new_vector();//省略后面内存不足的检测
    auto spv = new_share_vector();//使用智能指针方式

    read_ints(pv);
    print_int(pv);

    //使用智能指针则不再需要以下两行：
    delete pv;
    pv = nullptr;
}
```

不能将一个内置的指针形式隐式转换成一个智能指针，因为智能指针构造是explicit的，必须使用直接初始化，初始化智能指针的普通指针一般要指向动态内存，否则就需要提供自己的操作去替代delete

```cpp
shared_ptr<int> p1(new int(1024));//正确
shared_ptr<int> p2 = new int(1024);//ERROR，必须使用直接初始化
//同理：
shared_ptr<int> clone(int p){return new int(p);}//ERROR，使用了隐式初始化
shared_ptr<int> clone(int p){return shared_ptr<int>(new int(p));}//正确
```

