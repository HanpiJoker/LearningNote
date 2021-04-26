# Unix标准及实现
## 1. Unix标准化
### 1. ISO C [标准库，C的可移植]
**历史**：
1. 1989年，C语言的ANSI标准X3.159-1989得到批准(这个是C89)。此标准也被采纳为国际标准ISO/IEC 9899:1990；(也就是所谓的C90)
> ANSI：美国国家标准学会
> ISO ：国际化标准组织
> IEC ：国际电子技术委员会 
2. 1999年，ISO C标准更新，并被批准为ISO/IEC 9899:1999[ C99 ]。
3. 2011年，ISO C是当前最新的C语言标准。这次修订新增了被主流C语言编译器(如GCC,Clang,Visual C++等)增加的内容，和引入了内  
存模型以更好的执行多线程。之前C99的一些被推迟的计划在C11中增加了，但是对C99仍保留向后兼容。

**简介&意义**
ISO C标准现在由ISO/IEC的C成语设计语言国际标准工作组维护和开发，该工作称为 ISO/IEC JTC1/SC22/WG14，简称WG14。  
ISO C的意图是提供C程序的可移植性，使其能适合于大量不同的操作系统。此外，此标准还定义了C程序设计语言的标准库。  

### 2. IEEE POSIX [接口标准，应用程序的可移植]
> IEEE：电器和电子工程师学会
> POSIX：可移植操作系统接口

POSIX 原来指的是IEEE标准1003.1-1988(操作系统接口)，后来扩展成包括很多标记为1003的标准及标准草案，eg：shell和实用程序(1003.2)  
**历史**
1. 1003.1标准的1988版修订了文本提交给ISO，最终文档作为IEEE标准的1003.1-1990正式出版，即为国际标准ISO/IEC 9945-1:1990。[POSIX.1]
2. 1996年，标准的修订版发布，包括了 1003.1-1990，1003.1b-1993实时扩展标准以及被称为pthreads的多线程编程接口。[ISO/IEC 9945-1:1996]
3. 1999年，发布IEEE标准1003.1d-1999，增加更多实时接口
4. 2000年，出版了IEEE标准1003.1j-2000,1003.1q-2000，前者包括更多实时接口，后者增加了标准在事件跟踪方面的扩展。
5. 2001年，发布了1003.1-2001标准。这个版本组合了多个1003.1的修正，1003.2标准和Single Unix Specification(SUS)第2版的若干。  
6. 2004年，POSIX.1说明随着技术勘误得到更新。2008年做了更多综合的改动并作为基本说明的第7版发布，2008年底ISO批准新的版本在  
   2009年进行发布，即ISO/IEC9945:2009。

*PS*：
  1003.1标准说明了一个接口，并不是一种实现。所以不区分系统调用和库函数。所有在标准中的例程被称为函数。此外POSIX.1 没有超  
级用户的概念，代之以规定了某些操作要求"适当的优先权"。POSIX.1标准现在由Austin Group开放工作组维护。
### 3. Single UNIX Specification[单一Unix规范，对POSIX.1标准的扩充] [Unix系统的标志]
**简介**：
Single UNIX Specification (SUS，单一Unix规范)，定义了一些附加接口扩展了POSIX.1规范提供的功能。

*Unix系统的特征*：
只有遵循**XSI**(X/Open System Interface, X/Open系统接口)的实现才能被称为Unix系统。POSIX.1中的XSI  
选项描述了可选的接口，定义了遵循XSI实现必须支持POSIX.1的那些可选部分。这些必须支持的部分包括：文  
件同步，线程栈地址和长度属性，线程进程共享同步以及_XOPEN_UNIX符号常量。

> 遵循XSI的系统的可选接口：
> 1. 加密：有符号常量_XOPEN_CRYPE标记
> 2. 实时：由符号常量_XOPEN_REALTIME标记
> 3. 高级实时
> 4. 实时线程：由符号常量_XOPEN _REALTIME_THREADS标记
> 5. 高级实时线程

Single UNIX Specification是Open Group 的出版物，而Open Group是由两个工业社团X/Open 和开放系统软件  
基金会在1996年合并构成的。X/Open 过去出版了 *X/Open Portability Guide(X/Open 可移植性指南)*，它采  
用了特定标准。填补了其他标准缺失功能的空白。这些指南的目的是改善应用的可移植性。

