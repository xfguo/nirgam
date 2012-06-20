
/*
 * switch_reg.h
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
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file switch_reg.h
/// \brief Defines register to store a flit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __SWITCHREG__
#define __SWITCHREG__

#include "flit.h"

///////////////////////////////////////////////////////////////
/// \brief register data structure
///
/// used in OC to store a single flit 
///////////////////////////////////////////////////////////////
struct switch_reg {
	flit	val;	///< flit stored in register
	bool	free;	///< register status (0 - register is occupied, 1 - register is free)

	/// Constructor
	switch_reg() {
		free = true;
    	}
};

#endif
