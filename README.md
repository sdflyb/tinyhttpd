## 注释
修改了部分程序，并添加了一些注释---------------------------------画蛇添足

## 使用方法
1. git clone https://github.com/lyb6537/tinyhttpd
2. cd tinyhttpd/
3. make clean && make
4. ./httpd
5. 在浏览器输入127.0.0.1:端口号

## 修改日志

### 2017 02 24 ---- 修改以后在ubuntu下可以运行
1. 去掉Makefile里的-lsocket,将-lpthread改为-pthread
2. 修改htdocs中的check.cgi和color.cgi中的perl路径
3. 添加color.cgi的可执行权限，chmod +x color.cgi
4. mv README README.md
5. 修改代码风格，使用TAB进行缩进--------吐槽一下源代码使用一个空格进行缩进
6. 修改源码，去除了编译的警告

## 参考资源
1. [参考博客](http://blog.csdn.net/yzhang6_10/article/details/51534409) 
 
## 以下是原文的README文件内容
 This software is copyright 1999 by J. David Blackstone.  Permission
is granted to redistribute and modify this software under the terms of
the GNU General Public License, available at http://www.gnu.org/ .

  If you use this software or examine the code, I would appreciate
knowing and would be overjoyed to hear about it at
jdavidb@sourceforge.net .

  This software is not production quality.  It comes with no warranty
of any kind, not even an implied warranty of fitness for a particular
purpose.  I am not responsible for the damage that will likely result
if you use this software on your computer system.

  I wrote this webserver for an assignment in my networking class in
1999.  We were told that at a bare minimum the server had to serve
pages, and told that we would get extra credit for doing "extras."
Perl had introduced me to a whole lot of UNIX functionality (I learned
sockets and fork from Perl!), and O'Reilly's lion book on UNIX system
calls plus O'Reilly's books on CGI and writing web clients in Perl got
me thinking and I realized I could make my webserver support CGI with
little trouble.

  Now, if you're a member of the Apache core group, you might not be
impressed.  But my professor was blown over.  Try the color.cgi sample
script and type in "chartreuse."  Made me seem smarter than I am, at
any rate. :)

  Apache it's not.  But I do hope that this program is a good
educational tool for those interested in http/socket programming, as
well as UNIX system calls.  (There's some textbook uses of pipes,
environment variables, forks, and so on.)

  One last thing: if you look at my webserver or (are you out of
mind?!?) use it, I would just be overjoyed to hear about it.  Please
email me.  I probably won't really be releasing major updates, but if
I help you learn something, I'd love to know!

  Happy hacking!

                                   J. David Blackstone
