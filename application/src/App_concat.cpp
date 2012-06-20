
/*
 * App_concat.cpp
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
/// \file App_concat.cpp
/// \brief Implements string concatenation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "App_concat.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
App_concat::App_concat(sc_module_name App_concat): ipcore(App_concat)
{

}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
////////////////////////////////////////////////
// you should include the process definition even if it is empty
// do not delete!
void App_concat::send()
{
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - recieve incoming flits
/// - assign arrival timestamps
/// - send them for processing (concatenation)
////////////////////////////////////////////////
void App_concat::recv()
{
    string data;
    while (true)
    {
        wait();	// wait until inport event
        if (flit_inport.event())
        {
            flit flit_recd = flit_inport.read();	// read incoming flit
            flit_recd.simdata.atimestamp = sim_count;	// record arrival time (in clock cycles)
            flit_recd.simdata.atime = sc_time_stamp();	// record absolute arrival time
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd<<flit_recd.simdata;
            cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd;
            get_data(flit_recd, data);
            if (LOG >= 1)
                eventlog<<"Recieved data: "<<data;
            cout<<"Recieved data: "<<data;

            processing(flit_recd);	// call method to process flit
        }
    }
}

////////////////////////////////////////////////
/// Method to process incoming flits
/// - concatenate strings recieved in flits
/// - send concatenated flit on recieving tail flit
////////////////////////////////////////////////
void App_concat::processing(flit newflit)
{
    string data;
    get_data(newflit, data);	// read string data field of newflit in string variable: data
    final = final + data + " ";	// concatenate
    if (LOG >= 1)
    {
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Processing flit at core "<<newflit;
        eventlog<<"Concatenate, new data: "<<final<<endl;
    }
    cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Processing flit at core "<<newflit;
    cout<<"Concatenate, new data: "<<final<<endl;

    // if tail flit or last flit
    if (newflit.pkthdr.nochdr.flittype == TAIL || newflit.pkthdr.nochdr.flittype == HDT)
    {
        send_concat_string(final);	// send final concatenated string to other tile (tile 6)
        final = string("\0");		// reset final concatenated string to null
    }
}

////////////////////////////////////////////////
/// Method to send a flit consisting of concatenated string to destination tile
/// Parameter: string to be sent
////////////////////////////////////////////////
void App_concat::send_concat_string(string temp_data)
{
    flit *flit_out;
    string data;

    // create hdt flit, parameters: pkt id = 0, flit id = 0, destination = 6
    flit_out = create_hdt_flit(0,0,6);
    // assign string recieved as parameter to flit payload
    set_data(flit_out, temp_data);

    if (LOG >= 1)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<*flit_out;
    get_data(*flit_out, data);
    cout<<"Sending data: "<<data<<endl;
    if (LOG >= 1)
        eventlog<<"Sending data: "<<data<<endl;

    // write flit to output port
    flit_outport.write(*flit_out);
}

// for dynamic linking
extern "C"
{
    ipcore *maker()
    {
        return new App_concat("App_concat");
    }
}
