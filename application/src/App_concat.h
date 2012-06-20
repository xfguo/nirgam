
/*
 * App_concat.h
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
/// \file App_concat.h
/// \brief Defines string concatenation application
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _App_concat_H_
#define _App_concat_H_

#include "../../core/ipcore.h"
#include <fstream>
#include <string>
#include <math.h>

/// required for stl
using namespace std;

//////////////////////////////////////////////////////////////
/// \brief Module to define string concatenation application
///
/// - Module derived from ipcore
/// - Also inherits send and recv processes
/////////////////////////////////////////////////////////////
struct App_concat : public ipcore {
	
	/// Constructor
	SC_CTOR(App_concat);
	
	// PROCESSES /////////////////////////////////////////////////////
	void send();			///< send flits
	void recv();			///< recieve flits
	void processing(flit);		///< process flits
	void send_concat_string(string concatenated_string);	///< send concatenated string to destination tile
	// PROCESSES END /////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////
	string final;	// string to store final concatenated string
	// VARIABLES END /////////////////////////////////////////////////////
};

#endif
