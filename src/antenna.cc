#include <antenna.h>

namespace cellularnetwork
{
    Define_Module(antenna);

    int byteRB(int cqi)     //correspondence between CQIs and bytes per RB
    {
        switch (cqi)
        {
            case 1:
            case 2:
                return 3;
            case 3:
                return 6;
            case 4:
                return 11;
            case 5:
                return 15;
            case 6:
                return 20;
            case 7:
                return 25;
            case 8:
                return 36;
            case 9:
                return 39;
            case 10:
                return 50;
            case 11:
                return 63;
            case 12:
                return 72;
            case 13:
                return 80;
            case 14:
            case 15:
                return 93;
            default:
                return 0;
        }
    }

    int antenna::searchId(int user)         //to search the user id in the iusers vector
    {
        unsigned int i;
        for (i=0; i<iusers.size(); i++)
        {
            if (iusers[i].id == user)
                break;
        }
        return  i;
    }

    void antenna::swap(std::vector<info>& v, int i, int j)
    {
        info tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }

    void antenna::sortCqi(std::vector<info>& v)     //to order users according decreasing CQI
    {
        int n = v.size();
        for (int i=0; i<n; i++)
        {
            bool swapped = false;
            for (int j=0; j<n-(i+1); j++)
            {
                if (v[j].cqi<v[j+1].cqi)
                {
                    swap(v, j, j+1);
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    void antenna::handleBeep()
    {
        cMessage* beep = new cMessage("beep");
        simtime_t timeslot = par("timeslot");
        scheduleAt(simTime()+timeslot, beep);
    }

    void antenna::initialize()
    {
        int n=par("num_users");
        info x;
        for(int i=0; i<n; i++)          //to inizialize iusers vector
        {
            iusers.insert(iusers.begin()+i, x);
            iusers[i].id=i;
            iusers[i].cqi=0;
        }

        //to register signal used for statistics
        throughput = registerSignal("throughput");
        globalResponseTime = registerSignal("globalResponseTime");
        queueLength = registerSignal("queueLength");
        usedRB = registerSignal("usedRB");

        //to schedule the start of the first timeslot
        cMessage* beep = new cMessage("beep");
        scheduleAt(simTime(), beep);
    }

    void antenna::handleMessage(cMessage* msg)
    {
        if(strcmp(msg->getName(),"data")==0)
        {
            handleData(check_and_cast<Data*>(msg));
        }
        else if(strcmp(msg->getName(),"cqi")==0)
        {
            handleCqi(check_and_cast<Cqi*>(msg));
        }
        else if (strcmp(msg->getName(), "beep")==0)
        {
            ev << "Start of timeslot..." << endl;

            delete(msg);

            //to request CQIs to users
            for (unsigned int i=0; i<iusers.size(); i++)
            {
                cMessage* mess = new cMessage("send_cqi");
                send(mess, "out", i);
            }
        }
    }

    void antenna::handleData(Data* data)
    {
        ev<<"Data arrived from "<< data->getId() <<endl;

        qelem el;
        el.dim = data->getDim();
        el.t = simTime();
        iusers[searchId(data->getId())].q.push(el);
        delete(data);
    }

    void antenna:: handleCqi(Cqi* cqi)
    {
        ev<<"CQI ("<< cqi->getCqi() <<") arrived from "<< cqi->getId() << endl;

        iusers[searchId(cqi->getId())].cqi= cqi->getCqi();  //as the iusers vector is not ordered by user id
        delete(cqi);
        for (unsigned int i=0; i<iusers.size(); i++)
        {
            if (iusers[i].cqi==0)       //wait until all users have sent the CQIs
                break;
            else if (i==iusers.size()-1)
            {
                sortCqi(iusers);
                handleFrame();
                handleBeep();
            }
        }
    }

    void antenna::handleFrame()         //to build the frame
    {
        int remainingRB = 25;
        std::vector<Frame*> frame;   //to contain the pieces of frame to be sent at the end of the timeslot
        int total_bytes = 0;        //to store the total number of bytes sent in the timeslot

        for (unsigned int i=0; i<iusers.size(); i++)
        {
            if (remainingRB==0)     //no more space in the RB; only for queue length statistic
            {
                emit(queueLength, iusers[i].q.size());
            }
            else
            {
                emit(queueLength, iusers[i].q.size());

                //check if there are packets of user i to send
                int num_bytes = 0;      //total number of byte in the queue
                std::queue<qelem> copy_q(iusers[i].q);      //temporary copy to scan the queue
                while (!copy_q.empty())
                {
                    num_bytes += copy_q.front().dim;
                    copy_q.pop();
                }


                if (num_bytes == 0)
                    ev << "[user=" << iusers[i].id << "] NO PACKETS TO SEND! " << endl;


                //if there are packets to send prepare the (piece of) frame for i
                if (iusers[i].q.size()!=0 && byteRB(iusers[i].cqi)!=0)
                {
                    int byte_RB = byteRB(iusers[i].cqi);    //byte per RB according to the table
                    int num_RB = num_bytes/byte_RB;         //number of RB required
                    if (num_bytes%byte_RB!=0)               //check if another RB is needed
                        num_RB++;

                    //if there is enough space in the (piece of) frame for packets of i
                    if (remainingRB>=num_RB)
                    {
                        total_bytes += num_bytes;        //bytes to be considered for the throughput statistic
                        remainingRB-=num_RB;

                        Frame* f = new Frame("frame");       //to create the piece of frame for the user i
                        f->setNumBytes(num_bytes);
                        f->setId(iusers[i].id);
                        f->setNumPkts(iusers[i].q.size());
                        f->setTEnqArraySize(iusers[i].q.size());


                        ev << "[user=" << iusers[i].id << "] Num bytes " << num_bytes << "; num pkts " << iusers[i].q.size() << endl;


                        int n = iusers[i].q.size();
                        for (int k=0; k<n; k++)         //for every packet in the queue of user i
                        {
                            simtime_t tt = iusers[i].q.front().t;       //to get the time when the packet arrived
                            f->setTEnq(k, tt);

                            /**DEBUG**/
                            ev<< "[user=" << iusers[i].id << "] Simtime of " << k+1 << " is " << tt << endl;
                            /*********/
                            iusers[i].q.pop();
                        }

                        frame.insert(frame.begin(), f);            //insert in the vector of pieces of frame
                    }
                    else    //there is no enough space in the frame for packets of i
                    {
                        ev << "[user=" << iusers[i].id << "] NO ENOUGH SPACE!" << endl;
                    }
                }
            }
        }

        //send all pieces of frame composed
        while (!frame.empty())
        {
            Frame* f = frame.front();
            frame.erase(frame.begin());
            send(f, "out", f->getId());
        }

        emit(throughput, total_bytes);

        int used = 25-remainingRB;
        emit(usedRB, used);

        for (unsigned int i=0; i<iusers.size(); i++)
        {
            iusers[i].cqi=0;        //so that the vector is in a consistent state when the following timeslot starts
        }
    }
}
