
/*
 * Trace_traffic.cpp
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
/// \file Trace_traffic.cpp
/// \brief Implements trace based traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Trace_traffic.h"
#include "../../config/extern.h"

/// Constructor
TraceTraffic::TraceTraffic(sc_module_name TraceTraffic) : TrafficGenerator(TraceTraffic)
{

}

// do nothing
// log based traffic generation in base class
double TraceTraffic::next_interval()
{
}

// for dynamic linking
extern "C"
{
    ipcore *maker()
    {
        return new TraceTraffic("TG");
    }
}
