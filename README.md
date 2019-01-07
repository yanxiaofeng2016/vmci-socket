# vmci-socket
This program is used to communicate between esxi(host) and the operating system running in the virtual machine.The operating system can be either Windows or Linux.
This program is able to input the commands supported by the client operating system on the host side and return the results after the client runs the commands。

# how to use this tools

First, you need to run the binary program "socket" (window for socket.exe) on the client's operating system. and then run the next command on esxi(host) side.

usage for linux system running in virtual machine
client cid:port command

[pc]#./client 100000:123456 "ls -l"
-rw-r--r--. 1 root root 2236 Jan  4 17:19 client.c
-rw-r--r--. 1 root root  590 Jan  4 17:24 GNUmakefile
-rw-r--r--. 1 root root  379 Jan  4 17:31 README.md
-rw-r--r--. 1 root root 5424 Jan  4 17:19 socket.c
-rw-r--r--. 1 root root  249 Jan  7 11:21 sock-portable.h
-rw-r--r--. 1 root root  310 Jan  7 11:21 sock-posix.c
-rw-r--r--. 1 root root  520 Jan  4 17:20 sock-windows.c
-rwxr-xr-x. 1 root root  317 Jan  4 17:23 vmci-cflags

usage for windows system running in virtual machine
client cid:port command

[pc]#./client 100000:123456 "ipconfig"
Windows IP Configuration
Ethernet adapter Ethernet0:

   Media State . . . . . . . . . . . : Media disconnected
   Connection-specific DNS Suffix  . : 
   
   
# How to build:
for linux:
[pc]#make

for window:
[pc]#make WINDOWS=1

so, the binary "client" is usually for linux because esxi system is derived from Linux.
and the binary "socket" or "socket.exe" need to be compiled for two parts. one is for linux ,the other is for windows system.

