<h2>Game Engine Architecture笔记</h2>
<h2>这里记录的只是我认为比较重要的，我认为我已经熟练掌握了的不再记录，如想要全面了解，请购买正版实体图书 https://book.douban.com/subject/25815142/ </h2>
<h3>Chapter 2 专业工具</h3>
1. 预处理器设置：所有现在的C++预处理器可以通过命令行的方式定义预处理宏，使得通过生成选项控制代码，而不需要重新修改代码本身<br>
2. 在调试生成时，通常会通过关闭优化的方式进行，通常也会关闭内联函数

典型的生成配置为Debug版、Release版、Production版（或者叫Final版）<br>
其中，release版是依然保留着调试信息并且开启断言的，只有Production版才会完全开启优化并且关上断言

可以通过自己编写make这种基于文本的生成工具生成混合生成版本(hybrid build)加快速率，即大部分单元是Release模式，要调试的部分是Debug模式

注意，有些时候Release模式甚至Production模式下的bug可能无法在Debug模式中复现，有可能是未初始化变量造成，有可能是一些关键代码被错误地放入了断言中，也可能是数据结构的大小或者打包方式在不同模式下出现区别，导致在内联或者编译器优化时引入bug，极端罕见的情况是编译器自身的bug（可能性从前向后依次递减）

主要的**调试发布版**的方法：
1. 因为现在的架构导致优化后不可避免的指令乱序的情况，学会单步执行反汇编是基本功
2. 通过监视寄存器去推理变量：当调试器不能再发布版中正确显示变量的值或对象内容时，如果变量初次使用距离不远，很可能变量还在寄存器里，可以监测寄存器的方式进行跟踪
3. 修改源代码：加入打印语句帮助解决问题
<br>

<h3>Chapter 3 游戏软件工程基础</h3>

**避免使用多重继承**，不然可能会造成菱形继承问题（Diamond_problem）

多态：容许采用单一接共同接口操作一组不同类型的对象/使得异质的对象集合从接口代码上变得统一

编码规范：需要着重注意接口为王（让.h文件整洁、简单、短小、易于理解并有良好注释）和显露错误（Making Wrong Code Look Wrong）这两点

基本数据类型根据平台不同是不确定的，当一定需要一个确定的数据类型时，可以通过__int8、__int16等进行声明

SIMD类型：single instruction，multiple data
游戏编程中最常见的是把4个32位IEEE-754的浮点数打包，存进一个128位的SIMD寄存器（即float4类型），所以，即使你在申明时使用的是float2 or float3很可能实际硬件实现时依然以float4方式进行

头文件内的定义与内联：
1. 对一般的函数，禁止把定义置于头文件中，很容易会导致“多重定义错误”
2. 对于内联函数，必要要把内联函数置于头文件中以供多于一个翻译文件使用，因为编译器必须“看见”函数主体才能够把函数内联，即将每个调用内联函数的地方都会复制该函数的机器码并把机器码直接嵌入调用方的函数中
```Cpp
//foo.h
//Correct Use
inline int max(int a, int b)
{
    return (a > b) ? a : b;
}

//Wrong Use
//因为编辑器实质上“看不到”min()的主体，导致min()只能在foo.cpp中内联，无法影响其他的调用该函数的.cpp文件
//foo.h
inline int min(int a, int b);

//foo.cpp
int min(int a, int b)
{
    return (a <= b) ? a : b;
}
```

C/C++内存布局：
1. 映像(image)文件：
    * 代码段：程序中的全部函数的可执行机器码
    * 数据段：全部**获初始化**的全局与静态变量
    * BSS段(block started by symbol, BSS)：程序中定义的所有未初始化的全局变量与静态变量。C/C++明确定义，任何未初始化的全局变量与静态变量值皆为零
    ```Cpp
    //foo.cpp
    //F32 32位IEEE-754浮点数
    F32 gInitializedGlobal = -2.0f;//储存在数据段中，已经被初始化
    F32 gUnInitializedGlobal;//未被初始化，存在BSS段中，按BSS规则，操作系统为其分配空间并且初始化为0
    ```
    * 只读数据段：包含程序中定义的只读（常量）全局变量，整数常量可能直接占用代码段的储存空间，而不储存在只读数据段中
2. 程序堆栈
    * 堆栈帧储存数据三类数据，从高到底分别是以下内容：
        + 调用函数的返回地址
        + 相关CPU寄存器的内容。当函数返回之后，各个寄存器会还原至调用方原来的状态，如果有返回值，会存在特殊的用于保存返回值的寄存器中
        + 函数中的所有局部变量<br>
