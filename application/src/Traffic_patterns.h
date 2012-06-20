
/*
 * Traffic_patterns.h
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


#include "systemc.h"
#include "../../config/constants.h"
#include <string>
#include <vector>
#include <time.h>

///////////////////////////////////////////////////////////////////////////
/// \brief Abstract router class 
/// 
/// classes implementing routing algorithms are derived from this class)
///////////////////////////////////////////////////////////////////////////
class traffic_patterns {
	
	public:
		/// Constructor
		traffic_patterns() {}
		/// virtual function that implements transpose
		int trafficTranspose(int , int, int) ;
		void fixRanges(int &destx , int &desty,int,int);
		void setBit(int &x, int w, int v);
		int getBit(int x, int w);
		double log2ceil(double x);
		int trafficShuffle(int tileID, int num_rows, int num_cols);
		int trafficBitReversal(int tileID, int num_rows, int num_cols);
		int trafficButterfly(int tileID, int num_rows, int num_cols);
};