**历史**
1. 1994年，X/Open发布了Single UNIX Specifiation第1版，因为它包含了1170个接口，所以又称为"Spec 1170"  
> Spec 1170 源于通用开放软件环境(COSE)的倡议，该倡议的目标是进一步改善应用程序在所有UNIX  
> 操作系统实现之间的可移植性。COSE的成员包括：Sun，IBM，HP，Novell/USL以及OSF等。它们的  
> UNIX都包括了通用商业化应用软件使用的接口，而从这些接口中选择了1170个接口被包括在下列标准  
> 中：X/Open通用应用环境[ CAE, 也被称为XPG4以表示它与前身X/Open Portability Guide(X/Open 可移植性指南)  
> 的历史关系 ]第四版，系统V接口定义第3版Level 1接口，OSF应用环境规范Full USE。

2. 1997年，Open Group 发布了SUS的第2版，增加了对线程，实时接口，64位处理，大文件以及增强的多字节字  
符处理等功能的支持
3. 2001年，SUS第3版发布(SUSv3)。SUSv3的基本规范与IEEE标准1003.1-2001相同。此外还包括了X/Open Curses  
第4发行版第2版。但该规范不是POSIX.1的组成部分。
4. 2004年4月，Open Group 发布了SUS第3版2004年版，将更多技术上的更正合并到标准的正文中。
5. 2008年，SUS再次更新，包括了更正和新的接口，移除弃用的接口以及将一些未来可能被删除的接口标记为弃用  
接口等。另外，有一些过去被认为可选的接口变成必选接口，其中包括异步I/O，屏障，时钟选择，存储映像文件，  
内存保护，读写锁，实时信号，POSIX信号两，旋转锁，线程安全函数，线程，超时机制以及时钟等。最终形成的标准  
即是POSIX.1-2008。Open Group把这个版本和X/Open Curses打包，并于2010年作为SUS的第4版发布。(SUSv4)

### 4. FIPS
FIPS：联邦信息处理标准，这一标准由美国政府发布的，并用于计算机系统的采购。  
POSIX.1 FIPS要求任何希望向美国政府出售符合POSIX.1标准的计算机系统的厂商  
都应支持POSIX.1的某些可选功能。不过目前这个标准已被撤回。

## 2. UNIX系统实现
*Unix系统历史发展*
各个版本起源于在PID-11系统上运行的UNIX分时系统第6版和第7版。进而演化出了3个分支。
1. AT&T 分支，从而引出了系统III和系统V (UNIX的商用版本)
2. 加州大学伯克利分校分支，从而引出了4.xBSD实现。
3. 由AT&T贝尔实验室的计算科学研究中心不断开发的UNIX研究版本，从而引出UNIX分时系统  
   第8版，第9版，终止于第10版[1990]

### 1. SVR4 
### 2. 4.4BSD
### 3. FreeBSD
### 4. Linux/GUN
### 5. Mac OS X
### 6. Solaris
### 7. 其他
AIX   (IBM版UNIX)
HP-UX (HP版UNIX)
IRIX  (Silicon Graphics版UNIX)
UnixWare (SVR4派生，由SCO销售)

## 3. 标准和实现的关系
重点是POSIX.1标准在系统中的实现，并指明了具体实现与POSIX的差别

## 4. 限制
Unix已有若干中可移植的方法用以确定这些幻数和具体实现定义的限制。 这非常有助于  
改善UNIX环境下软件的可移植性。
以下两种限制是必需的：
1. 编译时的限制 ( 例如，短整形的最大值是多少？ ) 
2. 运行时的限制 ( 例如，文件名有多少个字符？)
此外，在一个给定的实现中可能是固定的，而在另一个实现的则可能是变动的  
为了解决这类问题，提供了以下3种限制
1. 编译时限制 ( 头文件 )
2. 与文件或目录无关的运行时限制 ( sysconf函数 )
3. 与文件或目录有关的运行时限制 (pathconf 和 fpathconf函数)

### 1. ISO C限制
ISO C定义的所有编译时限制都列在头文件< limits.h >中。这些限制常量在一个给定的系统中  
并不会改变。在头文件< float.h >中，对浮点数据类型也有类似的一组的定义。

