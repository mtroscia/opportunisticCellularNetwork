
#include <packetSource.h>

namespace cellularnetwork
{

    Define_Module(packetSource);

    void packetSource::createBeep()
    {
        cMessage* msg = new cMessage("beep");
        double mean = par("mean");
        scheduleAt(simTime()+exponential(mean, 1), msg);
    }

    void packetSource::initialize()
    {
        createBeep();
    }

    void packetSource::handleMessage(cMessage* msg)
    {
        Data* data = new Data("data");

        int max = par("max_pkt_len");
        data->setDim(uniform(1, max, 0));

        int id = par("id");
        data->setId(id);
        send(data, "out");

        delete(msg);

        createBeep();
    }
}
