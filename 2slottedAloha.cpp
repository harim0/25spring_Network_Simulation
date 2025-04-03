#include <iostream>
#include <queue>
#include <cstdlib>
using namespace std;

const int NUM_NODE = 10;
const int MAXTIME = 1000000;
const int MAX_BACKOFF = 10;

class Packet {
private:
	int packet_ID;
	int gen_time;
	int dest_time;
public:
	Packet(int id, int time) : packet_ID(id), gen_time(time), dest_time(-1) {}
	void setArrivalTime(int time) { dest_time = time; }
	int getDelay() const { return dest_time - gen_time; }
};

class Node {
private:
	int node_ID;
	queue<Packet*> q;
public:
	Node(int id) : node_ID(id) {}

	bool isEmpty() const { return q.empty(); }

	void addPacket(Packet* pkt) {
		q.push(pkt);
	}

	Packet* popPacket() {
		Packet* pkt = q.front();
		q.pop();
		return pkt;
	}

	int qlen() const {
		return q.size();
	}

	double averageDelay() const {
		if (q.empty()) return 0.0;

		double total = 0.0;
		int count = 0;
		queue<Packet*> temp = q;

		while (!temp.empty()) {
			total += temp.front()->getDelay();
			temp.pop();
			count++;
		}

		return total / count;
	}
};

bool attemptTransmission(double prob) {
	return ((double)rand() / RAND_MAX) < prob;
}

void run_slotted_aloha(int SLOT_SIZE) {
	srand(42); 

	cout << "\n===== SLOT_SIZE = " << SLOT_SIZE << " =====" << endl;

	for (double prob = 0.01; prob < 0.25; prob += 0.01) {
		Node* AP = new Node(-1);
		Node* nodes[NUM_NODE];
		int backoff[NUM_NODE] = {0};
		int pkt_id = 0;

		int tx_attempts[NUM_NODE] = {0};
		int tx_success[NUM_NODE] = {0};
		int retransmissions[NUM_NODE] = {0};

		for (int i = 0; i < NUM_NODE; i++) {
			nodes[i] = new Node(i);
			nodes[i]->addPacket(new Packet(pkt_id++, 0));
		}

		for (int t = 0; t < MAXTIME; t++) {
			if (t % SLOT_SIZE != 0) continue; 

			bool attempt[NUM_NODE] = {false};
			int cnt = 0, sender = -1;

			for (int i = 0; i < NUM_NODE; i++) {
				if (backoff[i] == 0 && !nodes[i]->isEmpty()) {
					if (attemptTransmission(prob)) {
						attempt[i] = true;
						tx_attempts[i]++;
						sender = i;
						cnt++;
					}
				}
			}

			if (cnt == 1) {
				Packet* pkt = nodes[sender]->popPacket();
				pkt->setArrivalTime(t);
				AP->addPacket(pkt);
				tx_success[sender]++;
				nodes[sender]->addPacket(new Packet(pkt_id++, t));
			} else if (cnt > 1) {
				for (int i = 0; i < NUM_NODE; i++) {
					if (attempt[i]) {
						backoff[i] = rand() % MAX_BACKOFF;
						retransmissions[i]++;
					}
				}
			}

			for (int i = 0; i < NUM_NODE; i++) {
				if (backoff[i] > 0) backoff[i]--;
			}
		}

		cout << "prob = " << prob
            << " | throughput = " << (double)AP->qlen() / MAXTIME
            << " | avg_delay = " << AP->averageDelay() << endl;

    for (int i = 0; i < NUM_NODE; i++) {
        cout << "Node " << i
                << " | Tx: " << tx_attempts[i]
                << " | Success: " << tx_success[i]
                << " | Retrans: " << retransmissions[i]
                << " | SuccessRate: "
                << (tx_attempts[i] == 0 ? 0.0 : (double)tx_success[i] / tx_attempts[i])
                << endl;
		}
		cout << "---------------------------------------------" << endl;

		delete AP;
		for (int i = 0; i < NUM_NODE; i++) delete nodes[i];
	}
}

int main() {
	run_slotted_aloha(1);   // Pure ALOHA와 비슷
	// run_slotted_aloha(5);   // Slot 간격이 있음
	// run_slotted_aloha(10);  // 더 느린 전송 주기
}