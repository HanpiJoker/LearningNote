# 5. 标准I/O库
## 1. 流和FILE对象
- 第3章的所有文件I/O函数都是围绕文件描述符进行的。而对于标准I/O库, 它们的操作是围绕
    流(stream)进行的。流的定向决定了所读, 写的字符是多字节还是单字节的。
- 在未定向的流上使用一个多字节I/O函数, 则将流的定向设为宽定向; 如果使用一个单字节的I/O函数, 则将流定向为字节定向。 
- 只有两个函数可以改变流的定向: freopen()清除一个流的定向, fwide()设置流的定向。

```c
#include <stdio.h>
#include <wchar.h>
int fwide (FILE *fp, int mode);
/* 返回值:
 * 若流是宽定向, return 正值;
 * 若流是字节定向, return 负值;
 * 若流是未定向的, return 0;
 */
```

### 参数mode
根据mode的取值决定fwide的行为:
- mode < 0, fwide试图使指定的流是字节定向;
- mode > 0, fwide试图使指定的流是宽定向;
- mode = 0, fwide不设置流的定向, 返回标识该流定向的值;


## 2. 标准输入, 标准输出和标准错误
进程预定义了3个流: 标准输入, 标准输出和标准错误。这些流与文件描述符: STDIN_FILENO,
STDOUT_FILENO, STDERR_FILENO所引用的文件相同。3个标准流通过预定义指针stdin, stdout,
stderr加以引用。

## 3. 缓冲
标准I/O提供以下3种类型的缓冲:
1. 全缓冲: 在填满标准I/O缓冲区后才进行实际I/O操作。对于驻留在磁盘上的文件通过是由标准
I/O库实施全缓冲的。
> 术语冲洗(flush)--标准I/O缓冲区的写操作。缓冲区可以有标准I/O例程自动地冲洗, 或者可以
> 调用函数fflush; 在UNIX环境中, flush有两种意思。在标准I/O库方面, flush意味着将缓冲区
> 内容写到磁盘上; 在终端驱动程序方面, flush表示丢弃存储在缓冲区中的数据。

2. 行缓冲: 这种情况下, 当在输入和输出中遇到换行符时, 标准I/O库执行I/O操作。
> 行缓冲的两种限制:
> 1. 标准I/O每行的缓冲区的长度是固定的。只要填满了缓冲区, 那么即使没有换行符, 也进行
> I/O操作。
> 2. 通过标准I/O库要求从一个不带缓冲的流, 或者一个行缓冲的流得到输入数据, 那么就会冲洗
> 所有行缓冲输出流。

3. 不带缓冲: 标准I/O库不对字符进行缓冲存储。标准错误流stderr通常是不带缓冲;

ISO C 要求的缓冲特征:
- 只有标准输入和标准输出不指向交互式设备时, 它们才是全缓冲的;
- 标准错误绝对不是全缓冲的;
    多数系统默认缓冲类型:
- 标准错误是不带缓冲的;
- 若是指向终端设备的流, 则是行缓冲; 否则是全缓冲;

### 更改缓冲类型的函数
```c
#include <stdio.h>
void setbuf (FILE *restrict fp, char *restrict buf);
int setvbuf (FILE *restrict fp, char *restrict buf, int mode, size_t size);
/* 返回值: 若成功, return 0; 若失败, return 非0 */
```

函数 | mode | buf | 缓冲区及长度 | 缓冲类型
---- | ---- | --- | ------------ | --------
setbuf |  | 非空 | 长度为BUFSIZE的用户缓冲区buf | 全缓冲或行缓冲
|  | NULL | (无缓冲区) | 不带缓冲
setvbuf | \_IOFBF | 非空 | 长度为size的用户缓冲区buf | 全缓冲
|         | NULL | 合适长度的系统缓冲区buf   |
| \_IOLBF | 非空 | 长度为size的用户缓冲区buf | 行缓冲
|         | NULL | 合适长度的系统缓冲区buf   |
| \_IONBF | (忽略) | (无缓冲区) | 不带缓冲

### 强制冲洗一个流
```c
#include <stdio.h>
int fflush (FILE *fp);
/* 返回值: 若成功, return 0; 若失败. return EOF */
```
_当fp=NULL, 所有输出流被冲洗_

