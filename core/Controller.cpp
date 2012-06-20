/*
 * Controller.cpp
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Lavina Jain
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Controller.cpp
/// \brief Implements routing
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "Controller.h"
#include "../config/extern.h"

////////////////////////
/// Constructor
////////////////////////
template<UI num_ip>
Controller<num_ip>::Controller(sc_module_name Controller): sc_module(Controller)
{

    void *hndl;
    void *mkr;


    sim_count=0;    //doubt

    string libname = string("./router/lib/");

    switch (RT_ALGO)
    {
    case OE: //tg = new BurstyTraffic("TG");
        libname = libname + string("OE_router.so");
        break;
    case XY:
        libname = libname + string("XY_router.so");
        break;
    case SOURCE:
        libname = libname + string("source_router.so");
        break;
    case QRT:
        libname = libname + string("Q_router.so");
        break;
        //DYAD//
    case DYAD:
        libname=libname+ string("DYADrouter.so");
        break;
        //MAXY//
    case MAXY:
        libname=libname+ string("maxy_router.so");
        break;
        //FTXY//
    case FTXY:
        libname=libname+ string("ftxy.so");
        break;
        //PROM
    case PROM:
        libname=libname+ string("PROM_router.so");
        break;
        //ERA
    case ER:
        libname=libname+ string("ER_router.so");
        break;


    }
    hndl = dlopen(libname.c_str(), RTLD_NOW);
    if (hndl == NULL)
    {
        cerr << dlerror() << endl;
        exit(-1);
    }
    mkr = dlsym(hndl, "maker");
    rtable = ((router*(*)())(mkr))();

    SC_THREAD(allocate_route);
    for (UI i = 0; i < num_ip; i++)
        sensitive << rtRequest[i];
    /*added */
    // process sensitive to power credit info, updates power info corresponding to the neighbouring tiles
    SC_THREAD(update_power_credit);
    for (UI i = 0; i < num_ip-1; i++)
    {
        sensitive << power_credit_in[i];
    }
    SC_THREAD(send_power_info);	// Thread sensitive to clock
    sensitive_pos << switch_cntrl;

    /*end*/
    //for Q-router
    //For Q-Routing
    // initialize VC request to false
    //vcRequest.initialize(false);

    // initialize virtual channels and buffers
    /*	ctrQ.num_bufs = ESTBUF;
    	ctrQ.pntr = 0;
    	ctrQ.full = false;
    	ctrQ.empty = true;*/
    /*	check = 0;
    pQtop = 0;*/

//qrt************************************************************
    rs.est_out = 0;
    rs.est_buffer = 0;

    for (UI i = 0; i < num_ip - 1; i++)
    {
        r_in[i].free = true;
    }

    SC_THREAD(transmitEst);
    sensitive_pos << switch_cntrl;

    SC_THREAD(rcv_estimate);
    for (UI i = 0; i < num_ip-1; i++)
        sensitive << estIn[i];

    rtable->num_nb = num_ip-1;
    //end for Q-ROuter
//qrt************************************************************
    /*added */
    for (int i=0; i < num_ip-1 ; i++)
    {
        powerCreditLine t;
        t.Power = 0;
        power_credit_out[i].initialize(t);
    }
    /*end*/
}

//added
///////////////////////////////////////////////////////////////////////////
//Process Sesitive to clock
//////////////////////////////////////////////////////////////////////////

template<UI num_ip>
void Controller<num_ip>::send_power_info()
{
    while (true)
    {
        wait();	// wait for the next clk cycle
        if (switch_cntrl.event())
        {
            powerCreditLine t;
            t.Power=P;
            for (int i=0; i<num_ip-1; i++)
                power_credit_out[i].write(t);
        }
    }
}


