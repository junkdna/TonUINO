#ifndef _TIMEOUT_H_
#define _TIMEOUT_H_

class Timeout {
    protected:
        unsigned long start_millis;
        unsigned long end_millis;
        unsigned long duration_millis;

    public:
        Timeout(unsigned long duration) {
            start_millis = millis();
            end_millis = start_millis + duration;
            duration_millis = duration;
        }

        void start(unsigned long duration) {
            duration_millis = duration;
            reset();
        }

        void reset() {
            start_millis = millis();
            end_millis = start_millis + duration_millis;
        }

        bool expired() {
            unsigned long now;

            now = millis();
            if (now < start_millis)
                return true;
            else if (now >= end_millis)
                return true;
            else
                return false;
        }
};

#endif
// vim: ts=4 sw=4 et cindent
