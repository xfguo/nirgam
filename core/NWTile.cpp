/*
 * NWTile.cpp
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file NWTile.cpp
/// \brief Creates a network tile.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "systemc.h"
#include "NWTile.h"
#include <string>
#include <fstream>
#include "../config/extern.h"

/// array to store library name of application attached to ipcore on each tile
extern string app_libname[MAX_NUM_TILES];


extern string app_libname2[MAX_NUM_TILES];

//////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor to create a network tile.
/// Creates and connects the following submodules:
/// - Input Channels
/// - Output Channels
/// - Virtual Channel Allocator
/// - IP Core
/// - Controller
/// .
/// Template parameters:
/// - num_nb: Number of neighbors
/// - num_ic: Number of input channels
/// - num_oc: Number of output channels
/// Parameters to constructor:
/// - module name
/// - unique tile ID
/////////////////////////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
NWTile<num_nb, num_ic, num_oc>::NWTile(sc_module_name NWTile, UI id): BaseNWTile(NWTile, id), vcAlloc("VA"),ctr("Controller")
{

    /*added*/
    //initializing performance parameters
    prev_buf_read=0;
    prev_buf_write=0;
    prev_xbar_trav=0;
    prev_vc_reads=0;
    prev_vc_writes=0;
    prev_n_arb=0;
    /*end*/
    max_latency_GT=0;
    max_latency_BE=0;



    for (UI i = 0; i < num_ic; i++)
    {
        char name[4];
        sprintf(name, "IC%d", i);
        Ichannel[i] = new InputChannel<num_oc>(name);	// instantiate ICs
        sprintf(name, "OC%d", i);
        Ochannel[i] = new OutputChannel<num_ic>(name);	// instantiate OCs
    }

//	cout << "num_nb: " <<num_nb <<" num_oc: "<<num_oc<<" num_ic: "<<num_ic<<" tileID "<<id<<endl;

    // attach requested library to ipcore
    if (app_libname[id] == "NULL")
    {

//		ip = NULL; modified
        void *hndl;
        void *mkr;
        string libname = string("./application/lib/Sink.so");
        hndl = dlopen(libname.c_str(), RTLD_NOW);
        if (hndl == NULL)
        {
            cerr << dlerror() << endl;
            exit(-1);
        }
        mkr = dlsym(hndl, "maker");
        ip = ((ipcore*(*)())(mkr))();
        cout<<"Attaching application Sink.so"<<endl;
    }
    else
    {
        void *hndl;
        void *mkr;
        string libname = string("./application/lib/");
        libname = libname + app_libname[id];
        hndl = dlopen(libname.c_str(), RTLD_NOW);
        if (hndl == NULL)
        {
            cerr << dlerror() << endl;
            exit(-1);
        }
        mkr = dlsym(hndl, "maker");
        ip = ((ipcore*(*)())(mkr))();
        cout<<"Attaching application "<<app_libname[id]<<endl;
    }

    // set tile ID and map port numbers to directions
    setID(id);

    // Process sensitive to clock (writes buf utilization info at each clock)
    SC_THREAD(entry);
    sensitive_pos << switch_cntrl;

    // Interconnections between submodules //////////////////////////////////////////////

    // VCA //////////////////////////////////////////////////////////////////////////////
    vcAlloc.switch_cntrl(*nw_clock);	// clock

    //printf("Config vcAlloc starts\n");
    for (UI i = 0; i < num_ic; i++)
    {
        vcAlloc.vcRequest[i](vcReq[i]);	// VC Request from IC
        vcAlloc.opRequest[i](opReq[i]);	// Request for OC from IC
        vcAlloc.nextVCID[i](nextvc[i]);	// next VCID from VCA to IC
        vcAlloc.vcReady[i](vcReady[i]);	// vcReady signal to IC
        // credit info from IC
        for (UI j = 0; j < NUM_VCS; j++)
        {
            if (i == num_ic - 1)	// Core channel
                vcAlloc.Icredit[i][j](creditIC_CS[j]);
            else
                vcAlloc.Icredit[i][j](credit_in[i][j]);
        }



        vcAlloc.detect_GT[i](vc_type);


    }


    /*		vcAlloc.vcRequest[num_ic](vcReq[num_ic]);	// VC Request from CTR
    		vcAlloc.opRequest[num_ic](opReq[num_ic]);	// Request for OC from CTR
    		vcAlloc.nextVCID[num_ic](nextvc[num_ic]);	// next VCID from VCA to CTR
    		vcAlloc.vcReady[num_ic](vcReady[num_ic]);	// vcReady signal to CTR*/

    // END VCA //////////////////////////////////////////////////////////////////////////

