/*
 * provideroutagent.h
 *
 *  @date Nov 12, 2013
 *  @author Guzek:Mateusz
 */

#ifndef PROVIDEROUTAGENT_H_
#define PROVIDEROUTAGENT_H_
#include <tcpapp.h>
#include "agent.h"
#include "tcp.h"
#include <stdlib.h>
#include <iostream>

/*Buffered list based on the CBuf from TcpApp*/
class PoaBuf {
public:
	PoaBuf(void *c, int nbytes);
	~PoaBuf() {
	}
	void* pointer() { return pointer_; }
	int bytes() { return nbytes_; }


protected:
	friend class PoaBufList;
	void *pointer_;
	int nbytes_; 	/**< Total length of this transmission */
	PoaBuf *next_;

};

// A FIFO queue
class PoaBufList {
public:

	PoaBufList() : head_(NULL), tail_(NULL) {}
	~PoaBufList();

	void insert(PoaBuf *poabuf);
	PoaBuf* detach();

protected:
	PoaBuf *head_;
	PoaBuf *tail_;

};

class ProviderOutAgent : public TcpAgent {
public:
	ProviderOutAgent();
	virtual ~ProviderOutAgent();
	int updateAgentDataBytes();
	double updateTime();
	void sendmsg(int nbytes, void* pointer);
	void tryToSend();
protected:
	int lastTrackedBytes_;
	double lastTrackedTime_;
	PoaBufList poabuf_;
};

#endif /* PROVIDEROUTAGENT_H_ */
