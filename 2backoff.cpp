#include <iostream>
#include <queue>
#include <cstdlib>
using namespace std;

#define NUM_NODE 10
#define MAXTIME 1000000
#define MAX_BACKOFF 10

class Packet {
private:
	int packet_ID;
	int gen_time;
	int dest_time;
public:
	Packet() {}
	Packet(int id, int time) : packet_ID(id), gen_time(time) {}
	void setDT(int time) { dest_time = time; }
	int getDelay() { return dest_time - gen_time; }
};

class Node {
private:
	int Node_ID;
	queue<Packet*> q;
public:
	Node() {}
	Node(int id) { Node_ID = id; }
	bool isEmpty() { return q.empty(); }
	void addPacket(Packet* pkt) { q.push(pkt); }
	int qlen() { return q.size(); }
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
	int pkt_num;
	int backoff[NUM_NODE];

	for(double prob = 0.01; prob < 0.25; prob += 0.01) {
		Node* AP = new Node();
		Node* NodeContainer[NUM_NODE];
		pkt_num = 0;

		for(int i=0; i<NUM_NODE; i++) {
			NodeContainer[i] = new Node(i);
			backoff[i] = 0;
			Packet* pkt = new Packet(pkt_num++, 0);
			NodeContainer[i]->addPacket(pkt);
		}

		for(int t=0; t<MAXTIME; t++) {
			bool attempt[NUM_NODE] = {false};
			int cnt = 0;
			int sender = -1;

			for (int i = 0; i < NUM_NODE; i++) {
				if (backoff[i] == 0 && !NodeContainer[i]->isEmpty()) {
					if ((double)rand()/RAND_MAX < prob) {
						attempt[i] = true;
						cnt++;
						sender = i;
					}
				}
			}

			if (cnt == 1) {
				tx_success(t, NodeContainer[sender], AP);
				NodeContainer[sender]->addPacket(new Packet(pkt_num++, t));
			} else if (cnt > 1) {
				for (int i = 0; i < NUM_NODE; i++) {
					if (attempt[i]) {
						backoff[i] = rand() % MAX_BACKOFF;
					}
				}
			}

			for (int i = 0; i < NUM_NODE; i++) {
				if (backoff[i] > 0)
					backoff[i]--;
			}
		}

		cout << prob << "\t" << 1.0 * AP->qlen() / MAXTIME << "\t" << AP->avg_delay() << endl;

		delete AP;
		for (int i = 0; i < NUM_NODE; i++) delete NodeContainer[i];
	}
}