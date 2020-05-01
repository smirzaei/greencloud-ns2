/*
 * provideroutagent.cc
 *
 *  @date Nov 12, 2013
 *  @author Guzek:Mateusz
 */

#include "provideroutagent.h"
#include "cloudtask.h"

static class ProvOutAgentClass : public TclClass {
public:
	ProvOutAgentClass() : TclClass("Agent/TCP/ProvOutAgent") {}
	TclObject* create(int, const char*const*) {
		return (new ProviderOutAgent());
	}
} class_tsk_provoutagent;

ProviderOutAgent::ProviderOutAgent() : lastTrackedBytes_(0),lastTrackedTime_(0.0){

	lastTrackedTime_  = Scheduler::instance().clock();
//	lastTrackedBytes = this->getNdatabytes();
}

ProviderOutAgent::~ProviderOutAgent() {
	PoaBuf* tmp;
	while(true){
	tmp = poabuf_.detach();
	if(tmp!=NULL){
		delete tmp;
	} else {
		break;}
	}
}

int ProviderOutAgent::updateAgentDataBytes(){
	int result =  getNdatabytes() - lastTrackedBytes_;
	lastTrackedBytes_ = getNdatabytes();
	return result;
}

double ProviderOutAgent::updateTime(){
	double result = lastTrackedTime_;
	lastTrackedTime_ = Scheduler::instance().clock();
	return result;
}

void ProviderOutAgent::sendmsg(int nbytes, void* pointer){
	poabuf_.insert(new PoaBuf( pointer,nbytes));
	tryToSend();
}

void ProviderOutAgent::tryToSend(){
	if(current_pointer_==NULL){
		PoaBuf* cb = poabuf_.detach();
		if(cb!=NULL){
			TcpAgent::sendmsg(cb->bytes(),(void*) cb->pointer());
			delete cb;
		}
	}
}

PoaBuf::PoaBuf(void *c, int nbytes)
{
	nbytes_ = nbytes;
	pointer_=c;
	next_ = NULL;
}

PoaBufList::~PoaBufList()
{
	while (head_ != NULL) {
		tail_ = head_;
		head_ = head_->next_;
		delete tail_;
	}
}

void PoaBufList::insert(PoaBuf *poabuf)
{
	if (tail_ == NULL)
		head_ = tail_ = poabuf;
	else {
		tail_->next_ = poabuf;
		tail_ = poabuf;
	}
}

PoaBuf* PoaBufList::detach()
{
	if (head_ == NULL)
		return NULL;
	PoaBuf *p = head_;
	if ((head_ = head_->next_) == NULL)
		tail_ = NULL;
	return p;
}



