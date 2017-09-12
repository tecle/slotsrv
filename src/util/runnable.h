#ifndef RUNNABLE_H_
#define RUNNABLE_H_

namespace cgserver {
  class Thread;
  class Runnable {
  public:
    /*
     * 析构
     */
    virtual ~Runnable() {}
    /*
     * 运行入口函数
     */
    virtual void run(Thread *thread, void *arg) = 0;
    virtual void free() {}
  };
}

#endif /*RUNNABLE_H_*/