//printf("Config vcAlloc ends\n");
//printf("Config IC starts\n");
    // IC ///////////////////////////////////////////////////////////////////////////////
    for (UI i = 0; i < num_ic; i++)
    {
        Ichannel[i]->switch_cntrl(*nw_clock);	// clock

        switch (TOPO)
        {

        case TORUS:
            Ichannel[i]->cntrlID = i;
            break;

        case MESH:
            if (i == portN)
                Ichannel[i]->cntrlID = N;
            else if (i == portS)
                Ichannel[i]->cntrlID = S;
            else if (i == portE)
                Ichannel[i]->cntrlID = E;
            else if (i == portW)
                Ichannel[i]->cntrlID = W;
            else if (i == num_ic - 1)
                Ichannel[i]->cntrlID = C;
            break;

        }

        if (i == num_ic - 1)
        {
            Ichannel[i]->inport(flit_CS_IC);	// data input port of core IC

        }
        else
            Ichannel[i]->inport(ip_port[i]);	// data input port of IC

        for (UI j = 0; j < num_oc; j++)
        {
            Ichannel[i]->outport[j](flit_sig[i][j]); // data output port of IC
            Ichannel[i]->outReady[j](rdy[i][j]);	// ready signal form OC
        }
        Ichannel[i]->vcRequest(vcReq[i]);	// vcRequest to VC
        Ichannel[i]->opRequest(opReq[i]);	// request for o/p to VC
        Ichannel[i]->nextVCID(nextvc[i]);	// next VCID from VC
        Ichannel[i]->vcReady(vcReady[i]);	// ready signal from VC
        Ichannel[i]->rtRequest(rtReq[i]);	// route request to Ctr
        Ichannel[i]->destRequest(destReq[i]);	// dest to Ctr
        Ichannel[i]->sourceAddress(srcAddr[i]);	// source address to Ctr

        ///time stamp signal from Q Router
        Ichannel[i]->timestamp_ip(tstamp[i]);
        Ichannel[i]->est_pktID(eid[i]);//CODE999
//		Ichannel[i]->est_pktsrc(sid[i]);//CODE999
        Ichannel[i]->rtReady(rtReady[i]);	// ready signal from Ctr
        Ichannel[i]->nextRt(nextRt[i]);		// next hop from Ctr

        Ichannel[i]->notify_GT(vc_type);


        // send credit info to OC and VCA
        for (UI j = 0; j < NUM_VCS; j++)
        {
            if (i == num_ic - 1)
                Ichannel[i]->credit_out[j](creditIC_CS[j]);	// output credit info to ipcore
            else
                Ichannel[i]->credit_out[j](credit_out[i][j]);	// output credit info to neighbor tiles
        }
    }
    // END IC ////////////////////////////////////////////////////////////////////////////////////

    /* example to generate vcdtrace, commented */
    /*	if(id == 0) {
    		sc_trace(tracefile, Ichannel[num_ic - 1]->inport, "flitin");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.pntr, "size");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[0], "reg0");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[1], "reg1");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[2], "reg2");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[3], "reg3");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[4], "reg4");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[5], "reg5");
    		sc_trace(tracefile, Ichannel[num_ic - 1]->vc[0].vcQ.regs[6], "reg6");
    	}
    */
