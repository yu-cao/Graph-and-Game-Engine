```cpp
class A
{
public:
    A();
    A(A &);
    ~A();
};

A::A()
{
    cout << "执行构造函数创建一个对象\n" << endl;
}

A::A(A&)
{
    cout << "执行复制构造函数创建该对象的副本\n";
}

A::~A()
{
    cout << "执行析构函数删除该对象" << endl;
}

A func();

int main()
{
    A a;
    A q = func();
    return 0;
}

A func()
{
    A m;
    return m;
}
```

这段代码在g++或者clang++上都会报错（但是在Mac上的llvm和Windows下的Visual Studio都正常运行）:

```
main.cpp:35:7: error: no matching constructor for initialization of 'A'
    A q = func();
      ^   ~~~~~~
main.cpp:20:4: note: candidate constructor not viable: expects an l-value for 1st argument
A::A(A&)
   ^
main.cpp:15:4: note: candidate constructor not viable: requires 0 arguments, but 1 was provided
A::A()
   ^
1 error generated.
```

原因如下：func()返回一个rvalue，而一个rvalue并不能绑定在一个non-const的A&构造函数上，而可以是const A&。在类A中定义了个一个复制构造函数后，编译器不会再生成默认的const构造函数，所以找不到合适的复制构造函数了。修改成A::A(const &A)即可。