3. 对象内存布局
    * 对齐与包裹
    ```Cpp
    struct InefficientPacking
    {
        U32   mU1; //32bit
        F32   mF2; //32bit
        U8    mB3; // 8bit
        I32   mI4; //32bit
        bool  mB5; // 8bit
        char* mP6; //32bit
    }

    //节约了空间
    struct MoreEfficientPacking
    {
        U32   mU1; //32bit(4byte对齐)
        F32   mF2; //32bit(4byte对齐)
        I32   mI4; //32bit(4byte对齐)
        char* mP6; //32bit(4byte对齐)
        U8    mB3; // 8bit(1byte对齐)
        bool  mB5; // 8bit(1byte对齐)
    }

    struct BestPacking
    {
        U32   mU1;    //32bit(4byte对齐)
        F32   mF2;    //32bit(4byte对齐)
        I32   mI4;    //32bit(4byte对齐)
        char* mP6;    //32bit(4byte对齐)
        U8    mB3;    // 8bit(1byte对齐)
        bool  mB5;    // 8bit(1byte对齐)
        U8    _pad[2];//明确的填充让空间更加清晰
    }
    ```
    * C++中类的布局
        + 虚函数：当类中含有/继承了一个或者多个虚函数，就会在类的布局最前端添加4字节（或者目标硬件中指针所需的字节数）作为**虚表指针(vpointer)**，指向**虚函数表(vtable)**
        + 虚函数表：每个类的虚函数表中，包含了该类*声明或继承而来*的所有虚函数指针，**每个含有虚函数的具体类都有一个虚函数表**，并且这些类的实例都会有虚表指针指向该虚函数表<br>
        虚函数表是多态的核心，使得我们不再需要考虑代码是和那个具体类进行沟通
        ```cpp
        举例如下：
        定义了Shape基类中有一个纯虚函数Draw()，Circle、Rectangle、Triangle都是派生类并且重载了此函数，提供了各自的实现
        那么，任何继承了Shape的类，它的虚函数表中都会有Draw()这个函数的条目，只是这个条目的指向上会指向具体类中的函数实现入口
        所以，如果我们有一个指针
        Shape *pShape
        当我们把它指向一个Circle类，调用Draw：对Circle实例中的vpointer进行解引用，取得vtable，然后从表中找到Draw()进行调用，所以执行Circle::Draw()
        ```
        这里再深入分析一下虚函数表与虚表指针
        当pShape指向了一个派生类实例，当我们调用pShape->Draw()时，虚表指针（注意，这个虚表指针是实例自身的）会解引用，得到该实例的虚函数表（这个表中存在的都是指向一个个具体函数的指针），从这个表中找到那个指向具体执行函数（因为是virtual，所以这个指针优先是指向在该类中具体重新实现的函数，当没有自己实现时，也就指向原先基类的函数作为继承，此时相当于virtual这个修饰词对这个派生类没有意义）的指针，然后根据这个指针进行具体的实现函数调用

错误的捕捉与处理
1. 用户错误：提示并继续运行
2. 开发者错误：让错误变得明显，并使得团队可在问题存在的情况下继续可以工作
    * 错误码
    * 异常：在游戏主机中效能有限，游戏引擎应该去完全关掉异常处理；但是在基于PC而开发的游戏引擎，可以尝试正常地使用异常处理的方式
    * 断言：开关断言可以依靠宏实现，十分方便。**注意：永远不要用断言捕捉用户错误，当断言失败，应该总是终止整个游戏程序！** 所以，断言应该用来捕捉严重错误，而失败后依然不是那么有影响的错误，不应该使用断言方式

<h3>Chapter 4 游戏所需的三维数学</h3>
因为这里的数学公式、矩阵之类的很多，使用Markdown有诸多不便，就省略一些，主要记录SSE和随机数生成

