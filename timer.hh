#ifndef TIMER_HH
#define TIMER_HH

#include <ctime>
#include <cmath>

#include <iostream>

class Timer {
public:
	Timer() {
		clock_gettime(CLOCK_MONOTONIC, &start_);
		prev_.tv_nsec = 0;
		prev_.tv_sec = 0;
	}

	// Palauttaa toden jos prev_ ajasta on kulunut näin monta nanosekuntia
	// Jos on kulunut asettaa prev_ nykyiseen aikaan
	// Jos ei ole kulunut palauttaa false eikä muuta prev_
	bool ellapsed(unsigned int nsec) {
		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);

		unsigned int ellapsedNs = (now.tv_sec - prev_.tv_sec) * pow(10, 9) + (now.tv_nsec - prev_.tv_nsec);

		if (ellapsedNs > nsec) {
			// std::cout << "Kulutunut " << ellapsedNs << " nsec, fps = " << (pow(10, 9) / ellapsedNs) << std::endl;
			prev_ = now;
			return true;
		}
		return false;
	}

	// palauttaa millisekuntit timerin käynnistämisestä
	// esim. 16.7sec -> 167msec
	double millisecondsSinceStart() const {
		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		return 10 * (now.tv_sec - start_.tv_sec) + ((now.tv_nsec - start_.tv_nsec) / pow(10, 8));
	}

private:
	timespec start_;
	timespec prev_;
};

#endif
