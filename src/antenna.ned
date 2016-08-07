
package cellularnetwork;

simple antenna
{
	parameters:
	    int num_users;
	    double timeslot @unit(s) = 1ms;
	    @statistic[queueLength](title="queueLength"; source="queueLength"; record=stats, vector); 
	    @statistic[usedRB](title="usedRB"; source="usedRB"; record=stats, vector);
	    @statistic[throughput](title="throughput"; source="throughput"; record=stats, vector);
	 	@statistic[globalResponseTime](title="globalResponseTime"; source="globalResponseTime"; record=stats, vector);
	
	gates:
	    input in_c[num_users];		//input from cellular
	    input in_ps[num_users];		//input from packet sources
	    output out[num_users];		//output to cellular
}