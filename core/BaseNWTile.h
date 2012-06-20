
/*
 * BaseNWTile.h
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
/// \file BaseNWTile.h
/// \brief Defines abstract module for network tile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BASE_NWTILE_
#define _BASE_NWTILE_

#include "systemc.h"
#include "../config/constants.h"
#include "flit.h"
#include "credit.h"

///////////////////////////////////////////////////////////////////
/// \brief Abstract class to represent network tile.
//////////////////////////////////////////////////////////////////
struct BaseNWTile : public sc_module {

////////////////////////////////////////////////************************************///////////////////////////////////////////////////////
UI priority;
ULL max_latency_GT;
ULL max_latency_BE;
////////////////////////////////////////////////************************************///////////////////////////////////////////////////////

	UI tileID;	///< unique tile id
	UI portN;	///< port representing North direction
	UI portS;	///< port representing South direction
	UI portE;	///< port representing East direction
	UI portW;	///< port representing West direction
/*added for power*/
	UI prev_buf_read;
	UI prev_buf_write;
	UI prev_xbar_trav;
	UI prev_link_trav;
	UI prev_vc_reads;
	UI prev_vc_writes;	
	UI prev_n_arb;
	UI buf_read;
	UI buf_write;
	UI xbar_trav;
	UI link_trav;
	UI vc_reads;
	UI vc_writes;
	UI n_arb;
	double Pavg;//total average power associated with each tile
	double Plink;
	
/*end*/
	BaseNWTile() {	}	///< default constructor

	/// systemC constructor
	/// parameters - module name, tile id.
	SC_HAS_PROCESS(BaseNWTile);
	BaseNWTile(sc_module_name BaseNWTile, UI id) : sc_module(BaseNWTile) {
	}
	
	virtual float return_latency(int) = 0;		///< returns average latency per packet for a channel
	virtual float return_latency_flit(int) = 0;	///< returns average latency per flit for a channel
	virtual float return_avg_tput(int) = 0;		///< returns average throughput for a channel
	virtual ULL return_total_latency() = 0;		///< returns total latency for a channel
	virtual ULL return_total_flits() = 0;		///< returns total number of flits through a channel
	//added
	virtual void update_power_info()=0;
	//end
//qrt************************************************************
	virtual void calc_results()=0;
//qrt************************************************************

/******************************************************************************************/

	virtual int return_total_latency_GT() = 0;		///< returns total latency for a channel for GT traffic
	virtual int return_total_latency_BE() = 0;		///< returns total latency for a channel for BE traffic
        virtual ULL cal_no_of_pkts_per_tile() = 0;        
virtual ULL cal_no_of_pktsGT_per_tile() = 0;        
        virtual int return_total_flits_GT() = 0;		///< returns total number of flits through a channel for GT traffic
	virtual int return_total_flits_BE() = 0;		///< returns total number of flits through a channel for BE traffic
	
/******************************************************************************************/

	/*added*/
	
	virtual UI return_buf_read()=0;
	virtual UI return_buf_write()=0;
	virtual UI return_xbar_trav()=0;
	virtual UI return_vc_reads()=0;
	virtual UI return_vc_writes()=0;	
	virtual UI return_n_arb()=0;//arb
	virtual UI return_link_trav()=0;
	//virtual ULL cal_no_of_pkts_core()=0;
	//virtual float return_total_throughput()=0;
	/*end*/




};

#endif
