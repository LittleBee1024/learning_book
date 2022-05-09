#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
int x = 0;
//创建读写锁变量
pthread_rwlock_t myrwlock;

void *read_thread(void *args)
{
   printf("------%lu read_thread ready\n", pthread_self());
   while (1)
   {
      sleep(1);
      //请求读锁
      pthread_rwlock_rdlock(&myrwlock);
      printf("read_thread: %lu,x=%d\n", pthread_self(), x);
      sleep(1);
      //释放读写锁
      pthread_rwlock_unlock(&myrwlock);
   }
   return NULL;
}

void *write_thread(void *param)
{
   printf("------%lu write_thread ready!\n", pthread_self());
   while (1)
   {
      sleep(1);
      // 请求写锁
      pthread_rwlock_wrlock(&myrwlock);
      ++x;
      printf("write_thread: %lu,x=%d\n", pthread_self(), x);
      sleep(1);
      //释放读写锁
      pthread_rwlock_unlock(&myrwlock);
   }
   return NULL;
}

int main()
{
   int i;
   //初始化读写锁
   pthread_rwlock_init(&myrwlock, NULL);
   //创建 3 个读 x 变量的线程
   pthread_t readThread[3];
   for (i = 0; i < 3; ++i)
   {
      pthread_create(&readThread[i], NULL, read_thread, NULL);
   }
   //创建 1 个修改 x 变量的线程
   pthread_t writeThread;
   pthread_create(&writeThread, NULL, write_thread, NULL);
   //等待各个线程执行完成
   pthread_join(writeThread, NULL);

   for (int i = 0; i < 3; ++i)
   {
      pthread_join(readThread[i], NULL);
   }
   //销毁读写锁
   pthread_rwlock_destroy(&myrwlock);
   return 0;
}
