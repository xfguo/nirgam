
/*
 * App_send.h
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
/// \file App_send.h
/// \brief Defines source application (for demo string concatenation) to send flits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _App_send_H_
#define _App_send_H_

#include "../../core/ipcore.h"
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
/// \brief Module to define send application for string concatenation
///
/// - Module derived from ipcore
/// - Also inherits send and recv processes
//////////////////////////////////////////////////////////////////////
struct App_send : public ipcore {
	
	/// Constructor
	SC_CTOR(App_send);
	
	// PROCESSES /////////////////////////////////////////////////////
	void send();			///< send flits
	void recv();			///< recieve flits
	void processing(flit);		///< process incoming flits
	// PROCESSES END /////////////////////////////////////////////////////
};

#endif
