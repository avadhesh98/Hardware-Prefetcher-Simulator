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
	int position;
	int size = 1024;
	struct table entry[size];
	u_int32_t addrindex = ((req.addr>>4)<<22)>>22;
	u_int32_t prevaddrindex;
	/*if(!_ready && !req.HitL1){
		_nextReq.addr = req.addr + 16;
		_ready = true;
	}*/
	if(!_ready || !req.HitL2){
		position = addrindex;
		prevaddrindex = ((entry[position].prevaddr>>4)<<22)>>22;
		if(addrindex != (prevaddrindex + entry[position].stride)){
			if(entry[position].state == 0){
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
			}
			else if(entry[position].state == 1){
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
				entry[position].state = 0;
			}
		}
		else if(addrindex == (prevaddrindex + entry[position].stride)){
			if(entry[position].state == 0){
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
			else if(entry[position].state == 1){
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
		}
		if(entry[position].state == 1){
			_nextReq.addr = req.addr + (entry[position].stride)<<4;
			_ready = true;
		}
		else{
			_nextReq.addr = req.addr + 32;
			_ready = true;
		}
	}
}