* SSE：单指令多数据拓展流(streaming SIMD extensions,SSE)，在引擎中多将4个32位的float值打包进单个128位寄存器中，单个指令就可以对4对浮点数进行并行运算<br>
    SSE在C/C++中提供了__m128这个内建的数据类型，它的运算可以通过汇编实现，也可以通过C/C++的内联汇编实现(inline assembly)。但是现在为了更好的编译器优化与跨平台，通常**建议使用编译器提供的内部函数**进行实现<br>
    **始终记住，将普通的float的加法和SIMD运算结合的混合代码无比愚蠢，必须规避或优化**
    ```cpp
    #include<xmmintrin.h>//SSE内部函数与__m128这个数据类型必须要包括这个.h文件
    #include <cstdio>

    //内联汇编
    __m128 addWithAssembly(__m128 a, __m128 b)
    {
        __m128 r;
        __asm
        {
            movaps xmm0, xmmword ptr [a]
            movaps xmm1, xmmword ptr [b]
            addps xmm0, xmm1
            movaps xmmword ptr [r], xmm0
        }
        return r;
    }

    //内部函数
    __m128 addWithIntrinsics(__m128 a,__m128 b)
    {
        __m128 r = _mm_add_ps(a, b);
        return r;
    }

    int main()
    {
        __declspec(align(16)) float A[] = { 2.0f,-1.0f,3.0f,4.0f };
        __declspec(align(16)) float B[] = { -1.0f,3.0f,4.0f,2.0f };
        __declspec(align(16)) float C[] = { 0.0f,0.0f,0.0f,0.0f };
        __declspec(align(16)) float D[] = { 0.0f,0.0f,0.0f,0.0f };

        __m128 a = _mm_load_ps(&A[0]);
        __m128 b = _mm_load_ps(&B[0]);

        __m128 c = addWithAssembly(a, b);
        __m128 d = addWithIntrinsics(a, b);

        _mm_store_ps(&C[0], c);
        _mm_store_ps(&D[0], d);

        printf("%g %g %g %g\n", C[0], C[1], C[2], C[3]);
        printf("------------------\n");
        printf("%g %g %g %g\n", D[0], D[1], D[2], D[3]);

        return 0;
    }
    ```
    也可以通过这个实现快速的矢量与矩阵乘法<br>
    **r** = **vM**<br>
    其中，v是1\*4矢量，M是4\*4矩阵
    ```cpp
    //一定是通过Row进行计算，最后才能并行化；如果使用Col进行计算，最后会有4个类似于vxM11 + vyM21 + vzM31 + vwM41这样的普通浮点add运算，导致性能大幅度损失
    #define SHUFFLE_PARAM(x,y,z,w) \
        ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))
    #define _mm_replicate_x_ps(v) \
        _mm_shuffle_ps((v),(v),SHUFFLE_PARAM(0,0,0,0))

    #define _mm_replicate_y_ps(v) \
        _mm_shuffle_ps((v),(v),SHUFFLE_PARAM(1,1,1,1))

    #define _mm_replicate_z_ps(v) \
        _mm_shuffle_ps((v),(v),SHUFFLE_PARAM(2,2,2,2))

    #define _mm_replicate_w_ps(v) \
        _mm_shuffle_ps((v),(v),SHUFFLE_PARAM(3,3,3,3))


    __m128 mulVectorMatrixAttemp1(__m128 v,
        __m128 Mrow1,
        __m128 Mrow2,
        __m128 Mrow3,
        __m128 Mrow4)
    {
        __m128 xMrow1 = _mm_mul_ps(_mm_replicate_x_ps(v), Mrow1);
        __m128 yMrow2 = _mm_mul_ps(_mm_replicate_y_ps(v), Mrow2);
        __m128 zMrow3 = _mm_mul_ps(_mm_replicate_z_ps(v), Mrow3);
        __m128 wMrow4 = _mm_mul_ps(_mm_replicate_w_ps(v), Mrow4);

        __m128 result = _mm_add_ps(xMrow1, yMrow2);
        result = _mm_add_ps(result, zMrow3);
        result = _mm_add_ps(result, wMrow4);

        return result;
    }

    //对上面的函数，我们也使用乘并加的指令进行简化，通常为madd：把前面两个参数相乘，与最后一个参数加和，但是SSE不支持madd，所以我们使用宏代替
    #define _mm_madd_ps(a,b,c) \
        _mm_add_ps(_mm_mul_ps((a),(b)), (c))

    __m128 mulVectorMatrixFinal(__m128 v,
        __m128 Mrow1,
        __m128 Mrow2,
        __m128 Mrow3,
        __m128 Mrow4)
    {
        __m128 result;
        result = _mm_mul_ps(_mm_replicate_x_ps(v), Mrow1);
        result = _mm_madd_ps(_mm_replicate_y_ps(v), Mrow2, result);
        result = _mm_madd_ps(_mm_replicate_z_ps(v), Mrow3, result);
        result = _mm_madd_ps(_mm_replicate_w_ps(v), Mrow4, result);
        return result;
    }
    ```
    矩阵之间相乘也可以依靠上面的运算实现，具体针对Visual Studio上的提供的SSE内部函数，可参阅MSDN
    
* 产生随机数
    因为确定性算法的原因，这些算法产生的都是伪随机数，仅仅只是非常复杂而已
    + 线性同余产生器(LCG)：不能产生特别高质量的伪随机序列，只要种子相同，序列就相同，很多时候不是很符合要求
    + 梅森旋转(MT)算法：通过了Diehard测试，其中一个采用SIMD矢量指令进行优化的SFMT很高效而有趣
    + Xorshift产生器/所有伪随机产生器之母(mother of all pseudo-random number generator)

<h3>Chapter 5 游戏支持系统</h3>
1. 游戏子系统<br>
    游戏需要子系统进行一系列例行但是关键任务的处理，对于子系统的构建与终止，最简单也是最实用的方法是明确地为每个单例管理器类定义启动与终止函数，用来取代构造、析构函数的作用

```cpp
    class RenderManager
    {
        public:
        RenderManager(){/*不做事情*/}
        ~RenderManager(){/*不做事情*/}

        void startUp()
        {
            //启动管理器
        }
        void shutDown()
        {
            //终止管理器
        }
    };
    //...类似的一系列类的定义

    RenderManager gRenderManager;
    //...一系列实例化

    int main(int argc, const char* argv)
    {
        //正确方式启动引擎
        gMemoryManager.startUp();
        gFileSystemManager.startUp();
        //...

        //运行游戏
        gSimulationManager.run();

        //以反向方式关闭引擎
        //...
        gFileSystemManager.shutDown();
        gMemoryManager.shutDown();

        return 0;
    }
```