## 4. 打开流
```c
#include <stdio.h>
FILE *fopen (const char *restrict pathname, const char *restrict type);
FILE *freopen (const char *restrict pathname, const char *restrict type, FILE *restrict fp);
FILE *fdopen (int fd, const char *restrict type);
/* 返回值: 若成功, return 文件指针; 若失败, return NULL */
```
### 函数分析
1. 区别:
> 1. fopen函数打开路径名为pathname的一个指定的文件;
> 2. freopen函数在一个指定流上打开一个指定的文件。若该流已经打开, 则先关闭该流;
> 若该流已经定向, 则使用freopen清除该定向。
> 3. fdopen函数取一个已有的文件描述符, 与标准I/O流相结合。

2. type参数

type | 说明 | open(2)标志
---- | ---- | -----------
r或rb | 为读而打开 | O_RDONLY
w或wb | 把文件截断至0长, 或为写而打开 | O_WRONLY &#124; O_CREAT &#124; O_TRUNC
a或ab | 追加: 为在文件尾写而打开, 或为写而创建 | O_WRONLY &#124; O_CREAT &#124; O_APPEND
r+或r+b或rb+ | 为读和写而打开 | O_RDWR
w+或w+b或wb+ | 把文件截断至0长, 或为写和读而打开 | O_WRONLY &#124; O_CREAT &#124; O_TRUNC
a+或a+b或ab+ | 为在文件尾读和写而打开或创建 | O_WRONLY &#124; O_CREAT &#124; O_APPEND

- 字符b作为type的一部分, 使得标准I/O系统可以区分文本文件和二进制文件。
- 对于fdopen函数来说, type的意义有所区别; fdopen为写而打开并不截断该文件。
- 如果多个进程通过标准I/O追加写方式打开同一文件, 那么来自每个进程的数据都将正确的写入文件。
- 当以读和写类型打开一个文件有如下限制:
> + 如果中间没有fflush, fseek, fsetpos, rewind, 则在输出的后面不能直接跟随输入;
> + 如果中间没有fseek, fsetpos, rewind, 或一个输入操作没有达到文件尾端, 则在输入
> 操作之后不能直接跟随输出;

    打开一个流的6种不同方式:


限制               | r  | w  | a  | r+ | w+ | a+
------------------ | -- | -- | -- | -- | -- | --
文件必须已经存在   | +  |    |    | +  |    |
放弃文件以前的内容 |    | +  |    |    | +  |
流可以读           | +  |    |    | +  | +  | +
流可以写           |    | +  | +  | +  | +  | +
流只可在尾端处写   |    |    | +  |    |    | +

**除非流引用终端设备此时流是行缓冲, 否则系统默认流被打开时是全缓冲的**

### 关闭一个打开的流
```c
#include <stdio.h>
int fclose (FILE *fp);
/* 返回值: 若成功, return 0; 若出错, return EOF */
```

文件关闭之前, 冲洗缓冲中的输出数据。缓冲区的任何输入数据被丢弃。如果标
准I/O为该流分配了一个缓冲区, 则释放此缓冲区;

## 5. 读和写流
打开流之后, 有3种不同类型的非格式化I/O可供选择进行读和写操作:
1. 每次一个字符的I/O;
2. 每次一行的I/O;
3. 直接I/O; fread和fwrite函数支持这种I/O操作, 每次I/O操作读或写某种数量的对象,
而每个对象具有指定的长度。这两个函数常用于从二进制文件中读或写一个结构;
> 直接I/O又被称为: 二进制I/O, 一次一个对象I/O, 面向记录I/O或面向结构I/O。

### 输入函数
```c
#include <stdio.h>
int getc (FILE *fp);
int fgetc (FILE *fp);
int getchar (void);
/* 返回值: 若成功, return 下一个字符; 若出错或已达到文件尾端, return EOF */
```

#### 函数分析
- 函数getchar等同于getc(stdin)。前两个函数的区别在于getc可以被实现为宏, fgetc不能:
> 1. getc的参数不应当是具有副作用的表达式, 因为它可能会被计算多次;
> 2. 可以将fgetc的地址作为一个参数传递给另一个函数;
> 3. 调用fgetc的时间可能长于getc; 调用函数的时间长于调用宏;

- 为了区分出错还是到达文件尾端, 必须调用ferror或feof
>     #include <stdio.h>
>     int ferror (FILE *fp);
>     int feof (FILE *fp);
>         /* 返回值: 若条件为真, return 非0(真）; 否则, return 0(假) */
>     void clearerr (FILE *fp);
>
> 调用clearerr可以清除每个流在FILE对象中维护的两个标志:
> - 出错标志;
> - 文件结束标志;

