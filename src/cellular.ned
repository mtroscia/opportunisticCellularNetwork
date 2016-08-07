
package cellularnetwork;

simple cellular
{
	parameters:
	    int distr_cqi;			//to indicate which distribution, the uniform or the binomial, has to be used for generating CQIs			
	    int id;
	    @statistic[responseTime](title="responseTime"; source="responseTime"; record=stats, vector);
	    
	gates:
	    input in;			//input from antenna
	    output out;			//output to antenna
}