///////////////////////////////////////////////////////////////////////////
/// Process sensitive to incoming power credit information.
/// updates credit info (power values) of neighbor tiles
///////////////////////////////////////////////////////////////////////////
/*added*/
template<UI num_ip>
void Controller<num_ip>::update_power_credit()
{
    for (int i=0; i<4; i++)
    {
        rtable->power[i]=0;
    }
    while (true)
    {
        wait();	// wait until change in credit info
        powerCreditLine t;
        for (UI i = 0; i < num_ip-1; i++)
        {
            if (power_credit_in[i].event())
            {

                Pnb[i] = power_credit_in[i].read().Power;	// update credit

                UI dir=idToDir(i);
                rtable->update_power(dir,Pnb[i]);
            }
        }

    }//end while
}
//end

///////////////////////////////////////////////////////////////////////////
//Process Sesitive to clock
//////////////////////////////////////////////////////////////////////////
template<UI num_ip>
void Controller<num_ip>::transmitEst()
{

    int sim = 0;
    while (sim_count <SIM_NUM)
    {
        flit flit_out;
        wait(); //wait for clock
        //sim++;
        if (switch_cntrl.event() )  //&& (sim % 2)
        {
            sim_count++;
            /*	//		late();
            		//	cout<<this->name()<<":(trans) "<<sim_count<<endl;
            			if(ctrQ.empty == false) { //enter only if there is a estimate packet in the Q
            				flit_out = ctrQ.flit_out();  //get the packet
            				UI i;
            				switch(TOPO) { // decide the direction

            					case TORUS:
            						//#ifdef TORUS
            						i = flit_out.pkthdr.esthdr.dir;
            						//#endif
            						break;

            					case MESH:
            						//#ifdef MESH
            						UI dir = flit_out.pkthdr.esthdr.dir;
            						switch(dir) {
            							case N: i = portN;
            								break;
            							case S: i = portS;
            								break;
            							case E: i = portE;
            								break;
            							case W: i = portW;
            								break;
            							case C: i = num_ip - 1;
            								break;
            						}
            						//#endif
            						break;
            				}


            				if(ocReady_in[i].read() == true) {	// OC ready to recieve flit

            					if(LOG >= 4)
            					eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Ctr (QRT): Attempting to forward  estimate flit: "<<flit_out;

            					// VC request
            					if(LOG >= 4)
            						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID <<"Controller Requesting VC for dir: "<<i;

            					vcRequest.write(true);
            					opRequest.write(i);
            					wait();	// wait for ready event from VC


            					if(vcReady.event()) {
            						if(LOG >= 4)
            							eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Ctr: vcReady event..."<<endl;
            					}
            					else if(switch_cntrl.event()) {
            						sim_count++;
                                                        //    late();
            						if(LOG >= 4)
            							eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Ctr: unknown clock event..."<<endl;
            					}
            					// read next VCid sent by VC
            					UI vc_next_id = nextVCID.read();

            					if(vc_next_id == NUM_VCS+1) {	// VC not granted
            						if(LOG >= 4)
            							eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Ctr: No free next vc, pushing flit in Q" <<endl;
            						// push flit back in fifo
            						flit_out.simdata.num_waits++;
            						if(flit_out.simdata.num_waits < WAITS)
            							ctrQ.flit_push(flit_out);
            						vcRequest.write(false);
            						continue;
            					}
            						// write flit to output port
            					flit_out.simdata.num_sw++;
            					flit_out.simdata.ctime = sc_time_stamp();
            					flit_out.vcid = vc_next_id;

            					ocPort_out[i].write(flit_out);

            					if(LOG >= 2)
            						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<"Controller says  Transmitting estimate flit to output port: "<<i<<flit_out;


            					vcRequest.write(false);
            				} // end outReady == true
            				else {
            					if(LOG >= 4)
            						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Ctr: OC cannot accept flit!"<<endl;
            						flit_out.simdata.num_waits++;
            					//	if(flit_out.simdata.num_waits < WAITS)
            							ctrQ.flit_push(flit_out);
            				}
            			}  //end of if Q not empty
            			rs.est_buffer = ctrQ.pntr;*/
        } // end of if switch_cntrl
    }// end of while
} // end transmitest



