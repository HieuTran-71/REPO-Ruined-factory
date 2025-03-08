
#ifndef IMP_TIMER_H_
#define IMP_TIMER_H_

class ImpTimer
{
public:
    ImpTimer();
    ~ImpTimer();

    void start(); //chay
    void stop(); //dung lai
    void paused();
    void unpaused();

    int get_ticks();

    bool is_started();
    bool is_paused();



private:
    int start_tick_;
    int paused_tick_;

    bool is_paused_;
    bool is_started_;
};



#endif