```c
#include <stdio.h>
int ungetc (int c, FILE *fp);
/* 返回值: 若成功, return c; 若出错: return EOF; */
```
从流中读取数据之后, 可以调用ungetc将字符再压送回流中。
- 压送回流中的数据可以再从流中读出, 不过读出字符的顺序与压送的顺序相反;
- ISO C支持多次的回送, 但每次回送只能回送一个字符;
- 一次成功的回送会清除文件结束标志;

### 输出函数
```c
#include <stdio.h>
int putc (int c, FILE *fp);
int fputc (int c, FILE *fp);
int putchar (int c);
/* 返回值: 若成功, return c; 若出错: return EOF; */
```
与输入函数一样, putchar(c)等同于putc(c, stdout), putc可被实现为宏, 而fputc不可以;

## 6. 每次一行I/O
### 输入函数
```c
#include <stdio.h>
char *fgets  (char *restrict buf, int n, FILE *restrict fp);
char *gets (char *buf);
/* 返回值: 若成功, return buf; 若出错或已达到文件尾端, return NULL */
```
- 两个函数都指定了缓冲区, gets从标准输入读, fgets从指定的流读;
- 对于fgets, 必须指定缓冲的长度n, 函数一直读到换行符截止, 不过不能超过n-1个字符,
    读入的字符为送入缓冲区。该缓冲区以null字节结尾, 如若读入的该行的字符超过了n-1, 会
    返回一个不完整的行, 下次再调用fgets函数时会继续读该行。
- gets不被推荐的原因是因为没有指定缓冲区的长度, 可能造成溢出。(1988年的因特网蠕虫事件
    利用了这个缺陷), fgets和gets的另一个区别是, gets并不在缓冲区中存入换行符;

### 输出函数
```c
#include <stdio.h>
char *fputs  (const char *restrict str, FILE *restrict fp);
char *puts (const char *str);
/* 返回值: 若成功, return 非负值; 若出错或已达到文件尾端, return EOF */
```
- fputs将一个以null字节种植的字符串写到指定流, 尾端的终止符null不写出。
- puts将一个以null字节种植的字符串写到标准输出, 尾端的终止符null不写出; 但随后puts追加
    一个换行符到标准输出;

## 7. 标准I/O的效率
标准I/O库与直接调用read和write函数相比并不慢太多。对于大多数比较复杂的应用程序, 最主要的
用户CPU时间是由应用本身的各种处理消耗的。

## 8. 二进制I/O
为了一次读或写一个完整的结构, 而前面的一次读写一个字节, 或一次读写一行并不满足要求;
```c
#include <stdio.h>
size_t fread  (void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
size_t fwrite (const void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
/* 返回值: 读或写的对象数 */
```

函数的两种常见用法:
- 读或写一个二进制数组: (将一个浮点数组的第2~5个元素写至一个文件上)
>     float data[10];
>
>     if (fwrite(&data[2], sizeof(float), 4, fp) != 4)
>         err_sys ("fwrite error");

其中, 指定size为每个数组元素的长度, nobj为欲写的元素个数;

- 读或写一个结构:
>     struct {
>         short count;
>         long total;
>         char name[NAMESIZE];
>     } item;
>
>     if (fwrite (&item, sizeof (item), 1, fp) != 1)
>         err_sys ("fwrite error");

### 函数的缺陷(问题)
1. 一个结构中, 同一成员的偏移量可能随编译程序和系统的不同而不同(由于不同的对齐要求);
2. 用来存储多字节整数和浮点值的二进制格式在不同的系统结构间也可能不同。

## 9. 定位流
3种用于定位标准I/O流的方法:
1. ftell和fseek函数, 假定文件的位置存放在一个长整型中; V7以来就存在了。
2. ftello和fseeko函数, 使用 off_t数据类型代替了长整型; Single UNIX Specification 引入的。
3. fgetpos和fsetpos函数, 使用抽象数据类型fpos_t记录文件的位置。这种数据类型可以根据需要定义
一个足够大的数; ISO C引入。

*需要移植到非UNIX系统上运行的应用程序应当使用fgetpos和fsetpos。*

```c
#include <stdio.h>
long ftell (FILE *fp);
/* 返回值: 若成功, return 当前文件位置指示; 若出错, return -1L */
int fseek (FILE *fp, long offset, int whence);
/* 返回值: 若成功, return 0; 若出错, return -1 */
void rewind (FILE *fp);
```
- rewind 可以将一个流设置到文件的起始位置;
- 对于一个二进制文件, 其文件位置指示器是从文件起始位置开始度量, 并以字节为度量单位;
> - ftell用于二进制文件时, 其返回值是这种字节位置;
> - fseek使用时必须指定offset, 以及解释这种偏移量的方式。whence的值与前文中的lseek函数
> 中的whence取值相同;

