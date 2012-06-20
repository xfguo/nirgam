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

/*
 * TODO:  (2) add routing table
 *
 * FIXME: (1) ignore internal nodes of tri-state buffer till we find a good solution
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SIM_parameter.h"
#include "SIM_array.h"
#include "SIM_misc.h"
#include "SIM_router.h"
#include "SIM_static.h"
#include "SIM_clock.h"
#include "SIM_util.h"
#include "orion_router_power.h"

/* FIXME: request wire length estimation is ad hoc */
int SIM_router_power_init(SIM_router_info_t *info, SIM_router_power_t *router)
{
    double cbuf_width, req_len = 0;

    router->I_static = 0;
    router->I_buf_static = 0;
    router->I_crossbar_static = 0;
    router->I_vc_arbiter_static = 0;
    router->I_sw_arbiter_static = 0;

    /* initialize crossbar */
    if (info->crossbar_model)
    {
        SIM_crossbar_init(&router->crossbar, info->crossbar_model, info->n_switch_in, info->n_switch_out, info->xb_in_seg, info->xb_out_seg, info->flit_width, info->degree, info->connect_type, info->trans_type, info->crossbar_in_len, info->crossbar_out_len, &req_len);
        /* static power */
        router->I_crossbar_static = router->crossbar.I_static;
        router->I_static += router->I_crossbar_static;
    }

    /* HACK HACK HACK */
    if (info->exp_xb_model)
        SIM_crossbar_init(&router->exp_xb, info->exp_xb_model, 2 * info->n_switch_in - 1, 2 * info->n_switch_out - 1, info->exp_in_seg, info->exp_out_seg, info->flit_width, info->degree, info->connect_type, info->trans_type, 0, 0, NULL);

    /* initialize various buffers */
    if (info->in_buf)
    {
        SIM_array_power_init(&info->in_buf_info, &router->in_buf);
        /* static power */
        router->I_buf_static = router->in_buf.I_static * info->n_in * info->n_v_class * (info->in_share_buf ? 1 : info->n_v_channel);
    }
    if (info->cache_in_buf)
        SIM_array_power_init(&info->cache_in_buf_info, &router->cache_in_buf);
    if (info->mc_in_buf)
        SIM_array_power_init(&info->mc_in_buf_info, &router->mc_in_buf);
    if (info->io_in_buf)
        SIM_array_power_init(&info->io_in_buf_info, &router->io_in_buf);
    if (info->out_buf)
    {
        SIM_array_power_init(&info->out_buf_info, &router->out_buf);
        /* static power */
        router->I_buf_static += router->out_buf.I_static * info->n_out * info->n_v_class * (info->out_share_buf ? 1 : info->n_v_channel);
    }

    router->I_static += router->I_buf_static;

    if (info->central_buf)
    {
        /* MUST initialize array before crossbar because the latter needs array width */
        SIM_array_power_init(&info->central_buf_info, &router->central_buf);

        /* WHS: use MATRIX_CROSSBAR, info->connect_type, info->trans_type rather than specifying new parameters */
        cbuf_width = info->central_buf_info.data_arr_width + info->central_buf_info.tag_arr_width;
        req_len = info->central_buf_info.data_arr_height;

        /* assuming no segmentation for central buffer in/out crossbars */
        SIM_crossbar_init(&router->in_cbuf_crsbar, MATRIX_CROSSBAR, info->n_switch_in, info->pipe_depth * info->central_buf_info.write_ports, 0, 0, info->flit_width, 0, info->connect_type, info->trans_type, cbuf_width, 0, NULL);
        SIM_crossbar_init(&router->out_cbuf_crsbar, MATRIX_CROSSBAR, info->pipe_depth * info->central_buf_info.read_ports, info->n_switch_out, 0, 0, info->flit_width, 0, info->connect_type, info->trans_type, 0, cbuf_width, NULL);

        /* dirty hack */
        SIM_fpfp_init(&router->cbuf_ff, info->cbuf_ff_model, 0);
    }

    /* initialize switch allocator arbiter */
    if (info->sw_in_arb_model)
    {
        SIM_arbiter_init(&router->sw_in_arb, info->sw_in_arb_model, info->sw_in_arb_ff_model, info->n_v_channel*info->n_v_class, 0, &info->sw_in_arb_queue_info);
        if (info->n_cache_in)
            SIM_arbiter_init(&router->cache_in_arb, info->sw_in_arb_model, info->sw_in_arb_ff_model, info->cache_class, 0, &info->cache_in_arb_queue_info);
        if (info->n_mc_in)
            SIM_arbiter_init(&router->mc_in_arb, info->sw_in_arb_model, info->sw_in_arb_ff_model, info->mc_class, 0, &info->mc_in_arb_queue_info);
        if (info->n_io_in)
            SIM_arbiter_init(&router->io_in_arb, info->sw_in_arb_model, info->sw_in_arb_ff_model, info->io_class, 0, &info->io_in_arb_queue_info);

        router->I_sw_arbiter_static = router->sw_in_arb.I_static * info->in_n_switch * info->n_in;
    }

    /* WHS: must after switch initialization */
    if (info->sw_out_arb_model)
    {
        SIM_arbiter_init(&router->sw_out_arb, info->sw_out_arb_model, info->sw_out_arb_ff_model, info->n_total_in - 1, req_len, &info->sw_out_arb_queue_info);

        router->I_sw_arbiter_static += router->sw_out_arb.I_static * info->n_switch_out;
    }
    /*static energy*/
    router->I_static += router->I_sw_arbiter_static;

    /* initialize virtual channel allocator arbiter */
    if (info->vc_allocator_type == ONE_STAGE_ARB && info->n_v_channel > 1 && info->n_in > 1)
    {
        if (info->vc_out_arb_model)
        {
            SIM_arbiter_init(&router->vc_out_arb, info->vc_out_arb_model, info->vc_out_arb_ff_model,
                             (info->n_in - 1) * info->n_v_channel, 0, &info->vc_out_arb_queue_info);

            router->I_vc_arbiter_static = router->vc_out_arb.I_static * info->n_out * info->n_v_channel * info->n_v_class;
        }
        else
        {
            fprintf (stderr, "error in setting vc allocator parameters\n");
        }
    }
    else if (info->vc_allocator_type == TWO_STAGE_ARB && info->n_v_channel > 1 && info->n_in > 1)
    {
        if (info->vc_in_arb_model && info->vc_out_arb_model)
        {
            // first stage
            SIM_arbiter_init(&router->vc_in_arb, info->vc_in_arb_model, info->vc_in_arb_ff_model,
                             info->n_v_channel, 0, &info->vc_in_arb_queue_info);

            router->I_vc_arbiter_static = router->vc_in_arb.I_static * info->n_in * info->n_v_channel * info->n_v_class;


            //second stage
            SIM_arbiter_init(&router->vc_out_arb, info->vc_out_arb_model, info->vc_out_arb_ff_model,
                             (info->n_in - 1) * info->n_v_channel, 0, &info->vc_out_arb_queue_info);
            router->I_vc_arbiter_static += router->vc_out_arb.I_static * info->n_out * info->n_v_channel * info->n_v_class;
        }
        else
        {
            fprintf (stderr, "error in setting vc allocator parameters\n");
        }
    }
    else if (info->vc_allocator_type == VC_SELECT && info->n_v_channel > 1)
    {
        SIM_array_power_init(&info->vc_select_buf_info, &router->vc_select_buf);
        /* static power */
        router->I_vc_arbiter_static = router->vc_select_buf.I_static * info->n_out * info->n_v_class;
    }

    router->I_static += router->I_vc_arbiter_static;
    return 0;
}


