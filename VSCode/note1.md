# VSCode使用笔记

## 教程链接

- [极客教程](https://geek-docs.com/vscode/vscode-tutorials/what-is-vscode.html)

## 看Go代码时关闭超链接

一开始使用VSCode写Go代码的时候，会发现每次点import的包名，都会提示是否要用浏览器打开这个超链接，真的不胜其烦。查了一下可以通过设置里的`editor.links`控制这一行为，设为false就好了。

