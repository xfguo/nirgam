
/*
 * credit.h
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
/// \file credit.h
/// \brief Defines and implements structure for buffer status
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CREDIT_
#define _CREDIT_

#include "systemc.h"

//////////////////////////////////////////////////////////
/// \brief credit data structure to transmit buffer status
//////////////////////////////////////////////////////////
struct creditLine {
	sc_bit	freeVC;		///< status of virtual channel (1 - a VC is free, 0 - No VC is free)
	sc_bit	freeBuf;	///< status of buffers (1 - a buffer is free, 0 - No buffer is free)

	// overloading equality operator for creditLine
	inline bool operator == (const creditLine& rhs) const {
		return (rhs.freeVC == freeVC && rhs.freeBuf == freeBuf);
	}
};
/*added*/
struct powerCreditLine{
	double Power;
	inline bool operator == (const powerCreditLine& rhs) const {
		return (rhs.Power == Power );
	}
};
/*end*/

// overloading extraction operator for creditLine
inline ostream& operator << ( ostream& os, const creditLine& a) {
	os << "freeVC: " << a.freeVC << " freeBuf: "<< a.freeBuf << endl;
	return os;
}

// overloading sc_trace for creditLine
inline void sc_trace( sc_trace_file*& tf, const creditLine& a, const std::string& name) {
	sc_trace( tf, a.freeVC, name+".freeVC");
	sc_trace( tf, a.freeBuf, name+".freeBuf");
}
/*added*/
// overloading extraction operator for power creditLine
inline ostream& operator << ( ostream& os, const powerCreditLine& a) {
	os << "Power: " << a.Power  << endl;
	return os;
}
// overloading sc_trace for creditLine
inline void sc_trace( sc_trace_file*& tf, const powerCreditLine& a, const std::string& name) {
	sc_trace( tf, a.Power, name+".Power");
	}
/*end*/



#endif
