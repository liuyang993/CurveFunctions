#ifndef TIMER_H
#define TIMER_H


#include <thread>
#include <chrono>


namespace timer {
    class Timer{
    public:
        //Timer(void (*f) (void),int interval);
        Timer(std::function<void ()> ,int interval);

		std::thread* start();
        void stop();
    private:
        int interval;
        bool running;
        //void (*f) (void);
		std::function<void ()> f;

    };
}
#endif