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

void run_csma_non_persistent() {
    srand(42);
    cout << "\n=== Non-Persistent CSMA ===" << endl;

    for (double prob = 0.01; prob < 0.25; prob += 0.01) {
        Node* AP = new Node(-1);
        Node* nodes[NUM_NODE];
        int backoff[NUM_NODE] = {0};
        int pkt_id = 0;
        int tx_attempts[NUM_NODE] = {0};
        int tx_success[NUM_NODE] = {0};

        for (int i = 0; i < NUM_NODE; i++) {
            nodes[i] = new Node(i);
            nodes[i]->addPacket(new Packet(pkt_id++, 0));
        }

        bool channel_busy = false;

        for (int t = 0; t < MAXTIME; t++) {
            for (int i = 0; i < NUM_NODE; i++) {
                if (backoff[i] == 0 && !nodes[i]->isEmpty()) {
                    if (!channel_busy && attemptTransmission(prob)) {
                        tx_attempts[i]++;
                        Packet* pkt = nodes[i]->popPacket();
                        pkt->setArrivalTime(t);
                        AP->addPacket(pkt);
                        tx_success[i]++;
                        nodes[i]->addPacket(new Packet(pkt_id++, t));
                        channel_busy = true;
                    } else if (channel_busy) {
                        backoff[i] = rand() % MAX_BACKOFF;
                    }
                }
            }

            for (int i = 0; i < NUM_NODE; i++) {
                if (backoff[i] > 0) backoff[i]--;
            }

            channel_busy = false;
        }

        cout << "prob = " << prob
			<< " | throughput = " << (double)AP->qlen() / MAXTIME
			<< " | avg_delay = " << AP->averageDelay() << endl;

        for (int i = 0; i < NUM_NODE; i++) delete nodes[i];
        delete AP;
    }
}

void run_csma_1_persistent() {
    srand(42);
    cout << "=== 1-Persistent CSMA ===" << endl;

    for (double prob = 0.01; prob < 0.25; prob += 0.01) {
        Node* AP = new Node(-1);
        Node* nodes[NUM_NODE];
        int backoff[NUM_NODE] = {0};
        int pkt_id = 0;
        int tx_attempts[NUM_NODE] = {0};
        int tx_success[NUM_NODE] = {0};

        for (int i = 0; i < NUM_NODE; i++) {
            nodes[i] = new Node(i);
            nodes[i]->addPacket(new Packet(pkt_id++, 0));
        }

        bool channel_busy = false;

        for (int t = 0; t < MAXTIME; t++) {
            int sender = -1, cnt = 0;
            for (int i = 0; i < NUM_NODE; i++) {
                if (backoff[i] == 0 && !nodes[i]->isEmpty()) {
                    if (!channel_busy && attemptTransmission(prob)) {
                        sender = i;
                        cnt++;
                    }
                }
            }

            if (cnt == 1) {
                tx_attempts[sender]++;
                Packet* pkt = nodes[sender]->popPacket();
                pkt->setArrivalTime(t);
                AP->addPacket(pkt);
                tx_success[sender]++;
                nodes[sender]->addPacket(new Packet(pkt_id++, t));
                channel_busy = true;  
            } else if (cnt > 1) {
                for (int i = 0; i < NUM_NODE; i++) {
                    if (backoff[i] == 0 && attemptTransmission(prob)) {
                        backoff[i] = rand() % MAX_BACKOFF;
                    }
                }
            } else {
                channel_busy = false; 
            }

            for (int i = 0; i < NUM_NODE; i++) {
                if (backoff[i] > 0) backoff[i]--;
            }
        }

        cout << "prob = " << prob
			<< " | throughput = " << (double)AP->qlen() / MAXTIME
			<< " | avg_delay = " << AP->averageDelay() << endl;

        for (int i = 0; i < NUM_NODE; i++) delete nodes[i];
        delete AP;
    }
}

int main() {
	cout<<"::run_csma_1_persistent::"<<endl;
	run_csma_1_persistent();
	cout<<"\n";
	cout<<"::run_csma_non_persistent::"<<endl;
	run_csma_non_persistent();
}