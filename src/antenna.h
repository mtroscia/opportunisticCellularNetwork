
#ifndef ANTENNA_H_
#define ANTENNA_H_
#include <omnetpp.h>
#include <queue>
#include <cqi_m.h>
#include <data_m.h>
#include <frame_m.h>

namespace cellularnetwork{

class antenna : public cSimpleModule
{
    private:

        struct qelem{           //info about the packet
            int dim;
            simtime_t t;       //arrival time
        };

        struct info{            //info about the user
            int id;
            int cqi;
            std::queue<qelem> q;
        };

        std::vector<info> iusers;

        simsignal_t throughput;
        simsignal_t queueLength;
        simsignal_t usedRB;

    public:

        simsignal_t globalResponseTime;

    protected:

        virtual void initialize();
        virtual void handleMessage(cMessage* msg);
        int searchId(int u);
        void swap(std::vector<info>& v, int i, int j);
        void sortCqi(std::vector<info>& v);
        void handleBeep();
        void handleData(Data* data);
        void handleCqi(Cqi* cqi);
        void handleFrame();
};
}

#endif
