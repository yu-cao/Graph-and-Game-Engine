<h2>模板与泛型编程</h2>

类型参数：`template<typename T>`，每个类型参数之前都需要加上关键字`typename`或`class`（这两者含义相同），如`template <typename T, class U> T calc (const T&, const U&);`

非类型参数：表示一个值而不是一个类型，实例化时被一个用户提供或编译器推断的值所代替，这些值必须是常量表达式

```cpp
template <unsigned N, unsigned M>
int compare(const char (&p1)[N], const char (&p2)[M])
{
    return strcmp(p1,p2);
}

compare("hi","mom");//会使用字面常量的大小替代N和M，实例化模板，N会为3，M为4
```

<hr>

`inline`和`constexpr`函数模板应该把说明符放在模板参数之后，返回类型之前

```cpp
template <typename T> 
inline T min(const T&, const T&);
```

<hr>

如果真的关心类型无关于可移植性，可以把原来的`<`变成`less`来定义函数

```cpp
template <typename T>
int compare(const T &v1, const T &v2)
{
    if(less<T>()(v1,v2)) return -1;
    if(less<T>()(v2,v1)) return 1;
    return 0;
};
```

<hr>

模板程序应该尽量减少对实参类型的要求；函数模板和类模板成员函数的定义通常放在头文件中，普通函数与类的成员函数定义放在源文件中

保证传递给模板的实参是支持模板所要求的操作，以及这些操作能够在模板中进行是调用者的责任

例子：打印数组，使用模板的代码如下：

```cpp
template <typename T, size_t N>
constexpr int arr_size(const T (&a)[N])
{
    return N;
|

template <typename T, size_t N>
void print(const T (&a)[N])
{
    for (int i = 0; i < arr_size(a); i++)
        cout << a[i] << " ";
    cout << endl;
}
```

<hr>

类模板：一个类模板的每一个实例都形成一个独立的类；对于一个实例化的类模板，其成员只有在使用时才被实例化

```cpp
template <typename T> class Blob{
public:
    typedef T value_type;
    typedef typename vector<T>::size_type size_type;
    
    Blob() : data(make_shared<vector<T>>()){};
    
    Blob(initializer_list<T> il) : data(make_shared<vector<T>>(il)){};
    
    size_type size() const{ return data->size(); }
    bool empty() const{ return data->empty(); }
    
    void push_back(const T &t){ data->push_back(t); }
    void push_back(T &&t){ data->push_back(std::move(t)); }
    void pop_back()
    {
        check(0, "pop_back on empty Blob");
        data->pop_back();
    }
    
    T& back()
    {
        check(0, "back on empty Blob");
        return data->back();
    }
    T& operator[](size_type i)
    {
        check(i, "subscript out of range");
        return (*data)[i];
    }
    
private:
    shared_ptr<vector<T>> data;
    void check(size_type i, const string &msg) const
    {
        if(i >= data->size())
            throw std::out_of_range(msg);
    }
};

//实例化Blob<int>和接收initializer_list<int>构造函数
Blob<int> squares = {0,1,2,3,4,5,6,7,8,9};
//实例化Blob<int>::size() const函数
for(size_t i = 0; i != squares.size(); ++i)
    squares = i * i;
```

只有在类模板的作用域中，才能够直接使用模板名而不必指定模板实参，其他情况使用一个类模板类型必须提供模板实参：

```cpp
template <typename T>
BlobPtr<T> BlobPtr<T>::operator++(int)
{
    BlobPtr ret = *this;//这里与BlobPtr<T> ret = *this;等价，作用域内无需指明模板实参
    ++*this;
    return ret;
};
```

<hr>

类模板与友元：

一对一友好：

```cpp
//前置声明
template <typename> class BlobPtr;
template <typename> class Blob;
template <typename T>
bool operator==(const Blob<T>&, const Blob<T>&);

template <typename T> class Blob{
    friend class BlobPtr<T>;
    friend bool operator==(const Blob<T>, const Blob<T>&);
    //其他成员定义同上
};

Blob<char> ca;//BlobPtr<char>和operator==<char>都是本对象的友元
```

通用与特定的友好关系

```cpp
//前置声明，将模板的一个特定实例声明为友元时使用
template <typename T> class Pal;

//C是非模板类
class C{
    friend class Pal<C>;//用C实例化的Pal是C的友元
    template <typename T> friend class Pal2;//Pal2的所有实例都是C的友元，这种情况无需前置声明
};

//C2是模板类
template <typename T> class C2{
    friend class Pal<T>;//C2的每个实例与相同实例化的Pal声明为友元
    template <typename X> friend class Pal2;//Pal2的所有实例都是C的友元，这种情况无需前置声明
    friend class Pal3;//Pal3是非模板类，是C2所有实例的友元，不需要Pal3的前置声明
};
```

**C++11**：模板类型参数可以声明为友元

```cpp
template <typename Type> class Bar{
    friend Type;//访问权限授予用来实例化Bar的类型
    //...
};

//对于某个类型名Foo，Foo将会成为Bar<Foo>的友元，如Sales_data将成为Bar<Sales_data>的友元
```

<hr>

模板类型别名

可以通过定义`typedef`来引用已经实例化的类（必须是已经实例化的，不能定义`typedef`引用一个模板，因为模板不是一个类型）；**C++11**：使用`using`允许我们为类模板定义一个别名

```cpp
typedef Blob<string> StrBlob;

template <typename T> using twin = pair<T, T>;
twin<string> authors;//authors是一个pair<string, string>类型
```

<hr>

模板参数

模板参数会隐藏外层作用域中声明的相同名字，一个特定文件所需要的所有模板声明一起放在文件开始处，出现于任何使用这些模板的代码之前

```cpp
typedef double A;
template <typename A, typename B> void f(A a,B b)
{
    A tmp = a;//这里tmp的类型是模板参数A的类型，由传入实参推断得到，而不是double
    double B;//错误，模板参数名不能重用（可以想象成B是int，你不能取一个变量名称叫做int）
}
```

**C++假定通过作用域运算符`::`访问的名字不是类型**，所以我们当使用某个模板类型参数的类型成员，就一定要显式地使用关键字`typename`告诉编译器这个名字是一个类型（否则编译器会混淆`T::size_type *p`到底是定义一个指针还是`size_type`的静态数据成员与名为`p`的变量相乘）

```cpp
template <typename T>
typename T::value_type top(const T& c)//使用关键字提示编译器是类型
{
    if (!c.empty())
        return c.back();
    else 
        return typename T::value_type();
}
```

默认模板实参

**C++11**：我们可以为函数与类模板提供默认实参，只有当它右侧的所有参数都有默认实参时，模板参数才可以有默认实参

```cpp
template<typename T, typename F = less<T>>//F表示可调用对象的类型
int compare(const T &v1, const T &v2, F f = F())//定义函数参数f绑定到可调用对象上
{
	if (f(v1, v2)) return -1;
	if (f(v2, v1)) return 1;
	return 0;
}

bool i = compare(0,42);//i = -1

Sales_data item1(cin), item2(cin);
bool j = compare(item1, item2, compareIsbn);//传入一个可调用对象，返回结果能转换成bool值，接收的实参能与前两者实参兼容，这里compare结果依赖于isbn
```

无论何时使用一个类模板，都必须在模板名之后接上尖括号

```cpp
template <class T = int> class Numbers{
public:
    Numbers(T v = 0): val(v){}
private:
    T val;
};

Numbers<long double> lots_of_precision;
Numbers<> average_precision;//空的<>表示我们希望使用默认类型
```

<hr>

成员模板