- 对于一个文本文件, 其文件位置可能不以简单的字节来度量。原因在于: 非UNIX系统通过不同格式
    来存储文本。为了定位一个文本文件, whence一定取值为SEEK_SET, 而offset只有两种取值: 0 或者
    对该文件的ftell的返回值;

```c
#include<stdio.h>
off_t ftello (FILE *fp);
/* 返回值: 若成功, return 当前文件位置; 若出错, return (off_t) -1; */
int fseeko (FILE *fp, off_t offset, int whence);
/* 返回值: 若成功, return 0; 若出错, return -1; */
```
除了偏移量的类型是off_t而不是long, ftello函数和ftell相同; fseeko函数和fseek相同;

```c
#include<stdio.h>
int fgetpos (FILE *restrict fp, fpos_t *restrict pos);
int fsetpos (FILE *fp, const fpos_t *pos);
/* 返回值: 若成功, return 0; 若出错, return 非0; */
```
fgetpos将文件位置指示器的当前值存入由pos指向的对象中。以后调用fsetpos时, 可以使用此值将流重定向至该位置;

## 10. 格式化I/O
### 1. 格式化输出
```c
#include <stdio.h>
int printf (const char *restrict format, ...);
int fprintf (FILE *restrict fp, const char *restrict format, ...);
int dprintf (int fd, const char *restrict format, ...);
/* 返回值: 若成功, return 输出字符数; 若输出出错, return 负值; */
int sprintf (char *restrict buf, const char *restrict format, ...);
/* 返回值: 若成功, return 存入数组的字符数; 若编码出错, return 负值; */
int snprintf (char *restrict buf, size_t n, const char *restrict format, ...);
/* 返回值: 若缓冲区足够大, return 将要存入数组的字符数; 若编码出错, return 负值; */
```
#### 函数分析
- printf将格式化数据写到标准输出; fprintf写至指定流fp; dprintf写至指定文件描述符fd; sprintf将格式化数据
    送入数组buf, 同时sprintf在该数组的尾端自动加一个null字节, 但该字节不包括在返回值当中;
- 由于sprintf函数可能造成buf指向的缓冲区溢出, 所以有了snprintf函数。在snprintf中, 缓冲区长度是一个显式
    参数。超过缓冲区尾端所写的所有字符被丢弃;
- dprintf不需要调用fdopen将文件描述符转换为文件指针;
- 格式说明控制了其余参数怎么写以及如何显示输出; 每个参数按照转换说明编写, 转换说明以百分号%开始:
```
%[flags][fldwidth][precision][lenmodifier]convtype
```

