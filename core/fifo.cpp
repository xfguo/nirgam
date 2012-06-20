
/*
 * fifo.cpp
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
/// \file fifo.cpp
/// \brief Implements first-in-first-out buffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "flit.h"
#include "fifo.h"

////////////////////////////////////////////////////////
/// Method to insert a flit in fifo
////////////////////////////////////////////////////////
void fifo::flit_in(const flit& data_flit)
{
    regs[pntr++] = data_flit;	// insert flit in register array
    empty = false;	// set empty status to false
    if (pntr == num_bufs) full = true;	// if fifo is full, set full status to true

    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" Inserting flit, pntr: "<<pntr<<" num_bufs: "<<num_bufs<<" full: "<<full;

}

///////////////////////////////////////////////////////
/// Method to read a flit from fifo
///////////////////////////////////////////////////////
flit fifo::flit_out()
{
    flit temp;
    temp = regs[0];		// read flit in front of queue
    if (--pntr==0) empty = true;
    else
        for (sc_uint<BUF_BITSIZE> i=0; i<=pntr-1; i++)
            regs[i] = regs[i+1];
    if (pntr!=num_bufs) full = false;

    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" Removing flit, pntr: "<<pntr;
    return(temp);
}
///////////////////////////////////////////////////////
/// Method to read a flit from fifo without removing it
///////////////////////////////////////////////////////
flit fifo::flit_read()
{
    flit temp;
    temp = regs[0];		// read flit in front of queue
    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" Reading flit, pntr: "<<pntr;
    return(temp);
}
///////////////////////////////////////////////////////////////////////////
/// Method to push back a flit in its original position in fifo
///////////////////////////////////////////////////////////////////////////
void fifo::flit_push(flit pack)
{
    pntr++;
    empty = false;
    if (pntr == num_bufs) full = true;
    for (sc_uint<BUF_BITSIZE> i = pntr; i > 0; i--)
        regs[i] = regs[i-1];
    regs[0] = pack;
    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" Pushing back flit, pntr: "<<pntr;
}
