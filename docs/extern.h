
/*
 * extern.h
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

/////////////////////////////////////////////////////////////
/// \file extern.h
/// \brief Defines extern variables available to all modules
/////////////////////////////////////////////////////////////

#ifndef _Extern_H_
#define _Extern_H_

#include <string>

extern float BW_GT;//for allocating vc's to gt and be flits

extern topology TOPO;		///< topology (NoC)
extern int num_rows;		///< number of rows in topology
extern int num_cols;		///< number of columns in topology
extern int num_tiles;		///< number of tiles in topology
extern sc_clock *nw_clock;	///< pointer to clock

extern routing_type RT_ALGO;	///< routing algorithm
extern int LOG;			///< log level (0 - 4)
extern ULL WARMUP;		///< warmup period (in clock cycles) before traffic generation begins
extern ULL SIM_NUM;		///< Total simulation cycles
extern ULL TG_NUM;		///< Number of clock cycles until which traffic is generated

extern int NUM_BUFS;		///< buffer depth (number of buffers in i/p channel fifo)
extern int FLITSIZE;		///< size of flit (in bytes)
extern int HEAD_PAYLOAD;	///< payload size (in bytes) in head/hdt flit
extern int DATA_PAYLOAD;	///< payload size (in bytes) in data/tail flit

extern ofstream eventlog;	///< file stream to log events
extern sc_trace_file* tracefile;///< file stream to generate vcd trace
extern ofstream results_log;	///< file stream to log results
extern ofstream qresults_log;	///< file stream to log results
extern ofstream GTresults_log;
extern ofstream latency_throughput_result1;
extern ofstream BEresults_log;


extern double CLK_FREQ;		///< clock frequency (in GHz)
extern double CLK_PERIOD;	///< clock period (in ns)
extern float ALPHA;
extern ULL QXY_SIMCOUNT;
extern UI ESTBUF;
extern UI WAITS;
extern double BETA;
extern double DELTA;
extern UI PWAIT;

/////NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN///////////////////////////////
extern int fail[40][4],failno;
////NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN///////////////////////////////
extern int congestion;
#endif
 
