/*-------------------------------------------------------------------------
 *                             ORION 2.0
 *
 *         					Copyright 2009
 *  	Princeton University, and Regents of the University of California
 *                         All Rights Reserved
 *
 *
 *  ORION 2.0 was developed by Bin Li at Princeton University and Kambiz Samadi at
 *  University of California, San Diego. ORION 2.0 was built on top of ORION 1.0.
 *  ORION 1.0 was developed by Hangsheng Wang, Xinping Zhu and Xuning Chen at
 *  Princeton University.
 *
 *  If your use of this software contributes to a published paper, we
 *  request that you cite our paper that appears on our website
 *  http://www.princeton.edu/~peh/orion.html
 *
 *  Permission to use, copy, and modify this software and its documentation is
 *  granted only under the following terms and conditions.  Both the
 *  above copyright notice and this permission notice must appear in all copies
 *  of the software, derivative works or modified versions, and any portions
 *  thereof, and both notices must appear in supporting documentation.
 *
 *  This software may be distributed (but not offered for sale or transferred
 *  for compensation) to third parties, provided such third parties agree to
 *  abide by the terms and conditions of this notice.
 *
 *  This software is distributed in the hope that it will be useful to the
 *  community, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "orion_router_power.h"
#include "SIM_parameter.h"
#include "SIM_router.h"
#include "SIM_router_area.h"
#include "SIM_link.h"

#include "../../config/extern_orion.h"
#include <math.h>

double power_main(double buf_rl,double buf_wl,double c_tl,double n_arb,double nvc_read,double nvc_write)
{
    int max_flag = AVG_ENERGY;//calculate average energy
    int plot_flag = 1;
    u_int print_depth = 5;
    u_int data_width=FLITSIZE*8;//FLITSIZE (in bytes) can be defined in nirgam.config. default value is set to 5 in default.h
    double freq;
    freq = CLK_FREQ* 1e9;//CLK_FREQ (in Ghz) can be defined in nirgam.config. default value is set to 1 in default.h
    double Erouter,Prouter;
    SIM_router_init(&GLOB(router_info), &GLOB(router_power), &GLOB(router_area));
    Erouter=SIM_router_stat_energy(&GLOB(router_info), &GLOB(router_power), print_depth,  max_flag, buf_wl,buf_rl,c_tl,n_arb,nvc_read,nvc_write, plot_flag, freq);
    Prouter=Erouter*freq;
    return (Prouter);
}

double link_power(double link_tl)
{
    double link_len;
    u_int data_width;
    double Pdynamic, Pleakage,Ptotal;
    double freq;
    double load;
    double link_area;

#if ( PARM(TECH_POINT) <= 90 )

    /* link length is also the core size*/
    link_len = LINK_LEN; //unit micro-meter
    link_len = link_len * 1e-6; //unit meter
    load = link_tl;

    freq = CLK_FREQ * 1e9;
    data_width = FLITSIZE*8;

    Pdynamic = 0.5 * load * LinkDynamicEnergyPerBitPerMeter(link_len, Vdd) * freq * link_len * (double)data_width;
    Pleakage = LinkLeakagePowerPerMeter(link_len, Vdd) * link_len * data_width;
    Ptotal = (Pdynamic + Pleakage) ;
#else
    //fprintf(stderr, "Link power and area are only supported for 90nm, 65nm, 45nm and 32nm\n");
    return 0;
#endif
    return Ptotal;
}


