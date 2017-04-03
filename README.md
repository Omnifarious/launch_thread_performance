A simple little program to test various ways to call functions to see how much
overhead they introduce.

One way is to have a worker thread and communicate to it with a queue.  For this
I use the lock-free queue implementation from
[https://github.com/cameron314/readerwriterqueue](https://github.com/cameron314/readerwriterqueue).

This is a git subrepo, so you will also need git to check it out.

To run this on a system with make and a program that calls itself g++ and
accepts g++ flags, just type `make`.