/* THIS FUNCTION IS OBSOLETE */
int SIM_router_power_report(SIM_router_info_t *info, SIM_router_power_t *router)
{
    double epart, etotal = 0;

    if (info->crossbar_model)
    {
        epart = SIM_crossbar_report(&router->crossbar);
        fprintf(stderr, "switch: %g\n", epart);
        etotal += epart;
    }

    fprintf(stderr, "total energy: %g\n", etotal);

    return 0;
}


/*ORION original parameters
 * time unit:	1 cycle
  * e_buf_wrt:	average # of input buffer writes of all ports during unit time
 * 		e_buf_wrt = e_fin * n_buf_in
 * e_buf_rd:	average # of input buffer reads of all ports during unit time
 * 		e_buf_rd = e_buf_wrt
 * 		  (splitted into different input ports in program)
 * e_cbuf_fin:	average # of flits passing through the switch during unit time
 * 		e_cbuf_fin = e_fin * n_total_in
 * e_cbuf_wrt:	average # of central buffer writes during unit time
 * 		e_cbuf_wrt = e_cbuf_fin / (pipe_depth * pipe_width)
 * e_cbuf_rd:	average # of central buffer reads during unit time
 * 		e_cbuf_rd = e_cbuf_wrt
 * e_arb:	average # of arbitrations per arbiter during unit time
 * 		assume e_arb = 1
 *
 * NOTES: (1) negative print_depth means infinite print depth
 *
 * FIXME: (1) hack: SIM_array_stat_energy cannot be used for shared buffer,
 *            we use it now anyway
 */

