
/*
 * Bursty.cpp
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
/// \file Bursty.cpp
/// \brief Implements Varying traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VBR.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
VaryingTraffic::VaryingTraffic(sc_module_name VaryingTraffic) : TrafficGenerator(VaryingTraffic)
{
    // clear traffic log
    system("rm -f log/traffic/*");

    char str_id[3];
    ofstream trafstream;
    hotspot=-1;
    // create new traffic log file
    for (int i = 0; i < num_tiles; i++)
    {
        sprintf(str_id, "%d", i);
        string traffic_filename = string("log/traffic/tile-") + string(str_id);
        trafstream.open(traffic_filename.c_str());
        trafstream.close();
    }
    // thread sensitive to clock
    SC_CTHREAD(init, clock.pos());
}

////////////////////////////////////////////////
/// Process to generate Varying traffic
/// - read traffic configuration file
/// - generate traffic pattern in log file
////////////////////////////////////////////////
void VaryingTraffic::init()
{
    /*added*/
    traffic_patterns *tpattern;
    float *Probmat;
    NEDTraffic * nedt = new NEDTraffic(num_rows,num_cols,tileID);
    Probmat= new float(num_rows*num_cols);
    // open traffic config file
    char str_id[3];
    double rand_no;
    hsrng=new UniformRandomVariable(0,1);
    sprintf(str_id, "%d", tileID);
    string traffic_filename = string("config/traffic/tile-") + string(str_id);
    ifstream instream;
    instream.open(traffic_filename.c_str());

    while (!instream.eof())
    {
        string field;
        instream >> field;
        if (field == "PKT_SIZE")  	// read packet size
        {
            int value;
            instream >> value;
            pkt_size = value;
        }
        else if (field == "LOAD1")  	// read load percentage
        {
            int value;
            instream >> value;
            load1 = value;
        }
        else if (field == "LOAD2")  	// read load percentage
        {
            int value;
            instream >> value;
            load2 = value;
        }
        else if (field == "BURST_TIME")  	// read average burst length
        {
            int value;
            instream >> value;
            bursttime = value;
        }
        else if (field == "OFF_TIME")  	// read average offtime
        {
            int value;
            instream >> value;
            offtime = value;
        }
        else if (field == "DESTINATION")
        {
            instream >> dst_type;	// read destination type
            if (dst_type == "FIXED")  	// if fixed destination, read destination tileID or route code
            {
                int value;
                instream >> value;
                route_info = value;
            }
            /*added*/
            else if (dst_type=="TRANSPOSE")
            {
                route_info=tpattern->trafficTranspose(tileID,num_rows,num_cols);
            }

            else if (dst_type=="BIT_REVERSAL")
            {
                route_info=tpattern->trafficBitReversal(tileID,num_rows,num_cols);
            }
            else if (dst_type=="BIT_SHUFFLE")
            {
                route_info=tpattern->trafficShuffle(tileID,num_rows,num_cols);
            }
            else if (dst_type=="BUTTERFLY")
            {
                route_info=tpattern->trafficButterfly(tileID,num_rows,num_cols);
            }
            else if (dst_type=="NED")
            {

                nedt->get_NEDTraffic(Probmat);
                Probmat[tileID]=0.0;
                for (int i=1 ; i<num_rows*num_cols; i++)
                {

                    Probmat[i]=Probmat[i]+Probmat[i-1];
                }

//                for (int i=0; i<num_rows; i++)
//                {
//                    cout<<"\n\n";
//                    for (int j=0; j<num_rows; j++)
//                    {
//                        cout<<Probmat[i*num_rows+j]<<" ";
//                    }
//                }
                delete nedt;
            }
            /*end*/
        }
        else if (field == "FLIT_INTERVAL")  	// read inter-flit interval
        {
            int value;
            instream >> value;
            flit_interval = value;
        }
        else if (field == "HOTSPOT")  	// read inter-flit interval
        {
            int value;
            instream >> value;
            hotspot = value;
            double prob;
            instream >> prob;
            hotspot_prob = prob;
        }
    }

    instream.close();

    // compute number of flits in packet (function of pkt size and flit size)
    num_flits = (int)((ceil)((float)(pkt_size - HEAD_PAYLOAD)/(DATA_PAYLOAD)) + 1);
    // compute inter-packet interval
    pkt_interval1 = (int)((ceil)(100.0/load1) * num_flits * cycles_per_flit);
    pkt_interval2 = (int)((ceil)(100.0/load2) * num_flits * cycles_per_flit);
    curr = 0;
    // open traffic log file
    traffic_filename = string("log/traffic/tile-") + string(str_id);
    ofstream trafstream;
    trafstream.open(traffic_filename.c_str());
    // write inter-flit interval
    trafstream<<"FLIT_INTERVAL: "<<flit_interval<<endl;
    for (int i = WARMUP; i <= TG_NUM; i++ )
    {
        next_pkt_time = next_interval();

        trafstream<<"NEXT_PKT_INTERVAL: "<<next_pkt_time<<endl;	// write next packet interval
        if (dst_type == "RANDOM")
        {
            if (hotspot!=-1)
            {
                rand_no=hsrng->value();
                if (rand_no<=hotspot_prob)
                    route_info=hotspot;
                else
                    route_info = get_random_dest();		// get random destination
            }
            else
            {
                route_info = get_random_dest();		// get random destination
            }
        }
        else if (dst_type=="NED")
        {
            rand_no=hsrng->value();
            for (route_info=0 ; route_info<num_rows*num_cols; route_info++)
            {
                if (Probmat[route_info]>=rand_no)
                    break;
            }
        }
        trafstream<<"NUM_FLITS: "<<num_flits<<endl;	// write number of flits
        trafstream<<"DESTINATION: "<<route_info<<endl;	// write destination ID or route code

    }
    trafstream.close();	// close traffic log file
}

////////////////////////////////////////////////
/// Method to return inter-packet interval
////////////////////////////////////////////////
double VaryingTraffic::next_interval()
{
//    cout<<"curr :"<<curr<<" calc "<<curr % (offtime+bursttime)<<"off time"<<offtime<<endl;
    if (curr % (offtime+bursttime) < offtime )
    {
        curr += pkt_interval1;
        return pkt_interval1;
    }
    else
    {
        curr += pkt_interval2;
        return pkt_interval2;
    }
}

// for dynamic linking
extern "C"
{
    TrafficGenerator *maker()
    {
        return new VaryingTraffic("TG");
    }
}
