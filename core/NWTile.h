
/*
 * NWTile.h
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
/// \file NWTile.h
/// \brief Defines a network tile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NW_TILE__
#define __NW_TILE__

#include "systemc.h"
#include "../config/constants.h"
#include "flit.h"
#include "credit.h"
#include "InputChannel.h"
#include "OutputChannel.h"
#include "VCAllocator.h"
#include "ipcore.h"
#include "Controller.h"
#include "BaseNWTile.h"

#define ptr (NWTile<NUM_NB, NUM_IC, NUM_OC> *)
#define ptr_b (NWTile<NUM_NB_B, NUM_IC_B, NUM_OC_B> *)
#define ptr_c (NWTile<NUM_NB_C, NUM_IC_C, NUM_OC_C> *)



///////
struct NWTRESULTS {
	ULL total_ic_pktin;
	ULL total_ic_pktout;
	ULL total_ic_estin;
	UI total_ic_buffersize;
	ULL total_ic_pktin_core;	
	ULL total_oc_pktout_core;
	ULL total_pkt_dropped;
	UI total_ctr_estbuffer;
	ULL total_ctr_estout;
	ULL total_time_insim;
	double avg_ctr_estvalue;
};




//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN/////////////////////////////////////

struct temp {

	sc_in<flit>	ip_port[2];	///< input data/flit ports
	sc_out<flit> op_port[2];	///< output data/flit ports
	};
	
///////////////////////////NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN/////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a tile in NoC
///
/// This module defines a network tile and submodules within it.
/// It is derived from abstract class BaseNWTile.
/// Template parameters:
/// - num_nb: Number of neighbors
/// - num_ic: Number of input channels
/// - num_oc: Number of output channels
//////////////////////////////////////////////////////////////////////////
template <int num_nb = NUM_NB, int num_ic = NUM_IC, int num_oc = NUM_OC>
struct NWTile : public BaseNWTile {

	// PORTS ////////////////////////////////////////////////////////////////////////////////////
	sc_in<bool>	switch_cntrl;		///< input clock port
	sc_in<flit>	ip_port[num_nb];	///< input data/flit ports
	sc_out<flit> op_port[num_nb];	///< output data/flit ports

	sc_in<creditLine>	credit_in[num_nb][NUM_VCS];	///< input ports for credit line (buffer status)
	sc_out<creditLine>	credit_out[num_nb][NUM_VCS];	///< output ports for credit line (buffer status)
	// PORTS END ////////////////////////////////////////////////////////////////////////////////

	sc_in<flit> estIn_port[num_nb]; //CODELINE
	sc_out<flit> estOut_port[num_nb];//CODELINE
	/*added*/
	sc_in<powerCreditLine>	p_credit_in[num_nb];	///< input ports for credit line (buffer status)
	sc_out<powerCreditLine>	p_credit_out[num_nb];	///< output ports for credit line (buffer status)
	/*end*/

	/// Constructor
	// Parameter - module name, tile id.
	SC_HAS_PROCESS(NWTile);
	NWTile(sc_module_name NWTile, UI tileID);

	// PROCESSES //////////////////////////////////////////////////////////////////////////////////////////
	void entry();		///< Writes buffer utilization information at the tile, at each clock cycle
	void setID(UI);		///< sets unique tile id and associates ports with directions
	void closeLogs();	///< closes log files at the end of simulation
	float return_latency(int port_dir);		///< returns average latency per packet for a channel
	float return_latency_flit(int port_dir);	///< returns average latency per flit for a channel
	float return_avg_tput(int port_dir);		///< returns average throughput for a channel
	ULL return_total_latency();			///< returns total latency across all channels in the tile
	ULL return_total_flits();			///< returns total flits across all channels in the tile
	int getportid(int port_dir);			///< returns id corresponding to a port direction (N, S, E, W)
	ULL return_total_packets();
	void calc_results();



/****************************************************/
	int return_total_latency_GT();			///< returns total latency across all channels in the tile for GT traffic
	int return_total_latency_BE();			///< returns total latency across all channels in the tile for BE traffic

	int return_total_flits_GT();			///< returns total flits across all channels in the tile for GT traffic 
	int return_total_flits_BE();			///< returns total flits across all channels in the tile for BE traffic

	//added
	void update_power_info();
	/*added*/
	UI return_buf_read();
	UI return_buf_write();
	UI return_vc_reads();
	UI return_vc_writes();	
	//arb
	UI return_n_arb();
	UI return_link_trav();
	UI return_xbar_trav();
	/*end*/


        ULL cal_no_of_pkts_per_tile();


        ULL cal_no_of_pktsGT_per_tile();	
	
	ULL cal_no_of_pktsBE_per_tile();	
	
        ULL cal_latency_per_tile();
	
	ULL cal_latencyGT_per_tile();
        ULL cal_latencyBE_per_tile();
/****************************************************/




// PROCESS END /////////////////////////////////////////////////////////////////////////////////////

	// SUBMODULES /////////////////////////////////////////////////////////////////
	InputChannel<num_oc>	*Ichannel[num_ic];	///< Input channels
	OutputChannel<num_ic>	*Ochannel[num_oc];	///< Output channels
	VCAllocator<num_ic> 	vcAlloc;		///< Virtual Channel Allocator
	ipcore 			*ip;			///< IP Core
	Controller<num_ic>	ctr;			///< Controller

	// SUBMODULES END //////////////////////////////////////////////////////////////////////////////////

	// SIGNALS ///////////////////////////////////////////////////////////////////////////////////
	/// signals to connect data outport of ICs to the data inport of the OCs
	sc_signal<flit>	flit_sig[num_ic][num_oc];
	/// data line from ipcore to input channel
	sc_signal<flit>	flit_CS_IC;
	/// data line from output channel to ipcore
	sc_signal<flit> flit_OC_CR;

	/// ready signals from ICs to OCs of neighboring tiles
	sc_signal<bool>	rdy[num_ic][num_oc];

	/// Request signal for virtual channel allocation from IC to VCA
	sc_signal<bool> vcReq[num_ic];
	/// Output port requested from IC to VCA
	sc_signal<sc_uint<2> >	opReq[num_ic];

	sc_signal<sc_uint<2> >	vc_type;
	
	/// Ready signal from VCA to IC
	sc_signal<bool> vcReady[num_ic];
	/// Virtual channel id allocated from VCA to IC
	sc_signal<sc_uint<VCS_BITSIZE+1> >	nextvc[num_ic];

	/// credit line from core channel to VCA and ipcore
	sc_signal<creditLine>	creditIC_CS[NUM_VCS];
	//sc_signal<creditLine>	creditCR_OC[NUM_VCS];

	/// Routing request signal from IC to Ctr
	sc_signal<request_type> rtReq[num_ic];
	/// Destination address from IC to Ctr
	sc_signal<sc_uint<ADDR_SIZE> > destReq[num_ic];
	/// Source address from IC to Ctr
	sc_signal<sc_uint<ADDR_SIZE> > srcAddr[num_ic];
	/// EST signal for Q Router
	sc_signal<sc_uint<64> > tstamp[num_ic];
	sc_signal<sc_uint<32> > eid[num_ic];

	/// Ready signal from Ctr to IC
	sc_signal<bool> rtReady[num_ic];
	/// Route (output port) signal from Ctr to IC
	sc_signal<sc_uint<5> > nextRt[num_ic];
	// SIGNALS END ///////////////////////////////////////////////////////////////////////////////////////////////////
	
	NWTRESULTS rs;

};

#endif
