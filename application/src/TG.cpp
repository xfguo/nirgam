
/*
 * TG.cpp
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
/// \file TG.cpp
/// \brief Implements abstract traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TG.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
TrafficGenerator::TrafficGenerator(sc_module_name TrafficGenerator): ipcore(TrafficGenerator)
{
    flit_interval = 1;	// inter flit interval
    cycles_per_flit = 2;	// clock cycles required for processing of a flit
//	cout<<"\nTG.CPP\n";
}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
/// - send flits as per traffic configuration
////////////////////////////////////////////////
void TrafficGenerator::send()
{
    int num_pkts_gen = 0;	// initialize number of packets generated to zero
    wait(WARMUP);		// wait for WARMUP period

    string field;
////////////**********************************************/////////////////////////////////////

    /*	string nirgam_filename = string("GENERATED_FLITS.txt");// edit
    	ofstream fil1; // edit
    string nirgam_filename_GT = string("GENERATED_GT_FLITS.txt");// edit
    	ofstream filGT; // edit
    string nirgam_filename_BE = string("GENERATED_BE_FLITS.txt");// edit
    	ofstream filBE; // edit
    */
////////////**********************************************/////////////////////////////////////

    // open traffic log file
    char str_id[3];
    sprintf(str_id, "%d", tileID);
    string traffic_filename = string("log/traffic/tile-") + string(str_id);
    ifstream trafstream;
    trafstream.open(traffic_filename.c_str());

    string traffic_filename1 = string("testingsend/tile-") + string(str_id);
    ofstream sending;
    sending.open(traffic_filename1.c_str());

    // read inter-flit interval
    trafstream >> field >> flit_interval;

    // generate traffic until TG_NUM
    while (sim_count <= TG_NUM && !trafstream.eof())
    {

        // read inter-pkt interval
        trafstream >> field >> next_pkt_time;
        // read no. of flits in packet
        trafstream >> field >> num_flits;
        // read destination or route code
        trafstream >> field >> route_info;
        /*added*/
        if (route_info==tileID)
            continue;
        /*end*/

        int num_flits_gen = 0;
        flit flit_out;
///////////**********************************************/////////////////////////////////////
        /*	fil1.open(nirgam_filename.c_str(),ios::app);
        	filGT.open(nirgam_filename_GT.c_str(),ios::app);
        	filBE.open(nirgam_filename_BE.c_str(),ios::app);*/
////////////**********************************************/////////////////////////////////////
        // create hdt/head flit
        if (num_flits == 1)
        {
            flit_out = *create_hdt_flit(num_pkts_gen, num_flits_gen, route_info);
////////////**********************************************/////////////////////////////////////
            if (flit_out.pkthdr.nochdr.priority==0)
                genbe++;
            else if (flit_out.pkthdr.nochdr.priority==1)
                gengt++;
////////////**********************************************/////////////////////////////////////
        }
        else
        {
////////////**********************************************/////////////////////////////////////
            //	fil1<<"HEAD\n";
////////////**********************************************/////////////////////////////////////
            flit_out = *create_head_flit(num_pkts_gen, num_flits_gen, route_info);
/////////////******************************************************//////////////////////////////////
            if (flit_out.pkthdr.nochdr.priority==0)
                genbe++;
            else if (flit_out.pkthdr.nochdr.priority==1)
                gengt++;
//////////////********************************************************////////////////////////////////
        }
////////////**********************************************/////////////////////////////////////
        /*			fil1.close();
        			filGT.close();
        			filBE.close(); */
////////////**********************************************/////////////////////////////////////
        if (LOG >= 1)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<flit_out;

        // wait while buffer has space
        while (!credit_in[0].read().freeBuf)
        {
            //cout<<"Time: "<<sc_time_stamp()<<" ipcore: "<<tileID<<" No space in core buffer"<<endl;
            wait();
            next_pkt_time--;
        }

        // write flit to output port

        flit_outport.write(flit_out);
        sending<<sc_time_stamp()<<flit_out;
        if (LOG >= 1)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Sending flit from core "<<flit_out;

        num_flits_gen = 1;
        // generate subsequent flits in packet
        while (num_flits_gen < num_flits)
        {
            wait(flit_interval);
            next_pkt_time -= flit_interval;

            // create flit
///////////**********************************************/////////////////////////////////////
            /*				fil1.open(nirgam_filename.c_str(),ios::app);
            				filGT.open(nirgam_filename_GT.c_str(),ios::app);
            				filBE.open(nirgam_filename_BE.c_str(),ios::app); */
////////////**********************************************/////////////////////////////////////
            if (num_flits_gen == num_flits-1)
            {
                flit_out = *create_tail_flit(num_pkts_gen, num_flits_gen);
////////////**********************************************/////////////////////////////////////
                if (flit_out.pkthdr.nochdr.priority==0)
                    genbe++;
                else if (flit_out.pkthdr.nochdr.priority==1)
                    gengt++;

////////////**********************************************/////////////////////////////////////
            }
            else
            {
                flit_out = *create_data_flit(num_pkts_gen, num_flits_gen);
////////////**********************************************/////////////////////////////////////
                if (flit_out.pkthdr.nochdr.priority==0)
                    genbe++;
                else if (flit_out.pkthdr.nochdr.priority==1)
                    gengt++;

////////////**********************************************/////////////////////////////////////
            }
////////////**********************************************/////////////////////////////////////
            /*	fil1.close();
            	filGT.close();
            	filBE.close(); */
////////////**********************************************/////////////////////////////////////

            num_flits_gen++;

            //trafstream<<sim_count<<endl;
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<flit_out;

            // wait until space in buffer
            while (!credit_in[0].read().freeBuf)
            {
                //cout<<"Time: "<<sc_time_stamp()<<" ipcore: "<<tileID<<" No space in core buffer"<<endl;
                wait();
                next_pkt_time--;
            }

            // send flit

            flit_outport.write(flit_out);
            sending<<sc_time_stamp()<<flit_out;
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Sending flit from core "<<flit_out;

        }

        num_pkts_gen++;
        if (next_pkt_time > 0)
            wait(next_pkt_time);
        else wait(1);
    }
    trafstream.close();	// close traffic config file
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - recieve incoming flits
/// - assign arrival timestamps
////////////////////////////////////////////////
void TrafficGenerator::recv()
{
    ////////////**********************************************/////////////////////////////////////
    char s1[9],s2[9];

    /*
    string nirgam_filename = string("OVERALL_LATENCYRESULT.txt");
    	ofstream fil1;
    string nirgam_filename2 = string("GT_LATENCYRESULT.txt");
    	ofstream filGT;
    string nirgam_filename3 = string("BE_LATENCYRESULT.txt");
    	ofstream filBE;
    	static float sum=0;
    	static float sumGT=0,sumBE=0;
    	static int count=0, countGT=0, countBE=0;
    	static int maximum=0;
    	static int minimum=99999; */
    /*
    extern float sum;
    extern float sumGT,sumBE;
    extern int count,countGT,countBE;
    extern int maximum;
    extern int minimum;
    */
    ofstream jitter;
    int difference;
    sum=0;
    sumGT=0;
    sumBE=0;
//count_total=0;
    countGT=0;
    countBE=0;
    maximum=0;
    minimum=99999;


////////////**********************************************/////////////////////////////////////
    while (true)
    {

        wait();	// wait until inport event

        if (flit_inport.event())
        {
////////////**********************************************/////////////////////////////////////
            difference=0;

////////////**********************************************/////////////////////////////////////
            flit flit_recd = flit_inport.read();	// read incoming flit
            flit_recd.simdata.atimestamp = sim_count;	// record arrival time (in clock cycles)
            flit_recd.simdata.atime = sc_time_stamp();	// record absolute arrival time
////////////**********************************************/////////////////////////////////////
            //fil1.open(nirgam_filename.c_str(),ios::app);
            difference=flit_recd.simdata.atimestamp-flit_recd.simdata.gtimestamp;
            //if(flit_recd.pkthdr.nochdr.flittype==
            if (maximum<difference)
                maximum=difference;
            if (minimum>difference)
                minimum=difference;
            count_total++;
            sum+=difference;

            if (flit_recd.pkthdr.nochdr.flittype==HEAD || flit_recd.pkthdr.nochdr.flittype==HDT)
            {

                /////////////////////****JITTER ***///////////////////////
                if (flit_recd.pkthdr.nochdr.priority==1) // GT
                {
                    sprintf(s1,"%d",int(flit_recd.src));
                    sprintf(s2,"%d",int(flit_recd.pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst));
                    string jitter_filename2=string("jitter/GT/pair-")+string(s1)+string("-")+string(s2);
                    jitter.open(jitter_filename2.c_str(),ios::app);
                    jitter<<difference<<"\n";
                    jitter.close();

                }
                else if (flit_recd.pkthdr.nochdr.priority==0) // BE
                {
                    sprintf(s1,"%d",int(flit_recd.src));
                    sprintf(s2,"%d",int(flit_recd.pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst));
                    string jitter_filename2=string("jitter/BE/pair-")+string(s1)+string("-")+string(s2);
                    jitter.open(jitter_filename2.c_str(),ios::app);
                    jitter<<difference<<"\n";
                    jitter.close();

                }

                //////////////////////////////////////////////////////////
                //fil1<<"\nCount so far : "<<count_total<<"\tAvg latency so far: "<<((float)sum/count_total)<<"\t maximum so far : "<<maximum<<"\tMin so far : "<<minimum<<"Source is "<<flit_recd.src<<"Dest is "<<flit_recd.pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst;
            }
            //else
            //fil1<<"\nCount so far : "<<count_total<<"\tAvg latency so far: "<<((float)sum/count_total)<<"\t maximum so far : "<<maximum<<"\tMin so far : "<<minimum;
            //fil1.close();

            if (flit_recd.pkthdr.nochdr.priority==1)
            {
                sumGT+=difference;
                countGT++;
                //filGT.open(nirgam_filename2.c_str(),ios::app);
                //filGT<<"\nCount so far : "<<countGT<<"\tAvg latency so far: "<<((float)sumGT/countGT);
                //filGT.close();
            }
            else if (flit_recd.pkthdr.nochdr.priority==0)
            {
                sumBE+=difference;
                countBE++;
                //filBE.open(nirgam_filename3.c_str(),ios::app);
                //filBE<<"\nCount so far : "<<countBE<<"\tAvg latency so far: "<<((float)sumBE/countBE);
                //filBE.close();
            }
////////////**********************************************/////////////////////////////////////
            if (LOG >= 1)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Recieved flit at core "<<flit_recd<<flit_recd.simdata;
        }
    }

}

////////////////////////////////////////////////
/// Method to convert time unit from string representation to systemC representation
////////////////////////////////////////////////
sc_time_unit TrafficGenerator::strToTime(string unit)
{
    if (unit == "s")
        return SC_SEC;
    if (unit == "ms")
        return SC_MS;
    if (unit == "us")
        return SC_US;
    if (unit == "ns")
        return SC_NS;
    if (unit == "ps")
        return SC_PS;
    if (unit == "fs")
        return SC_FS;
}


