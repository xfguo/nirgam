
/*
 * Multimedia.h
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
/// \file Multimedia.h
/// \brief Defines Bursty traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MultimediaTraffic_
#define _MultimediaTraffic_

#include "TG.h"
#include "../../core/ranvar.h"
#include "Traffic_patterns.h"
#include "NED_traffic.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
/// \brief Module to define Multimedia traffic generator
///
/// derived from TrafficGenerator
//////////////////////////////////////////////////////////////////////
typedef double real;
struct complex{real Re; real Im;} ;

struct MultimediaTraffic : public TrafficGenerator {

	/// Constructor
	SC_CTOR(MultimediaTraffic);
	
	// PROCESSES /////////////////////////////////////////////////////////
	void init();		///< generate traffic log as per traffic configuration
	double next_interval();	///< return next packet interval
	void hurst_cal(float H,int n,double * ret);
	double dgamma(double x);
	void ifft( complex *v, int n, complex *tmp );
	void FGN_B_est(double *lambda, float H, int n, double *result);
	void FGN_spectrum(double *lambda, float H, int n, double *r);
	// PROCESSES END /////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////
	int pkt_size;		///< size of packet (in bytes)
	int pkt_interval;	///< inter-packet interval
	float hurst_par;
	int load;
	int maxo;		/// < Maximum value of the trace to be mapped
	int mino;		/// < Minimum value of the trace to be 
	ExponentialRandomVariable var_offtime;	///< exponentially distributed random variable for offtime
	UniformRandomVariable *hsrng ;///< uniform random number to get destination
	int hotspot;		///< hotspot destination
	double hotspot_prob;    ///< hotspot node probability
	string dst_type;	///< string to determine wether destination is fixed or random
	// VARIABLES END /////////////////////////////////////////////////////
};

#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

#endif
