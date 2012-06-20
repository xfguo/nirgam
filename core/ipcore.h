
/*
 * ipcore.h
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
/// \file ipcore.h
/// \brief Defines abstract ipcore module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _IPCORE_
#define _IPCORE_

#include "systemc.h"
#include "../config/constants.h"
#include "flit.h"
#include "credit.h"
#include "rng.h"

#include <fstream>
#include <string>
#include <math.h>
#include <dlfcn.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an ipcore
///
/// This module defines an ipcore
//////////////////////////////////////////////////////////////////////////
struct ipcore : public sc_module {

	// PORTS //////////////////////////////////////////////////////////////////////////////////
	sc_in_clk clock;			///< input clock port
	sc_in<flit> flit_inport;		///< input data/flit port
	sc_out<flit> flit_outport;		///< ouput data/flit port
	sc_in<creditLine> credit_in[NUM_VCS];	///< input ports to recieve credit info (buffer status)
	// PORTS END //////////////////////////////////////////////////////////////////////////////
	
	/// Constructor
	SC_CTOR(ipcore);

	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	void entry();			///< process to keep track of simulation count in the module, sensitive to clock
	virtual void recv() = 0;	///< abstract process to recieve flits, sensitive to clock and input flit port
	virtual void send() = 0;	///< abstract process to send flits, sensitive to clock
 
//************************************************************************************	
	void setID(UI tileID,UI priority);		///< sets tileID
//************************************************************************************	


	/// create a hdt flit with given packet id, flit id and destination
	flit* create_hdt_flit(int pkt_id, int flit_id, UI route_info);
	/// create a head flit with given packet id, flit id and destination
	flit* create_head_flit(int pkt_id, int flit_id, UI route_info);
	/// create a data flit with given packet id and flit id
	flit* create_data_flit(int pkt_id, int flit_id);
	/// create a tail flit with given packet id and flit id
	flit* create_tail_flit(int pkt_id, int flit_id);	
	
	/// sets command field of flit equal to given value
	void set_cmd(flit*, int cmd_value);
	/// sets integer data field of flit equal to given value
	void set_data(flit*, int data_int_value);
	/// sets string data field of flit equal to given value
	void set_data(flit*, string data_string_value);
	/// sets command field and integer data field of flit equal to given values
	void set_payload(flit*, int cmd_value, int data_int_value);
	/// sets command field and string data field of flit equal to given values
	void set_payload(flit*, int cmd_value, string data_string_value);

	/// return command field of flit in second parameter
	void get_cmd(flit, int &cmd_value);
	/// return integer data field of flit in second parameter
	void get_data(flit, int &data_int_value);
	/// return string data field of flit in second parameter
	void get_data(flit, string &data_string_value);
	/// return command field and integer data field of flit in second and third parameter respectively
	void get_payload(flit, int &cmd_value, int &data_int_value);
	/// return command field and string data field of flit in second and third parameter respectively
	void get_payload(flit, int &cmd_value, string &data_string_value);
	/// return a randomly chosen destination
	int get_random_dest();
	// FUNCTIONS END /////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< unique tile id

//*****************************************************************
	UI tile_priority;	
//*****************************************************************


	ULL sim_count;	///< number of clock cycles
	RNG *ran_var;	///< random variable
	// VARIABLES END /////////////////////////////////////////////////////////////////////////
};

#endif
