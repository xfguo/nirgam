
/*
 * constants.h
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
/// \file constants.h
/// \brief Defines constant parameters for NIRGAM
///////////////////////////////////////////////////////

#ifndef _TYPES_H_
#define _TYPES_H_

/// current time
#define CURRENT_TIME sc_simulation_time()

/// number of virtual channels
#define NUM_VCS 4

// parameters for topology
/// maximum number of rows
#define MAX_NUM_ROWS 9
/// maximum number of columns
#define MAX_NUM_COLS 9
/// maximum number of tiles
#define MAX_NUM_TILES MAX_NUM_ROWS * MAX_NUM_COLS

// parameters for NWTile
/// number of neighbors of a general tile in mesh/torus
#define NUM_NB 4
/// number of input channels in a general tile in mesh/torus
#define NUM_IC 5
/// number of output channels in a general tile in mesh/torus
#define NUM_OC 5

// parameters for mesh (non-toroidal) topology
/// number of neighbors of a corner tile in mesh
#define NUM_NB_C 2
/// number of input channels in a corner tile in mesh
#define NUM_IC_C 3
/// number of output channels in a corner tile in mesh
#define NUM_OC_C 3

// parameters for mesh (non-toroidal) topology
/// number of neighbors of a border tile in mesh
#define NUM_NB_B 3
/// number of input channels in a border tile in mesh
#define NUM_IC_B 4
/// number of output channels in a border tile in mesh
#define NUM_OC_B 4

// direction ids
/// North direction
#define N 0
/// South direction
#define S 1
/// East direction
#define E 2
/// West direction
#define W 3
/// Core direction
#define C 4

/// not defined
#define ND 5

// buffer parameters
#define	BUF_BITSIZE	4
/// maximum buffer depth
#define	MAX_NUM_BUFS	16

// parameters for bitwidth
#define ADDR_SIZE 32
#define VCS_BITSIZE 2

/////////////////////////////////////////
/// types of flits: HEAD, DATA, TAIL, HDT
////////////////////////////////////////
enum flit_type{
	HEAD,
	DATA,
	TAIL,
	HDT
};

/////////////////////////////////////////
/// types of packets: ANT, NOC
////////////////////////////////////////
enum pkt_type {
	ANT,
	NOC,
	EST
};

/////////////////////////////////////////
/// types of ant packets: FORWARD, BACKWARD
////////////////////////////////////////
enum ant_type {
	FORWARD,
	BACKWARD
};

////////////////////////////////////////////////
/// types of routing algorithms: SOURCE, XY, OE
////////////////////////////////////////////////
enum routing_type {
	SOURCE,
	XY,
	OE,
	QRT,
	DYAD,
	MAXY
};

/////////////////////////////////////////
/// types of request to controller
////////////////////////////////////////
enum request_type {
	NONE,
	ROUTE,
	UPDATE,
	ESTUPDATE
};

/////////////////////////////////////////
/// types of topology: MESH, TORUS
////////////////////////////////////////
enum topology {
	MESH,
	TORUS
};



#define UI  unsigned int
#define UL  unsigned long
#define ULL unsigned long long

#define borderN(ID) ((ID) < num_cols)
#define borderS(ID) ((ID) >= num_tiles - num_cols)
#define borderE(ID) (((ID) + 1) % num_cols == 0)
#define borderW(ID) (((ID) % num_cols) == 0)

#define border(ID) (borderN(ID) || borderS(ID) || borderE(ID) || borderW(ID))

#define cornerNW(ID) ((ID) == 0)
#define cornerNE(ID) ((ID) == num_cols - 1)
#define cornerSW(ID) ((ID) == num_tiles - num_cols)
#define cornerSE(ID) ((ID) == num_tiles - 1)

#define corner(ID) (cornerNW(ID) || cornerNE(ID) || cornerSW(ID) || cornerSE(ID))
#define ESTIMATE_BUFFER_SIZE 20
#endif
