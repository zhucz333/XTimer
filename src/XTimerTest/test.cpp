#include "XTimer.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class A
{
public:
	A() : start(std::chrono::steady_clock::now()){};
    void TimerFunc(int i) {
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        printf("printf %d timer, duration %lld us\n", i, duration);
    }

	std::chrono::steady_clock::time_point start;
} ;

int main()
{
	XTimer timer;
    A a;
	timer.Start();
	for (int i = 0; i < 100; i++)
	{
		auto func = std::bind(&A::TimerFunc, &a, i);
		timer.Post(func, Milliseconds(i), 1);
	}

	getchar();
    timer.Stop();

    return 0;
}
