<h2>用于大型程序的工具</h2>

异常处理

通过抛出一条表达式来引发一个异常；被抛出的表达式类型和调用链共同决定了哪段处理代码该处理这个异常

**执行一个throw时，跟在throw后面的语句不再被执行，控制权交给catch模块，沿着调用链创建的函数会提前退出，沿着调用链创建的对象将会被销毁**

当throw出现在try block中时，检查与该try向关联的catch子句。找到了匹配的catch，就处理这个异常，如果没有找到，就继续检查与外层try匹配的catch子句...这个过程被称为栈展开

在栈展开的过程中，依次退出某些块，编译器会负责将这些块中创建的对象进行正确的销毁，析构函数总是会被执行的，但是函数中负责释放资源的代码却可能被跳过**（使用类来控制资源的分配没有这个问题）**，析构函数不应该抛出异常，如果抛出且自身没有捕获，程序就被终止

异常对象是一种特殊的对象，编译器通过异常抛出表达式对于异常对象进行拷贝初始化；异常对象在编译器管理的空间内，所以无论最终调用哪个catch子句都可以访问到这个空间；处理完毕后，异常对象销毁

抛出表达式的时候，该表达式的静态编译时的类型决定了异常对象的类型

catch子句中的异常声明看似是只包含一个形参的函数形参列表。声明的类型决定了处理代码所就能捕获的异常类型，必须是完全类型，可以是左值引用，不能是右值引用；异常声明的静态类型决定了catch语句所能执行的操作，如果catch接收的异常与某个继承体系有关，最好把这个catch参数定义为引用类型

catch语句的顺序很重要，按照其出现顺序进行逐一匹配的，匹配规则比较严格，只允许非常量向常量转换，派生类向基类转换和数组与指针转换

应该catch子句从最低派生类型到最高派生类型排序，以便派生类处理代码出现在基类类型catch之前

重新抛出：当一个catch不能完整处理某个异常时，可以通过重新抛出的方式将异常传递给另外一个catch语句，只有当传入类型是引用时，才会改变其参数内容

```cpp
catch(my_error &eObj)//引用类型
{
	eObj.status = errCodes::severeErr;//修改了异常对象
	throw;//异常对象的status成员是servereErr
}
catch(other_error eObj)//非引用类型
{
	eObj.status = errCodes::severeErr;//只修改了异常对象的局部副本
	throw;//异常对象的status成员没有改变
}
```

一次性统一捕获所有异常，我们使用`catch(...)`（当与几个catch共同使用时必须放在最后，否则后续的catch将永远不会被匹配）

```cpp
int main()
{
	try{
		//...
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
		abort();
	}
	return 0;
}
```

想要处理构造初始值抛出的异常，将构造函数写成函数try语句块的形式（这也是唯一能够处理构造函数初始值异常的方法）

```cpp
template<typename T>
Blob<T>::Blob(initializer_list<T> il) try : data(make_shared<vector<T>>(il))
{
	//空函数体
} catch (const bad_alloc &e)
{
	handle_out_of_memory(e);
}
```

**C++11**提供noexpect说明指定某个函数不会抛出异常，这个声明要么出现在所有声明函数与定义函数中，要么不要出现

`void recoup(int) noexcept;`：不会抛出异常（旧标准：`void recoup(int) throw();`)<br>
`void alloc(int);`：可能抛出异常

一个函数声明了noexcept，但是依然抛出了异常的话，编译器会确保遵守noexcept这个承诺不抛出。

`noexcept`说明符实参常常与`noexcept`运算符混合使用；`noexcept`运算符是一个一元运算符，返回一个bool类型的右值常量表达式，用于表示给定的表达式会否抛出异常

比如上面`void recoup(int)`声明使用了`noexcept`，所以这个表达式为`true`

```cpp
noexcept(recoup(i));//recoup不抛出异常则结果为true，否则结果为false

noexcept(e);//当e调用的所有函数都做了不抛出说明且e本身不含有throw语句，返回为true，否则为false
```

一般可以这两者联用方式如下：

```cpp
void f() noexcept(noexcept(g()));//f将与g的异常说明保持一致
```

异常说明对函数指针与给指针所指的函数的关系必须是一致的，也就是说：函数保证不抛出异常，那么函数指针可以声明为`noexcept`，也可以不声明；但是如果函数没有保证，函数指针就不能声明为`noexcept`。

```cpp
void (*pf1)(int) noexcept = recoup;
void (*pf2)(int) = recoup;//正确，recoup不会抛出异常，pf2可能抛出异常，二者互不干扰
pf1 = alloc;//错误，alloc可以抛出异常，pf1已经声明不会抛出异常
pf2 = alloc;//正确
```

一个虚函数保证了它不抛出异常，那么后续派生继承的虚函数(override)必须做出同样承诺；如果不保证它不抛出异常，那么后续派生的虚函数可以给出保证，也可以不给出（因为给出保证`noexcept`是一个更严格的限定，这是允许的）

<hr>