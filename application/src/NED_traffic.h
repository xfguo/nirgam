
/*
 * NED.h
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
/// \file NED_traffic.h
/// \brief Defines NED traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _NED_Traffic_
#define _NED_Traffic_

#include "TG.h"
#include "../../core/ranvar.h"
//#include "../../config/default.h"

//////////////////////////////////////////////////////////////////////
/// \brief Module to define NED traffic generator
///
//////////////////////////////////////////////////////////////////////
class NEDTraffic
{
	private:
		int num_rows, num_cols, tileID, WARMUP;
		int Sr[2], *MatR, dst_K;
		float m;
		int *dst_vect;
		float *Probability;
	// PROCESSES /////////////////////////////////////////////////////////
	void makeR();  
	float calculateQ(float q1, float q2);
	// PROCESSES END /////////////////////////////////////////////////////

	public:
	// Constructor
	NEDTraffic(int num_row, int num_col, int tile_id);
	
	// PROCESSES /////////////////////////////////////////////////////////
	void get_NEDTraffic(float *tiles);
	// PROCESSES END /////////////////////////////////////////////////////
};
#endif
