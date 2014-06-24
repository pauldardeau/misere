misere
======

Misère is a simple, high-performance C++ HTTP application server.

License
=======
BSD!

Objectives/Purpose
==================
1. Coding is fun!
2. I wanted to develop a 'from scratch' high-performance C++ server
3. HTTP seemed like a reasonable 'top-level' protocol for the server
4. This is NOT meant to be a general purpose web server (i.e., no support for serving up files, caching, etc.)

Configuration File
==================
Misere uses an .INI for configuration. See 'What's an INI file'
if you're not familiar with them. The .INI format was chosen
because it's very simple and I like simple.

Threading Options
=================
This project was initially coded to use Posix threads (pthreads).
Since C++11 has built-in support for threading, that was recently
added and is still being tweaked. Additionally, Apple's libdispatch
(foundation of Grand Central Dispatch [GCD]) is available on Mac
OS X, as well as on some other systems.

Status/Maturity
===============
Parts of the code are very mature and well tested, and other parts
aren't. I wouldn't use this for anything important just yet. It'll
slowly get there.

C++ Standards
=============
The code is somewhat C++11 compliant. The intention is to make it highly
compliant, although this will take some time. Currently, there is very
minimal use of C++11 smart pointers. The plan is to start converting
over to using smart pointers in place of raw pointers.

Coding Style
============
You may find my coding style slightly peculiar, and that's ok. It's
my personal style that I've adopted after a lot of experimentation
about what works best for me.

Meaning of Misère
=================
What does 'Misère' mean?  It's a French word that means "trouble" or "misery".
Fitting for a C++ project name, no?  All the English words that I thought
about using were already being used in 1 or more projects on github, so I
tried to find something that was unique.  I grew up in South Louisiana and
got to hear a fair bit of Cajun French spoken when I was growing up.  It
was not uncommon for a mischievous kid to be told something like "You're
giving me the misères!"


