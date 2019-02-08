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

普通类的成员模板

```cpp
class DebugDelete{
public:
    DebugDelete(ostream &s = cerr) : os(s){ }
    template <typename T> void operator()(T *p) const
      { os << "delete unique_ptr" << endl; delete p;}
private:
    ostream &os;
};

double *p = new double;
DebugDelete d;
d(p);//调用DebugDelete::operator()(double*)释放p
DebugDelete()(p)//在一个临时对象DebugDelete对象上调用operator()(int*)
```

类模板的成员模板

```cpp
template <typename T> class Blob{
    template <typename It> Blob(It b,It e);
    //...
};

//在模板外为其定义成员模板时，需要同时提供模板参数列表
template <typename T>
template <typename It>
    Blob<T>::Blob(It b,It e): data(std::make_shared<vector<T>>(b,e)){ }
```

<hr>

控制实例化

**C++11**：显式实例化，避免多个文件中实例化相同模板引发的额外开销；实例化定义会实例化所有成员，所以所用类型必须能用于模板的所有成员函数

遇到`extern`模板声明时不会在本文件中生成实例化代码

```cpp
extern template class Blob<string>;//声明
template int compare(const int&, const int &);//定义

Blob<string> sa1, sa2;//实例化会出现在其他位置

//在本文件中实例化
Blob<int> a1 = {0,1,2,3,4};
Blob<int> a2(a1);

int i = compare(a1[0], a2[0]);//实例化会出现在其他位置
```

<hr>

模板实参推断

从函数实参来确定模板实参的过程；如果函数的形参的类型使用了模板类型参数，则只允许两种转换：

+ const转换：非const对对象的引用（指针）传递给一个const对象（指针）的形参
+ 数组/函数到指针的的转换：数组实参转换为数组首地址指针，函数实参与该函数类型的指针

```cpp
template <typename T> int compare(const T&, const T&);

compare("Hi", "world");//不合法，允许数组到指针的转换，但是如果是引用，数组不会转换成一个指针
```

提供显式模板实参：在函数名之后，实参列表之前，用尖括号给出（注意，显式模板实参从左至右顺序与对应的模板参数匹配，否则要我们自己提供）

```cpp
template <typename T1, typename T2, typename T3>
T1 sum(T2, T3);//因为没有任何实参供编译器推断T1，所以调用时必须为T1提供显式模板实参

auto val3 = sum<long long>(i, lng);//类型为long long sum(int, long)

//不好的设计
template <typename T1, typename T2, typename T3>
T3 bad_sum(T2, T1);

//在这种设计下，我们需要为每个参数指定实参类型
auto val2 = bad_sum<long long, int, long>(i, lng);
```

**C++11**：尾置返回类型与类型转换

显式指定模板实参有时会给用户带来不必要的麻烦，而且不会有什么好处；假如我们不知道返回结果的准确类型，但知道所需类型是所处理的序列的元素类型时，我们可以使用尾置返回类型与`decltype`配合进行类型推导

```cpp
template <typename T>
??? &fcn(It beg, It end)
{
    //处理序列
    return *beg;//返回其中一个元素的引用
}

vector<int> vi = {1,2,3,4,5};
Blob<string> ca = {"hi", "bye"};
auto &i = fcn(vi.begin(),vi.end());//fcn返回int&
auto &s = fcn(ca.begin(),ca.end());//fcn返回string&

//正确方法：
template <typename It>
auto fcn(It beg, It end) -> decltype(*beg)
{
    //处理序列
    return *beg;//返回其中一个元素的引用
}
```

我们这里存在一个问题：对于传递的参数的类型，我们一无所知，只可以使用迭代器，但这些迭代器只是引用，而不会生成元素

所以，当我们需要获得元素类型，可以使用标准库中的类型转换，这些模板定义在`<type_traits>`头文件下；