这种方法既简单又实现方便，而且理解容易，易于后续的调试与维护<br>
2. 内存管理<br>
    在可行的情况下，代码中尽量避免使用**动态内存分配**，就像上面的代码，绝大多数的单例都是静态分配的对象<br>
    此外，在现代CPU的局部性原理的作用下，数据尽量分配在**细小**（希望一次都能载入CPU或者Cache中而不需要多次处理）而又**连续**（一次性读入CPU或者Cache，减少找的时间）的内存块中，CPU的操作会高效得多
* 堆分配<br>
    通过malloc()/free()/new等方式获得动态内存——非常慢（原因：1. 这是一个通用函数，需要处理任何大小的请求，需要大量的管理开销 2. 在多数操作系统上，malloc()/free()会导致系统模式从user mode -> kernel mode -> user mode，这些转换需要大量的时间开销）<br>
    经验规则：**维持最低限度的堆分配，并且永不在紧凑循环中使用堆分配**
* 改进之道<br>
    在一开始就通过malloc()或者new进行动态内存申请，之后的维护由程序员自己维护，不再使用malloc()或者new
    * 基于堆栈的分配器
    申请所得的连续内存安排一个指针指向堆栈的顶端，指针以下的是已经分配的，指针以上的是未分配的<br>
    **注意：使用堆栈分配器时必须要以分配时相反的顺序释放内存** 可以通过在上面做标记(Marker)的方式，释放时释放从栈顶到Marker之间的所有内存
    * 双端堆栈分配器(原理类似,略)
    * 池分配器 通常为了分配实践中遇到的**大量同等尺寸的小块内存**这种情况<br>
    简单来说，池内的每个元素会串成一个单链表，当需要的时候就取出，不用的时候放回去，每次都是 *O(1)* 的复杂度<br>
    为了充分节约空间(节约掉链表中\*next这个指针，因为每个元素都要有，而每个指针一般都要4字节，太浪费)：当这些自由元素不用时，自由元素里面就储存下一个节点的地址（用内存本身来存储自由列表中的"next"指针）但是，如果元素尺寸\<指针时，那么就可以使用池元素的索引代替指针实现链表。
    * 含有对齐功能的分配器
    * 单帧和双缓冲内存分配器 在游戏循环中常常分配一些临时用数据，当循环迭代结束时丢弃
        + 单帧分配器
        先预留一块内存，每帧开始时就把堆栈顶端指针重置到内存块的底端，然后在该帧过程中不断向上生长<br>
        缺点：程序员要有清醒的认识与自制，**决不可把指向单帧内存块的指针去跨帧使用！**
        + 双缓冲分配器
        相比较上面的单帧分配器可以多保留一帧，允许在第(i+1)帧时调用第i帧的内存<br>
        实现策略：建立两个相同尺寸的单帧堆栈分配器，并且在每帧交替使用
* 内存碎片<br>
因为开销等原因，很多设备都不支持可以规避这个问题的“虚拟内存”策略，一旦出现了很多内存碎片，可能会遇到尽管内存有空但是无法取用的尴尬境地
    + 堆栈/池分配器 不会产生内存碎片
    + 其他情况 要分配和释放的内存是不同大小的对象，并且是以随机次序进行的<br>
    需要对堆进行定期**碎片整理**：把内存空洞（未分配内存）移动到高地址（冒泡法般地进行比较，复制，交换，形成“上浮”效果）<br>
    但是，这样的操作影响了**已分配**的内存块，当程序中有指针指向这些内存块时，这些移动不可避免地会使指针失效。而指针重定位在C/C++中是很难实现的，通常使用智能指针或句柄实现（主要是使用句柄的方式以避免重定位指针）<br>
    句柄：实现往往是索引，这些索引指向句柄表中的元素，每个元素储存指针。句柄表本身不能被重定位，但是当要移动某个已分配的内存时，只需要扫描句柄表，并自动修改相应的指针即可。
    + 成本分摊 当进行碎片整理时操作因为涉及复制，所以是很慢的，我们可以把碎片整理成本分摊到多个帧中，每帧进行N次内存移动，N一般很小如8或16，这样在多个帧中完成这样的操作能够不会对游戏帧率产生影响
* 缓存一致性 由于局部性原理，我们应该尽可能避免缓存命中失败的情况，以下是经验法则：
    + 高效能的代码体积越短越好，体积按机器码指令数目为单位
    + 在关键性能的代码段，避免调用函数
    + 要调用某函数，尽量把该函数置于最靠近调用函数的地方，不要把该函数置于另一个翻译单元
    + **审慎使用内联函数**：内联小型函数的确能提升性能，但是过多的内联会增大代码体积，而指令也是通过缓存载入CPU的，这样可能会导致性能关键的代码不再能完全装入缓存之中，如果有一个紧凑循环，那么每个循环都会产生两次miss产生大量时间浪费，可能需要重构这部分代码
