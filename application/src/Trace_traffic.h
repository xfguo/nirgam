
/*
 * Trace_traffic.h
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
/// \file Trace_traffic.h
/// \brief Defines trace based traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _Trace_Traffic_
#define _Trace_Traffic_


#include "TG.h"
#include "../../core/rng.h"

//////////////////////////////////////////////////////////////////////
/// \brief Module to define trace based traffic generator
///
/// - This module is derived from TrafficGenerator.
/// - The purpose of this module is to allow instantiation of abstract module traffic generator.
/// - In effect processes in base class generate traffic as per traffic log generated in previous simulation
//////////////////////////////////////////////////////////////////////
struct TraceTraffic : public TrafficGenerator {

	/// Constructor
	SC_CTOR(TraceTraffic);
		
	double next_interval();		///< returns next packet interval
};

#endif