**说明**：
虽然ISO C标准规定了整形数据类型可接受的最小值，但POSIX.1对C的标准进行了扩充。为了符合  
POSIX.1的标准，具体实现必须支持INT_MAX的最小值2 147 483 547，INT_MIN为-2 147 483 647,  
UINT_MAX为4 294 967 295。因为POSIX.1 要求具体实现支持8位的char类型，所以CHAR_BIT必须是8，
SCHAR_MIN必须是-128，SCHAR_MAX必须是127，UCHAR_MAX必须是255。

ISO C 其他常量：

1. FOPEN_MAX: 这是具体实现保证可同事打开的标准I/O流的最小个数，该值在头文件< stdio.h >  
中定义，其最小值是8。POSIX.1中的STREAM_MAX(若定义的话)则应与FOPEN_MAX具有相同的值。
2. TMP_MAX: 在< stdio.h > 中定义了常量TMP_MAX，这是由tmpnam函数产生的唯一文件名的最大个数。
3. FILENAME_MAX: 虽然有定义，但是应该避免使用。因为POSIX.1提供了更好的替代常量(NAME_MAX和PATH_MAX)。

### 2. POSIX限制
限制和常量分成下列7类：
* 数值限制：LONG_BIT，SSIZE_MAX和WORD_BIT。
* 最小值：见< limits.h >。这些最小值不变的--它们不随系统的改变而改变，它们指定了这些特征  
  最具约束性的值，一个符合POSIX.1的实现应当提供至少这样大的值。
* 最大值：_POSIX_CLOCKRES_MIN。
* 运行时可以增加的值：CHARCLASS_NAME_MAX，COLL_WEIGHTS_MAX，LINE_MAX，NGROUPS_MAX和RE_DUP_MAX。
* 运行时不变值(可能不确定)：见< limits.h >。
* 其他不变值：NL_ARGMAX，NL_MSGMAX，NL_SETMAX和NL_TEXTMAX。
* 路径名可变值：FILESIZEBITS，LINK_MAX，MAX_CANON，MAX_INPUT，NAME_MAX，PATH_MAX，PIPE_BUF和SYMLINK_MAX。

### 3. XSI限制
XSI定义了代表实现限制的几个常量：
- 最小值：NL_LANGMAX，NZERO，_XOPEN_IOV_MAX，_XOPEN_NAME_MAX，_XOPEN_PATH_MAX；
- 运行时不变值：IOV_MAX，PAGE_SIZE。

### 4. sysconf，pathconf和fpathconf

```
    #include <unistd.h>
    long sysconf (int name);
    long pathconf (const char *pathname, int name);
    long fpathconf (int fd, int name);
```
**讨论( 返回值 )**：
- 如果name参数并不是一个合适的常量，这3个函数和都返回-1，并把errno置为EINVAL。  
- 有些name会返回一个变量值(返回值 >= 0)或者提示该值是不确定的。不确定的值通过  
 返回-1来体现，而不改变errno的值。
- 此外，_SC_CLK_TCK的返回值是美妙的时钟滴答数，用于times函数的返回值

**讨论( pathconf的参数pathname和fpathconf的参数fd )**：
- _PC_MAX_CANON和_PC_MAX_INPUT引用的文件必须是终端文件。
- _PC_LINK_MAX和_PC_TIMESTAMP_RESOLUTION引用的文件可以使文件或目录。如果是目录，  
则返回值用于目录本身，而不用于目录内的文件名项。 
- _PC_FILESIZEBITS和_PC_NAME_MAX引用的文件必须是目录，返回值用于该目录中的文件名。
- _PC_PATH_MAX引用的文件必须是目录。当所指定的目录是工作目录时，返回值是相对路径  
名的最大长度。
- _PC_PIPE_BUF引用的文件必须是FIFO，管道或目录。在管道或FIFO的情况下，返回值是对  
所引用的管道或FIFO的限制值。对于目录，返回值是对在该目录中创建的任一FIFO的限制值。
- _PC_SYMLINK_MAX引用的文件必须是目录。返回值是该目录中符号链接可包含字符串的最大  
长度。


< 未完待续 >