//printf("Config IC ends\n");
//printf("Config OC starts\n");
    // OC ////////////////////////////////////////////////////////////////////////////////////////
    for (UI i = 0; i < num_oc; i++)
    {
        Ochannel[i]->switch_cntrl(*nw_clock);	// clock

        switch (TOPO)
        {

        case TORUS:
            Ochannel[i]->cntrlID = i;
            break;

        case MESH:
            if (i == portN)
                Ochannel[i]->cntrlID = N;
            else if (i == portS)
                Ochannel[i]->cntrlID = S;
            else if (i == portE)
                Ochannel[i]->cntrlID = E;
            else if (i == portW)
                Ochannel[i]->cntrlID = W;
            else if (i == num_oc - 1)
                Ochannel[i]->cntrlID = C;
            break;
        }
        for (UI j = 0; j < num_ic; j++)
        {
            Ochannel[i]->inport[j](flit_sig[j][i]);	// data (flit) input from IC
            Ochannel[i]->inReady[j](rdy[j][i]);	// ready signal to IC
        }
        if (i == num_oc - 1)
            Ochannel[i]->outport(flit_OC_CR);	// data (flit) output to core
        else
            Ochannel[i]->outport(op_port[i]);	// data (flit) output to output port (connects to neighbor tile)
        // credit info from IC
        for (UI j = 0; j < NUM_VCS; j++)
        {
            if (i == num_oc - 1)
                Ochannel[i]->credit_in[j](creditIC_CS[j]);	// credit info from core IC
            else
                Ochannel[i]->credit_in[j](credit_in[i][j]);	// credit info from IC
        }
    }
    // END OC ///////////////////////////////////////////////////////////////////////////////////////////
//printf("Config OC ends\n");

    // IPcore ///////////////////////////////////////////////////////////////////////////////////////////
    //if(app_libname[id] != "NULL") {
    ip->clock(*nw_clock);

    // data (flit) input from core OC
    ip->flit_inport(flit_OC_CR);

    // data (flit) output to core IC
    ip->flit_outport(flit_CS_IC);

    // credit info from core IC
    for (int i = 0; i < NUM_VCS; i++)
        ip->credit_in[i](creditIC_CS[i]);
    //}
    // END IPcore /////////////////////////////////////////////////////////////////////////////////////

    // Controller ///////////////////////////////////////////////////////////////////////////////////////
    // clock
    ctr.switch_cntrl(*nw_clock);

//    printf("Config ctr starts\n");
    for (UI i = 0; i < num_ic; i++)
    {
        ctr.rtRequest[i](rtReq[i]);		// route request from IC
        ctr.destRequest[i](destReq[i]);		// dest address from IC
        ctr.sourceAddress[i](srcAddr[i]);	// source address from IC
        ctr.rtReady[i](rtReady[i]);		// ready signal to IC
        ctr.nextRt[i](nextRt[i]);		// next hop to IC
        //Q-Router time stamp signal
        ctr.timestamp_ip[i](tstamp[i]);
        ctr.est_pktID[i](eid[i]); //CODE999
//		ctr.est_pktsrc[i](sid[i]); //CODE999
        // credit info from IC
        for (UI j = 0; j < NUM_VCS; j++)
        {
            if (i == num_ic - 1)
                ctr.Icredit[i][j](creditIC_CS[j]);	// credit info from core IC
            else
                ctr.Icredit[i][j](credit_in[i][j]);	// credit info from IC
        }
    }
    /*added*/
    for (UI i = 0; i < num_nb; i++)
    {
        ctr.power_credit_in[i](p_credit_in[i]);
        ctr.power_credit_out[i](p_credit_out[i]);
    }
    /*end*/
    for (UI i = 0; i < num_nb; i++)
    {
        ctr.estOut[i](estOut_port[i]);
        ctr.estIn[i](estIn_port[i]);
    }
    ///////////ADDED FOR QROUTER
//printf("Config ctr ends--- i have no idea where the problem is\n");
//	ctr.vcRequest(vcReq[num_ic]);	// vcRequest to VC
//	ctr.opRequest(opReq[num_ic]);	// request for o/p to VC
//	ctr.nextVCID(nextvc[num_ic]);	// next VCID from VC
//	ctr.vcReady(vcReady[num_ic]);	// ready signal from VC

