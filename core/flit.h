
/*
 * flit.h
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
/// \file flit.h
/// \brief Defines flow control unit(flit) for NoC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FLIT_INC_
#define _FLIT_INC_

#include "systemc.h"
#include "../config/constants.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

/// required for stl
using namespace std;

////////////////////////////////////////////////
/// \brief routing header for source routing algorithm
////////////////////////////////////////////////
struct source_hdr {
	UI route;	///< route code
};

//////////////////////////////////////////////////////
/// \brief routing header for XY and OE routing algorithm.
///
/// It can be used for any other routing algorithms which are a function of only the destination.
//////////////////////////////////////////////////////
struct rt_dst_hdr {
	UI dst;		///< destination tile ID
};

////////////////////////////////////////////////////
/// \brief routing header for Ant based routing algorithm
////////////////////////////////////////////////////
struct AntNet_hdr {
	UI dst;		///< destination tileID
	UI route;	///< route code for reverse path
};





////////////////////////////////////////////////
/// \brief routing header in flit
///
/// One of the available headers, depending on routing algorithm
////////////////////////////////////////////////
union routing_hdr {
	source_hdr sourcehdr;	///< source routing header
	rt_dst_hdr dsthdr;	///< destination routing header
	AntNet_hdr AntNethdr;	///< Ant routing header
};

////////////////////////////////////////////////
/// \brief payload in head/hdt flit
///
/// Size: 1 byte
////////////////////////////////////////////////
struct payload_hdr {
	int cmd;			///< command
	int data_int;			///< integer data
	const char *data_string;	///< string data
};

////////////////////////////////////////////////
/// \brief flit header in head/hdt flit
////////////////////////////////////////////////
struct flit_head {
	routing_type	rtalgo;		///< routing algorithm (SOURCE, XY, OE)
	routing_hdr 	rthdr;		///< routing header
	payload_hdr	datahdr;	///< payload
};

////////////////////////////////////////////////
/// \brief flit header in data/tail flit
////////////////////////////////////////////////
struct flit_data {
	int cmd;			///< command
	int data_int;			///< integer data
	const char *data_string;	///< string data
};

///////////////////////////////////////////////
/// \brief flit header, depending on type of flit (hdt, head, data, tail)
//////////////////////////////////////////////
union flit_hdr {
	flit_head header;	///< flit header in hdt/head flit
	flit_data payload;	///< flit header in data/tail flit
};

////////////////////////////////////////////////
/// \brief packet header for ant packets
////////////////////////////////////////////////
struct ant_hdr {
public:
	ant_type 	anttype;	///< type of ant
	routing_type 	rtalgo;		///< routing algorithm
	routing_hdr 	rthdr;		///< routing header
	//memory;
};



////////////////////////////////////////////////
/// \brief packet header for noc packets
////////////////////////////////////////////////
struct noc_hdr {
public:
	UI		pktid;		///< packet id
	UI		flitid;		///< flit id
	flit_type	flittype;	///< flit type (HDT, HEAD, DATA, TAIL)
	flit_hdr 	flithdr;	///< flit header (depending on flit type)


	UI		priority;//set priority

};


////////////////////////////////////////////////
/// \brief packet header for estimate packet
////////////////////////////////////////////////
struct est_hdr {
	UI dir;		///< Direction to send the estimate
	UI d;		///< d as in Q[y,d]
	ULL estimate;	///< estimate in clock cycles
	ULL pktID;
	UI src;
};

////////////////////////////////////////////////
/// \brief packet header (depending on packet type)
////////////////////////////////////////////////
union pkt_hdr {
	ant_hdr anthdr;
	noc_hdr nochdr;
	est_hdr esthdr; //for Q-Routing
};

////////////////////////////////////////////////
/// \brief Error control code
////////////////////////////////////////////////
struct ECC {
};

////////////////////////////////////////////////
/// \brief header to record simulation data
////////////////////////////////////////////////
struct sim_hdr {     
	sc_time gtime;		///< flit generation time (in time units)
	sc_time atime;		///< flit arrival time (in time units)
	sc_time ctime;		///< instantaneous time (in time units)
	ULL	gtimestamp;	///< flit generation timestamp (in clock cycle)
	ULL	atimestamp;	///< flit arrival timestamp at reciever (in clock cycle)
	ULL	ICtimestamp;	///< input channel time stamp (in clock cycles)
	ULL OCtimestamp;    ///< output channel time stamp (in clock cycles)
	ULL	num_waits;	///< no. of clock cycles spent waiting in buffer
	ULL	num_sw;		///< no. of switches traversed

};

////////////////////////////////////////////////
/// \brief flit data structure
///
/// This is the structure that represents flow control unit(flit) in NoC
////////////////////////////////////////////////
struct flit {
	pkt_type pkttype;	///< packet type (ANT, NOC, EST)
	sim_hdr	 simdata;	///< simulation header
	pkt_hdr  pkthdr;	///< packet header (depending on packet type)
	int	 vcid;		///< virtual channel id
	UI	 src;		///< source tileID

	// overloading equality operator
	inline bool operator == (const flit& temp) const {
		if(temp.pkttype != pkttype || temp.simdata.gtime != simdata.gtime || temp.simdata.atime != simdata.atime || temp.simdata.ctime != simdata.ctime || temp.src != src)
			return false;
		switch(temp.pkttype) {
			case ANT: break;
			case NOC:
				if(temp.pkthdr.nochdr.pktid != pkthdr.nochdr.pktid || temp.pkthdr.nochdr.flitid != pkthdr.nochdr.flitid || temp.pkthdr.nochdr.flittype != pkthdr.nochdr.flittype)
				return false;
				break;
		}
		return true;
	}
};

// overloading extraction operator for flit
inline ostream&
operator << ( ostream& os, const flit& temp ) {
	if(temp.pkttype == ANT)
		os<<"ANT PACKET";
	else if(temp.pkttype == NOC) {
		os<<"NOC PACKET, ";
		switch(temp.pkthdr.nochdr.flittype) {
			case HEAD: os<<"HEAD flit, "; break;
			case DATA: os<<"DATA flit, "; break;
			case TAIL: os<<"TAIL flit, "; break;
			case HDT: os<<"HDT flit, "; break;
		}
		os<<"src: "<<temp.src<<" pktid: "<<temp.pkthdr.nochdr.pktid<<" flitid: "<<temp.pkthdr.nochdr.flitid;
	}
	else if(temp.pkttype == EST){
		os<<"EST PACKET,dir: "<<temp.pkthdr.esthdr.dir<<" d: "<<temp.pkthdr.esthdr.d<<" est: "<<temp.pkthdr.esthdr.estimate;
	}
	os<<endl;
	return os;
}

// overloading extraction operator for simulation header
inline ostream&
operator << ( ostream& os, const sim_hdr& temp ) {
	os<<"gtimestamp: "<<temp.gtimestamp<<" gtime: "<<temp.gtime;
	os<<"\natimestamp: "<<temp.atimestamp<<" atime: "<<temp.atime<<endl;
	return os;
}

// overloading sc_trace for flit
inline void sc_trace( sc_trace_file*& tf, const flit& a, const std::string& name) {
	//sc_trace( tf, a.pkttype, name+".pkttype");
	sc_trace(tf, a.src, name+".src");
	sc_trace(tf, a.pkthdr.nochdr.pktid, name+".pktid");
	sc_trace(tf, a.pkthdr.nochdr.flitid, name+".flitid");
	//sc_trace( tf, a.freeBuf, name+".freeBuf");
}

#endif
