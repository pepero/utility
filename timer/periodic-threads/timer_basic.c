#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

class TickPeriodTimer{
  public:
    TickPeriodTimer(int tick_in_millsec,
                    int period_in_sec):
      m_isEnabled(false)
    {
      struct timerspec itval; 
      BOSS3_ASSERT(m_tickInMs != 0);
      struct itimerspec itval;
      BTInt32 ret;
      /* get the current time */
      ret = clock_gettime(itval.it_value);
      if (ret == -1) {
        perror("timerfd clock_gettime failed");
        itval.it_value.tv_sec = 1;
        itval.it_value.tv_nsec = 0;
      }
      itval.it_interval.sec = tick_in_millsec / 1000;
      itval.it_interval.nsec = (tick_in_millsec % 1000) * 1000 * 1000 ;
      m_timerfd = timerfd_create(CLOCKID, TFD_CLOEXEC);
      ret = timerfd_settime(m_timerfd, 0, &itval, NULL);
      pthread_create(&m_threadId, NULL, running, period_in_sec);             
    }
    void enable(bool state){
      m_isEnabled = state;
    }
    void running(){
      while(1){
        ret = read (m_timerfd, &missed, sizeof (missed));
        if (ret == -1) {
          perror ("read timer");
          return;
        }
        handleTick();
      }
    }
    void handleTick(int tick_in_sec){
      if(m_isEnabled) {
        // accumulate ticks
        m_tickCounts += tick_in_ms; 
        if(m_tickCounts >= m_period) {
          m_tickCounts = 0;
          handlePeriod();
        }
      }
    }

    void handlePeriod(){
      // disable for expire
      enable(false);
    }

  private:
    int m_timerfd;
    bool m_isEnabled;
    int m_tickCounts;
    pthread_t m_threadId;
}

int main(){
  TickPeriodTimer tptimer(100, 1);
  tptimer.enable();
}
