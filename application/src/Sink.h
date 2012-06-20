
/*
 * Sink.h
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
/// \file Sink.h
/// \brief Defines sink application to recieve flits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _Sink_H_
#define _Sink_H_

#include "../../core/ipcore.h"
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//////////////////////////////////////////////////////////////
/// \brief Module to define sink application for an ipcore
///
/// This application should be plugged in to any core which is not generating any traffic, but is capable of recieving any incoming flits.
/////////////////////////////////////////////////////////////
struct Sink : public ipcore {
	
	/// Constructor
	SC_CTOR(Sink);
	
	// PROCESSES /////////////////////////////////////////////////////
	void send();			///< send flits
	void recv();			///< recieve flits
	// PROCESSES END/////////////////////////////////////////////////////
};

#endif
