
/*
 * default.h
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

///////////////////////////////////////////////////////
/// \file default.h
/// \brief Defines default values for NIRGAM parameters
///////////////////////////////////////////////////////

#ifndef _Default_H_
#define _Default_H_

topology TOPO = MESH;			///< topology
int num_rows = 4;			///< number of rows in topology
int num_cols = 4;			///< number of columns in topology
int num_tiles = num_rows * num_cols;	///< number of tiles in topology

routing_type RT_ALGO  = XY;		///< routing algorithm
int  LOG = 0;				///< log level

float BW_GT=.75;//bandwidth reserved for GT traffic

int NUM_BUFS = 32;	///< buffer depth (number of buffers) in input channel fifo
int FLITSIZE = 5;	///< size of flit in bytes
int HEAD_PAYLOAD = 1;	///< payload size (in bytes) in head/hdt flit
int DATA_PAYLOAD = 4;	///< payload size (in bytes) in data/tail flit

ULL WARMUP = 5;		///< warmup period (in clock cycles)
ULL SIM_NUM = 3000;	///< Simulation cycles
ULL TG_NUM = 1000;	///< clock cycles until which traffic is generated

double CLK_FREQ = 1;			///< clock frequency (in GHz)
double CLK_PERIOD = (1/CLK_FREQ);	///< clock period (in ns)

float ALPHA = .8;
ULL QXY_SIMCOUNT = 0;
UI ESTBUF = 10;
UI WAITS = 100;
double BETA = .2;
double DELTA = .1;
UI PWAIT = 20;
int failno=0;
int fail[40][4];
int congestion=0;
#endif
