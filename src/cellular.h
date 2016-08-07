#ifndef CELLULAR_H_
#define CELLULAR_H_

#include <omnetpp.h>
#include <cqi_m.h>
#include <frame_m.h>
#include <antenna.h>

namespace cellularnetwork
{
    class cellular : public cSimpleModule
    {
        private:
            simsignal_t responseTime;
            antenna* antennaRef;
            double p;           //parameter for the binomial distribution
        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage* msg);
            void createCqi();
    };
}
#endif /* CELLULAR_H_ */
