# Image Toolkit

## How to Build ##

```
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
$ bin/imgtk
Usage: imgtk [options]

Options:
  --help                print help messages
  -c [ --cmd ] arg      command
  -s [ --src ] arg      src path
  -d [ --dst ] arg      dst path
  -l [ --list ] arg     image list file path
  -o [ --ops ] arg      operations config file path
  -v [ --verbose ]      produce verbose output
```

## Examples ##

### List all keys in DB ###

```
$ bin/imgtk -s /path/to/local/dir -c list
```

```
$ bin/imgtk -s leveldb:/path/to/leveldb/dir -c list
```

### Display images ###

```
$ echo "test.png" | bin/imgtk -s /path/to/local/dir -c show
$ echo "test.png" | bin/imgtk -s /path/to/local/dir -c cat
```

```
$ bin/imgtk -s /path/to/local/dir -c show -l img.lst
$ cat img.lst
test.png
```

### Copy images ###

```
$ bin/imgtk -s /path/to/local/dir -d leveldb:/path/to/leveldb/dir -c save -l img.lst
$ cat img.lst
0.png::0.jpg
1.png::1.jpg
```

### Delete images ###

```
$ bin/imgtk -s lmdb:/path/to/lmdb/dir -c delete -l delete.lst
$ cat delete.lst
/key/of/0.png
/key/of/1.png
```

### Pipe Operations ###

```
$ bin/imgtk -s /path/to/local/dir -c pipe -l img.lst -o ops.yaml
$ cat img.lst
test.png::test_10_15_50_80.jpeg::10::15::50::80
$ cat ops.yaml
- cmd: sizeup
  size: 1024
- cmd: clip
  x: $3
  y: $4
  w: $5
  h: $6
- cmd: resize
  width: 28
  height: 28
- cmd: gray
- cmd: invert
- cmd: equ
- cmd: save
  db: /path/to/dst/dir
  key: $2
- cmd: show
```