* 容器
    * 链表 
        + 外露式表（extrusive list）：一个元素能够同时置于多个链表之中，但是必须要动态分配节点，通常可以用池分配器进行分配（32位机器上就是12个字节）
        ```cpp
        template<typename ELEMENT>
        struct Link
        {
            Link<ELEMENT>* m_pPrev;
            Link<ELEMENT>* m_pNext;
            ELEMENT*       m_pElem;//指向元素具体位置
        }
        ```
        + 侵入式表（intrusive list）：节点的数据结构已经被嵌入到目标元素之中，避免了动态内存分配，但是丧失了每个元素能够同时置于多个链表之中的弹性，比如像存储一些实例，而实例的类是第三方库的，就不能使用侵入式表
        ```cpp
        template<typename ELEMENT>
        struct Link
        {
            Link<ELEMENT>* m_pPrev;
            Link<ELEMENT>* m_pNext;
            //由于继承的关系，无需ELEMENT* 指针
        };

        class SomeElement : public Link<SomeElement>
        {
            //其他成员
        };
        ```

<h3>Chapter 6 资源与文件系统</h3>
游戏的本质就是多媒体与玩家的交互体验，所以游戏引擎必须要有载入与管理多种媒体的能力

1. 文件系统 
    * 在Unix下，使用正斜线符(/)的方式作为路径分隔符，而Windows下为反斜线(\\)
    * Windows下是支持以卷为单位分开目录层次，而Unix下一切资源都是挂接在主层次下的某一个子树上
    * 注意，每个操作系统都会禁止一些字符出现在文件与目录名称中，有些特殊情况需要(\\)这种转义或者双引号才能够有效
  
  搜索路径是含有一串路径的字符串，各路径之间由冒号或分号进行分割，找文件是就会按照这些路径去找例如Windows就会先找到当前目录下的可执行文件，再找PATH环境变量下提供的路径中的文件

2. 文件I/O：
   * C标准程序中，含缓冲功能的与不含缓冲功能的API表（具体使用可以查询各种官方文档）：
  
    |操作|有缓冲|无缓冲|
    |:-:|:-:|:-:|
    |开启文件|fopen()|open()|
    |关闭文件|fclose()|close()|
    |读取文件|fread()|read()|
    |写入文件|fwrite()|write()|
    |移动访问位置|fseek()|seek()|
    |返回当前位置|ftell()|tell()|
    |读入单行|fgets()|无|
    |写入单行|fputs()|无|
    |格式化读取|fscanf()|无|
    |格式化写入|fprintf()|无|
    |查询文件状态|fstat()|stat()|

具体上的使用通常会把这些API包装成为自己的自定义I/O函数，这样可以保证API能够自主可控地在所有目标平台上保证同样的效果，而且API也可以根据引擎的需要进行一部分简化，提供可延伸功能

* 同步文件I/O C标准库中的两种文件I/O库都是同步的，即程序发出I/O请求后必须要等到整个读/写完毕才能继续运行
    ```cpp
    #include<iostream>
    typedef int8_t U8;
    bool syncReadFile(const char* filePath,
        U8* buffer, size_t bufferSize, size_t& rBytesRead)
    {
        FILE* handle = fopen(filePath, "rb");
        if(handle)
        {
            //阻塞直至所有数据读取完毕
            size_t bytesRead = fread(buffer, 1, bufferSize, handle);
            int err = ferror(handle);//如果过程出错，取得错误码
            fclose(handle);
            if(0==err)
            {
                rBytesRead = bytesRead;
                return true;
            }
        }
        return false;
    }
    void main(int argc,const char* argv[])
    {
        U8 testBuffer[512];
        size_t bytesRead = 0;

        if(syncReadFile("C:\\test.bin",testBuffer,sizeof(testBuffer),bytesRead))
        {
            printf("SUCCESS: read %u bytes\n", bytesRead);
            //可以使用缓冲内容了
        }
    }
    ```
* 异步文件I/O 串流(streaming)指在背景载入数据，而主程序同时继续运行，为了支持串流，就要使用异步(asynchronous)文件I/O库，如果目标平台不提供这样的库，也可以自己开发一个（前提是平台能提供线程或类似服务）
    ```cpp
    #include<iostream>
    typedef int8_t U8;

    AsyncRequestHandle g_hRequest;//异步I/O请求的句柄
    U8 g_asyncBuffer[512];

    static void asyncReadComplete(AsyncRequestHandle g_hRequest);

    void main(int argc,const char* argv[])
    {
        //注意：这里调用asyncOpen()可能本身就是异步的，在这里暂时忽略
        //假设该函数是阻塞的
        AsyncRequestHandle hFile = asyncOpen("C:\\test.bin");

        if(hFile)
        {
            //此函数做出读取请求，然后立即返回（非阻塞）
            g_hRequest = asyncReadFile(
                hFile,                  //文件句柄
                g_asyncBuffer,          //输入缓冲
                sizeof(g_asyncBuffer),  //缓冲大小
                asyncReadComplete);     //回调函数
        }

        for(;;)
        {
            OutputDebugString("zzz...\n");
            Sleep(50);
        }
    }

    //当数据都读入后，才会调用此函数
    void asyncReadComplete(AsyncRequestHandle hRequest)
    {
        if(hRequest == g_hRequest && asyncWasSuccessful(hRequest))
        {
            //现在数据已经读入g_asyncBuffer[]。查询实际读入的字节数量
            size_t bytes = asyncGetBytesReadOrWritten(hRequest);

            char msg[256];
            sprintf(msg, "async success, read %u bytes\n", bytes);
            OutputDebugString(msg);
        }
    }
    ```
    需要谨记文件I/O是一个实时系统，要有时限与优先级，异步I/O系统需要有暂停低优先级的请求的能力，才能够让高优先级的请求在合理的时限中完成<br>
    异步I/O工作原理：主线程调用异步函数，把请求放在一个队列中并立即传回；同时I/O线程会从队列中拉取请求并以阻塞I/O函数如read()或者fread()等处理这些请求；完成后就会调用主线程之前的提供的回调函数，告知这个操作已完成<br>
    因此，任何的同步操作都可以通过代码置于另一个线程上变成异步操作

