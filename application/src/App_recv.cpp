
/*
 * App_recv.cpp
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
/// \file App_recv.cpp
/// \brief Implements sink application (for demo string concatenation) to recieve flit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "App_recv.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
App_recv::App_recv(sc_module_name App_recv): ipcore(App_recv)
{

}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
////////////////////////////////////////////////
// you should include the process definition even if it is empty
// do not delete!
void App_recv::send()
{
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - recieve incoming flits
/// - assign arrival timestamps
////////////////////////////////////////////////
void App_recv::recv()
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

            // generate logs
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd<<flit_recd.simdata;
            cout<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd;
            get_data(flit_recd, data);
            cout<<"Recieved data: "<<data<<endl;
            if (LOG >= 1)
                eventlog<<"Recieved data: "<<data<<endl;
        }
    }
}

// for dynamic linking
extern "C"
{
    ipcore *maker()
    {
        return new App_recv("App_recv");
    }
}