/*NIRGAM parameters:
time unit:	1 cycle
 * buf_wl:	average # of input buffer writes of all ports during unit time

 * buf_rl:	average # of input buffer reads of all ports during unit time

 * c_tl :	average # of flits passing through the switch during unit time

 * n_arb:	average # of arbitrations (including switch and routing) during unit time

 * nvc_read:    average # of reads during VC allocation

 * nvc_write:    average # of writes during VC allocation
 */

double SIM_router_stat_energy(SIM_router_info_t *info, SIM_router_power_t *router, int print_depth, int max_avg, double buf_rl,double buf_wl,double c_tl,double n_arb,double nvc_read,double nvc_write, int plot_flag, double freq)
{
    double Eavg = 0, Eatomic, Estruct, Estatic = 0;
    double Pbuf = 0, Pxbar = 0, Pvc_arbiter = 0, Psw_arbiter = 0, Pclock = 0, Ptotal = 0;
    double Pbuf_static = 0, Pxbar_static = 0, Pvc_arbiter_static = 0, Psw_arbiter_static = 0, Pclock_static = 0;
    double Pbuf_dyn = 0, Pxbar_dyn = 0, Pvc_arbiter_dyn = 0, Psw_arbiter_dyn = 0, Pclock_dyn = 0;
    double e_in_buf_rw, e_cache_in_buf_rw, e_mc_in_buf_rw, e_io_in_buf_rw;
    double e_cbuf_fin, e_cbuf_rw, e_out_buf_rw;
    int next_depth;
    u_int path_len, n_regs;
    int vc_allocator_enabled = 1;


    next_depth = NEXT_DEPTH(print_depth);//this macro defined in SIM_parameter.h

    /* input buffers */
    if (info->in_buf)
    {
        Eavg += SIM_array_stat_energy(&info->in_buf_info, &router->in_buf, buf_rl, buf_wl, next_depth, max_avg); //this function is defined in SIM_array_m.c
    }

    /* In accordance with NIRGAM architecture
       *cache_in_buf
       *mc_in_buf
       *io_in_buf
       *out_buf
       *central_buf
        all are set to 0,*/

    /*not needed in NIRGAM (begin)
    if (info->cache_in_buf) {
    	Eavg += SIM_array_stat_energy(&info->cache_in_buf_info, &router->cache_in_buf, e_cache_in_buf_rw, e_cache_in_buf_rw, next_depth,  max_avg);
    }
    if (info->mc_in_buf) {
    	Eavg += SIM_array_stat_energy(&info->mc_in_buf_info, &router->mc_in_buf, e_mc_in_buf_rw, e_mc_in_buf_rw, next_depth, max_avg);
    }
    if (info->io_in_buf) {
    	Eavg += SIM_array_stat_energy(&info->io_in_buf_info, &router->io_in_buf, e_io_in_buf_rw, e_io_in_buf_rw, next_depth, max_avg);
    }
        not needed in NIRGAM (end)*/

    /* output buffers *///no o/p buffers assumed
    /*not needed in NIRGAM (begin)
    if (info->out_buf) {
    	// local output ports don't use router buffers
    	Eavg += SIM_array_stat_energy(&info->out_buf_info, &router->out_buf, e_out_buf_rw, e_out_buf_rw, next_depth, max_avg);
    }

    // central buffer //no central buffers assumed
    if (info->central_buf) {
    	Eavg += SIM_array_stat_energy(&info->central_buf_info, &router->central_buf, e_cbuf_rw, e_cbuf_rw, next_depth, max_avg);
    	Eavg += SIM_crossbar_stat_energy(&router->in_cbuf_crsbar, next_depth, max_avg, e_cbuf_fin);
    	Eavg += SIM_crossbar_stat_energy(&router->out_cbuf_crsbar, next_depth, max_avg, e_cbuf_fin);
    	// dirty hack, REMEMBER to REMOVE Estruct and Eatomic
    	Estruct = 0;
    	n_regs = info->central_buf_info.n_set * (info->central_buf_info.read_ports + info->central_buf_info.write_ports);

    	// ignore e_switch for now because we overestimate wordline driver cap

    	Eatomic = router->cbuf_ff.e_keep_0 * (info->pipe_depth - 1) * (n_regs - 2 * (e_cbuf_rw + e_cbuf_rw));
    	Estruct += Eatomic;
    	Eatomic = router->cbuf_ff.e_clock * (info->pipe_depth - 1) * n_regs;
    	Estruct += Eatomic;
     	Eavg += Estruct;
    }
    not needed in NIRGAM (end)*/

    Pbuf_dyn = Eavg * freq;
    Pbuf_static = router->I_buf_static * Vdd * SCALE_S;
    Pbuf = Pbuf_dyn + Pbuf_static;

    /* main crossbar */
    if (info->crossbar_model)
    {
        Eavg += SIM_crossbar_stat_energy(&router->crossbar, next_depth, max_avg, c_tl);
    }

    Pxbar_dyn = (Eavg * freq - Pbuf_dyn);
    Pxbar_static = router->I_crossbar_static * Vdd * SCALE_S;
    Pxbar = Pxbar_dyn + Pxbar_static;

    /* switch allocation (arbiter energy only) */
    /* input (local) arbiter for switch allocation*/
    //In NIRGAM, the first stage of Switch Allocation is similar to that what is done before sending a routing request, so we are calculating the combined energy of all these arbitrations over here
    if (info->sw_in_arb_model)
    {
        /*total active arbiters during unit time=n_arb
         * assume (info->n_v_channel*info->n_v_class)/2 vcs are making request at each arbiter */

        Eavg += SIM_arbiter_stat_energy(&router->sw_in_arb, &info->sw_in_arb_queue_info, (info->n_v_channel*info->n_v_class)/2, next_depth, max_avg) * n_arb; //enrgy of one arbiter * total active arbiters

        /*not needed in NIRGAM (begin)
        		if (info->n_cache_in) {
        			Eavg += SIM_arbiter_stat_energy(&router->cache_in_arb, &info->cache_in_arb_queue_info, e_fin / info->cache_n_switch, next_depth, max_avg) * info->cache_n_switch * info->n_cache_in;
        		}
        		if (info->n_mc_in) {
        			Eavg += SIM_arbiter_stat_energy(&router->mc_in_arb, &info->mc_in_arb_queue_info, e_fin / info->mc_n_switch, next_depth, max_avg) * info->mc_n_switch * info->n_mc_in;
        		}

        		if (info->n_io_in) {
        			Eavg += SIM_arbiter_stat_energy(&router->io_in_arb, &info->io_in_arb_queue_info, e_fin / info->io_n_switch, next_depth, max_avg) * info->io_n_switch * info->n_io_in;
        		}not needed in NIRGAM (end)*/
    }


    //NIRGAM ensures no conflict for the output ports, so no need of any output arbitrations
    /* output (global) arbiter for switch allocation*/
    /*not needed in NIRGAM (begin)
    if (info->sw_out_arb_model) {
    	// assume # of active output arbiters is (info->n_switch_out * (e_cbuf_fin/info->n_switch_out))
    	 // assume (info->n_in)/2 request at each output arbiter

    	Eavg += SIM_arbiter_stat_energy(&router->sw_out_arb, &info->sw_out_arb_queue_info, info->n_in / 2, next_depth,  max_avg) * info->n_switch_out * (nvc_arb / info->n_switch_out);
    }not needed in NIRGAM (end)*/

    if (info->sw_in_arb_model || info->sw_out_arb_model)
    {
        Psw_arbiter_dyn = Eavg * freq - Pbuf_dyn - Pxbar_dyn;
        Psw_arbiter_static = router->I_sw_arbiter_static * Vdd * SCALE_S;
        Psw_arbiter = Psw_arbiter_dyn + Psw_arbiter_static;
    }

    /* virtual channel allocation (arbiter energy only) */
    //We are considering VC_SELECT type VC Allocation scheme in NIRGAM
    //Todo: modify it to include ONE_STAGE and TWO_STAGE as well by taking proper statistics from NIRGAM

    if (info->vc_allocator_type == ONE_STAGE_ARB && info->vc_out_arb_model  )
    {
        /* one stage arbitration (vc allocation)*/
        /* # of active arbiters */ //not used in nirgam for stage_one_arb
        double nActiveArbs =nvc_write ; //flit_rate * n_in * 0.2 / 2

        /* assume for each active arbiter, there is 2 requests on average (should use expected value from simulation) */
        Eavg += SIM_arbiter_stat_energy(&router->vc_out_arb, &info->vc_out_arb_queue_info,2, next_depth, max_avg)*nActiveArbs  ;

    }
    else if (info->vc_allocator_type == TWO_STAGE_ARB && info->vc_in_arb_model && info->vc_out_arb_model)
    {
        /* first stage arbitration (vc allocation)*/
        if (info->vc_in_arb_model)
        {
            // # of active stage-1 arbiters (# of new header flits)
            double nActiveArbs = nvc_write ;


            /* assume an active arbiter has n_v_channel/2 requests on average (should use expected value from simulation) */
            Eavg += SIM_arbiter_stat_energy(&router->vc_in_arb, &info->vc_in_arb_queue_info, info->n_v_channel/2, next_depth, max_avg) * nActiveArbs;
        }

        /* second stage arbitration (vc allocation)*/
        if (info->vc_out_arb_model)
        {
            /* # of active stage-2 arbiters */
            double nActiveArbs = nvc_write ; //flit_rate * n_in * 0.2 / 2

            /* assume for each active arbiter, there is 2 requests on average (should use expected value from simulation) */
            Eavg += SIM_arbiter_stat_energy(&router->vc_out_arb, &info->vc_out_arb_queue_info,
                                            2, next_depth,max_avg) * nActiveArbs;

        }
    }
    else if (info->vc_allocator_type == VC_SELECT && info->n_v_channel > 1 && info->n_in > 1)
    {
        double n_read = nvc_read;
        double n_write = nvc_write;
        Eavg += SIM_array_stat_energy(&info->vc_select_buf_info, &router->vc_select_buf, n_read , n_write, next_depth,  max_avg);
    }
    else
    {
        vc_allocator_enabled = 0; //set to 0 means no vc allocator is used
    }

    if (info->n_v_channel > 1 && vc_allocator_enabled)
    {
        Pvc_arbiter_dyn = Eavg * freq - Pbuf_dyn - Pxbar_dyn - Psw_arbiter_dyn;
        Pvc_arbiter_static = router->I_vc_arbiter_static * Vdd * SCALE_S;
        Pvc_arbiter = Pvc_arbiter_dyn + Pvc_arbiter_static;
    }

    /*router clock power (supported for 90nm and below) */
    if (PARM(TECH_POINT) <=90 && info->clock_enabled)
    {
        Eavg += SIM_total_clockEnergy(info, router);
        Pclock_dyn = Eavg * freq - Pbuf_dyn - Pxbar_dyn - Pvc_arbiter_dyn - Psw_arbiter_dyn;
        Pclock_static = router->I_clock_static * Vdd * SCALE_S;
        Pclock = Pclock_dyn + Pclock_static;
    }

    /* static power */
    Estatic = router->I_static * Vdd * Period * SCALE_S;
    Eavg += Estatic;
    Ptotal = Eavg * freq;
    return Eavg;
}
