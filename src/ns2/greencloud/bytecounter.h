/*
 * bytecounter.h
 *
 *  @date Nov 27, 2013
 *  @author Guzek:Mateusz
 */

#ifndef BYTECOUNTER_H_
#define BYTECOUNTER_H_

#include "scheduler.h"

class ByteCounter {
public:
	ByteCounter();
	virtual ~ByteCounter();
	/**
	 * Returns the value of bytes since last check.
	 */
	int resetBytesSince();
	/**
	 * Returns the time of the last check.
	 */
	double getLastBytesSinceTime();
protected:
	int bytes_since_;
	double last_bytes_since_;
};

#endif /* BYTECOUNTER_H_ */
