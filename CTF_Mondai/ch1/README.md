# reverse engineering

## 手順
```shell
$ file runme.exe
runme.exe: PE32 executable (console) Intel 80386, for MS Windows, 3 sections
```

IDA PRO で探すと、`sub_401034`関数から一文字ずつ比較しているのがわかる。
-> それがflag
