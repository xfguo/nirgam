
/*
 * CBR.h
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
/// \file CBR.h
/// \brief Defines CBR (constant bit rate) traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CBR_
#define _CBR_


#include<math.h>
#include "TG.h"
#include "../../core/rng.h"
#include "../../core/ranvar.h"
#include "Traffic_patterns.h"
#include "NED_traffic.h"


//////////////////////////////////////////////////////////////////////
/// \brief Module to define CBR (constant bit rate) traffic generator
///
/// derived from TrafficGenerator
//////////////////////////////////////////////////////////////////////
struct CBRTraffic : public TrafficGenerator {

	/// Constructor
	SC_CTOR(CBRTraffic);
	
	// PROCESSES /////////////////////////////////////////////////////////
	void init();		///< generate traffic log as per traffic configuration
	double next_interval();	///< return next packet interval (constant for CBR)
	// PROCESSES END /////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////
	int pkt_interval;	///< inter-packet interval (in clock cycles) 
	int pkt_size;		///< packet size (in bytes)
	int load;		///< load percentage
	RNG *rnum;		///< random number generator
	string dst_type;	///< string to determine wether destination is fixed or random
	UniformRandomVariable *hsrng ;///< uniform random number to get destination
	int hotspot;		///< hotspot destination
	double hotspot_prob;    ///< hotspot node probability

	// VARIABLES END /////////////////////////////////////////////////////
};

#endif
 
