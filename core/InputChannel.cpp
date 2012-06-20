
/*
 * InputChannel.cpp
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
/// \file InputChannel.cpp
/// \brief Implements module InputChannel (reads and processes incoming flits)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputChannel.h"
#include "../config/extern.h"

extern string app_libname[MAX_NUM_TILES];

////////////////////////
/// Constructor
////////////////////////
template<UI num_op>
InputChannel<num_op>::InputChannel(sc_module_name InputChannel): sc_module(InputChannel)
{

    // process sensitive to inport event, reads in flit and stores in buffer
    SC_THREAD(read_flit);
    sensitive << inport;

    // transmit flit at the front of fifo to output port at each clock cycle
    SC_THREAD(transmit_flit);
    sensitive_pos << switch_cntrl;
    sensitive << vcReady;

    // route flit at the front of fifo if required
    SC_THREAD(route_flit);
    sensitive_pos << switch_cntrl;
    sensitive << rtReady;

    // initialize VC request to false
    vcRequest.initialize(false);

    // initialize route request to NONE
    rtRequest.initialize(NONE);

    // initialize virtual channels and buffers
    for (UI i=0; i < NUM_VCS ; i++)
    {
        vc[i].vcQ.num_bufs = NUM_BUFS;
        vc[i].vcQ.pntr = 0;
        vc[i].vcQ.full = false;
        vc[i].vcQ.empty = true;
        vc[i].vc_route = 5;
        vc[i].vc_next_id = NUM_VCS + 1;
        //*******************************************************
        vc[i].id=i;
        //**********************************************************
    }

    // reset buffer counts to zero
    resetCounts();

    // Send initial credit info (buffer status)
    for (int i=0; i < NUM_VCS ; i++)
    {
        creditLine t;
        t.freeVC = true;
        t.freeBuf = true;
        credit_out[i].initialize(t);
    }


    //for results
    rs.pkt_in = 0;
    rs.pkt_out = 0;
    rs.est_in = 0;
    rs.buffer_size = 0;
    rs.pkt_in_core = 0;
    rs.pkt_dropped = 0;
    //results

}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to inport event
/// Reads flit from input port and calls function to store in buffer
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op> :: read_flit()
{
    //flit that is read into the input channel
    flit flit_in;
    while (true)
    {
        wait(); // waiting for next flit
        // inport event
        if (inport.event())
        {

            flit_in = inport.read();	// read flit
            flit_in.simdata.ICtimestamp = sim_count - 1;	// set input timestamp (required for per channel latency stats)
            //cout << "ICtimestamp in IChannel :: "<<	flit_in.simdata.ICtimestamp<<endl;
            if (LOG >= 2)
                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrl: "<<cntrlID <<" Inport event!"<<" flitID: "<<flit_in;
//			cout<<"\nPKT TYPE(reading frm tg)= "<<flit_in.pkttype<<" FLIT TYPE(reading frm tg)= "<<flit_in.pkthdr.nochdr.flittype<<"\n";
            switch (flit_in.pkttype)
            {

            case ANT:
                break;
                /*case EST:		//same as NOC packet
                //	num_est_ic_rcv++; //will be removed
                	if(LOG >= 4)
                		eventlog<<this->name()<<":(recEst) Received estimate packet!"<<endl;*/
            case NOC:
                store_flit_VC(&flit_in);	// store flit in buffer
                break;

            } // end switch pkt type
            // find buffers and VCs occupied
            numBufsOcc = 0;
            numVCOcc = 0;
            for (int i = 0; i < NUM_VCS; i++)
            {
                numBufsOcc += vc[i].vcQ.pntr;
                if (vc[i].vc_next_id != NUM_VCS+1) numVCOcc++;
            }

        } // end if inport
    } // end while
} //end entry()

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Calls routing functions if head/hdt flit at the front of fifo
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op> :: route_flit()
{
    sim_count = 0;
    //int flag=1,flag2=1;
    while (sim_count < SIM_NUM)
    {
        wait();		// wait for next clock cycle
        flit flit_out;
        if (switch_cntrl.event())
        {
            sim_count++;
            //	cout<<this->name()<<":(router) "<<sim_count;

            //	flag=1;
            //	flag2=1;


            ULL vc_to_serve;

            if (cntrlID == C)	// assuming only 1 VC at IchannelC
                vc_to_serve = 0;
            else
                vc_to_serve = (sim_count-1) % NUM_VCS;	// serving VCs in round robin manner

            //cout<<"\nVC TO SERVE(ROUTE): "<<vc_to_serve;


            if (vc[vc_to_serve].vc_route == 5)  	// route not set, require routing
            {
                if (vc[vc_to_serve].vcQ.empty == false)  	// fifo not empty
                {
                    // read flit at front of fifo
                    //flag=0;

                    flit_out = vc[vc_to_serve].vcQ.flit_read();
                    //numBufReads++;

                    // call routing function depending on type of routing algorithm
                    switch (flit_out.pkttype)
                    {
                    case NOC:
                        if (flit_out.pkthdr.nochdr.flittype == HEAD || flit_out.pkthdr.nochdr.flittype == HDT)
                        {
                            /*added*/
                            numArb++;
                            //end
                            routing_type rt = flit_out.pkthdr.nochdr.flithdr.header.rtalgo;
                            routing_hdr *rt_hdr = &(flit_out.pkthdr.nochdr.flithdr.header.rthdr);

                            if (rt == SOURCE)
                            {
                                routing_src(&flit_out);
                            }
                            else
                            {
                                routing_dst(&flit_out);
                            }
                        }
                        break;
                        /*case EST:
                        	routing_dst(&flit_out);
                        	break;*/
                    case ANT:
                        break;
                    }

                    // push back flit in fifo
                    //if(flit_out.pkttype != EST)
                    //	vc[vc_to_serve].vcQ.flit_push(flit_out);
                    /*else{
                                            vc[vc_to_serve].vc_next_id = NUM_VCS+1;
                                                creditLine t; t.freeVC = true; t.freeBuf = true;
                                                credit_out[vc_to_serve].write(t);
                                                //if(cntrlID == C)
                                                vc[vc_to_serve].vc_route = 5;
                    }*/

                    if (LOG >= 2)
                        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrl: "<<cntrlID<<" Routing flit: "<<flit_out;
                }


                /*****************************************************/
                /*     else //ie vcQ is empty
                                {
                   flag=0;
                                   if(cntrlID!=C)//ie its not a core
                                      {
                                         float temp=float(NUM_VCS) * BW_GT;
                                         if(vc[vc_to_serve].id<int(temp))//ie its a GT VC
                                            {

                                       vc_to_serve = ((sim_count-1) % (int((NUM_VCS)-int((NUM_VCS) * BW_GT)))) + (int((NUM_VCS) * BW_GT));
                           	      flag=1;
                                        flag2=0;
                 			  }
                                       else if(vc[vc_to_serve].id>=int(temp) && flag2==0)//its a BE VC given chance when one of the GT VC is empty
                                              {
                  	      if(vc_to_serve+1<NUM_VCS)
                                                {
                                                    vc_to_serve++;
                  	          flag=1;
                  	       }
                                              }
                  	else //its  a BE VC
                			    {
                                                flag=0;
                  	      flag2=1;
                  	    }
                                      }
                                 }*/
                /***************************************/


            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Transmits flit at front of fifo to OC
/// - If head/hdt flit, send VC request
/// - write flit to output port if ready signal from OC
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op> :: transmit_flit()
{
    ULL sim_count = 0;

    //UI flag=1,flag2=1;

    while (sim_count < SIM_NUM)
    {
        wait();		// wait for next clock cycle
        flit flit_out;
        if (switch_cntrl.event())  	// clock event
        {
            sim_count++;
            //	cout<<this->name()<<":(transmit) "<<sim_count;

            // find buffers and VCs occupied
            /*			numBufsOcc = 0; numVCOcc = 0;
            			for(int i = 0; i < NUM_VCS; i++) {
            				numBufsOcc += vc[i].vcQ.pntr;
            				if(vc[i].vc_next_id != NUM_VCS+1) numVCOcc++;
            			}
            */
            // serve a VC in round robin manner
            ULL vc_to_serve;

            //flag=1;

            if (cntrlID == C)	// assuming only 1 VC at IchannelC
                vc_to_serve = 0;
            else
                vc_to_serve = (sim_count-1) % NUM_VCS;

            //	cout<<"\n VC TO SERVE (transmit flit) and tileid: "<<vc_to_serve<<" & "<<tileID;


            /*********************************************************************/
            /*     while(flag==1)
                     {
              flag=0;
               if(vc[vc_to_serve].vcQ.empty == true)//ie fifo is empty
                              {
                                flag=0;
              if(cntrlID != C)
               	     {
                                 if(vc_to_serve>=0 && vc_to_serve< int(NUM_VCS * BW_GT))
                                      {
              	vc_to_serve = ((sim_count-1) % (int((NUM_VCS)-int((NUM_VCS) * BW_GT)))) + (int((NUM_VCS) * BW_GT));
              	flag=1;
              	flag2=0;

              		}
                      else if(vc_to_serve>=int(NUM_VCS * BW_GT) && flag2==0)
                                       {
              		if(vc_to_serve+1<NUM_VCS)
                                                {
                                                 vc_to_serve++;
              			   flag=1;
              			  }
              		 }
              		else //its  a BE VC
            					    {
                                                    flag=0;
              			      flag2=1;
              			    }
                             }
                  }
                      }*/

            /*********************************************************************/



            if (vc[vc_to_serve].vc_route == 5)	// routing decision pending, before transmission
            {
                //if (tileID==0)
                //cout<<"\n flits that havent got routing info "<<vc[vc_to_serve].vc_route;
                continue;
            }

            // Routing decision has been made, proceed to transmission
            int oldvcid;

            UI i;
            switch (TOPO)
            {

            case TORUS:
                //#ifdef TORUS
                i = vc[vc_to_serve].vc_route;
                //#endif
                break;

            case MESH:
                //#ifdef MESH
                UI dir = vc[vc_to_serve].vc_route;
                switch (dir)
                {
                case N:
                    i = portN;
                    break;
                case S:
                    i = portS;
                    break;
                case E:
                    i = portE;
                    break;
                case W:
                    i = portW;
                    break;
                case C:
                    i = num_op - 1;
                    break;
                default:
                    cout<<"ERRORERRORERROR";
                }
                if (i == 5)
                {
                    cout<<this->name()<<endl;
                    cout<<"dir = "<<dir<<"	i = 5"<<endl;
#define dprint(value)	cout<<#value "= "<<value<<endl;
                    dprint(portN);
                    dprint(portS);
                    dprint(portE);
                    dprint(portW);
                    dprint(num_op);
                    //cin>>i;
                }
                /*added*/
                numArb++;
                /*end*/
#define iprint(value)   " " #value "= "<<value
                if (LOG == 10)
                    cout<<this->name()<<iprint(i)<<iprint(portE)<<iprint(portN)<<iprint(portW)<<endl;

                //#endif
                break;
            }

            if (vc[vc_to_serve].vcQ.empty == false)  	// fifo not empty
            {
                if (outReady[i].read() == true)  	// OC ready to recieve flit
                {
                    //flit_out = vc[vc_to_serve].vcQ.flit_out();	// read flit from fifo

                    flit_out = vc[vc_to_serve].vcQ.flit_read();	// read flit from fifo

                    numBufReads++;		// increase buffer read count
                    /*added*/
                    numBufReadsPower++;
                    /*end*/
                    oldvcid = flit_out.vcid;

                    if (LOG >= 4)
                        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: Attempting to forward  flit: "<<flit_out;

                    if (i != num_op - 1)  	// not to be done for core OC
                    {
                        if ( vc[vc_to_serve].vc_next_id == NUM_VCS+1 && cntrlID != C)
                        {
                            if (flit_out.pkttype == NOC && (flit_out.pkthdr.nochdr.flittype == DATA || flit_out.pkthdr.nochdr.flittype == TAIL))
                            {
                                //should have been a head, need to clean out the fifo Q
                                //cout<<"\nHELLO TEST";
                                if (LOG >= 4)
                                    eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: flit is not a head..Error"<<endl;
                                vc[vc_to_serve].vcQ.pntr = 0;
                                vc[vc_to_serve].vcQ.empty = true;
                                vc[vc_to_serve].vcQ.full = false;
                                continue;
                            }
                        }

//cout<<"PKT TYPE= "<<flit_out.pkttype<<" FLIT TYPE= "<<flit_out.pkthdr.nochdr.flittype<<"\n";


                        if ( (flit_out.pkttype == NOC && (flit_out.pkthdr.nochdr.flittype == HEAD || flit_out.pkthdr.nochdr.flittype == HDT)) || (flit_out.pkttype == ANT && flit_out.pkthdr.anthdr.anttype == FORWARD))
                        {

                            // VC request
                            if (LOG >= 4)
                                eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrlID: "<<cntrlID<<" vcRequest: "<<i;

                            vcRequest.write(true);
                            opRequest.write(i);
                            /****************************************************************************************/
                            if (qos==1)
                            {
                                if (cntrlID != C)
                                {
                                    //cout<<"\nin transmit flit (except core)() : tileID  = "<<tileID;
                                    if (vc_to_serve>=0 && vc_to_serve< int(NUM_VCS * BW_GT))//ie its a VC having GT flits

                                        notify_GT.write(1);

                                    else if (vc_to_serve>=int(NUM_VCS * BW_GT)&& vc_to_serve< NUM_VCS )//ie its a VC having BE flits
                                        notify_GT.write(2);
                                    else
                                    {

                                    }
                                }
                                else//ie its a core
                                {
//cout<<"\nin transmit flit (for core)() : tileID  = "<<tileID;
                                    if (app_libname[tileID]=="Bursty.so")
                                        notify_GT.write(1);
                                    else if (app_libname[tileID]=="CBR.so")
                                        notify_GT.write(2);
                                    else
                                    {

                                    }

                                }
                            }
                            /****************************************************************************************/


                            wait();	// wait for ready event from VC
                            if (vcReady.event())
                            {
                                if (LOG >= 4)
                                    eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: vcReady event..."<<endl;
                            }
                            else if (switch_cntrl.event())
                            {
                                if (LOG >= 4)
                                    eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: unknown clock event..."<<endl;
                            }


                            // read next VCid sent by VC

                            vc[vc_to_serve].vc_next_id = nextVCID.read();
                            //if(vc[vc_to_serve].vc_next_id == NUM_VCS+1)
                            //cout<<"\n NEXT VCID(transmit flit)& tileID[not toward core] & dir to: "<<vc[vc_to_serve].vc_next_id<<" & "<<tileID<<" & "<<i;

                            if (vc[vc_to_serve].vc_next_id == NUM_VCS+1)  	// VC not granted
                            {
                                if (LOG >= 4)
                                    eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: No free next vc, pushing flit in Q" <<endl;
                                // push flit back in fifo
                                flit_out.simdata.num_waits++;
                                /*added*/
                                numVCReads++;
                                /*end*/


//qrt************************************************************

                                ///////TRYING TO MINIMIZE DEADLOCK IF PROBLEM == DEADLOCK
                                if (RT_ALGO == QRT)
                                {
                                    if (flit_out.simdata.num_waits >= WAITS)
                                    {
                                        //	cout << "Tile :"<<tileID<<" num_op: "<<num_op<<" cntrlID: "<<cntrlID;
                                        if (num_op-1 != 2)
                                        {
                                            UI new_dir = 0;
                                            if (num_op - 1 == 3)
                                            {
                                                for (UI k = 0; k < num_op-1; k++)
                                                {
                                                    if (k != i && k != cntrlID)
                                                    {
                                                        new_dir = k;
                                                        //					cout<<" New Dir:"<<new_dir<<endl;
                                                        break;
                                                    }
                                                }
                                            }
                                            if (new_dir == portN) new_dir = N;
                                            else if (new_dir == portS) new_dir = S;
                                            else if (new_dir == portE) new_dir = E;
                                            else if (new_dir == portW) new_dir = W;
                                            else cout<<"ERROR:ERROR:ERROR"<<endl;
                                            vc[vc_to_serve].vc_route = new_dir;
                                            flit_out.simdata.num_waits =  0;
                                        }
                                        //	cout<<endl;
                                    }
                                }
                                ///////////////////////////////////////////////////////////
//qrt************************************************************


                                //vc[vc_to_serve].vcQ.flit_push(flit_out);

                                vcRequest.write(false);
                                continue;
                            }
                            else
                            {
                                /*added*/
                                numVCReads++;
                                numVCWrites++;
                                /*end*/
                            }
                        }

                        oldvcid = flit_out.vcid;

                        //flit_out.vcid =	vc[vc_to_serve].vc_next_id ;	//doubt

                        //	if(flit_out.pkttype == ANT && flit_out.pkthdr.anthdr.anttype == BACKWARD) {
                        //oldvcid = phit_out.pkt_hdr.vc_id;
                        //		flit_out.vcid = NUM_VCS - 1;
                        //}
                    } // end if, this block not executed for core OC
                    //added
                    numBufReadsPower++;
                    //end
                    flit_out = vc[vc_to_serve].vcQ.flit_out();	// read flit from fifo

                    if (i != num_op - 1)  	// not to be done for core OC
                    {
                        flit_out.vcid = vc[vc_to_serve].vc_next_id;
                    }
                    // write flit to output port
                    flit_out.simdata.num_sw++;
                    flit_out.simdata.ctime = sc_time_stamp();
                    //				if(tileID==0)
                    //				cout<<"\n nExT VCID(transmit flit)& tileID[anywhere] & dir to: "<<vc[vc_to_serve].vc_next_id<<" & "<<tileID<<" & "<<i;

                    
                    outport[i].write(flit_out);

                    //qrt*************************************************************************************************
                    //results
                    rs.pkt_out++;
                    //end
                    //qrt*************************************************************************************************

                    if (LOG >= 2)
                        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrl: "<<cntrlID<<" Transmitting flit to output port: "<<i<<flit_out;

                    //if hdt/tail flit, put freeVC signal on creditLine
                    //if head/data flit, if fifo buf is free, then put freeBuf signal creditLine
                    // Update credit info
                    if (LOG >= 4)
                        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: Updating freeVC status for vc:"<<vc_to_serve<<endl;

                    // if hdt/tail flit, free VC
                    // else free buffer
                    if ((flit_out.pkttype == NOC && (flit_out.pkthdr.nochdr.flittype == TAIL || flit_out.pkthdr.nochdr.flittype == HDT)) || flit_out.pkttype == ANT)
                    {
                        vc[oldvcid].vc_next_id = NUM_VCS+1;
                        creditLine t;
                        t.freeVC = true;
                        t.freeBuf = true;
                        credit_out[vc_to_serve].write(t);
                        //if(cntrlID == C)
                        vc[vc_to_serve].vc_route = 5;
                    }
                    else
                    {
                        creditLine t;
                        t.freeVC = false;
                        t.freeBuf = true;
                        credit_out[vc_to_serve].write(t);
                    }
                    vcRequest.write(false);
                } // end outReady == true
                else
                {
                    if (LOG >= 4)
                        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: OC cannot accept flit!"<<endl;
                }
            } // end serving VC
        } //end if switch_cntrl
    } // end while
} //end transmit_flit()

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::setTileID(UI id, UI port_N, UI port_S, UI port_E, UI port_W)
{
    tileID = id;
    portN = port_N;
    portS = port_S;
    portE = port_E;
    portW = port_W;
    resetCounts();
}

///////////////////////////////////////////////////////////////////////////
/// Method to resut buffer stats to zero
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::resetCounts()
{
    numBufReads = numBufWrites = numBufReadsPower=numArb=numVCReads=numVCWrites= 0;
    numBufsOcc = 0;
    numVCOcc = 0;
}

///////////////////////////////////////////////////////////////////////////
/// Method to close logfiles
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::closeLogs()
{
//	logcout.close();
}

///////////////////////////////////////////////////////////////////////////
/// Method to store flit in fifo buffer
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::store_flit_VC(flit *flit_in)
{
    int vc_id = flit_in->vcid;

    if (vc[vc_id].vcQ.full == true)
    {
        rs.pkt_dropped++;
        if (LOG >= 4)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" Error: DATA has arrived. vcQ is full!"<<endl;
    }


    else
    {
        vc[vc_id].vcQ.flit_in(*flit_in);
        /*added*/
        numBufWrites++;/*end*/
        creditLine t;

        /*if((flit_in->pkttype == NOC && flit_in->pkthdr.nochdr.flittype == HDT)){
        	if(vc[vc_id].vcQ.full == true) {
        		t.freeVC = false; t.freeBuf = false;
        	}
        	else {
        		t.freeVC = true; t.freeBuf = true;
        	}
        }
        else*/
        {
            t.freeVC = false;
            t.freeBuf = !vc[vc_id].vcQ.full;
        }
        credit_out[vc_id].write(t);

        if (cntrlID == C)
            rs.pkt_in_core++;

        rs.pkt_in++;

//		if(flit_in->pkttype == EST)
//			rs.est_in++;
        rs.buffer_size = vc[vc_id].vcQ.pntr;
    }
}

///////////////////////////////////////////////////////////////////////////
/// Method to call Controller for source routing
///////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::routing_src(flit *flit_in)
{
    int vc_id = flit_in->vcid;
    rtRequest.write(ROUTE);
    sourceAddress.write(flit_in->src);
    destRequest.write(flit_in->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route);
    flit_in->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route = flit_in->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route >> 3; //Right shift
    flit pack = vc[vc_id].vcQ.flit_out();
pack.pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route = flit_in->pkthdr.nochdr.flithdr.header.rthdr.sourcehdr.route;
vc[vc_id].vcQ.flit_push(pack);
    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: rtRequest sent!"<<endl;
    wait();
    if (rtReady.event())
    {
        if (LOG >= 4)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: rtReady event..."<<endl;
    }
    else if (switch_cntrl.event())
    {
        if (LOG >= 4)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: unknown clock event..."<<endl;
    }
    else if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: Unknown Event!"<<endl;

    vc[vc_id].vc_route = nextRt.read();
    rtRequest.write(NONE);
}

///////////////////////////////////////////////////////////////////////////////////////
/// Method to call controller for routing algorithms that require destination address
//////////////////////////////////////////////////////////////////////////////////////
template<UI num_op>
void InputChannel<num_op>::routing_dst(flit *flit_in)
{
    int vc_id = flit_in->vcid;
    ///MAXY/////////////////////
    int nxtroute,dirx,diry;
    ////////////////////////////

    rtRequest.write(ROUTE);
    sourceAddress.write(flit_in->src);
    destRequest.write(flit_in->pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst);

    if (LOG == 10)
        cout<<this->name()<<"id:"<<flit_in->pkthdr.nochdr.pktid<<" "<<iprint(num_tiles)<<": "<<iprint(flit_in->pkthdr.nochdr.flithdr.header.rthdr.dsthdr.dst)<<iprint(flit_in->src)<<endl;

    //Why two ifs??
    if (RT_ALGO == QRT)
        est_pktID.write(flit_in->pkthdr.nochdr.pktid);//CODE999
    if (RT_ALGO == QRT)
        timestamp_ip.write(flit_in->simdata.OCtimestamp);



    if (LOG >= 4)
        eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: rtRequest sent!"<<endl;
    wait();
    if (rtReady.event())
    {
        if (LOG >= 4)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: rtReady event..."<<endl;
    }
    else if (switch_cntrl.event())
    {
        if (LOG >= 4)
            eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" IC: unknown clock event..."<<endl;
    }
    flit_in->simdata.OCtimestamp = sim_count - 1;
    //////////////////////////////////////////MAXY///////////////////////////
    nxtroute=nextRt.read();
    //added (modified)
    if (nxtroute==20||nxtroute==21||nxtroute==30||nxtroute==31 || nxtroute==32 || nxtroute==10 )
    {
        dirx=nxtroute/10;
        diry=nxtroute%10;
        int x,y;
        switch (dirx)
        {
        case E:
            x = portE;
            break;
        case W:
            x = portW;
            break;
        case N:
            x = portN;
            break;
        case S:
            x = portS;
            break;
        }
        switch (diry)
        {
        case E:
            y = portE;
            break;
        case W:
            y = portW;
            break;

        case N:
            y = portN;
            break;
        case S:
            y = portS;
            break;
        }
        //end
        if (outReady[y].read()==true)
        {
            vc[vc_id].vc_route=diry;
            rtRequest.write(NONE);
        }
        else if (outReady[x].read()==true)
        {
            vc[vc_id].vc_route=dirx;
            rtRequest.write(NONE);
        }
        else
        {
            vc[vc_id].vc_route=diry;
            rtRequest.write(NONE);
        }
    }
    else
        /////////////////////////////////////////////////////////////////////////////////
    {
        //if(flit_in->pkthdr.nochdr.pktid == 1){  // to know path taken in case of ftxy for one source dest pair
        //cout<<"\nDIR: "<<nxtroute<<"\n";
        //}
        vc[vc_id].vc_route = nxtroute;
        rtRequest.write(NONE);
    }
}

//////////////////////////////////////////////////
// Ant routines

// Route table lookup on basis of interconnects
template<UI num_op>
int InputChannel<num_op>::reverse_route(int rt_code)
{
    int rt_rev;
    switch (rt_code)
    {
    case 0:
        rt_rev = 1;
        break;
    case 1:
        rt_rev = 0;
        break;
    case 2:
        rt_rev = 3;
        break;
    case 3:
        rt_rev = 2;
        break;
    };
    return rt_rev;
}

// Ant routines end

template struct InputChannel<NUM_OC>;
template struct InputChannel<NUM_OC_B>;
template struct InputChannel<NUM_OC_C>;