///////////////////////////////////////////////////////////////////////////
//Process Sesitive to Estimate Inport
//////////////////////////////////////////////////////////////////////////
template<UI num_ip>
void Controller<num_ip>::rcv_estimate()
{
    while (true)
    {
        wait();
        //cout<<"Inpor Event!!"<<endl;
        for (UI i = 0; i < num_ip-1; i++)
        {
            if (estIn[i].event() )//&& r_in[i].free)
            {
                rs.est_buffer++;
                r_in[i].val = estIn[i].read();
                r_in[i].free = false;
                UI dest = r_in[i].val.pkthdr.esthdr.d ;
                UI est = r_in[i].val.pkthdr.esthdr.estimate;
                //		cout<<"Est received: for "<<r_in[i].val.pkthdr.esthdr.d<<" destination and value "<< r_in[i].val.pkthdr.esthdr.estimate<<endl;
                rtable->update_estimate(i, dest , est ,0);
                r_in[i].free = true;
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////
/// Process sensitive to route request
/// Calls routing algorithm
///////////////////////////////////////////////////////////////////////////
template<UI num_ip>
void Controller<num_ip>::allocate_route()
{
    int flag=0;

    while (true)
    {
        wait();
        //	cout<<this->name()<<":(route) "<<sim_count<<endl;


        if (LOG >= 4)
            eventlog<<this->name()<<" Says ";
        for (UI i = 0; i < num_ip; i++)
        {
            if (LOG >= 4)
                eventlog<<" i = "<<i;
            if (rtRequest[i].event() && rtRequest[i].read() == ROUTE)
            {

                sc_uint<ADDR_SIZE> src = sourceAddress[i].read();
                sc_uint<ADDR_SIZE> dest = destRequest[i].read();
//qrt************************************************************
                sc_uint<64> timestamp = timestamp_ip[i].read();
//qrt************************************************************

                UI ip_dir = idToDir(i);


//qrt************************************************************
                UI temp = 0;
                //added for Q-Routing
                if (RT_ALGO == QRT && sim_count < PWAIT-1)
                {
                    if ( DELTA != BETA)
                        QXY_SIMCOUNT = DELTA;
                    DELTA = .8;
                }
                else
                    DELTA = QXY_SIMCOUNT;

                if (RT_ALGO == QRT && sim_count < QXY_SIMCOUNT)
                    temp = 1;
                else if (RT_ALGO == QRT)
                    temp = 0;
                else
                    temp = ip_dir;
                /////////////////////////
//qrt************************************************************
                UI op_dir = rtable->calc_next(temp, src, dest);
#define iprint(value)	" " #value "= "<<value


//qrt************************************************************
                if (LOG == 10)
                    cout<<this->name()<<iprint(num_tiles)<<": "<<iprint(temp)<<iprint(ip_dir)<<iprint(op_dir)<<iprint(dest)<<iprint(src)<<iprint(i)<<iprint(timestamp)<<endl;
                if (RT_ALGO == QRT && !temp)
                    op_dir = idToDir(op_dir);
                if (LOG == 10)
                    cout<<iprint(op_dir)<<endl;
                if (RT_ALGO == QRT && ip_dir != C)
                {

                    sc_uint<64> timestamp = timestamp_ip[i].read();
                    UI estimate = op_dir == C ? 0 : rtable->get_estimate(dest);
                    estimate += (sim_count - timestamp);
                    flit* est_flit = new flit;
                    create_est_flit(estimate,dest,est_flit);
                    //	cout<<"Ctr: i "<<i<<" est value " <<est_flit->pkthdr.esthdr.estimate<<" dest = "<<est_flit->pkthdr.esthdr.estimate<<endl;

                    if (est_flit != NULL)
                    {
                        rs.est_out++;
                        estOut[i].write(*est_flit);
                    }
                    else
                        cout<< "Error: Est_flit not created"<<endl;
                }
//qrt************************************************************
                rtReady[i].write(true);
                nextRt[i].write(op_dir);

            }
            /*if(RT_ALGO == QRT && rtRequest[i].event() && rtRequest[i].read() == ESTUPDATE ) {
            	//num_est_ct_rcv++;//willbe removed
            	sc_uint<ADDR_SIZE> est = sourceAddress[i].read();
            	sc_uint<ADDR_SIZE> dest = destRequest[i].read();
            	sc_uint<64> timestamp = timestamp_ip[i].read();

            	sc_uint<32> estid = est_pktID[i].read();
            	sc_uint<32> estsrc = est_pktsrc[i].read();
            	//updateInfo(estsrc,estid);
            	updateInfo(estsrc,estid);

            	//cout <<"Timestamp removed( pktid:"<< estid <<" Source :"<<estsrc<<")"<<endl;

            	rtable->update_estimate(i, dest , est ,timestamp);
            	//sc_uint<3> next_hop = dirToId(op_dir);
            	//sc_uint<3> next_hop = rtable.calc_next(i, dest);
            	rtReady[i].write(true);
            	//nextRt[i].write(op_dir);
            }*/


            // request from IC to update //////////////////////////
            if (rtRequest[i].event() && rtRequest[i].read() == UPDATE)
            {
                sc_uint<ADDR_SIZE> src = sourceAddress[i].read();
                sc_uint<ADDR_SIZE> dest = destRequest[i].read();
                //rtable.update(dest, i);
                rtReady[i].write(true);
            }
            if (rtRequest[i].event() && rtRequest[i].read() == NONE)
            {
                rtReady[i].write(false);
            }
        }
        if (LOG >= 4)
            eventlog<<endl;
    }// end while
}// end allocate_route

/*template<UI num_ip>
void Controller<num_ip>::timeEst(UI timestamp, UI dir,UI src,UI pktid, UI dest){
	if(pQtop < 100){
	//	cout<< "inside timeEst "<<endl;
		pQ[pQtop].estid = pktid;
		pQ[pQtop].timestamp = timestamp;
		pQ[pQtop].dir = dir;
		pQ[pQtop].dest = dest;
		pQ[pQtop].src = src;
		pQtop++;
	}
}*/


/*template<UI num_ip>
void Controller<num_ip>::late() {
	int i = 0;
	while(i < pQtop)
	{
		if(pQ[i].timestamp < sim_count)
			i++;
		else
			break;
	}
	//pQtop = pQtop - i;
        cout<<"Timereceived = "<<sim_count<<endl;
	for(int j = 0;j < i;j++)
	{
		int dir = pQ[j].dir;
		int dest = pQ[j].dest;
		rtable->update_estimate(dir,dest,penalty,0);
//		cout<<"src = "<<pQ[j].src<<"estid = "<<pQ[j].estid<<"sim_count = "<<sim_count<<endl;
		if(WAITS == 1001)
                    cout<<"     p added: Pktid = "<<pQ[j].estid<<" eT = "<<pQ[j].timestamp<<" amt: "<<penalty<<endl;
	}
	for(int k=0,j = i;j < pQtop;k++,j++){
		pQ[k].estid = pQ[j].estid;
		pQ[k].timestamp = pQ[j].timestamp;
		pQ[k].dir = pQ[j].dir;
		pQ[k].dest = pQ[j].dest;
		pQ[k].src = pQ[j].src;
	}
	pQtop -= i;
//}

template<UI num_ip>
void Controller<num_ip>::updateInfo(UI src,UI pktid) {
	int i = 0;
	while(i < pQtop){
		if((pQ[i].estid == pktid && pQ[i].src == src) )//|| (pktid % 2)
                {
                        if(WAITS == 1001)
                            cout<<"Pktid = "<<pktid<<" TR = "<<sim_count<<" eT = "<<pQ[i].timestamp<<endl;
			break;
                }
		else
			i++;
	}
        if(i == pQtop){
                cout << "Est pkt arrived, Info not avilable , pktID :"<<pktid<<" Src:"<<src<<endl;
		return ;
        }
	while(i < pQtop-1 ){
		pQ[i].estid = pQ[i+1].estid;
		pQ[i].timestamp = pQ[i].timestamp;
		pQ[i].dir = pQ[i+1].dir;
		pQ[i].dest = pQ[i+1].dest;
		pQ[i].src = pQ[i+1].src;
		i++;
	}
	pQtop--;
}*/

//template<UI num_ip>
//flit* Controller<num_ip>::create_est_flit(UI estimate, UI dirtoRoute, UI src, UI d, UI i) {




//qrt************************************************************
template<UI num_ip>
void Controller<num_ip>::create_est_flit(UI estimate,UI d, flit *est_out )   //CODEFLIT
{

//	flit_out->src = tileID;//CODENR
//	flit_out->pkthdr.esthdr.dir = dirtoRoute ;//CODENR

//	flit_out->pkthdr.esthdr.pktID = est_pktID[i].read();//CODEPEN
//	flit_out->pkthdr.esthdr.src = src;//CODEPEN

    est_out->simdata.gtime = sc_time_stamp(); //CODENR
    /*	flit_out->simdata.ctime = sc_time_stamp();//CODENR
    	flit_out->simdata.atime = SC_ZERO_TIME;//CODENR
    	flit_out->simdata.atimestamp = 0;//CODENR
    	flit_out->simdata.num_waits = 0;//CODENR
    	flit_out->simdata.num_sw = 0;*///CODENR
    est_out->pkthdr.esthdr.d = d;
    est_out->pkthdr.esthdr.estimate = estimate;
}



/*template<UI num_ip>
void Controller<num_ip>::store_flit(flit *flit_in) {
	if(ctrQ.full == true) {
		if(LOG >= 2)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Error(QRT): DATA has arrived. ctrQ is full!"<<endl;
                cout<<"Controller Est Dropped Queue Full: estpkt id :"<<flit_in->pkthdr.esthdr.pktID <<"Src :"<<flit_in->pkthdr.esthdr.src<<endl;
        }
	else
	{
		ctrQ.flit_in(*flit_in);
		rs.est_out++;
	}
}*/


//qrt************************************************************

template<UI num_ip>
UI Controller<num_ip>::get_avgest()
{
    ULL total_est = 0;
    int i;
    for (i = 0; i < rtable->num_tiles; i++)
    {
        total_est += rtable->get_estimate(i);
    }

    return (double)total_est/i;

}


///Returns the reverse direction
template<UI num_ip>
UI Controller<num_ip>::reverseDir(UI dir)
{
    if (dir == N)
        return S;
    else if (dir == S)
        return N;
    else if (dir == E)
        return W;
    else if (dir == W)
        return E;
}

//qrt************************************************************




///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
template<UI num_ip>
void Controller<num_ip>::setTileID(UI id, UI port_N, UI port_S, UI port_E, UI port_W)
{
    tileID = id;
    portN = port_N;
    portS = port_S;
    portE = port_E;
    portW = port_W;
    rtable->setID(id);
}


/////////////////////////////////////////////////////////////////////
/// Returns direction (N, S, E, W) corresponding to a given port id
////////////////////////////////////////////////////////////////////
template<UI num_ip>
UI Controller<num_ip>::idToDir(UI port_id)
{
    if (port_id == portN)
        return N;
    else if (port_id == portS)
        return S;
    else if (port_id == portE)
        return E;
    else if (port_id == portW)
        return W;
    return C;
}

/////////////////////////////////////////////////////////////////////
/// Returns id corresponding to a given port direction (N, S, E, W)
////////////////////////////////////////////////////////////////////
template<UI num_ip>
UI Controller<num_ip>::dirToId(UI port_dir)
{
    switch (port_dir)
    {
    case N:
        return portN;
        break;
    case S:
        return portS;
        break;
    case E:
        return portE;
        break;
    case W:
        return portW;
        break;
    case C:
        return num_ip - 1;
        break;
    }
    return num_ip - 1;
}

template struct Controller<NUM_IC>;
template struct Controller<NUM_IC_B>;
template struct Controller<NUM_IC_C>;
