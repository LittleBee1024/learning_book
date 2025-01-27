#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

std::condition_variable cv;
std::mutex cv_m;

void waits()
{
   std::unique_lock<std::mutex> lk(cv_m);
   std::cout << "Waiting... \n";
   cv.wait(lk);
   std::cout << "...finished waiting\n";
}

void signals()
{
   std::this_thread::sleep_for(std::chrono::seconds(1));
   {
      std::lock_guard<std::mutex> lk(cv_m);
      std::cout << "Notifying...\n";
   }
   cv.notify_all();
}

int main()
{
   std::thread t1(waits), t2(waits), t3(waits), t4(signals);
   t1.join();
   t2.join();
   t3.join();
   t4.join();
}
