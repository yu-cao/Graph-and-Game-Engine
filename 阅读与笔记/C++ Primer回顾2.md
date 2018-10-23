<h3>命名空间的using</h3>

语法：`using namespace::name`，比如使用string类型就
```cpp
#include <string>
using std::string;
```
合理使用标准库而不要轻易自己造轮子，标准库的性能也是有所保障的

<h3>标准库的string</h3>

`string::size_type` 它是`string::size()`的返回值，是一个与机器无关的无符号类型的值且足够放下任何string对象的大小，在C++11中可以通过auto或decltype来进行推断
```cpp
auto len = line.size();//len类型就是无符号的string::size_type
```
注意：无符号数与有符号数进行加减是高危的，必须要谨慎

单独处理string对象中的字符，可以选择cctype头文件来处理，建议尽量使用C++版本的C标准头文件，例如使用\<cstdio>,\<cctype>代替\<stdio.h>,\<ctype.h>

**C++11** 范围for语句：遍历给定序列中的每个元素并对序列中的每个值执行某中操作
```cpp
//统计整个字符串的标点
string s("Hello World!!!");
decltype(s.size()) punct_cnt = 0;
for(auto c : s)
    if(ispunct(c))
        ++punct_cnt;
cout << punct_cnt << " punctuation characters in " << s << endl;

//改变整个字符串中的字符
string s("Hello World!!!");
for(auto &c : s)    //这个变量c依次绑定到s的每个字符上
    c = toupper(c);
cout << s << endl;
```