3. 资源管理器
    每个游戏都是由各种各样的资源构成，都需要一个资源管理器进行调度，每个资源管理器都由两个组件构成：
    * 离线资源管理：用来创建资产并且把他们转化成为引擎可用的形式<br>
    因为游戏中有大量而且内容庞大的美术、音乐等资源，对于大部分资产来说，这些东西都是由先进的数字内容创作（DCC）工具完成的，如Maya，ZBrush，Photoshop，Houdini，但是这些格式一般不适合游戏引擎直接使用，需要经过一些资产调节管道将资产转化为可支持的格式。我们不可能每次都用人工的方式进行处理每个资产，而需要一个半自动化的资源管道，而管道所需要的数据储存在某种数据库中（例如在虚幻中，资源数据库为UnrealEd，因为这个组件被设计为引擎的一部分，所以可以使得UnrealEd能够在创建资产后立即看到资产的模样，得知资产是否有效与正确配置，但是缺点是所以资源都存在少量但巨大的二进制包文件中，很难版本控制）
        + 导出器 把DCC的原生格式变成我们能处理的格式，方法是为DCC工具撰写自定义插件，变成某种中间格式供后续管道继续使用
        + 资源编译器 为由DCC导出的数据以不同方式做一些处理（类似对数据的重新编译）才能让引擎使用
        + 资源链接器 把多个资源先结合成一个有用的包再载入引擎，比如一个三维模型，需要绑定多个网格文件，材质文件，骨骼文件，动画文件等结合生成再导入

        资源依赖，因为资源之间存在相互依存关系构成一张有向图，在处理时我们修改了一个资源本身或者其数据格式之后，其他资源也可能会受到影响，需要重新生成（还是要以正确的顺序进行生成）
    * 运行时资源管理：确保资源在使用之前已经载入内存，在不需要的时候把它们从内存中取下<br>
        所有的资源都必须要有某个全局唯一标识符（Global Unique Identifier，GUID）常见的GUID选项就是资源的文件系统路径（储存为字符串或者hash值）而虚幻是通过包地址+包内地址串接组合而成。<br>
        
        以下是设计的要求与实现思路：
        + 确保在任何时刻，同一个资源在内存中只有一个副本：使用**资源注册表**的方式记录已载入的资源。实现方法：使用字典方式储存，键是资源唯一标识符，值是指向内存中资源的指针；当注册表中找不到时，就进行资源载入，这时可以使用loading界面给玩家或使用较难实现的异步方式加载（玩家在玩A时，后台慢慢加载后面一个场景B）
        + 管理一个资源的生命周期：玩家一直能听到或者看到的资源，都应该归于LSR(load-and-stay-resident)资源，生命周期无限，其余的资源的归还都是比较困扰的问题，需要仔细考量<br>
        解决方案：对资源进行使用计数（斜体代表该资源不在内存中，正体代表该资源已经载入内存，有括号表示该资源正要载入或卸下）此外，在具体分配方式考量中可以考虑使用之前介绍的几种资源分配方式进行分配（基于堆、堆栈、池等）

        |事件|A|B|C|D|E|
        |:-:|:-:|:-:|:-:|:-:|:-:|
        |起始状态|*0*|*0*|*0*|*0*|*0*|
        |关卡1的引用计数加1|*1*|*1*|*1*|*0*|*0*|
        |载入关卡1|(1)|(1)|(1)|*0*|*0*|
        |玩关卡1|1|1|1|*0*|*0*|
        |关卡2的引用计数加1|1|2|2|*1*|*1*|
        |关卡1的引用计数减1|0|1|1|*1*|*1*|
        |卸下关卡1，载入关卡2|*(0)*|1|1|(1)|(1)|
        |玩关卡2|*0*|1|1|1|1|
        
        + 保持引用完整性
            + 处理交叉引用等问题：指针只是内存地址，值离开运行的程序之后就没有意义，所以储存数据至文件时，不能使用指针表示对象之间的依赖性，而应该使用全局唯一标识符(GUID)进行交叉引用（把交叉引用储存为字符串或者散列码，内涵被引用的对象的唯一标识符）
            + 指针修正表的方式：储存时把**指针**变成**文件偏移值**（要注意32位与64位的区别，开发编译平台与目标平台的兼容）在载入文件到内存之后把偏移值转回指针；通过一个简单列表的方式储存每个类的中指针，当进入内存之后就能够进行修正
            + 外部(非单个资源文件内的对象)引用处理：除了要指明偏移值和GUID外，还需要加上资源对象所属文件的路径
        + 管理载入后的合理内存用量
            载入后的初始化：希望每个资源都能够经过离线工具完全处理，载入后内存能够立即使用。在C++中可以实现为两个虚函数的方式Init()和Destroy()进行（因为构造函数不可虚，所以无法对基类进行操作，因此选择自己写一个虚函数进行）
        + 通常保证单一统一接口处理多种资源类型
        + 支持串流操作（异步资源载入）


