#include "prefetcher.h"
#include <cstdio>
#include "mem-sim.h"

Prefetcher::Prefetcher() { _ready = false; }

bool Prefetcher::hasRequest(u_int32_t cycle) {
	return _ready;
}

Request Prefetcher::getRequest(u_int32_t cycle) {
	return  _nextReq;
}

void Prefetcher::completeRequest(u_int32_t cycle) { _ready = false; }

void Prefetcher::cpuRequest(Request req) {
	int position; //variable to store the position of the index where the the required data resides.
	int size = 1024; //size of the table = 1024 since index of D-cache is 10 bits
	struct table entry[size]; //create a table with 1024 entries.
	int b = 5; //number of offset bits
	int s = 10; //number of index bits
	u_int32_t addrindex = ((req.addr>>b)<<(32-s))>>(32-s); //calculate the index of the requested address.
	u_int32_t prevaddrindex;

	if(!_ready || !req.HitL2){ // check if a prefetch request can be taken or if there is a miss in L2 cache
		position = addrindex;
		prevaddrindex = ((entry[position].prevaddr>>b)<<(32-s))>>(32-s); //calculate the index of previous address.
		if(addrindex != (prevaddrindex + entry[position].stride)){ //case when the requested address is not equal to previous address + stride
			if(entry[position].state == 0){ //state = 0 means initial
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
			}
			else if(entry[position].state == 1){ //state = 1 means steady
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
				entry[position].state = 0;
			}
		}
		else if(addrindex == (prevaddrindex + entry[position].stride)){ //case when the reuested addess is equal to previous + stride
			if(entry[position].state == 0){ //if initial, change the state to steady
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
			else if(entry[position].state == 1){ //if steady, continue in steady state
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
		}
		if(entry[position].state == 1){ //if the state is steady, feed requested address + stride into the prefetch
			_nextReq.addr = req.addr + (entry[position].stride)<<5;
			_ready = true; //no more requests
		}
		else{ // In a condition where no steady stride is found, prefetch the next block in L2 cache
			_nextReq.addr = req.addr + 32;
			_ready = true;
		}
	}
}
