类型的选择：
1. 明确知道非负数时选择无符号类型
2. 整数运算使用int，超过就选long long
3. 算术表达式中不要用bool或者char。因为char在不同机器上对于是否有符号实现是不同的，如果需要一个小整数，就明确使用unsigned char或者signed char
4. 浮点运算就使用double

类型转换：
1. 如果赋给无符号类型一个超过其范围的值时，就额外取模，如256赋予unsigned char，所得为256 % 255 = 1；同理，-1 为 255。
**此外需要额外小心的是：当赋给带符号的类型一个超出范围的值时是UB行为**
2. 易犯错误：当一个算术表达式中又有无符号数又有int时，会把int转化成无符号数，**切勿混用有符号类型与无符号类型**
```cpp
    unsigned u = 10;
    int i = -42;
    std::cout << u + i << std::endl;//int是32位的答案为4294967264
    std::cout << i - u << std::endl;//答案为4294967244
    std::cout << i * u << std::endl;//答案为4294966876
```

C++中，初始化与赋值操作完全不同，需要严格区分<br>
**C++11**：花括号初始化变量（列表初始化 list initialization）已经被广泛使用，**优点是当初始值存在信息丢失风险时编译器会报错**，以下内容都是正确的初始化：
```cpp
//C++ 11
int a{0};
int c = {0};
//传统：
int b(0);
int d = 0;
```
新旧初始化的区别：
```cpp
double pi = 3.1415926;
int a{pi},b = {pi}; //编译器抛出ERROR，存在信息丢失风险
int c(pi),d = pi;   //正确执行且的确丢失了部分信息
```
变量只能够被定义一次，`extern`可以让我们只是声明而不是定义，但如果包含了显式初始化就转为了定义：`extern double pi = 3.1415;`在函数体内部试图初始化一个`extern`标记的变量会引发错误

左值引用：为对象起另外一个名字（就是起一个别名），引用必须初始化，这个过程中引用的值会与原来的值**绑定**，而且无法令引用重新绑定到另一个对象；此外，引用不是对象，不能构建引用的引用（这部分内容的例外可以参考引用折叠，可以间接构建起引用的引用，前提必须是类型别名或者模板参数，见P608~612；此外，**You are forbidden from declaring references to references, but compilers may produce them inparticular contexts.**）
```cpp
int ival = 1024;
int &refval = ival;
int &refval;    //ERROR
```
右值引用**C++11**：为了支持move操作，必须绑定到右值的引用。通过`&&`来获得右值引用。它只能绑定到一个将要销毁的对象，所以可以自由地将一个右值引用的资源“移动”到另一个对象

左值：对象的身份（在内存中的位置）
右值：对象的值（对象的内容）
原则：需要右值的地方可以用左值来代替，但是不能把右值当成左值来使用

指针：
引用不是对象，没有实际地址，所以不能定义指向引用的指针：`int &*p`这种，但是*指向指针的引用存在*，因为指针是对象（离变量名最近的符号对变量类型有最直接的影响（下文ptr1为符号&，因此ptr1是一个引用，其余部分确定引用的是什么））
```cpp
    int iVal=1024;
    int &refVal=iVal;
    int *ptr0 = &refVal;
    int *&ptr1 = ptr0;       //ptr1是对于指针ptr的引用
    int &*ptr2 = ptr0;       //ERROR: 'ptr2' declared as a pointer to a reference of type 'int &'
    int *&ptr3 = &refVal;   //ERROR: Non-const lvalue reference to type 'int *' cannot bind to a temporary of type 'int *'
```
解引用只能对确实指向了某个对象的指针进行，否则会引发段错误

空指针：
**C++11**：`nullptr`:特殊类型的字面值，可以被转化为任意其他的指针类型，这一点与传统的`NULL`不同，C++中，NULL被定义成0，显然这样会在某些情况下发生到底是普通整数还是指针的模糊，所以现在需要尽可能使用nullptr，抛弃NULL<br>
比较`nullptr`、`NULL`区别:万一有重载或者模板推导的时候，使用NULL编译器就无法给出正确结果了，比如：
```cpp
void f(void *){}
void f(int){}
int main()
{
    f(0); // what function will be called?
}
```
而且用nullptr可以更好地进行异常的捕获与处理
