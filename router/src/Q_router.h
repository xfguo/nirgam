
/*
 * XY_router.h
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
/// \file XY_router.h
/// \brief Defines class to implement XY routing algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Q_router__
#define __Q_router__

#include "../../core/router.h"
#include "../../core/rng.h"

#define	MAX_EST  10000
#define MAXq	 100

//////////////////////////////////////////////////////////////////////
/// \brief Class to implement XY routing algorithm
///
/// inherited from from class router
//////////////////////////////////////////////////////////////////////
class Q_router : public router {
	private:
		UI **Q;
		RNG *ran_var;
		float alpha;
		UI *minQ;
	public:
		/// Constructor
		Q_router() { }
		
		/// returns next hop for a given source and destination. ip_dir is the incoming direction
		UI calc_next(UI ip_dir, ULL source_id, ULL dest_id);
		void initialize();	///< any initializations to be done
		void setID(UI tileid);	///< set unique id

		UI get_estimate(UI d);
		void update_estimate(UI ip_dir, UI dest_id, UI estimate, ULL q);
		void update_power(int,double ){};
};

#endif