//	for(UI j = 0; j < num_ic; j++) {
//		ctr.ocPort_out[j](flit_sig[num_ic][j]); // data output port of IC
//		ctr.ocReady_in[j](rdy[num_ic][j]);	// ready signal form OC
//	}
    /////////END OF Q-ROUTER

    // END Controller ///////////////////////////////////////////////////////////////////////////////////
    //sc_trace(tracefile, rtReady[0], "buf_reg");
} // end constructor

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock.
/// Writes buffer utilization info at each clock
///////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
void NWTile<num_nb, num_ic, num_oc>::entry()
{
    while (true)
    {
        wait();

        //writing out instantaneous buffer utilization
        long totBufReads = 0;
        long totBufWrites = 0;
        long totBufsOcc = 0;
        long totVCOcc = 0;
        for (int i = 0; i < num_ic; i++)
        {
            totBufReads += Ichannel[i]->numBufReads;
            totBufWrites += Ichannel[i]->numBufWrites;
            totBufsOcc += Ichannel[i]->numBufsOcc;
            totVCOcc += Ichannel[i]->numVCOcc;
        }

        //float bufUtil = ((float)totBufsOcc)/(NUM_VCS*NUM_BUFS*num_ic);

        //float vcUtil = ((float)totVCOcc)/(NUM_VCS * num_ic);

        if (LOG >= 3)
        {
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" NumBufReads = "<<totBufReads<<" NumBufWrites = "<<totBufWrites<<" NumBufOccupied = "<<totBufsOcc;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
/// - set unique tile ID
/// - map port number to port direction
//////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
void NWTile<num_nb, num_ic, num_oc>::setID(UI id)
{
    tileID = id;



    //cout<<"\nin NWtile.cpp";
    /***********************/
    if (app_libname[id]=="Bursty.so")
        priority=1;
    else if (app_libname[id]=="CBR.so")
        priority=0;
    else if (app_libname[id]=="Multimedia.so")
        priority=1;
    else if (app_libname[id]=="Trace_traffic.so")

    {
        if (app_libname2[id]=="Bursty.so")
            priority=1;
        else if (app_libname2[id]=="CBR.so")
            priority=0;
        else
            priority=20;
    }
    else
        priority=20;
    /**********************/
    //  cout<<" P"<<priority<<endl;


    switch (TOPO)
    {

    case TORUS:
        portN = 0;
        portS = 1;
        portE = 2;
        portW = 3;
        break;

    case MESH:
        // South border
        if (borderS(id))
        {
            portS = ND;
        }
        else if (borderN(id))
        {
            portS = 0;
        }
        else portS = 1;

        // East border
        if (borderE(id))
        {
            portE = ND;
        }
        else if (borderN(id) || borderS(id))
        {
            portE = 1;
        }
        else portE = 2;

        //West border
        if (borderW(id))
        {
            portW = ND;
        }
        else if (corner(id))
        {
            portW = 1;
        }
        else if (border(id))
        {
            portW = 2;
        }
        else portW = 3;

        // North border
        if (borderN(id))
        {
            portN = ND;
        }
        else portN = 0;
        break;
    }

    // set id and port mapping for ICs
    for (int i = 0; i < num_ic; i++)
        Ichannel[i]->setTileID(id, portN, portS, portE, portW);

    // set id and port mapping for OCs
    for (int i = 0; i < num_oc; i++)
        Ochannel[i]->setTileID(id, portN, portS, portE, portW);

    // set id and port mapping for VCA
    vcAlloc.setTileID(id, portN, portS, portE, portW);

    // set id for IP core
    if (app_libname[id] != "NULL")

//******************************************************************
        ip->setID(id,priority);
//*****************************************************************

    // set id and port mapping for ctr
    ctr.setTileID(id, portN, portS, portE, portW);
}

template <int num_nb, int num_ic, int num_oc>
void NWTile<num_nb, num_ic, num_oc>::calc_results()
{
    //results IC
    rs.total_ic_pktin = 0;
    rs.total_ic_pktout = 0;
    rs.total_ic_estin = 0;
    rs.total_ic_buffersize = 0;
    rs.total_ic_pktin_core = 0;
    rs.total_oc_pktout_core = 0;
    rs.total_pkt_dropped = 0;
    rs.total_time_insim = 0;
    //end
    for (int i = 0 ; i < num_ic ; i++)
    {
        rs.total_ic_pktin += Ichannel[i]->rs.pkt_in;
        rs.total_ic_estin += Ichannel[i]->rs.est_in;
        rs.total_ic_buffersize += Ichannel[i]->rs.buffer_size;
        rs.total_ic_pktout += Ichannel[i]->rs.pkt_out;

        rs.total_ic_pktin_core += Ichannel[i]->rs.pkt_in_core;
        rs.total_pkt_dropped += Ichannel[i]->rs.pkt_dropped;

        rs.total_oc_pktout_core += Ochannel[i]->num_pkts_core;
        rs.total_time_insim += Ochannel[i]->time_insim;
    }
    if (RT_ALGO == QRT)
    {
        rs.total_ctr_estout = ctr.rs.est_out;
        rs.total_ctr_estbuffer = ctr.rs.est_buffer;
        rs.avg_ctr_estvalue = ctr.get_avgest();
    }
}

//////////////////////////////////////////////////////////
/// Closes log files at the end of simulation
/////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
void NWTile<num_nb, num_ic, num_oc>::closeLogs()
{
    for (int i = 0; i < num_ic; i++)
        Ichannel[i]->closeLogs();
    for (int i = 0; i < num_oc; i++)
        Ochannel[i]->closeLogs();
}

/////////////////////////////////////////////////////////////////
/// Returns average latency per packet across a given channel
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
float NWTile<num_nb, num_ic, num_oc>::return_latency(int port_dir)
{
    return Ochannel[getportid(port_dir)]->avg_latency;
}

/////////////////////////////////////////////////////////////////
/// Returns average throughput across a given channel
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
float NWTile<num_nb, num_ic, num_oc>::return_avg_tput(int port_dir)
{
    return Ochannel[getportid(port_dir)]->avg_throughput;
}

/////////////////////////////////////////////////////////////////
/// Returns average latency per flit across a given channel
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
float NWTile<num_nb, num_ic, num_oc>::return_latency_flit(int port_dir)
{
    return Ochannel[getportid(port_dir)]->avg_latency_flit;
}

/////////////////////////////////////////////////////////////////
/// Returns total latency across all channels in the tile
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::return_total_latency()
{
    ULL total_latency = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_latency += Ochannel[i]->latency;
    }
    //cout<<this->name()<<": "<<total_latency<<endl;
    return total_latency;
}



//qrt*************************************************************************************************
/////////////////////////
template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::return_total_packets()
{
    ULL total_packets = 0;
//	cout << "num_nb: " <<num_nb <<" num_oc: "<<num_oc<<" num_ic: "<<num_ic<<" tileID "<<tileID<<endl;
    for (int i = 0; i < num_oc; i++)
    {
        total_packets += Ochannel[i]->num_pkts;
    }
    return total_packets;
}
////////////////////////
//qrt*************************************************************************************************




/*********************************************************************************************/
template <int num_nb, int num_ic, int num_oc>
int NWTile<num_nb, num_ic, num_oc>::return_total_latency_GT()
{
    int total_latency_GT = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_latency_GT += Ochannel[i]->latency_GT;
        /*********************************************************************************************/
        if (max_latency_GT<Ochannel[i]->latency_GT)
            max_latency_GT=Ochannel[i]->latency_GT;
        /*********************************************************************************************/
    }
    return total_latency_GT;
}
/*********************************************************************************************/




/*********************************************************************************************/
template <int num_nb, int num_ic, int num_oc>
int NWTile<num_nb, num_ic, num_oc>::return_total_latency_BE()
{
    int total_latency_BE = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_latency_BE += Ochannel[i]->latency_BE;
        /*********************************************************************************************/
//              if(max_latency_BE<Ochannel[i]->latency_BE)
        //                  max_latency_BE=Ochannel[i]->latency_BE;
        /*********************************************************************************************/
    }
    return total_latency_BE;
}
/*********************************************************************************************/



