# 随心文本解析库 Suixin Text Parser Lib


## What is Suixin text?

A Suixin text consists of one or more items. Each item is formed by an ID and Content. Nothing fancy, just treat ID as Key and Content as Value.

Suixin use back quote (`) as the Mark char.

* A line that begins with 2 back quote = Line Comment.
* A line that begins with 1 back quote = ID.
* A line that begins with other char = Content.

```
``Line comment
`ID
Content
```

An ID is declared by putting the Mark char at the beginning of a line. The ID begins at the first character that is not a "Space" nor "Tab" after the Mark char. As such, the following are equivalent to each other. The ID ends when the line ends.
```
`ID
` ID
`	ID
```

An ID can contain virtually any character, except the one that breaks the line of course.

```
` Macro$oft corporation
` !! ERROR !! : yes this is valid ID
` 結城友奈は勇者である
` ○△○ 1+1+1 > 3
```

If you decide to begin an ID with back quote somehow, you must put a space or "Tab" after the Mark char, otherwise it will be treat as comment.

```
` `weird but legal ID
``WRONG: Not an ID, but comment
```

It is legal to declare an ID without Content. To declare such ID, declare another ID right after it without anything that can be considered as Content. It is legal to have a Suixin text that contains ID only.

```
`` foo, foobar, barbar are ID without Content
` foo

` bar
hmm

` foobar

` barbar
```

A Content can have multiple lines. The Content begins at the first character after the ID. The Content ends at the last character before the line of next ID begins. 

```
` Foo
foobar like to foobar
foobar is smart
be like foobar
` Bar
barfoo like to barbar
barbar is futile
avoid barbar
```

Suixin allows the use of blank lines to leave out some space between ID and content. Therefore, some blank lines will be ignored. The blank lines within the Content will be preserved.

```

`` Even before the first ID, you can have as much blank lines as you want.

` Foo

Foobar

Barfoo



` Bar
Foobar

Barfoo

`` The Content of "Foo" and "Bar" are same. Both are 3 lines.

```

## About the Parser

This parser is designed to be a solid and flexible foundation for other framework to be built on. This means that it is fairly low level that it can hardly do anything useful out of the box, because it aims to let you build any kind of framework that operates on Suixin text on it.

First it will ask for a data pointer and byte count to operate on. Every time it encounters something interesting like an ID, it will pass the relevant pointer and byte count to the user-defined callback. Then the callback will return a value telling the parser what to do next. Continue? Abort? Or skip the Content that will be served soon? This is pretty much what does this parser do, to give the maximum control to user-defined callback on what to do with the data. As such...

This parser does not do any encoding conversion.
> Whatever goes in, whatever comes out. If user passed in UTF-8 data pointer, user callback would get UTF-8 back. The parser will not try to do what it is not specialized in. It gives maximum flexibility to the user. User can choose to use the data as-is, or some external lib, or MultiByteToWideChar() in Windows.

This parser does not provide zero-terminated string to user-defined callback.
> For efficiency, the parser reuse back the data pointer that user passed in, to avoid memory allocation that might be unnecessary. For example, the raw data is in UTF-8 and you want UTF-16 for calling Windows API. In this case, why would you want to allocate memory for UTF-8 zero-terminated string?

This parser does not detect if there is duplicated ID.
> Keeping track of visited ID requires memory allocation, and not everyone want to block duplicated ID. Some would prefer to override the content of previous ID with the later one.

All of these can be easily implemented, as shown in the following example.

## The API and Behavior

`void SuixinParse(SuixinParseStat *, SuixinParseSpec const *);`

Both parameter must be specified (non-NULL).

#### struct SuixinParseStat [out]

> `SuixinParseStat::error`  
> Pretty straightforward. Refer the `enum SuixinParseErr` in header file.

> `SuixinParseSpec::(iRow, iCol, iByte)`  
> Zero-based index of row/col/byte that the parser worked on, before `SuixinParse()` returned.

#### struct SuixinParseSpec [in]

> `SuixinParseSpec::pData`  
> Pointer to data for parser to parse on.

> `SuixinParseSpec::cbData`  
> Byte count of data that parser should parse.

> `SuixinParseSpec::charIs16bit`  
> By default, parser assumes multibyte encoding (like UTF-8) and operates on the basis of `uint8_t`.  
> To correctly handle Windows Unicode (UCS-2 or UTF-16) string, set this to `true` so the parser operates on the basis of `uint16_t` instead.

> `SuixinParseSpec::user`  
> User-defined value that will be passed to user-defined callback.

> `SuixinParseSpec::pfnOnID`  
> Must not NULL. User-defined callback.  
> `(uint8_t const *pID, int cbID, void *user, void **ppItem)`  
> 
> * `pID` non-zero-terminated ID string
> * `cbID` byte count of ID string
> * `user` user-defined value passed in `SuixinParseSpec`
> * `ppItem` pointer to a void pointer that will be passed to `pfnOnContent`
>
> It is guaranteed that when this callback is called, `pID` will never be NULL and `cbID` will always > 0.
>
> User should set `*ppItem` to non-zero value if current ID is in interest, otherwise the Content of current ID will be skipped.
> 
> Return `ContinueSuixinParse` to continue, or `AbortSuixinParse` to abort.

> `SuixinParseSpec::pfnOnContent`  
> Must not NULL. User-defined callback.  
> `(uint8_t const *pCtnt, int cbCtnt, void *user, void *pItem)`  
> 
> * `pCtnt` non-zero-terminated Content string
> * `cbCtnt` byte count of Content string
> * `user` user-defined value passed in `SuixinParseSpec`
> * `pItem` void pointer specified by user in `pfnOnID`
>
> When this callback is called, `cbCtnt` will be zero if the item of current ID has no Content. Always check `cbCtnt` before attempting to access `pCtnt`.
>
> Return `ContinueSuixinParse` to continue, or `AbortSuixinParse` to abort.