<h3>Chapter 7 游戏循环与实时模拟</h3>
渲染循环：当摄像机在三维场景中移动的时候，屏幕或者视窗上的一切内容都会不断改变，所以将使用渲染循环：给观众快速连续显示一连串的静止影像

```cpp
while(!quit)
{
    updateCamera();

    //更新场景中的所有动态元素的位置、方向和一系列视觉状态
    updateSceneElements();

    //把静止的场景渲染至屏幕外的帧缓冲中
    renderScene();

    //交换背景缓冲与前景缓冲，令最近渲染的影像显示于屏幕之上
    swapBuffers();
}
```

游戏循环：
* 视窗消息泵：先处理来自Windows的消息，无消息时才去执行引擎的任务，但是这里的前提条件是设定了任务的优先次序，当玩家在改变游戏视窗等操作时会导致游戏直接卡住不动

```cpp
while(true)
{
    //处理所有待处理的Windows消息

    MSG msg;

    while(PeekMessage(&msg,NULL,0,0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //无Windows消息需要处理时才去真正执行游戏循环
    RunOneIterationOfGameLoop();
}
```

* 回调驱动框架：很多游戏引擎子系统和第三方游戏中间套件都是以程序库的形式构成的：程序员正确使用程序库提供的各个函数和类；也有一些游戏引擎与游戏中间件是用框架构成的，程序员需要提供框架中空缺的自定义实现或者复写框架中的预设行为，但是这样相比前者对于应用软件的控制流程只有少量控制

* 基于事件的更新：多数引擎都会存在一个事件系统，让每个引擎子系统关注某类型的事件，当那些事件发生时就一一回应。有些游戏引擎会使用事件系统来对所有或部分子系统进行周期性更新，也就是说：事件可以先置于队列，稍后才取出处理。当游戏进行更新时，只要简单加入事件，在事件处理器里面代码就可以以任何所需的周期进行更新

抽象时间线：
* 真实时间：直接使用CPU的高分辨率计时寄存器来度量时间。原点定义为计算机上次启动时间。CPU经历的周期 * 频率 => 秒数
* 游戏时间：独立于真实时间的一条时间线，如果希望暂停游戏，就可以简单暂停对于游戏时间的更新，如果想慢动作，就把游戏时间更新速度慢于真实时间。**注意：游戏暂停时游戏循环是继续进行的，只不过是游戏时钟停止了！**
* 局部与全局时间线：每个动画片段与音频片段都有一个局部时间线，原点定义为片段的开始，局部时间线就按照原来的制作或录制的时间量度播放，我们可以加速/减速/反向播放动画或音频，因为这些效果都可以视为局部与全局时间线之间的映射。

测量与处理时间：
因为&Delta;x = x1 + v&Delta;t 所以玩家对于物体运动的感知取决于帧时间&Delta;t，所以&Delta;是游戏编程的核心问题之一

目标：开发与CPU速度脱钩的游戏：
  * 方法一：读取CPU的高分辨率计时器两次（一次在帧开始之前，一次在帧开始之后，取差值就能精确度量上一帧的&Delta;t，但是问题是使用第k帧去度量第k+1帧的时间不一定正确，而且可以会导致恶性循环（第k帧30ms，k+1帧40ms，然后度量k+2帧会估计为40ms，然后...）
  * 方法二：调控帧率。控制每帧都耗时相同（30FPS就33.3ms），如果本帧的耗时超过了这个限度，就白等到下一个目标时间，而本帧的耗时小于这个，就让主线程休眠。这样只能在游戏帧率比较稳定而且接近目标帧率时才比较有效。优点：稳定帧率对于游戏的数值积分等运算比可变帧率更有优势，如果更新的速率不配合屏幕刷新率会导致画面撕裂（电子枪等在扫描过程中执行了交换了背景缓冲区和前景缓冲区，让画面上半部分显示老的影像，下半部分显示新的影像），而这个方式可以避免画面撕裂的发生

