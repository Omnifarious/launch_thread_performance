A simple little program to test various ways to call functions to see how much
overhead they introduce.

One way is to have a worker thread and communicate to it with a queue.  For this
I use the lock-free queue implementation from
[https://github.com/cameron314/readerwriterqueue](https://github.com/cameron314/readerwriterqueue).

I include that as an hg subrepo using a git+https url, so you will need the
hg-git extension to check it out.
