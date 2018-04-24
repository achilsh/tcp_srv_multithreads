/**
 * @file: t_threads.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-18
 */
#ifndef _T_THREADS_HPP_
#define _T_THREADS_HPP_ 

#ifdef __cplusplus
    extern "C" {
#endif

#include <pthread.h>

#ifdef __cplusplus
    }
#endif

#include <iostream>

namespace T_TCP
{


/**
* @brief: 
* 线程类的抽象，该类对外支持: 启动线程，停止线程，join线程，获取当前线程id等能力
* 如果要使用线程完成具体工作,只需继承该类即可,实现方法main(), Init().
*/
class PthreadBase
{
    public:
     PthreadBase();
     PthreadBase(pthread_mutex_t* pInitLock, pthread_cond_t* pInitCond, int* pInitNums);
     virtual ~PthreadBase();
     void Start();
     void JoinWork();
     pthread_t GetThreadId()
     {
         return m_iPid;
     }
    protected:
     void Stop();
    private:   
     static void* Entry(void* pData);
     void SetRun()
     {
         m_bRun = true;
     }
     bool IsRun() 
     {
         return m_bRun;
     }
    protected:

     /**
      * @brief: main 
      * 线程实现的具体内容，该接口被子类实现.子类不需要手动调用.
      * @return  0:succ, ~0:error
      */
     virtual int main() = 0;

     /**
      * @brief: Init 
      * 工作线程初始化，用于业务线程的一些初始化需求。如果线程子类需要初始化数据
      * 时，只需实现该接口就能完成数据的初始化工作。子类不需手动调用
      * @return 0:succ, ~0:error
      */
     virtual int Init() = 0;
     /**
      * @brief: RegistePthreadToPool()
      * 该接口为线程池场景设计. 调用该接口表明该线程任务初始化在此已经完成。
      * 该接口不需子类实现，也不直接被外部类调用，被派生类的main()手动调用.
      */
     void RegistePthreadToPool();

     pthread_t m_iPid;
     bool m_bRun;
     // add pthread pool init data struction, 经构造函数参数传入.
     pthread_mutex_t* m_pInitLock;
     pthread_cond_t*  m_pInitCond;
     int*  m_pReadyThreadNums;
};

///////
}
#endif
