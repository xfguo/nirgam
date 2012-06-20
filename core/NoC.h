
/*
 * NoC.h
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
/// \file NoC.h
/// \brief Defines network topology
///
/// This file defines:
/// - module NoC, a 2-dimentional topology of network tiles.
/// - module signals, a set of signals to connect tiles in the network.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NOC__
#define __NOC__

#include "NWTile.h"
#include "../config/extern.h"
#include "orion/orion_router_power.h"
///////////added for q router
#include <string>
#include <fstream>
#include <iostream>

///////////////////////////////////////////////
/// \brief signals to connect neighboring tiles
///////////////////////////////////////////////
struct signals {
	sc_signal<flit> sig_toS;			///< data line (flit line) from a tile to its South neighbor
	sc_signal<flit> sig_fromS;			///< data line (flit line) to a tile from its South neighbor
	sc_signal<flit> sig_fromE;			///< data line (flit line) to a tile from its East neighbor
	sc_signal<flit> sig_toE;			///< data line (flit line) from a tile to its East neighbor

	sc_signal<creditLine> cr_sig_toS[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel from a tile to its South neighbor
	sc_signal<creditLine> cr_sig_fromS[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel to a tile from its South neighbor
	sc_signal<creditLine> cr_sig_fromE[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel to a tile from its East neighbor
	sc_signal<creditLine> cr_sig_toE[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel from a tile to its East neighbor  

//////ESTIMATE SIGNALS//////////
	sc_signal<flit> est_sig_toS;			///< data line (flit line) from a tile to its South neighbor //CODELINE
	sc_signal<flit> est_sig_fromS;			///< data line (flit line) to a tile from its South neighbor //CODELINE
	sc_signal<flit> est_sig_fromE;			///< data line (flit line) to a tile from its East neighbor //CODELINE
	sc_signal<flit> est_sig_toE;			///< data line (flit line) from a tile to its East neighbor //CODELINE

/*added*/
	sc_signal<powerCreditLine> power_sig_toS;			///< data line (flit line) from a tile to its South neighbor //CODELINE
	sc_signal<powerCreditLine> power_sig_fromS;			///< data line (flit line) to a tile from its South neighbor //CODELINE
	sc_signal<powerCreditLine> power_sig_fromE;			///< data line (flit line) to a tile from its East neighbor //CODELINE
	sc_signal<powerCreditLine> power_sig_toE;			///< data line (flit line) from a tile to its East neighbor //CODELINE

};


/////NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN/////////////////////////////////////////
struct temp_signals {
	sc_signal<flit> tempsig_toS;			
	sc_signal<flit> tempsig_fromS;
	sc_signal<flit>	tempsig_toE;
	sc_signal<flit>	tempsig_fromE;	
	};
//////////NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN//////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Represents the entire Network-on-Chip
///
/// This module represents the NoC. It implements an array of network tiles and connectivity among them.
////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NoC : public sc_module {

	sc_in<bool> switch_cntrl;	///< Clock input port

	/// Constructor
	SC_HAS_PROCESS(NoC);
	NoC(sc_module_name NoC, int num_rows, int num_cols);
	
	int rows;	///< number of rows in topology
	int cols;	///< number of columns in topology
	
	BaseNWTile	*nwtile[MAX_NUM_ROWS][MAX_NUM_COLS];	///< A 2-d array of network tiles
	signals		sigs[MAX_NUM_ROWS][MAX_NUM_COLS];	///< Signals to interconnect network tiles
	
///NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN////////////////////////////	
	temp           *temp1[40]; 
	temp_signals    temp_sigs[40];
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN////////////////////////////


//	float qlat[50000][MAX_NUM_TILES];
//	int qflit[50000][MAX_NUM_TILES];

	void entry();	///< Keeps count of number of simulation cycles
};

#endif
