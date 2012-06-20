
/*
 * Bursty.h
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
/// \file Bursty.h
/// \brief Defines Bursty traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BurstyTraffic_
#define _BurstyTraffic_

#include "TG.h"
#include "../../core/ranvar.h"
#include "Traffic_patterns.h"
#include "NED_traffic.h"



//////////////////////////////////////////////////////////////////////
/// \brief Module to define Bursty traffic generator
///
/// derived from TrafficGenerator
//////////////////////////////////////////////////////////////////////
struct BurstyTraffic : public TrafficGenerator {

	/// Constructor
	SC_CTOR(BurstyTraffic);

	// PROCESSES /////////////////////////////////////////////////////////
	void init();		///< generate traffic log as per traffic configuration
	double next_interval();	///< return next packet interval
	// PROCESSES END /////////////////////////////////////////////////////

	// VARIABLES /////////////////////////////////////////////////////
	int pkt_size;		///< size of packet (in bytes)
	int load;		///< percentage load
	int avg_burstlen;	///< average burst length
	int avg_offtime;	///< average offtime (interval between two bursts)
	int pkt_interval;	///< inter-packet interval
	int rem;		///< number of packets remaining in a burst

	ExponentialRandomVariable var_burstlen;	///< exponentially distributed random variable for burst length
	ExponentialRandomVariable var_offtime;	///< exponentially distributed random variable for offtime
	UniformRandomVariable *hsrng ;///< uniform random number to get destination
	int hotspot;		///< hotspot destination
	double hotspot_prob;    ///< hotspot node probability
	int newburst;
	string dst_type;	///< string to determine wether destination is fixed or random

	// VARIABLES END /////////////////////////////////////////////////////
};

#endif
