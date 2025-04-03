#include <iostream>
#include <queue>
#include <stdlib.h>
using namespace std;

#define NUM_NODE 10

class Packet {
	private :
		int packet_ID;
		int gen_time;
		int dest_time;
	public :
		Packet() {}
		Packet(int id, int time) {
            packet_ID = id;
            gen_time = time;
		}
		void setDT(int time) {	// record the arrival time 
            dest_time = time;
		}
		int getDelay() {
            return dest_time - gen_time;
		}
};

class Node {
	private :
		int Node_ID;
		queue<Packet*> q;
	public :
		Node() {}
		Node(int id) {
			Node_ID = id;
		}
		bool isEmpty() {
			if(q.empty())
				return true;
			else
				return false;
		}
		void addPacket(Packet* pkt) {
			q.push(pkt);
		}
		int qlen() {
			return q.size();
		}
		Packet* popPacket() {
			Packet* p = q.front();
			q.pop();
			return p;
		}
		double avg_delay() {
            double total_delay = 0.0;
            int count = 0;

            queue<Packet*> temp = q;
            while (!temp.empty()) {
                Packet* pkt = temp.front(); 
                total_delay += pkt->getDelay();
                temp.pop();
                count++;
            }

            return (count == 0) ? 0.0 : total_delay / count;
		}
};

void tx_success(int time, Node* former, Node* next) {
    if (!former->isEmpty()) {
        Packet* pkt = former->popPacket();  
        pkt->setDT(time);                  
        next->addPacket(pkt);               
    }
}

int main() {

	int MAXTIME = 1000000;
	int pkt_num;
	bool attempt[NUM_NODE];

	for(double prob = 0.01; prob < 0.25; prob = prob + 0.01) {

		Node* AP = new Node();
		Node* NodeContainer[NUM_NODE];
        pkt_num = 0;
        int cnt = 0;
		for(int i=0; i<NUM_NODE; i++) {
			attempt[i] = false;
			Node* temp = new Node(i);
			NodeContainer[i] = temp;
            if (NodeContainer[i]->isEmpty()) {
                Packet* pkt = new Packet(pkt_num, 0); 
                NodeContainer[i]->addPacket(pkt);
                pkt_num++;
            }
		}
        
		for(int i=0; i<MAXTIME; i++) {
            // stations attempt transmission with prob. (attemp_prob)
            for(int num=0; num<NUM_NODE; num++){
                if(1.0*rand() / RAND_MAX < prob){
                    attempt[num] = true;
                }else{
                    attempt[num] = false;
                }
            }
            cnt = 0;
            for(int num=0; num<NUM_NODE; num++)
                cnt += attempt[num];
            
            if (cnt == 1) {
                for (int num = 0; num < NUM_NODE; num++) {
                    if (attempt[num]) {
                        tx_success(i, NodeContainer[num], AP);
                        Packet* pkt = new Packet(pkt_num++, i);
                        NodeContainer[num]->addPacket(pkt);
                        break;
                    }
                }
            }

			// if successful, 
			//   move the packet from source node to AP
			//   and record the delay 

		}

		cout << prob << "	" \
			<< 1.0* AP->qlen() / MAXTIME << "	" \
			<< AP->avg_delay() << endl;
	}
}
