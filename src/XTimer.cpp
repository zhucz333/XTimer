#include  "XTimer.h"

XTimer::XTimer() : m_bStart(false)
{
}

XTimer::~XTimer()
{
	Stop();
}

int XTimer::Start()
{
	if (m_bStart.load())
	{
		return 0;
	}

	m_bStart.store(true);
	m_threadTimerWalker = std::thread(&XTimer::TimeWalker, this);
	return 0;
}

int XTimer::Stop()
{
	if (!m_bStart.load())
	{
		return 0;
	}

	m_bStart.store(false);
	m_threadTimerWalker.join();

	return 0;
}

int XTimer::Clear()
{
	{
		std::unique_lock<std::mutex> ul(m_mutexTimerJob);
		while (!m_queueTimerJob.empty())
		{
			m_queueTimerJob.pop();
		}
	}

	return 0;
}

int XTimer::Cancel(std::shared_ptr<TimerJob>& job)
{
	{
		std::unique_lock<std::mutex> ul(m_mutexTimerJob);
		if (!job->m_atomicCancelled.load())
		{
			job->m_atomicCancelled.store(true);
		}
	}

	return 0;
}

int XTimer::CancelAll()
{
	{
		std::unique_lock<std::mutex> ul(m_mutexTimerJob);
		while (!m_queueTimerJob.empty()) 
		{
			m_queueTimerJob.pop();
		}
	}

	return 0;
}

std::shared_ptr<TimerJob> XTimer::Post(TimerFunc func, TimePoint time)
{
	auto job = std::make_shared<TimerJob>();
	job->m_timeExpireTime = time;
	job->m_nRoundDuration = Milliseconds(0);
	job->m_atomicCancelled.store(false);
	job->m_nRounds = 0;
	job->m_funcTimerFunc = func;
	{
		std::unique_lock<std::mutex> ul(m_mutexTimerJob);
		m_queueTimerJob.push(job);
	}

	m_cvTimerJob.notify_one();

	return job;
}

std::shared_ptr<TimerJob> XTimer::Post(TimerFunc func, Milliseconds duration, int rounds)
{
	auto job = std::make_shared<TimerJob>();
	job->m_timeExpireTime = SteadyClock::now() + duration;
	job->m_nRoundDuration = Milliseconds(duration);
	job->m_atomicCancelled.store(false);
	job->m_nRounds = rounds;
	job->m_funcTimerFunc = func;

	{
		std::unique_lock<std::mutex> ul(m_mutexTimerJob);
		m_queueTimerJob.push(job);
	}

	m_cvTimerJob.notify_one();

	return job;
}

void XTimer::TimeWalker()
{
	std::shared_ptr<TimerJob> job = nullptr;

	while (m_bStart.load())
	{
		{
			std::unique_lock<std::mutex> ul(m_mutexTimerJob);
			if (m_queueTimerJob.empty())
			{
				if (!m_cvTimerJob.wait_for(ul, Milliseconds(100), [this] { return !m_queueTimerJob.empty(); }))
				{
					continue;;
				}
			}

			job = m_queueTimerJob.top();
			if (job->m_atomicCancelled.load())
			{
				m_queueTimerJob.pop();
				continue;
			}

			if (job->m_timeExpireTime > SteadyClock::now())
			{
				std::this_thread::sleep_for(Nanoseconds(100));
				continue;
			}

			m_queueTimerJob.pop();
			if (job->m_nRounds == -1 || --job->m_nRounds > 0)
			{
				job->m_timeExpireTime += job->m_nRoundDuration;
				m_queueTimerJob.push(job);
			}
		}

		if (job)
		{
			job->m_funcTimerFunc();
		}
	}
}
