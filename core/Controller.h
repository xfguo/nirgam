
/*
 * Controller.h
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
/// \file Controller.h
/// \brief Defines Controller module that implements routing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CONTROLLER_
#define _CONTROLLER_

#include "systemc.h"
#include "credit.h"
#include "../config/constants.h"
#include "router.h"

#include "fifo.h"
#include "flit.h"
#include "switch_reg.h"

#include <string>
#include <fstream>
#include <iostream>
#include <dlfcn.h>

using namespace std;

struct CTRRESULTS {
	ULL est_out;
	UI est_buffer;
	ULL est_value;
};
struct PENALTY{
	ULL timestamp;
	UI dir;
	UI dest;
	UI estid;
	UI src;

};
#define penalty 60
//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a Controller
///
/// This module defines a Controller (implements router)
//////////////////////////////////////////////////////////////////////////
template<UI num_ip = NUM_IC>
struct Controller : public sc_module {

	// PORTS ////////////////////////////////////////////////////////////////////////////////
	sc_in<bool> switch_cntrl;		///< input clock port
	sc_in<request_type> rtRequest[num_ip];	///< input ports to recieve route request from ICs
	sc_in<sc_uint<ADDR_SIZE> > destRequest[num_ip];	///< input ports to recieve destination address
	sc_in<sc_uint<ADDR_SIZE> > sourceAddress[num_ip]; ///< input ports to recieve source address
	sc_in<creditLine> Icredit[num_ip][NUM_VCS];	///< input ports to recieve credit info (buffer info) from ICs
	sc_out<bool> rtReady[num_ip];		///< output ports to send ready signal to ICs
	sc_out<sc_uint<5> > nextRt[num_ip];	///< output ports to send routing decision to ICs
	//added
	sc_out<powerCreditLine> power_credit_out[num_ip-1];//o/p ports to send credit info(power values)from neighbouring tiles
	sc_in<powerCreditLine> power_credit_in[num_ip-1];//input ports to recieve credit info(power values)from neighbouring tiles
	//END
	// PORTS END /////////////////////////////////////////////////////////////////////////////
	
	/////Q-Router///////

/*	fifo	ctrQ;
	PENALTY	pQ[100];
	int pQtop;
	sc_out<bool> vcRequest;         ///< output port for sending request to VCA
	sc_in<bool> vcReady;            ///< input port for ready signal from VCA
    sc_out<sc_uint<2> >  opRequest; ///< output port for sending OC requested to VCA
	sc_in<sc_uint<VCS_BITSIZE+1> >  nextVCID;       ///< input port to recieve next VCID from VCA
	sc_out<flit> ocPort_out[num_ip];	///< ouput data/flit ports (one for each output channel)
	sc_in<bool>  ocReady_in[num_ip];	///< input ports for ready signal from OCs*/

	//From Neighbor Ctrls
		sc_in<flit> estIn[num_ip - 1]; //CODELINE
		switch_reg r_in[num_ip - 1]; //CODELINE
	//To Neighbor Ctrls
	    sc_out<flit> estOut[num_ip - 1];//CODELINE

	//From IC
		sc_in<sc_uint<64> > timestamp_ip[num_ip];
		sc_in<sc_uint<32> > est_pktID[num_ip];


	CTRRESULTS rs; //for results Q-Routing
	

	////////////////////
	/// Constructor
	SC_CTOR(Controller);
	
	// PROCESSES /////////////////////////////////////////////////////////////////////////////
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID, UI portN, UI portS, UI portE, UI portW);
	void allocate_route();	///< does routing
	UI idToDir(UI);		///< returns port id for a given direction (N, S, E, W)
	UI dirToId(UI);		///< returns direction (N, S, E, W) corresponding to a given port id
	// PROCESSES END /////////////////////////////////////////////////////////////////////////
	
	///Methods FOR Q_ROUTING
	void store_flit(flit *flit_in);	///< stores flit in buffer
	void create_est_flit(UI estimate,UI d, flit *est_out ); //CODEFLIT
	UI reverseDir(UI dir);  ///< Return the reverse direction
	int get_flit_index(UI op_dir);//returns index number of the flit destined to op_dir, -1 if no match
	void transmitEst();
	UI get_avgest();
	void rcv_estimate();
	//added
	void update_power_credit();
	void send_power_info();
	//end

	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< Unique tile identifier
	UI portN;	///< port number representing North direction
	UI portS;	///< port number representing South direction
	UI portE;	///< port number representing North direction
	UI portW;	///< port number representing North direction
	/*added*/	
	double P;//register to store power dissipated at the tile
	double Pnb[4];//registers to store the credit info of power from neighbouring nodes
	/*end*/
	ULL sim_count;
	router *rtable;	///< router (plug-in point for routing algorithm)
	// VARIABLES END /////////////////////////////////////////////////////////////////////////
};

#endif
 
