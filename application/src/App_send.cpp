
/*
 * App_send.cpp
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
/// \file App_send.cpp
/// \brief Implements source application (for demo string concatenation) to send flits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "App_send.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
App_send::App_send(sc_module_name App_send): ipcore(App_send)
{
}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
/// - sends flits for demo string concatenation application
/// - sends a packet consisting of 3 flits at an interval of 2 clock cycles to tileID 5
/// - each flit contains a string to be concatenated in its payload
////////////////////////////////////////////////
void App_send::send()
{
    flit *flit_out;	// pointer to flit data structure
    string data;
    wait(WARMUP);	// wait for clock cycles = WARMUP
    //int dst = get_random_dest();		// uncomment and use dst to send flits to a random destination

    // create a head flit, parameters: packet id, flit id, destination
    flit_out = create_head_flit(0,0,5);
    // set string data field of created flit
    set_data(flit_out, "Network");

    // generate logs
    if (LOG >= 1)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    get_data(*flit_out, data);
    if (LOG >= 1)
        eventlog<<"Sending data: "<<data<<endl;
    cout<<"Sending data: "<<data<<endl;

    if (credit_in[0].read().freeBuf)
        flit_outport.write(*flit_out);		// write flit to output port

    wait(2);	// wait for 2 clock cycles
    flit_out = create_data_flit(0,1);	// create data flit: pkt id = 0, flit id = 1
    set_data(flit_out, "on");		// assign string to flit payload
    // generate logs
    if (LOG >= 1)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    get_data(*flit_out, data);
    if (LOG >= 1)
        eventlog<<"Sending data: "<<data<<endl;
    cout<<"Sending data: "<<data<<endl;

    if (credit_in[0].read().freeBuf)
        flit_outport.write(*flit_out);	// write flit to output port

    wait(2);	// wait for 2 clock cycles
    flit_out = create_tail_flit(0,2);	// create tail flit: pkt id = 0, flit id = 2
    set_data(flit_out, "Chip");		// assign string to flit payload
    // generate logs
    if (LOG >= 1)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    get_data(*flit_out, data);
    if (LOG >= 1)
        eventlog<<"Sending data: "<<data<<endl;
    cout<<"Sending data: "<<data<<endl;

    if (credit_in[0].read().freeBuf)
        flit_outport.write(*flit_out);	// write flit to output port
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - recieve incoming flits
/// - assign arrival timestamps
////////////////////////////////////////////////
void App_send::recv()
{
    while (true)
    {
        wait();	// wait until flit inport event
        if (flit_inport.event())
        {
            flit flit_recd = flit_inport.read();	// read incoming flit
            flit_recd.simdata.atimestamp = sim_count;	// record arrival time (in clock cycles)
            flit_recd.simdata.atime = sc_time_stamp();	// record absolute arrival time
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd<<flit_recd.simdata;
        }
    }
}

////////////////////////////////////////////////
/// Method to process incoming flits
////////////////////////////////////////////////
void App_send::processing(flit newflit)
{

}

// for dynamic linking
extern "C"
{
    ipcore *maker()
    {
        return new App_send("App_send");
    }
}
