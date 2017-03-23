#include <chrono>
#include <thread>
#include <future>
#include <iostream>

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

int main()
{
   using ::std::cout;
   using ::std::async;
   using ::std::thread;
   using ::std::launch;
   cout << "  Do nothing calls per second: "
        << calls_per_second([]() { }, 5) << '\n';
   cout << "       Empty calls per second: "
        << calls_per_second([]() { do_something(); }, 5) << '\n';
   cout << "  New thread calls per second: "
        << calls_per_second(
              []() {
                 thread t{ do_something };
                 t.join();
              },
              5
           )
        << '\n';
   cout << "Async launch calls per second: "
        << calls_per_second(
              []() {
                 auto fut = async(launch::async | launch::deferred, do_something);
                 fut.wait();
              },
              5
           )
        << '\n';
   return 0;
}
