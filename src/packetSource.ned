
package cellularnetwork;

simple packetSource
{
	parameters:
	    int id;
	    double mean;	//mean for the exponential distribution
	    
	    //75 was chosen as the largest packet dimension must be such that it fits a frame at the minimum cqi
	    int max_pkt_len = default(75);	
	gates:
	    output out;			//output to antenna
}