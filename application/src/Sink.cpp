
/*
 * Sink.cpp
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
/// \file Sink.cpp
/// \brief Implements sink application to recieve flits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Sink.h"
#include "../../config/extern.h"
#include <fstream>
////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
Sink::Sink(sc_module_name Sink): ipcore(Sink)
{

}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
////////////////////////////////////////////////
// you should include the process definition even if it is empty
void Sink::send()
{
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - recieve incoming flits
/// - assign arrival timestamps
////////////////////////////////////////////////
void Sink::recv()
{
////////////**********************************************/////////////////////////////////////
    char s1[9],s2[9];
    char str_id[3];
    sprintf(str_id, "%d", tileID);
    string traffic_filename = string("testing/tile-") + string(str_id);
    ofstream instream;
    instream.open(traffic_filename.c_str());
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

            instream<<flit_recd.simdata.atime<<flit_recd;

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
//				cout<<"RECV P "<<flit_recd.pkthdr.nochdr.priority<<" source "<<flit_recd.src<<endl;
                //filGT.open(nirgam_filename2.c_str(),ios::app);
                //filGT<<"\nCount so far : "<<countGT<<"\tAvg latency so far: "<<((float)sumGT/countGT);
                //filGT.close();
            }
            else if (flit_recd.pkthdr.nochdr.priority==0)
            {
                sumBE+=difference;
                countBE++;
//				cout<<"RECV P "<<flit_recd.pkthdr.nochdr.priority<<" source "<<flit_recd.src<<endl;
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

// for dynamic linking
extern "C"
{
    ipcore *maker()
    {
        return new Sink("Sink");
    }
}
