#include "prefetcher.h"
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
	int size = 4096;
	struct table entry[size];
	addrindex = ((req.addr>>4)<<22)>>22;
	if(!_ready){
		for(int i=0; i<size; i++){
			if (req.pc == entry[i].pc){ position=i; }
			else if(entry[i].taken==0){
				entry[i].pc = req.pc;
				entry[i].prevaddr = req.addr;
				entry[i].stride = 0;
				entry[i].state = 0;
				entry[i].taken = 1;
			}
		}
		prevaddrindex = ((entry[position].prevaddr>>4)<<22)>>22;
		if(entry[position].state == 2){
			_nextReq.addr = req.addr + 32*entry[position].stride;
			_ready = true;
		}
		if(addrindex != (prevaddrindex + entry[position].stride)){
			if(entry[position].state == 0){
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
			else if(entry[position].state == 1){
				entry[position].stride = addrindex - prevaddrindex;
				entry[position].prevaddr = req.addr;
				entry[position].state = 3;
			}
			else if(entry[position].state == 2){
				entry[position].prevaddr = req.addr;
				entry[position].state = 0;
			}
			else if(entry[position].state == 3){
				entry[position].stride = addrstride - prevaddrstride;
				entry[position].prevaddr = req.addr;
			}
		}
		else if(addrindex == (prevaddrindex + entry[position].stride)){
			if(entry[position].state == 0 || entry[position].state == 1 || entry[position].state == 2){
				entry[position].prevaddr = req.addr;
				entry[position].state = 2;
			}
			else if(entry[position].state == 3){
				entry[position].prevaddr = req.addr;
				entry[position].state = 1;
			}
		}
	}
}
