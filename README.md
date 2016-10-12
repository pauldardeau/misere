misere
======

Misère is a simple, high-performance C++ HTTP application server.

License
-------
BSD

Dependencies
------------
Chaudière - C++ core functionality with sockets, threading, configuration, etc.

Objectives/Purpose
------------------
1. Coding is fun!
2. I wanted to develop a 'from scratch' high-performance C++ server
3. HTTP seemed like a reasonable 'top-level' protocol for the server
4. This is NOT meant to be a general purpose web server (i.e., no support for serving up files, caching, etc.)

Configuration File
------------------
Misere uses an .INI for configuration. See 'What's an INI file'
if you're not familiar with them. The .INI format was chosen
because it's very simple and I like simple.

Building
--------
Currently there is not autoconf support. There's a basic Makefile in
src directory.

Running
-------
./misere misere.ini

Load Testing
------------
Using siege: siege -b -r 10 -c 100 http://127.0.0.1:13001/Echo

Apache bench (ab): ab -t 60 -n 1000 -c 400 http://127.0.0.1:13001/Echo

Platforms/Compilers
-------------------
This project is implemented with C++11. It is tested with clang and
g++ compilers. It is also tested on Mac OS X, FreeBSD 10, PC-BSD 10,
and Ubuntu 14.04 Server.

Status/Maturity
---------------
Parts of the code are very mature and well tested, and other parts
aren't. I wouldn't use this for anything important just yet. It'll
slowly get there.

C++ Standards
-------------
The code is somewhat C++11 compliant. The intention is to make it more
and more C++11 compliant over time.

Coding Style
------------
You may find my coding style slightly peculiar. It's my personal style
that I've adopted after a lot of experimentation about what works
best for me.

What's An INI File
------------------
In the 'old days', Windows computers made extensive use of INI files.
This was before the system registry came along. An INI file is a
simple text file that is composed of sections, and each section
can have keys and values.

Meaning of Misère
-----------------
What does 'Misère' mean?  It's a French word that means "trouble" or "misery".
Fitting for a C++ project name, no?  All the English words that I thought
about using were already being used in 1 or more projects on github, so I
tried to find something that was unique.  I grew up in South Louisiana and
got to hear a fair bit of Cajun French spoken when I was growing up.  It
was not uncommon for a mischievous kid to be told something like "You're
giving me the misères!"


