#include "timer.h"

timer::Timer::Timer(std::function<void ()> f ,int interval){
    this->f = f;
    this->interval = interval;
}

std::thread* timer::Timer::start(){
    this->running = true;
    return new std::thread([this]()
    {
		if(this->running){
			std::this_thread::sleep_for(std::chrono::milliseconds(this->interval));
			this->f();
        }

        //while(this->running){
        //    this->f();
        //    std::this_thread::sleep_for(std::chrono::milliseconds(this->interval));
        //}
    });
    //return
}

void timer::Timer::stop(){
    this->running = false;
}