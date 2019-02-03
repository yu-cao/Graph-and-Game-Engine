<h2>重载运算与类型转换</h2>

建议：不要重载逻辑与`&&`，逻辑或`||`，逗号`,`，取地址`&`这四个运算符，因为无法保留下求值顺序和/或短路求值属性，后两者常常与类类型对象有关，对这些的重载可能对用户造成混淆（一直以来的求值规则被改变了）**程序员需要负责任地使用重载运算符，不应该强行曲解运算符常规含义以适合某个类型**

运算符定义为成员函数/非成员函数：

+ 赋值`=`，下标`[]`，调用`()`和成员访问箭头`->`运算符必须是成员函数
+ 复合赋值运算符一般来说应该是成员，但非必须
+ 改变对象状态的运算符或与给定类型密切相关的运算符，如`++`，`--`和解引用`*`运算符通常是成员
+ 具有对称性的运算符通常应该是非成员的，如算术，相等性，关系和位运算等

<h3>重载输入输出运算符</h3>

必须是非成员函数，一般被声明为友元

<h4>输出运算符<<</h4>

第一个形参：非常量ostream对象的引用；第二个形参：常量的引用，常量是我们想要打印的类类型；返回值：ostream形参。输出运算符尽量减少格式化操作（如换行）

```cpp
ostream &operator<<(ostream &os, const Sales_data &item)
{
    os << item.isbn() << " " << item.units_sold << " " << item.revenue << " " << item.avg_price();
    return os;
}
```

<h4>输入运算符>></h4>

第一个形参：将要读取的流的引用；第二个形参：要读到的对象的引用（非常量）；返回值：某个给定流的引用

```cpp
istream &operator>>(istream &is, Sales_data &item)
{
    double price;
    is >> item.bookNo >> item.units_sold >> price;
    if(is)//检查输入是否成功
        item.revenue = item.units_sold * price;
    else
        item = Sales_data();//输入失败，对象赋予默认状态
    return is;
}
```

输入运算符必须要处理输入时失败的情况：1、错误数据类型导致不匹配，无法构造（如int处输入了字符串），导致后续对流的其他使用都会失败 2、读取到达文件末尾或输入流出现其他错误

读取操作发生错误时，输入运算符应该负责从错误中恢复：设置failbit，eofbit，badbit等等标志进行标识

<h3>算术与关系运算符</h3>

除了赋值运算符外，其他的必须是非成员函数，一般被声明为友元<br>
赋值/复合赋值运算符应该在类内定义，返回左侧对象的引用

|算术符号|函数头|
|:-:|:-:|
|`+`/`-`/`*`/`/`|`Sales_data operator+(const Sales_data &lhs, const Sales_data &rhs)`|
|`==`/`!=`|`bool operator==(const Sales_data &lhs, const Sales_data &rhs)`|
|`<`/`<=`/`>`/`>=`|`bool operator<(const Sales_data &lhs, const Sales_data &rhs)`|
|`=`/`+=`/`-=`|`Sales_data& Sales_data::operator+=(const Sales_data &rhs)`|

<h3>下标运算符</h3>

必须是成员函数，通常以所访问元素的引用作为返回值，最好同时定义常量版本与非常量版本

```cpp
class StrVec{
public:
    std::string& operator[](std::size_t n)
        {return elements[n];}
    const std::string& operator[](std::size_t n) const//这里const含义为隐含的this底层指针是const的，也就是说*this这个对象内数据是只读的
        {return elements[n];}
private:
    std::string *elements;
};

StrVec svec;
const StrVec cvec = svec;//把svec元素拷贝到cvec

//如果svec中有元素，对一个元素运行string的empty函数
if(svec.size() && svec[0].empty()){
    svec[0] = "zero";//正确，下标运算符返回string的引用
    cvec[0] = "zip";//错误，对cvec取下标返回是常量引用
}
```

<h3>递增和递减运算符</h3>

建议将其设定为成员函数，因为它们改变了操作对象的状态，应该为类定义两个版本的递增/递减运算符（前置/后置）

后置版本接受一个额外的（不被使用的）int类型的形参进行区分，我们不会用到这个形参，所以无需对其命名

核心是保持这两者的设计操作与内置的尽可能一致

+ 前置：应该返回递增(/递减）后对象的**引用**
+ 后置：应该返回对象的原**值**（递增/递减前的值）

```cpp
class StrBlobPtr{
public:
    //前置
    StrBlobPtr& operator++();
    StrBlobPtr& operator--();
    
    //后置
    StrBlobPtr operator++(int);
    StrBlobPtr operator--(int);
}
```

<h3>成员访问运算符</h3>

解引用`*`和箭头运算符`->`，常常在迭代器类和智能指针类中使用，都应该是类的成员

重载的箭头运算符必须返回类的指针或自定义了箭头运算符的某个类的对象

```cpp
class StrBlobPtr{
public:
    std::string& operator*() const
    {
        auto p = check(curr, "dereference past end");//检查是否在作用范围内
        return (*p)[curr];//(*p)是对象所指的vector
    }
    std::string* operator->() const
    {
        return & this->operator*();//委托给解引用运算符，也就是把->变成(*p).的形式
    }
    //...
}
```

<h3>函数调用运算符</h3>