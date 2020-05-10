# Bash

## 概念

```
        USER
        ↓  ↑
Shell, KDE, Applications
        ↑  ↓
        Kernel
        ↑  ↓
        Hardware        
```

- 只要能够操作应用程式的界面程序都能够称为`Shell`
- Linux预设使用Bash

## 基础功能

- ↑↓ 历史命令
- 别名 alias
- Tab 命令、文件名补全
- 通配符

## 快捷键

|快捷键|功能|
|--|--|
Ctrl + a|跳到命令行首部（home 也一样）
Ctrl + e|跳到命令行尾部（end也一样）
**Alt + f**|**光标向右移动一个单词尾**
**Alt + b**|**光标向左移动一个单词首**
Ctrl + u|删除从行首到光标
Ctrl + k|删除从光标到行尾
Alt + r|删除当前整行
**Ctrl + w**|**从光标处向左删除至单词首**
**Alt + d**|**从光标处向右删除至单词尾**
Ctrl + y|将删除的字符粘贴到光标后
Ctrl + l|小写L 清屏

## 变量

- 变量未被设定时，调用为“空”
- `var=Value`，中间不能空格
- 强引用: `' '`，`$`之类的会保留原样；弱引用 `" "`，就可以在里面放变量了（弱引用用转义符`\`）
- `$(COMMAND)` 或 `点COMMAND点`(点就是`)
- **变量要在子程序中运行，要`export`为环境变量**
- `unset`取消变量 (`unset PATH` 以后重登就好了..)


### 环境变量

`env`列出当前所有，`export`设置环境变量

- `PATH`
- `HOME`
- `MAIL`
- `SHELL`
- `LANG`
- `RANDOM` 
  - 0~32767 随机数
  - `declare -i number=$RANDOM*10/32768`