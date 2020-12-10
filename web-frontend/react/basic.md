# React笔记

## JSX

Babel编译JSX

React DOM 在渲染所有输入内容之前，默认会进行转义。它可以确保在你的应用中，永远不会注入那些并非自己明确编写的内容。所有的内容在渲染之前都被转换成了字符串。这样可以有效地防止 XSS（cross-site-scripting, 跨站脚本）攻击。

## bind, this

事件处理，this问题，`bind`

需要好好看看
- https://zh-hans.reactjs.org/docs/handling-events.html
- https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/Function/bind


## 表单, 受控组件

https://zh-hans.reactjs.org/docs/forms.html