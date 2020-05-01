/*
 * bytecounter.cc
 *
 *  @date Nov 27, 2013
 *  @author Guzek:Mateusz
 */

#include "bytecounter.h"

ByteCounter::ByteCounter() {
	resetBytesSince();

}

ByteCounter::~ByteCounter() {

}

int ByteCounter::resetBytesSince(){
	int result =  this->bytes_since_;
	this->bytes_since_ = 0;
	this->last_bytes_since_=Scheduler::instance().clock();
	return result;
}

double ByteCounter::getLastBytesSinceTime(){
	return last_bytes_since_;
}

