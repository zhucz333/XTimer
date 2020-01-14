//////////////////////////////////////////////////////////////////////////
///@file	XTimer.h													//
///@brief	class XTimer												//
///@author	__ZHUCZ__(zhucz333@163.com)									//
///@date	2019/10/16													//
//////////////////////////////////////////////////////////////////////////

#ifndef __XTIMER_H__
#define __XTIMER_H__

#include <list>
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>

typedef std::function<void()> TimerFunc;
typedef struct TimerJob_ TimerJob;

using SteadyClock = std::chrono::steady_clock;
using TimePoint = std::chrono::steady_clock::time_point;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
using Nanoseconds = std::chrono::nanoseconds;

struct TimerJob_
{
	int m_nRounds;
	Milliseconds m_nRoundDuration;
	std::atomic<bool> m_atomicCancelled;
	TimePoint m_timeExpireTime;
	TimerFunc m_funcTimerFunc;
	inline bool operator > (TimerJob& b)
	{
		return m_timeExpireTime > b.m_timeExpireTime;
	}
};

auto comp = [](const std::shared_ptr<TimerJob>& task1, const std::shared_ptr<TimerJob>& task2) { return (*task1 > *task2); };
typedef std::priority_queue<std::shared_ptr<TimerJob>, std::vector<std::shared_ptr<TimerJob>>, decltype(comp)> TimerJobQueue;

class XTimer 
{
public:
	XTimer();
    ~XTimer();

    int Start();
    int Stop();
	int Clear();
	int Cancel(std::shared_ptr<TimerJob>& job);
	int CancelAll();
	std::shared_ptr<TimerJob> Post(TimerFunc func, TimePoint time);
	std::shared_ptr<TimerJob> Post(TimerFunc func, Milliseconds duration, int rounds = -1);

private:
    void TimeWalker();

private:
    std::atomic<bool> m_bStart;
	std::thread m_threadTimerWalker;

	std::condition_variable m_cvTimerJob;
	std::mutex m_mutexTimerJob;
	TimerJobQueue m_queueTimerJob{comp};
} ;

#endif
