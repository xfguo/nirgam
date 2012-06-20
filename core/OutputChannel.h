
/*
 * OutputChannel.h
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
/// \file OutputChannel.h
/// \brief Defines module OutputChannel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_OP_CHANNEL_
#define	_OP_CHANNEL_

#include "systemc.h"
#include "switch_reg.h"
#include "flit.h"
#include "credit.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an Output Channel
///
/// This module defines an Output Channel in a network tile
//////////////////////////////////////////////////////////////////////////
template<UI num_ip = NUM_IC>
struct OutputChannel : public sc_module {
	
	// PORTS ////////////////////////////////////////////////////////////////////////////////////
	sc_in<flit> inport[num_ip];		///< input data/flit ports (one for each IC)
	sc_out<bool> inReady[num_ip];		///< output port to send ready signal to IC
	sc_in<bool>  switch_cntrl;		///< clock input port
	sc_out<flit> outport;			///< output data/flit port
	sc_in<creditLine> credit_in[NUM_VCS];	///< input port to recieve credit info (buffer status) from ICs of neighbor tiles
	// PORTS END ////////////////////////////////////////////////////////////////////////////////////
	
	/// Constructor
	SC_CTOR(OutputChannel);
	
	// PROCESSES ///////////////////////////////////////////////////////////////////////////////////////////////
	void entry();			///< reads and processes incoming flit
	void closeLogs();		///< closes logfiles at the end of simulation and computes performance stats
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID, UI portN, UI portS, UI portE, UI portW);
	// PROCESSES END //////////////////////////////////////////////////////////////////////////////////////////
	///////TO BE REMOVED

	ULL cal_packets_per_oc();
	ULL cal_latency_per_oc();
	
//qrt*************************************************************************************************
	int num_est_oc_rcv;
	int num_est_oc_snt;
//qrt*************************************************************************************************


	// VARIABLES //////////////////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< unique tile ID
	UI cntrlID;	///< control ID to identify channel direction (N, S, E, W, C)
	UI portN;	///< port number representing North direction
	UI portS;	///< port number representing South direction
	UI portE;	///< port number representing East direction
	UI portW;	///< port number representing West direction

	switch_reg	r_in[num_ip];	///< registers to store flit from inport
	switch_reg	r_vc[NUM_VCS];	///< registers, one for each next VCID

	ULL latency;			///< total latency
	ULL num_pkts;			///< total number of packets
	ULL num_flits;			///< total number of flits
	




/*****************************************************************************************/
	
      




        ULL latency_GT;			///< total latency of GT traffic
	ULL num_pkts_GT;			///< total number of GT packets
	ULL num_flits_GT;			///< total number of GT flits
	
        ULL latency_BE;			///< total latency  of BE traffic
	ULL num_pkts_BE;			///< total number of BE packets
	ULL num_flits_BE;			///< total number of BE flits

/*****************************************************************************************/


	ULL num_estpkts;			///< total number of estimate packets
	ULL input_time;			///< generation timestamp of head flit of a packet



	float avg_latency;		///< average latency (in clock cycles) per packet
	float avg_latency_flit;		///< average latency (in clock cycles) per flit
	float avg_estthroughput;	///< average estimate pkt latency
	float avg_throughput;		///< average throughput (in Gbps)

/*****************************************************************************************/

        float avg_latency_GT;		///< average latency (in clock cycles) per packet
	float avg_latency_flit_GT;		///< average latency (in clock cycles) per flit
	float avg_throughput_GT;		///< average throughput (in Gbps)


        float avg_latency_BE;		///< average latency (in clock cycles) per packet
	float avg_latency_flit_BE;		///< average latency (in clock cycles) per flit
	float avg_throughput_BE;		///< average throughput (in Gbps)





/*****************************************************************************************/	

	
	int beg_cycle;			///< clock cycle in which first flit is recieved in the channel
	int end_cycle;			///< clock cycle in which last flit leaves the channel
	int total_cycles;		///< total number of clock cycles

	//Q-Routing 
	UI num_estflits;
	UI num_rin_est;
	float avg_estthrougput;

	float return_avg_latency();

		//for Q-Router results
	ULL num_pkts_core;
	ULL time_insim;
		/////////////
	//End Q-Routing
	/*added*/
	UI numXbarTrav;        ///<num of xbar traversals
	UI numLinkTrav;     ///num of link traversal
	/*end*/
	// VARIABLES END //////////////////////////////////////////////////////////////////////////////////////////
};
#endif
