# cpp_peg
cpp peg parser with operator overload

parser part is finished, I also implement a simple virtual machine instructions which is developping(bug not fixed).

I implement peg parser with only cpp operator overloading,without any templates which is inspired by lpeg.

example:
```cpp
	string code = "abcd 124zzzz";
	Pattern grammer = capture(
		(range('a','z')^1) * capture(
			(ch(' ')^1)* (range('1','9')^1),"tag2"
		)
		,"tag1"
	);
	Ctx ctx = grammer.parse(code);
	cout << ctx.ast.matched << endl;

```

18年左右写的，感觉留着没啥用处，开源出来，靠c++的重载操作符实现。
peg解析部分已经写完，顺便实现了类c语法的解释器，支持向量，vm指令生成还有bug，以后有空再找。
该项目可以作为一个学习编译器的项目。编译器前端是靠peg语法实现。