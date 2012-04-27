#ifndef TIMER_HH
#define TIMER_HH

#include <ctime>
#include <cmath>

#include <iostream>

const unsigned int ONE_SEC_NS = pow(10, 9);

class Timer {
public:
	Timer() {
		// palauttaa sellaisen kellonajan joka kertoo
		// esim. kauanko kone ollut käynnissä tai jotain vastaavaa
		// tässä ei ole mitään hyötyä oikeasta kellonajasta
		// vaan riittää että voidaan mitata aikaa.
		// Myös hyötyä vrt. "rannekelloaikaan" että tähän ei vaikuta
		// jos ntp tms muuttaa kellonaikaa (vois tulla negatiivinen
		// kulunut aika).
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

		unsigned int ellapsedNs = (now.tv_sec - prev_.tv_sec) * ONE_SEC_NS + (now.tv_nsec - prev_.tv_nsec);

		if (ellapsedNs > nsec) {
			// std::cout << "Kulutunut " << ellapsedNs << " nsec, fps = " << (pow(10, 9) / ellapsedNs) << std::endl;
			prev_ = now;
			return true;
		}
		return false;
	}

	double secondsSinceStart() const {
		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		return (1.0 * now.tv_sec) - (1.0 * start_.tv_sec) + (((1.0 * now.tv_nsec) - start_.tv_nsec) / ONE_SEC_NS);
	}

private:
	timespec start_;
	timespec prev_;
};

#endif
