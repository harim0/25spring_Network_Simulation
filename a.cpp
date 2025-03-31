#include <iostream>
#define N 10
#define T 1000000

using namespace std;

int main(){
    int attempt[N];
    double p = 0.01;
    cout<<"Tx prob.\t # Success\t # Collision\t # No TX"<<endl;
    while(p<=0.25){
        int cnt = 0;
        double success = 0;
        double collision = 0;
        double no_transmission = 0;
        for(int t=0; t<T; t++){
            for(int i=0; i<N; i++){
                if(1.0*rand() / RAND_MAX < p){
                    attempt[i] = 1;
                }else{
                    attempt[i] = 0;
                }
            }
            cnt = 0;
            for(int i=0; i<N; i++)
                cnt += attempt[i];
            if(cnt == 1)
                success += 1;
            else if(cnt >= 2)
                collision += 1;
            else if(cnt == 0)
                no_transmission += 1;
        }
        cout.precision(6);
        cout<<p<<" \t "<<success/T<<" \t "<<collision/T<<" \t "<<no_transmission/T<<" \t "<<endl;
        p+=0.01;
    }
}