> flags  | 说明
> :----: | ----
> `      | (撇号)将整数按千位分组字符
> -      | 在字段内左对齐输出
> +      | 总是显示带符号转换的正负号
> (空格) | 如果第一个字符不是正负号, 则在其前面加上一个空格
> #      | 指定另一种转换形式
> 0      | 添加前导0(而非空格)进行填充
>
> - fldwidth: 说明最小字段宽度; 若转换之后参数字符数小于宽度, 多余位置用空格填补;
> - precision: 说明整形转换后最少输出数字位数, 浮点数转换后小数点的最少位数, 字符串转换后最大的字节数;
>> 宽度是一个非负十进制数, 精度是一个点(.), 其后跟随一个可选的非负十进制数; 其中宽度和精度都可以为'*'
>> 此时需要一个额外的整形参数来指定宽度或精度的值;

> - lenmodifier说明参数长度:
>
> lenmodifier | 说明
> :---------: | ----
> hh | 将相应参数按signed或unsigned char类型输出
> h  | 将相应参数按signed或unsigned short类型输出
> l  | 将相应参数按signed或unsigned long或宽字符类型输出
> ll | 将相应参数按signed或unsigned long long类型输出
> j  | intmax_t 或 uintmax_t
> z  | size_t
> t  | ptrdiff_t
> L  | long double
>
> - convtype是必须的, 它控制如何解释参数:
>
> convtype | 说明
> :------: | ----
> d, i | 有符号十进制
> o    | 无符号八进制
> u    | 无符号十进制
> x, X | 无符号十六进制
> f, F | 双精度浮点数
> e, E | 指数格式双精度浮点数
> g, G | 根据转换后的值解释为f, F, e或E
> a, A | 十六进制数格式双精度浮点数
> c    | 字符(若带长度修饰符l, 为宽字符)
> s    | 字符串(若带长度修饰符l, 为宽字符)
> p    | 指向void的指针
> n    | 到目前为止, 此printf调用输出的字符的数目将写入到指针所指向的带符号整型
> %    | 一个%字符
> C    | 宽字符(等效于lc)
> S    | 宽字符串(等效于ls)
>
> 宽字符: 是以两个字节存储一个字符

#### printf族的变体
```c
#include <stdarg.h>
#include <stdio.h>
int vprintf (const char *restrict format, va_list arg);
int vfprintf (FILE *restrict fp, const char *restrict format, va_list arg);
int vdprintf (int fd, const char *restrict format, va_list arg);
/* 返回值: 若成功, return 输出字符数; 若输出出错, return 负值; */
int vsprintf (char *restrict buf, const char *restrict format, va_list arg);
/* 返回值: 若成功, return 存入数组的字符数; 若编码出错, return 负值; */
int vsnprintf (char *restrict buf, size_t n, const char *restrict format, va_list arg);
/* 返回值: 若缓冲区足够大, return 将要存入数组的字符数; 若编码出错, return 负值; */
```

### 2. 格式化输入
```c
#include <stdio.h>
int scanf (const char *restrict format, ...);
int fscanf (FILE *restrict fp, const char *restrict format, ...);
int sscanf (const char *restrict buf, const char *restrict format, ...);
/* 返回值: 赋值的输入项数, 若输入出错或在任一转换前已达到文件尾端, 返回EOF */
```

#### 函数分析
**格式之后的各个参数包含了变量的地址, 用转换结果对这些变量赋值;**
格式说明控制如何转换参数, 转换说明从%字符开始。

%[*][fldwidth][m][lenmodifier]convtype

- [*]用于抑制转换, 按照转换说明的其他部分进行转换, 但结果不存入变量中;
- fldwidth 和 lenmodifier 参数说明参考printf族函数;
- convtype 也是类似于printf族函数, 不过有些差别:

convtype | 说明
:------: | ----
d | 有符号十进制, 基数为10
i | 有符号十进制, 基数由输入格式决定
O | 无符号八进制(输入可选地有符号)
u | 无符号十进制, 基数为10(输入可选地有符号)
x, X | 无符号十六进制(输入可选地有符号)
a, A, e, E, f, F, g, G | 浮点数
c | 字符(若带长度修饰符l, 为宽字符)
s | 字符串(若带长度修饰符l, 为宽字符)
[ | 匹配列出的字符序列, 以]终止
[^ | 匹配除列出字符以外的所有字符, 以]终止
p | 指向void的指针
n | 将目前为止该函数调用读取的字符数写入到指针所指向的无符号整型中
% | 一个%符号
C | 宽字符(等效于lc)
S | 宽字符串(等效于ls)

- 字符宽度和长度修饰符之间的可选项m是 *赋值分配符* , 用于%c, %s, %[
    转换符, 迫使内存缓冲区分配空间以接纳转换字符串。此时, 相关参数是 **指
    针的地址**, 分配的缓冲的地址复制给该指针。如果调用成功, 需要人工free
    缓冲区;

#### 函数变族
```c
#include<stdarg.h>
#include<stdio.h>
int vscanf (const char *restrict format, va_list arg);
int vfscanf (FILE *restrict fp, const char *restrict format, va_list arg);
int vsscanf (const char *restrict buf, const char *restrict format, va_list arg);
/* 返回值: 赋值的输入项数, 若输入出错或在任一转换前已达到文件尾端, 返回EOF */
```

## 11. 实现细节
```c
#include<stdio.h>
int fileno (FILE *fp);
/* 返回值: 与该流相关联的文件描述符 */
```
对流调用fileno函数来获得其描述符;

## 12. 临时文件
### 标准I/O库提供了两个函数以帮助创建临时文件:
```c
#include<stdio.h>
char *tmpnam(char *ptr);
/* 返回值: 指向唯一路径名的指针; */
FILE *tmpfile (void);
/* 若成功, 返回文件指针; 若出错, 返回NULL; */
```

#### 函数分析
1. tmpnam函数产生一个与现有文件名不同的一个有效路径名字符串。每次调用它时, 都产生一个不同的路径名,
最多调用次数是TMP_MAX。TMP_MAX定义在< stdio.h >中。若ptr是NULL, 则产生的路径名存放在一个静态区中,
指向该静态区的指针作为函数值返回。后续调用tmpnam时, 会重写该静态区(意味着想要保存路径名, 应该保存
路径名的副本, 而不是指针的副本); 若ptr不是NULL, 则认为它应该是指向长度至少是L_tmpnam个字符的数组,
路径名存放在该数组中。

2. tmpfile创建一个临时二进制文件(类型wb+), 在关闭该文件或程序结束时将自动删除文件。

### XSI扩展部分的临时文件处理函数
```c
char *mkdtemp (char *template);
/* 返回值: 若成功, 返回指向目录名的指针; 若出错, return NULL */
int mkstemp (char *template);
/* 返回值: 若成功, 返回文件描述符; 若出错, return -1 */
```

#### 函数分析
1. mkdtemp函数创建了一个目录; mkstemp函数创建了一个文件; 名字通过template字符串进行选择的。
template是后六位设置为XXXXXX的路径名。函数将这些占位符替换成不同的字符构建一个唯一的路径名;
2. mkdtemp函数创建的目录具有下列权限位集: S_IRUSR | S_IWUSR | S_IXUSR。
3. mkstemp函数以唯一的名字创建一个普通文件并且打开该文件, 文件访问权限位: S_IRUSR | S_IWUSR。
4. 与tempfile不同, mkstemp创建的文件不会自动删除。如果希望从文件系统命名空间中删除该文件,
必须自己对它解除链接。
5. 使用tmpnam和tempnam至少有一种缺点: 在返回唯一的路径名和用该名字创建文件之间存在一个时间窗口,
在这个时间窗口中, 另一个进程可以用相同的名字创建文件。因此最好使用tmpfile和mkstemp函数;

## 13. 内存流
在SUSv4中支持了内存流。这就是标准的I/O流, 虽然仍使用FILE指针访问, 但其实没有底层文件。
有3个函数可以用于内存流的创建:

### fmemopen函数
```c
#include<stdio.h>
FILE *fmemopen(void *restrict buf, size_t size, const char *restrict type);
/* 返回值: 若成功, 返回流指针; 若错误, return NULL */
```
fmemopen函数可以让调用者提供缓冲区用于内存流: buf指向缓冲区开始的位置, size指定了缓冲大小的字节数;
如果buf为空, fmemopen自己分配一个大小为size的缓冲区。不过流关闭时缓冲区同时被释放。

####　type参数
type可能的取值参考第4节的参数分析, 不过也有一些区别:
1. 对于任何追加写操作, 当前文件位置会设为缓冲区中第一个null字节, 如果没有null字节, 当前位置设为
缓冲区结尾的后一个字节; 当不是追加写时, 文件位置设为缓冲区的开始位置。二进制文件不适合存储于内
存流中(二进制数据在数据尾端之前可能包含多个null字节);
2. 如果buf参数是一个null指针, 那么打开流进行读或者写的操作是没有意义的。只写意味着我们无法读取已
写入的数据, 只读意味着我们只能读取无法写入的缓冲区中的数据。
3. 任何时候需要增加流缓冲区中数据量以及调用fclose, fflush, fseek, fseeko, or fsetpos, 都会在当前
位置写入一个null字节;

### open_memstream 和 open_wmemstream
```c
#include<stdio.h>
FILE *open_memstream (char **bufp, size_t *sizep);
#include<wchar.h>
FILE *open_wmemstream (wchar_t **bufp, size_t *sizep);
/* 返回值: 若成功, 返回流指针; 若出错, return NULL */
```

#### 函数分析
1. open_memstream创建的流是面向字节的, open_wmemstream创建的流是面向宽字节的。
2. 两者与fmemopen的区别:
> + 创建的流只能写打开;
> + 不能指定自己的缓冲区, 但可以分别通过bufp和sizep参数访问缓冲区的地址和大小;
> + 关闭流后自动释放缓冲区;
> + 对流添加字节会增加缓冲区大小

3. 缓冲区的地址和大小的使用原则:
> + 缓冲区地址和长度只有在调用fclose或fflush后才有效;
> + 这些值只有下一次流写入或调用fclose前才有效;

**因为避免了内存溢出, 内存流很适合用于创建字符串。此外, 由于内存流只访问主存, 所以对于把标准I/O流作为参数
用于临时文件的函数来说, 会有很大性能提升**