/////////////////////////////////////////////////////////////////
/// Returns total number of flits across all channels in the tile
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::return_total_flits()
{
    int total_flits = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_flits += Ochannel[i]->num_flits;
    }
    return total_flits;
}

/*******************************************************************************************/

template <int num_nb, int num_ic, int num_oc>
int NWTile<num_nb, num_ic, num_oc>::return_total_flits_GT()
{
    int total_flits_GT = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_flits_GT += Ochannel[i]->num_flits_GT;
    }
    return total_flits_GT;
}
/*******************************************************************************************/


/*******************************************************************************************/
template <int num_nb, int num_ic, int num_oc>
int NWTile<num_nb, num_ic, num_oc>::return_total_flits_BE()
{
    int total_flits_BE = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_flits_BE += Ochannel[i]->num_flits_BE;
    }
    return total_flits_BE;
}

/*******************************************************************************************/



/*******************************************************************************************/

template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_no_of_pkts_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->cal_packets_per_oc();

    return temp;
}


/********************************************************************************************/



/*******************************************************************************************/

template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_no_of_pktsGT_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->num_pkts_GT;

    return temp;
}


template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_no_of_pktsBE_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->num_pkts_BE;

    return temp;
}

/********************************************************************************************/







/*******************************************************************************************/

template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_latency_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->cal_latency_per_oc();

    return temp;
}


