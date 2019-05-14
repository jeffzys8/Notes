# Filters

> 等价于SQL中的WHERE

## and

"fields":[]

## or

同上

## not

"field":

## selector

key = value

## columnComparison

value_key1 = value_key2

## regex

类似selector，值用java正则过滤

## javascript

类似selector，值用js函数过滤

## search

??

## in

同sql

## like

同sql，但是前面的search用来干嘛

## bound

```c
21 <= age <= 31
```
<==>
```json
{ "type": "bound", "dimension": "age", "lower": "21", "upper": "31" , "ordering": "numeric" }
```

lowerStrict, upperStrict 使用 > 与 <

ordering(比较方式): Lexicographic(默认), Alphanumeric, Strlen, Numeric, Version

## interval

- ONLY long millisecond values!! (比如 `__time`); 
- boundaries specified as `ISO 8601` time intervals!!! ("2014-10-01T00:00:00.000Z/2014-10-07T00:00:00.000Z")

start <= time < end (左闭右开) 

可以用 `OR` 代替

## **extractionFn**

插入在filters中；在执行filter之前先执行extractionFn

javascript的感觉很方便

lookup用于替换