时间单位与时钟变量:
* 64位整数时钟：同时支持高精度(3GHz CPU每周期位0.33ns)与很大的数值范围(3GHz CPU需要195年才会出现循环，远超使用寿命)是最好的方式
* 32位整数时钟：量取高精度与较短时间的策略（一般用来量取&Delta;t）
* 32位浮点时钟：把较小的持续时间以秒为单位储存为浮点数。必须避免使用浮点时钟变量储存很长的持续时间，最多能度量几分钟，而且需要定期重置为0，减少误差

应付断点：Debug会暂停游戏循环，但是时间依然前进，这样会导致巨大的增量时间，而如果这些时间被错误地传入了子系统中很可能导致整个游戏等的崩溃；解决策略：当量度到某帧的持续时间超过预设的时间，就假设游戏刚从断点中回复，人工地将增量时间设定为1/30s等而避免出现一个巨大的帧时间尖峰

简单的时钟类：通常含有一个变量，用来记录自时钟创建以来经过的绝对时间，一般选择以机器周期为单位的64位无符号整数最简单与方便

```cpp
typedef int64_t U64;
typedef float F32;

class Clock
{
    U64 m_timeCycles;
    F32 m_timeScale;
    bool m_isPaused;
    static F32 s_cyclesPerSecond;

    static inline U64 secondsToCycles(F32 timeSeconds)
    {
        return (U64)(timeSeconds * s_cyclesPerSecond);
    }

    static inline F32 cyclesToSeconds(U64 timeCycles)
    {
        return (F32)(timeCycles / s_cyclesPerSecond);
    }

public:
    static void init()
    {
        s_cyclesPerSecond = (F32)readHiResTimerFrequency();
    }

    explicit Clock(F32 startTimeSeconds = 0.0f) :
        m_timeCycles(secondsToCycles(startTimeSeconds)),
        m_timeScale(1.0f),
        m_isPaused(false)
    {}

    U64 getTimeCycles() const
    {
        return m_timeCycles;
    }

    F32 calcDeltaSecond(const Clock& other)
    {
        U64 dt = m_timeCycles - other.m_timeCycles;
        return cyclesToSeconds(dt);
    }

    void update(F32 dtRealSeconds)
    {
        if(!m_isPaused)
        {
            U64 dtScaledCycles = secondsToCycles(dtRealSeconds * m_timeCycles);
            m_timeCycles += dtScaledCycles;
        }
    }

    void setPaused(bool isPaused)
    {
        m_isPaused = isPaused;
    }

    bool isPaused() const
    {
        return m_isPaused;
    }

    void setTimeScale(F32 scale)
    {
        m_timeCycles = scale;
    }

    F32 getTimeScale() const
    {
        return m_timeCycles;
    }

    void singleStep()
    {
        if(m_isPaused)
        {
            U64 dtScaledCycles = secondsToCycles((1.0f / 30.0f) * m_timeCycles);
            m_timeCycles += dtScaledCycles;
        }
    }
};
```

多处理器的游戏循环：
* 分叉与汇合：把一个单位的工作分割成更小的子单位，再把这些工作量分配到多个核或者硬件线程之中（分叉），最后待所有工作完成后再合并结果（汇合）
* 每个子系统运行与独立线程：主控线程负责控制与同步这些子系统的次级子系统，并应付游戏的大部分高级逻辑（游戏主循环），子系统去重复执行那些比较有隔离性的功能，比如渲染引擎，物理模拟，动画管道，音频引擎等等

异步程序设计（为了多处理器的硬件而写的代码）：当操作请求发出之后，通常不会马上得到结果，很可能是在某帧的开始启动请求，下一帧才能够提取结果。调用的函数只会建立一个作业并把它加到队列之中，然后主线程看情况调用空闲的CPU或者核去处理这个作业

网络多人游戏循环：
* 主从式模式：大部分游戏逻辑运行中服务器上，客户端只是做渲染工作（单机游戏可以看成一个客户端且服务器与客户端运行在同一机器上）两者的代码可以以不同的频率循环
* 点对点模式：每一部机器既有服务器的特性，也有客户端的特性，机器对拥有管辖权的对象就像服务器，对无管辖权的对象就像客户端

<h3>Chapter 8 人体学接口设备</h3>
技术：
* 轮询：定期轮询来读取输入，查询设备状态（读取硬件寄存器/读取经内存映射的I/O端口）
* 中断：只有当状态出现变化之后才会把数据传送至游戏引擎（硬件中断方式通信）

输入类型
* 数字式按钮：按下/释放两种状态
* 模拟式轴与按钮：获得一个范围的值的模拟信号，然后把它数字化传入引擎（死区：因为存在电压噪声，所以需要引入一个小死区来控制输入稳定，死区需要容纳最大的噪声，也要足够小避免影响玩家操作体验）

输入事件检测：游戏需要检测事件——状态改变而不是每帧的当前状态
技巧：把上一帧与本帧的按钮状态字进行位异或，某个位为1代表两帧中出现了状态的改变，而测定这个事件是按下还是释放按钮，可以再通过后续审视每个按钮的状态

<h3>Chapter 9 调试及开发工具</h3>
