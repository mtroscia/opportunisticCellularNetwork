
package cellularnetwork.simulations;

import cellularnetwork.antenna;
import cellularnetwork.cellular;
import cellularnetwork.packetSource;

network cellularNetwork
{
    parameters:
        int numUsers;
        
	@display("bgb=497,311,white,white");
	
	
    submodules:
        
        cellular[numUsers]: cellular {
            id = index;			//returns the index of the istance of the module in the array
            @display("i=device/cellphone;is=s;p=433,196");
        }
        
        packetsource[numUsers]: packetSource {
            id = index;			//returns the index of the istance of the module in the array
            @display("i=device/server2;is=n;p=167,188");
        }
        
        antenna: antenna {
            num_users = numUsers;
            @display("i=device/antennatower,,0;is=vl;p=298,73");
        }


    connections:
        
        for i=0..numUsers-1 {
            cellular[i].out --> antenna.in_c[i];
            packetsource[i].out --> antenna.in_ps[i];
            antenna.out[i] --> cellular[i].in;
        }
}