template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_latencyGT_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->latency_GT;

    return temp;
}

template <int num_nb, int num_ic, int num_oc>
ULL NWTile<num_nb, num_ic, num_oc>::cal_latencyBE_per_tile()
{
    ULL temp=0;
    for (int i = 0; i < num_oc; i++)
        temp+= Ochannel[i]->latency_BE;

    return temp;
}


/********************************************************************************************/



/*added*/
/////////////////////////////////////////////////////////////////
/// Returns total number of buf reads across all channels in the tile
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_buf_read()
{
    UI total_buf_reads = 0;
    for (int i = 0; i < num_ic; i++)
    {
        total_buf_reads += Ichannel[i]->numBufReadsPower;
    }
    return total_buf_reads;
}

/////////////////////////////////////////////////////////////////
/// Returns total number of buf writes across all channels in the tile
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_buf_write()
{
    UI total_buf_writes = 0;
    for (int i = 0; i < num_ic; i++)
    {
        total_buf_writes += Ichannel[i]->numBufWrites;
    }
    return total_buf_writes;
}
/////////////////////////////////////////////////////////////////
/// Returns total number of xbar traversal
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_xbar_trav()
{
    UI total_xbar_trav = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_xbar_trav += Ochannel[i]->numXbarTrav;
    }
    return total_xbar_trav;
}

/////////////////////////////////////////////////////////////////
/// Returns total number of link traversal
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_link_trav()
{
    UI total_link_trav = 0;
    for (int i = 0; i < num_oc; i++)
    {
        total_link_trav += Ochannel[i]->numLinkTrav;
    }
    return total_link_trav;
}

/////////////////////////////////////////////////////////////////
/// Returns total number of reads of the status registers during vc allocation
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_vc_reads()
{
    UI total_vc_reads = 0;
    for (int i = 0; i < num_ic; i++)
    {
        total_vc_reads += Ichannel[i]->numVCReads;
    }
    return total_vc_reads;
}
/////////////////////////////////////////////////////////////////
/// Returns total number writes of the status registers during vc allocation
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_vc_writes()
{
    UI total_vc_writes = 0;
    for (int i = 0; i < num_ic; i++)
    {
        total_vc_writes += Ichannel[i]->numVCWrites;
    }
    return total_vc_writes;
}

/////////////////////////////////////////////////////////////////
/// Returns total number of arbitrations
////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
UI NWTile<num_nb, num_ic, num_oc>::return_n_arb()
{
    UI total_n_arb = 0;
    for (int i = 0; i < num_ic; i++)
    {
        total_n_arb += Ichannel[i]->numArb;
    }
    return total_n_arb;
}

template <int num_nb, int num_ic, int num_oc>
void NWTile<num_nb, num_ic, num_oc>::update_power_info()
{
    ctr.P=Pavg;
}

/*end*/








/////////////////////////////////////////////////////////////////////
/// Returns id corresponding to a given port direction (N, S, E, W)
////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
int NWTile<num_nb, num_ic, num_oc>::getportid(int port_dir)
{
    switch (port_dir)
    {
    case N:
        return portN;
    case S:
        return portS;
    case E:
        return portE;
    case W:
        return portW;
    }
    return 0;
}

template struct NWTile<NUM_NB, NUM_IC, NUM_OC>;
template struct NWTile<NUM_NB_B, NUM_IC_B, NUM_OC_B>;
template struct NWTile<NUM_NB_C, NUM_IC_C, NUM_OC_C>;
