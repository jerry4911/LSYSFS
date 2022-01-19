# Guidelines for Implementing In-memory Filesystem in User Space

## Outlines

1. Basic Steps for In-memory Filesystem Development (90 marks)
    1. LSYFS Installation on Linux Environment (6 marks)
    2. Support basic commands (i.e., cd, ls, mkdir, and touch)
    3. Support file and directory deletion (i.e., rmdir, and rm)
    4. Writing a string to a file (i.e., echo “string” >> file)
    5. Reading file in the in-memory filesystem
2. Advanced Modifications on In-memory Filesystem (10 marks for each item)
    1. Support big directory in the in-memory filesystem
    2. Support big file in the in-memory filesystem
3. References

## Contents

- LSYFS Installation on Linux Environment
    - Install FUSE
        - 到 [FUSE release](https://github.com/libfuse/libfuse/releases)下載 FUSE 2.9.7
        - 解壓縮後安裝
        
        ```bash
        tar xvf fuse-2.9.7.tar.gz #Extract file
        
        #安裝 FUSE
        ./configure
        make -j8
        make install
        fusermount -V #Get the version of fusermount
        ```
        
    - 編譯 lsysfs.c 後執行
        
        ```bash
        sudo su #切換身分到root
        #Compile
        gcc lsysfs.c -o lsysfs `pkg-config fuse --cflags --libs`
        
        #Mount the filesystem after compilation
        ./lsysfs -f { mount point }
        
        #會卡住 --> 正常
        #再另外開一個terminal操作
        ```

## References

[1]. libfuse, “releases”, [https://github.com/libfuse/libfuse/releases](https://github.com/libfuse/libfuse/releases)

[2]. Oracle, “Installing FUSE”, [https://docs.oracle.com/cd/E76382_01/bigData.Doc/install_onPrem/src/tins_prereq_index_fuse.html](https://docs.oracle.com/cd/E76382_01/bigData.Doc/install_onPrem/src/tins_prereq_index_fuse.html)

[3]. Mohammed Q. Hussain, “Writing Less Simple, Yet Stupid Filesystem Using FUSE in C”,
[http://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem](http://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem)using-FUSE-in-C/
[4]. Mohammed Q. Hussain, “LSYSFS”, [https://github.com/MaaSTaaR/LSYSFS](https://github.com/MaaSTaaR/LSYSFS)
