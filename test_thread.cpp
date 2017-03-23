#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include "readerwriterqueue.h"
#include <stdexcept>

extern void do_something();

typedef long long unsigned int calls_p_sec_t;
typedef ::std::function<void ()> the_call_t;

template <typename T>
calls_p_sec_t calls_per_second(const T &thecall, unsigned int interval)
{
   auto now = []() -> auto {
      return ::std::chrono::high_resolution_clock::now();
   };
   typedef ::std::chrono::duration<long double> timediff_t;
   unsigned long long count = 0;
   const auto start = now();

   long double curtime_as_ldbl = (timediff_t(now() - start)).count();
   while (curtime_as_ldbl < interval) {
      thecall();
      ++count;
      curtime_as_ldbl = (timediff_t(now() - start)).count();
   }
   return count / interval;
}


static bool end_thread_flag = false;

void end_thread()
{
   end_thread_flag = true;
}

void worker_thread(::moodycamel::BlockingReaderWriterQueue<the_call_t> &to_me,
                   ::moodycamel::BlockingReaderWriterQueue<bool> &from_me)
{
   while (!end_thread_flag) {
      the_call_t a_call;
      to_me.wait_dequeue(a_call);
      a_call();
      if (!from_me.enqueue(true)) {
         throw ::std::runtime_error("Unable to queue notification!");
      }
   }
}

int main()
{
   using ::std::cout;
   using ::std::async;
   using ::std::thread;
   using ::std::launch;
   cout << "   Do nothing calls per second: "
        << calls_per_second([]() { }, 5) << '\n';
   cout << "        Empty calls per second: "
        << calls_per_second([]() { do_something(); }, 5) << '\n';
   cout << "   New thread calls per second: "
        << calls_per_second(
              []() {
                 thread t{ do_something };
                 t.join();
              },
              5
           )
        << '\n';
   cout << " Async launch calls per second: "
        << calls_per_second(
              []() {
                 auto fut = async(launch::async | launch::deferred, do_something);
                 fut.wait();
              },
              5
           )
        << '\n';
   {
      ::moodycamel::BlockingReaderWriterQueue<the_call_t> from_main;
      ::moodycamel::BlockingReaderWriterQueue<bool> to_main;
      thread worker{ [&from_main, &to_main]() { worker_thread(from_main, to_main); } };
      cout << "Worker thread calls per second: "
           << calls_per_second(
              [&from_main, &to_main]() {
                 if (!from_main.enqueue(do_something)) {
                    throw ::std::runtime_error("Unable to send request to worker.");
                 }
                 bool dummy = false;
                 to_main.wait_dequeue(dummy);
              },
              5
           )
        << '\n';
      from_main.enqueue(end_thread);
      {
         bool dummy;
         to_main.wait_dequeue(dummy);
      }
      worker.join();
   }
   return 0;
}
