
packet Frame {
    int id;				//id user
    int numBytes;
    int numPkts;
    simtime_t tEnq[];	//vector containing times at which each packet arrived on the antenna
}
