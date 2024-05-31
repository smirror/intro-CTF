# cipher

## 手順
```shell
$ unzip unzip.zip
Archive:  unzip
[unzip] backnumber08.txt password: 
   skipping: backnumber08.txt        incorrect password
   skipping: backnumber09.txt        incorrect password
   skipping: flag                    incorrect password
```
パスワード付き

```shell
$ zipinfo unzip.zip 
Archive:  unzip.zip
Zip file size: 21710 bytes, number of entries: 3
-rw-r-----  3.0 unx    14182 TX defN 15-Nov-30 16:23 backnumber08.txt
-rw-r-----  3.0 unx    12064 TX defN 15-Nov-30 16:22 backnumber09.txt
-rw-------  3.0 unx    22560 BX defN 15-Dec-01 15:21 flag
3 files, 48806 bytes uncompressed, 21148 bytes compressed:  56.7%
```

ファイル名で検索すると出てくる。

既知平文攻撃で解読する。

```shell
$ zip backnumber08.zip backnumber08.txt
$ pkcrack -C unzip.zip -c backnumber08.txt -p backnumber08.txt -P backnumber08.zip -d decrypted.zip
```

本来はzipの圧縮率も調査する必要がある