
/*
 * router.h
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

////////////////////////////////////////////////////////////////////////////////////////////////
/// \file router.h
/// \brief Defines abstract router class
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __noc_router__
#define __noc_router__

#include "systemc.h"
#include "../config/constants.h"
#include <string>
#include <vector>
#include <time.h>

///////////////////////////////////////////////////////////////////////////
/// \brief Abstract router class 
/// 
/// classes implementing routing algorithms are derived from this class)
///////////////////////////////////////////////////////////////////////////
class router {
	protected:	
		UI id;	///< Unique tile identifier
	
	public:
		//For Q-Router
		UI num_nb;
		UI num_tiles;
		//end for Q-Route
		//added
		double power[4];
		//end
	
		/// Constructor
		router() {}
		/// virtual function that implements routing
		virtual UI calc_next(UI ip_dir, ULL src_id, ULL dst_id) = 0;
		/// virtual function to perform some initialization in routing algorithm
		virtual void initialize() = 0;
		/// virtual function to set identifier
		virtual void setID(UI) = 0;
		
		virtual UI get_estimate(UI d) = 0;
		virtual void update_estimate(UI ip_dir, UI dest_id, UI estimate, ULL q) = 0;
		//added
		virtual void update_power(int,double )=0;
		//end
};

#endif
