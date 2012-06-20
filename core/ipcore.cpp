
/*
 * ipcore.cpp
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
/// \file ipcore.cpp
/// \brief Implements module ipcore (base class for applications)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ipcore.h"
#include "../config/extern.h"


////////////////////////
/// Constructor
////////////////////////
ipcore::ipcore(sc_module_name ipcore): sc_module(ipcore)
{
//	cout<<"\nInside ipcore"; // REMOVE
    sim_count = 0;
    ran_var = new RNG((RNG::RNGSources)2,1);

    // process sensitive to clock, sends out flit
    SC_CTHREAD(send, clock.pos());

    // process sensitive to clock, to keep track of simulation count
    SC_CTHREAD(entry, clock.pos());

    // process sensitive to clock and inport event, recieves incoming flit
    SC_THREAD(recv);
    sensitive << flit_inport << clock;

}

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile ID
///////////////////////////////////////////////////////////////////////////
void ipcore::setID(UI id,UI priority)
{
    tileID = id;
    tile_priority=priority;
//	cout<< "TP"<<tile_priority<<endl;
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Keeps track of clock cycles in the module
///////////////////////////////////////////////////////////////////////////
void ipcore::entry()
{
    sim_count = 0;
    while (true)
    {
        wait();
        sim_count++;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return a random destination
///////////////////////////////////////////////////////////////////////////
int ipcore::get_random_dest()
{
    int dest = ran_var->uniform(num_tiles);
    while (dest == tileID)
        dest = ran_var->uniform(num_tiles);
    return dest;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a hdt flit
/// hdt flit represents a packet consisting of single flit.
/// Parameters:
/// - packet id
/// - flit id
/// - route info
///   - destination tileID for XY and OE routing
///   - route code fro source routing
///   .
/// .
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////
flit* ipcore::create_hdt_flit(int pkt_id, int flit_id, UI route_info)
{
    flit *flit_out = new flit;
    flit_out->pkttype = NOC;
    flit_out->vcid = 0;
    flit_out->src = tileID;

    flit_out->pkthdr.nochdr.flittype = HDT;
    flit_out->pkthdr.nochdr.pktid = pkt_id;
    flit_out->pkthdr.nochdr.flitid = flit_id;
    flit_out->pkthdr.nochdr.flithdr.header.rtalgo = RT_ALGO;

    flit_out->pkthdr.nochdr.priority=tile_priority;

    if (RT_ALGO == SOURCE)
        flit_out->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route = route_info;
    else
        flit_out->pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst = route_info;

    flit_out->simdata.gtime = sc_time_stamp();
    flit_out->simdata.ctime = sc_time_stamp();
    flit_out->simdata.atime = SC_ZERO_TIME;
    flit_out->simdata.atimestamp = 0;
    flit_out->simdata.num_waits = 0;
    flit_out->simdata.num_sw = 0;
    flit_out->simdata.gtimestamp = sim_count;

    return flit_out;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a head flit
/// Parameters:
/// - packet id
/// - flit id
/// - route info
///   - destination tileID for XY and OE routing
///   - route code fro source routing
///   .
/// .
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////
flit* ipcore::create_head_flit(int pkt_id, int flit_id, UI route_info)
{
    flit *flit_out = new flit;
    flit_out->pkttype = NOC;
    flit_out->vcid = 0;
    flit_out->src = tileID;

    flit_out->pkthdr.nochdr.flittype = HEAD;
    flit_out->pkthdr.nochdr.pktid = pkt_id;
    flit_out->pkthdr.nochdr.flitid = flit_id;
    flit_out->pkthdr.nochdr.flithdr.header.rtalgo = RT_ALGO;


    flit_out->pkthdr.nochdr.priority=tile_priority;

    if (RT_ALGO == SOURCE)
        flit_out->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route = route_info;
    else
        flit_out->pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst = route_info;


    flit_out->simdata.gtime = sc_time_stamp();
    flit_out->simdata.ctime = sc_time_stamp();
    flit_out->simdata.atime = SC_ZERO_TIME;
    flit_out->simdata.atimestamp = 0;
    flit_out->simdata.num_waits = 0;
    flit_out->simdata.num_sw = 0;
    flit_out->simdata.gtimestamp = sim_count;

    flit_out->simdata.OCtimestamp = sim_count;
    return flit_out;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a data flit
/// Parameters:
/// - packet id
/// - flit id
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////
flit* ipcore::create_data_flit(int pkt_id, int flit_id)
{
    flit *flit_out = new flit;
    flit_out->pkttype = NOC;
    flit_out->vcid = 0;
    flit_out->src = tileID;

    flit_out->pkthdr.nochdr.flittype = DATA;
    flit_out->pkthdr.nochdr.pktid = pkt_id;
    flit_out->pkthdr.nochdr.flitid = flit_id;

    flit_out->pkthdr.nochdr.priority=tile_priority;


    flit_out->simdata.gtime = sc_time_stamp();
    flit_out->simdata.ctime = sc_time_stamp();
    flit_out->simdata.atime = SC_ZERO_TIME;
    flit_out->simdata.atimestamp = 0;
    flit_out->simdata.num_waits = 0;
    flit_out->simdata.num_sw = 0;
    flit_out->simdata.gtimestamp = sim_count;

    return flit_out;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a tail flit
/// Parameters:
/// - packet id
/// - flit id
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////
flit* ipcore::create_tail_flit(int pkt_id, int flit_id)
{
    flit *flit_out = new flit;
    flit_out->pkttype = NOC;
    flit_out->vcid = 0;
    flit_out->src = tileID;

    flit_out->pkthdr.nochdr.flittype = TAIL;
    flit_out->pkthdr.nochdr.pktid = pkt_id;
    flit_out->pkthdr.nochdr.flitid = flit_id;


    flit_out->pkthdr.nochdr.priority=tile_priority;

    flit_out->simdata.gtime = sc_time_stamp();
    flit_out->simdata.ctime = sc_time_stamp();
    flit_out->simdata.atime = SC_ZERO_TIME;
    flit_out->simdata.atimestamp = 0;
    flit_out->simdata.num_waits = 0;
    flit_out->simdata.num_sw = 0;
    flit_out->simdata.gtimestamp = sim_count;

    return flit_out;
}

///////////////////////////////////////////////////////////////////////////
/// Method to assign value to command field of a flit
/// Parameters:
/// - pointer to flit
/// - integer representing command
///////////////////////////////////////////////////////////////////////////
void ipcore::set_cmd(flit* inflit, int cmd_value)
{
    if (inflit->pkthdr.nochdr.flittype == HEAD || inflit->pkthdr.nochdr.flittype == HDT)
    {
        inflit->pkthdr.nochdr.flithdr.header.datahdr.cmd = cmd_value;
    }
    else
    {
        inflit->pkthdr.nochdr.flithdr.payload.cmd = cmd_value;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to assign value to integer data field of a flit
/// Parameters:
/// - pointer to flit
/// - integer data
///////////////////////////////////////////////////////////////////////////
void ipcore::set_data(flit* inflit, int data_int_value)
{
    if (inflit->pkthdr.nochdr.flittype == HEAD || inflit->pkthdr.nochdr.flittype == HDT)
    {
        inflit->pkthdr.nochdr.flithdr.header.datahdr.data_int = data_int_value;
    }
    else
    {
        inflit->pkthdr.nochdr.flithdr.payload.data_int = data_int_value;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to assign value to string data field of a flit
/// Parameters:
/// - pointer to flit
/// - string data
///////////////////////////////////////////////////////////////////////////
void ipcore::set_data(flit* inflit, string data_string_value)
{
    if (inflit->pkthdr.nochdr.flittype == HEAD || inflit->pkthdr.nochdr.flittype == HDT)
    {
        inflit->pkthdr.nochdr.flithdr.header.datahdr.data_string = data_string_value.c_str();
    }
    else
    {
        inflit->pkthdr.nochdr.flithdr.payload.data_string = data_string_value.c_str();
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to assign value to both command and integer data field of a flit
/// Parameters:
/// - pointer to flit
/// - integer representing command
/// - integer data
///////////////////////////////////////////////////////////////////////////
void ipcore::set_payload(flit* inflit, int cmd_value, int data_int_value)
{
    if (inflit->pkthdr.nochdr.flittype == HEAD || inflit->pkthdr.nochdr.flittype == HDT)
    {
        inflit->pkthdr.nochdr.flithdr.header.datahdr.cmd = cmd_value;
        inflit->pkthdr.nochdr.flithdr.header.datahdr.data_int = data_int_value;
    }
    else
    {
        inflit->pkthdr.nochdr.flithdr.payload.cmd = cmd_value;
        inflit->pkthdr.nochdr.flithdr.payload.data_int = data_int_value;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to assign value to both command and string data field of a flit
/// Parameters:
/// - pointer to flit
/// - integer representing command
/// - string data
///////////////////////////////////////////////////////////////////////////
void ipcore::set_payload(flit* inflit, int cmd_value, string data_string_value)
{
    if (inflit->pkthdr.nochdr.flittype == HEAD || inflit->pkthdr.nochdr.flittype == HDT)
    {
        inflit->pkthdr.nochdr.flithdr.header.datahdr.cmd = cmd_value;
        inflit->pkthdr.nochdr.flithdr.header.datahdr.data_string = data_string_value.c_str();
    }
    else
    {
        inflit->pkthdr.nochdr.flithdr.payload.cmd = cmd_value;
        inflit->pkthdr.nochdr.flithdr.payload.data_string = data_string_value.c_str();
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return command field of a flit
/// Parameters:
/// - flit
/// - integer variable in which command value is returned
///////////////////////////////////////////////////////////////////////////
void ipcore::get_cmd(flit inflit, int &cmd_value)
{
    if (inflit.pkthdr.nochdr.flittype == HEAD || inflit.pkthdr.nochdr.flittype == HDT)
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.cmd;
    }
    else
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.payload.cmd;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return integer data field of a flit
/// Parameters:
/// - flit
/// - integer variable in which integer data value is returned
///////////////////////////////////////////////////////////////////////////
void ipcore::get_data(flit inflit, int &data_int_value)
{
    if (inflit.pkthdr.nochdr.flittype == HEAD || inflit.pkthdr.nochdr.flittype == HDT)
    {
        data_int_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.data_int;
    }
    else
    {
        data_int_value = inflit.pkthdr.nochdr.flithdr.payload.data_int;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return string data field of a flit
/// Parameters:
/// - flit
/// - string variable in which string data value is returned
///////////////////////////////////////////////////////////////////////////
void ipcore::get_data(flit inflit, string &data_string_value)
{
    if (inflit.pkthdr.nochdr.flittype == HEAD || inflit.pkthdr.nochdr.flittype == HDT)
    {
        data_string_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.data_string;
    }
    else
    {
        data_string_value = inflit.pkthdr.nochdr.flithdr.payload.data_string;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return both command field and integer data field of a flit
/// Parameters:
/// - flit
/// - integer variable in which command value is returned
/// - integer variable in which integer data value is returned
///////////////////////////////////////////////////////////////////////////
void ipcore::get_payload(flit inflit, int &cmd_value, int &data_int_value)
{
    if (inflit.pkthdr.nochdr.flittype == HEAD || inflit.pkthdr.nochdr.flittype == HDT)
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.cmd;
        data_int_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.data_int;
    }
    else
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.payload.cmd;
        data_int_value = inflit.pkthdr.nochdr.flithdr.payload.data_int;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to return both command field and string data field of a flit
/// Parameters:
/// - flit
/// - integer variable in which command value is returned
/// - string variable in which string data value is returned
///////////////////////////////////////////////////////////////////////////
void ipcore::get_payload(flit inflit, int &cmd_value, string &data_string_value)
{
    if (inflit.pkthdr.nochdr.flittype == HEAD || inflit.pkthdr.nochdr.flittype == HDT)
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.cmd;
        data_string_value = inflit.pkthdr.nochdr.flithdr.header.datahdr.data_string;
    }
    else
    {
        cmd_value = inflit.pkthdr.nochdr.flithdr.payload.cmd;
        data_string_value = inflit.pkthdr.nochdr.flithdr.payload.data_string;
    }
}