||标准类型转换模板||
|:-:|:-:|:-:|
|对`Mod<T>`，其中`Mod`为|若`T`为|则`Mod<T>::type`为|
|`remove_reference`|`X&`或`X&&`|`X`|
||否则|`T`|
|`add_const`|`X&`、`const X`或函数|`T`|
||否则|`const T`|
|`add_lvalue_reference`|`X&`|`T`|
||`X&&`|`X&`|
||否则|`T&`|
|`add_rvalue_reference`|`X&`或`X&&`|`T`|
||否则|`T&&`|
|`remove_pointer`|`X*`|`X`|
||否则|`T`|
|`add_pointer`|`X&`或`X&&`|`X*`|
||否则|`T*`|
|`make_signed`|`unsigned X`|`X`|
||否则|`T`|
|`make_unsigned`|带符号类型|`unsigned X`|
||否则|`T`|
|`remove_extent`|`X[n]`|`X`|
||否则|`T`|
|`remove_all_extents`|`X[n1][n2]...`|`X`|
||否则|`T`|

```cpp
//将会得到元素类型本身
template <typename It>
auto fcn2(It beg, It end)->typename remove_reference<decltype(*beg)>::type
{
    return *beg;
}

//保证容纳加法结果的sum（要求a+b还是要有某个内置的整数类型进行容纳）
template<typename T1, typename T2>
auto sum(T1 a, T2 b) -> decltype(a + b)
{
    return a + b;
}
```

<hr>

引用折叠：只能应用于间接创建的引用的引用，如类型别名或模板参数

```cpp
template <typename T> void f3(T&&);
f3(42);//正确，实参是int的右值，模板参数T是int

int i= 5;
f3(i);//绑定左值也是正确的，这里引发了引用折叠
```

**C++11**：通常我们不能把一个右值引用绑定到一个左值上，但是有两个例外：

+ 编译器会推断模板参数T的类型是左值引用，也就是`int&`，而不是`int`；现在f3的函数参数看上去像`int& &&`，我们一般不能定义一个引用的引用，但是通过类型别名或通过模板类型参数间接定义是可以的
+ 如果我们间接创建了一个引用的引用，则这些引用形成了“折叠”。除了类型`X&& &&`折叠形成`X&&`外，其他`X& &`、`X& &&`和`X&& &`都折叠形成`X&`（即普通左值引用）

这里暗示了：我们可以将任何类型的实参传递给`T&&`类型的函数参数

`std::move`：从一个左值通过`static_cast`到一个右值引用是允许的，将右值引用绑定到左值的特性允许它们截断左值

```cpp
template <typename T>
typename remove_reference<T>::type&& move(T&& t)
{
    return static_cast<typename remove_reference<T>::type&&>(t);
}
```

转发：在调用中使用`std::forward`保持类型信息（能够保持原始实参的类型），定义在头文件`<utility>`中

```cpp
template <typename F,typename T1, typename T2>
void flip2(F f, T1 &&t1, T2 &&t2)
{
    f(t2,t1);//错误，右值引用类型int不能绑定到左值的int（有名字的都是左值）
}

void f(int &&i,int &j)
{
    cout << i << " " << j << endl;
}

int main(int argc, char *argv[])
{
    int i = 5;
    flip2(f,i,42);
}
```

上面代码证明了简单转发是不可行的，`forward`实现了完美转发，必须通过显式模板实参来进行调用，`forward`返回该显式实参类型的右值引用。即`forward<T>`的返回类型是`T&&`

**通常情况下，我们使用`forward`传递那些定义为模板类型参数的右值引用的函数参数，通过引用折叠，`forward`保持给定实参的左值/右值属性**

```cpp
template <typename Type> intermediary(Type &&arg)
{
    finalFcn(std::forward<Type>(arg));
    // ...
}
```

使用`forward`之后，可以实现完美转发：

```cpp
template <typename F, typename T1, typename T2>
void flip(F f, T1 &&t1, T2 &&t2)
{
    f(std::forward<T2>(t2), std::forward<T1>(t1));
}
```

<hr>

重载与模板