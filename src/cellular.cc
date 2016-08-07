
#include <cellular.h>

namespace cellularnetwork
{

    Define_Module(cellular);

    void cellular::createCqi()
    {
        Cqi* msg = new Cqi("cqi");
        int type = par("distr_cqi");
        int i;
        if (type==0)      //uniform distribution
        {
            i = uniform(1, 15, 1);
        }
        else              //binomial distribution
        {
            i = binomial(14, p, 1)+1;
        }
        msg->setCqi(i);
        int id = par("id");
        msg->setId(id);
        send(msg, "out");
    }

    void cellular::initialize()
    {
        responseTime = registerSignal("responseTime");

        //to get a reference to the antenna and let it emit the global response time
        cModule* temp;
        temp = getModuleByPath("antenna");
        antennaRef = check_and_cast<antenna*>(temp);

        int type = par("distr_cqi");
        if (type==1)            //if the binomial distribution is used, p is initialized
        {
            p = uniform(0, 1, 0);
        }
    }

    void cellular::handleMessage(cMessage* msg)
    {
        if (strcmp(msg->getName(), "send_cqi")==0)
        {
            delete(msg);
            createCqi();
        }
        else if (strcmp(msg->getName(), "frame")==0)
        {
            ev << "Frame arrived" << endl;
            Frame* frame = check_and_cast<Frame*>(msg);

            //to compute response time for every packet arrived in the piece of frame
            for (unsigned int i=0; i<frame->getTEnqArraySize(); i++)
            {
                simtime_t diff = simTime()-frame->getTEnq(i);
                emit(responseTime, diff);
                antennaRef->emit(antennaRef->globalResponseTime, diff);
            }

            delete(msg);
        }
    }


    /*void cellular::finish()
    {
        guessNextEvent(); //returns the reference to the first elemenet of the FES (queue of events)
    }*/



}
