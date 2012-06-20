
/*
 * TG.h
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
/// \file TG.h
/// \brief Defines abstract traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TrafficGen__
#define __TrafficGen__

#include "../../core/ipcore.h"
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//////////////////////////////////////////////////////////////
/// \brief Module to define traffic generator
///
/// - Abstract traffic generator
/// - inherited from ipcore
/// - Traffic generator applications must be derived from this module.
/////////////////////////////////////////////////////////////
struct TrafficGenerator : public ipcore {

	/// Constructor
	SC_CTOR(TrafficGenerator);

	// PROCESSES /////////////////////////////////////////////////////
	virtual double next_interval() = 0;	///< returns inter packet interval, defined in derived module
	void send();			///< generate traffic according to traffic config file
	void recv();			///< recieve flits
	sc_time_unit strToTime(string);	///< convert time unit from string representation to systemC representation
	// PROCESSES END ////////////////////////////////////////////////

	// VARIABLES ////////////////////////////////////////////
	int num_flits;		///< Number of flits per packet
	int flit_interval;	///< Inter-flit interval (in clock cycles)
	int route_info;		///< destination address or routing code (for source routing)
	int next_pkt_time;	///< inter packet interval (in clock cycles)
	int cycles_per_flit;	///< number of cycles required for processing one flit
	// VARIABLES END ///////////////////////////////////////////

};

#endif

