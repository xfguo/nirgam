
/*
 * InputChannel.h
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
/// \file InputChannel.h
/// \brief Defines module InputChannel (reads and processes incoming flits)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_IP_CHANNEL_
#define	_IP_CHANNEL_

#include "fifo.h"
#include "systemc.h"
#include "credit.h"
#include "flit.h"
#include <string>
#include <iostream>
#include <fstream>

/// required for stl
using namespace std;

///////////////////////////////////////////////
/// \brief Virtual channel (VC) data structure
///////////////////////////////////////////////
struct  VC {

	int id;
	
	//sc_uint<VCS_BITSIZE>	id;		
	sc_uint<VCS_BITSIZE+1>	vc_next_id;	
	sc_uint<BUF_BITSIZE>	num_buf;	
	sc_uint<5>		vc_route;	
	fifo			vcQ;		
};



//For creating results log, (Q-Router)
struct RESULTS {
	//if not updated these values are taken
	ULL pkt_in;
	ULL pkt_out;
	UI buffer_size;
	ULL est_in;
	ULL pkt_in_core;
	ULL pkt_dropped;
};
//End results log


//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an Input Channel
///
/// This module defines an Input Channel in a network tile
//////////////////////////////////////////////////////////////////////////
template<UI num_op = NUM_OC>
struct InputChannel : public sc_module {

	// PORTS //////////////////////////////////////////////////////////////////////////////////
	sc_in<bool> switch_cntrl;	///< input clock port
	sc_in<flit> inport;		///< input data/flit port
	sc_out<flit> outport[num_op];	///< ouput data/flit ports (one for each output channel)
	
	sc_in<bool>  outReady[num_op];	///< input ports for ready signal from OCs
	sc_out<bool> vcRequest;		///< output port for sending request to VCA
	sc_in<bool> vcReady;		///< input port for ready signal from VCA
	sc_out<sc_uint<2> >  opRequest;	///< output port for sending OC requested to VCA
	sc_in<sc_uint<VCS_BITSIZE+1> >	nextVCID;	///< input port to recieve next VCID from VCA
	sc_out<creditLine> credit_out[NUM_VCS];		///< output ports to send credit info (buffer status) to OC, VCA and Ctr
	
	sc_out<request_type> rtRequest;			///< output port to send request to Controller
	sc_in<bool> rtReady;				///< input port to recieve ready signal from Controller
	sc_out<sc_uint<ADDR_SIZE> > destRequest;	///< output port to send destination address to Controller
	sc_out<sc_uint<ADDR_SIZE> > sourceAddress;	///< output port to send source address to Controller
	sc_in<sc_uint<5> > nextRt;			///< input port to recieve routing decision (next hop) from Controller
	
	sc_out<sc_uint<2> > notify_GT;;

	// PORTS END //////////////////////////////////////////////////////////////////////////////
	
	//PORTS FOR Q_ROUTER 	
	sc_out<sc_uint<64> > timestamp_ip;
	sc_out<sc_uint<32> > est_pktID;
	//PORTS FOR Q-ROUTER ENDS


	/// Constructor
	SC_CTOR(InputChannel);

	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	void read_flit();		///< reads flit from i/p port and calls function to store it in buffer
	void store_flit_VC(flit*);	///< stores flit in buffer
	void route_flit();		///< routes the flit at the front of fifo buffer
	void routing_src(flit*);	///< routing function for algorithms containing entire path in header (source routing)
	void routing_dst(flit*);	///< routing function for algorithms containing destination address in header

	void transmit_flit();		///< transmits flit at the front of fifo to output port
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID, UI portN, UI portS, UI portE, UI portW);
	void resetCounts();		///< resets buffer counts to zero
	void closeLogs();		///< closes logfiles

	int reverse_route(int);		///< reverses route (to be used in future)
	// FUNCTIONS END /////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	VC	vc[NUM_VCS];	///< Virtual channels
	UI	cntrlID;	///< Control ID to identify channel direction
	UI	tileID;		///< Tile ID
	UI	portN;		///< port number representing North output direction
	UI	portS;		///< port number representing South output direction
	UI	portE;		///< port number representing East output direction
	UI	portW;		///< port number representing West output direction
	UI	numBufReads;	///< number of buffer reads in the channel
	UI	numBufWrites;	///< number of buffer writes in the channel
	UI	numBufsOcc;	///< number of occupied buffers
	UI	numVCOcc;	///< number of occupied virtual channels
	ULL 	sim_count;	///< keeps track of number of clock cycles
	/*added*/
	UI	numBufReadsPower;	///< number of buffer reads in the channel
	UI      numArb;     ///<number of arbitrations>
	UI 	numVCReads;   ///number of status Registers registers read during VC allocation (assuming VC allocation is of type VC Select)
	UI 	numVCWrites;  ///number of status Registers write during VC allocation
	/*end*/

	// VARIABLES END /////////////////////////////////////////////////////////////////////////
	//
	//
	//added for Q-Router stats
	
	RESULTS rs;
};

#endif

