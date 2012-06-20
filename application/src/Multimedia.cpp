
/*
 * Multimedia.cpp
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
/// \file Multimedia.cpp
/// \brief Implements Multimedia traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Multimedia.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
MultimediaTraffic::MultimediaTraffic(sc_module_name MultimediaTraffic) : TrafficGenerator(MultimediaTraffic)
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
/// Process to generate Multimedia traffic
/// - read traffic configuration file
/// - generate traffic pattern in log file
////////////////////////////////////////////////
void MultimediaTraffic::init()
{
    /*added*/
    traffic_patterns *tpattern;
    float *Probmat;
    NEDTraffic * nedt = new NEDTraffic(num_rows,num_cols,tileID);
    Probmat= new float[num_rows*num_cols];
    ofstream cal_H;
    cal_H.open("calc0");
    char str_id[3];
    double avg_offtime;
    double rand_no;
    hsrng=new UniformRandomVariable(0,1);
    // open traffic config file
    sprintf(str_id, "%d", tileID);
    string traffic_filename = string("config/traffic/tile-") + string(str_id);
    ifstream instream;
    instream.open(traffic_filename.c_str());
    double *frm_len = new double[TG_NUM];
    while (!instream.eof())
    {
        string field;
        instream >> field;

        if (field == "HURST_PAR")  	// read load percentage
        {
            float value;
            instream >> value;
            hurst_par = value;
        }
        else if (field == "AVG_OFFTIME")  	// read average offtime
        {
            double value;
            instream >> value;
            avg_offtime = value;
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

//							for (int i=0; i<num_rows; i++)
//								{
//									cout<<"\n\n";
//									for (int j=0; j<num_rows; j++)
//										{
//											cout<<Probmat[i*num_rows+j]<<" ";
//										}
//								}
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
        else if (field == "MIN")  	// read inter-flit interval
        {
            int value;
            instream >> value;
            mino = value;
        }
        else if (field == "MAX")  	// read inter-flit interval
        {
            int value;
            instream >> value;
            maxo = value;
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
    // calculating trace based on hurst parameter
    instream.close();
    hurst_cal(hurst_par,TG_NUM,frm_len);

    float minn,maxn;
    minn=maxn=frm_len[0];
    for (int i=1; i<TG_NUM; i++)
    {
        if (frm_len[i]<minn)
            minn=frm_len[i];
        if (frm_len[i]>maxn)
            maxn=frm_len[i];

    }

    for (int i=0; i<TG_NUM; i++)				// Mapping of Generated trace in the Minimum and Maximum Range
    {
        frm_len[i]=((frm_len[i]-minn)/(maxn-minn))*(maxo-mino)+mino;
        cal_H<<frm_len[i]<<endl;
    }
    var_offtime.setavg(avg_offtime);
    // compute inter-packet interval
    //pkt_interval = (int)((ceil)(100.0/load) * num_flits * cycles_per_flit);
    // open traffic log file
    traffic_filename = string("log/traffic/tile-") + string(str_id);

    ofstream trafstream;
    trafstream.open(traffic_filename.c_str());
    traffic_filename = string("frame_len/tile-") + string(str_id);
    ofstream frame_len;
    frame_len.open(traffic_filename.c_str());

    // write inter-flit interval
    trafstream<<"FLIT_INTERVAL: "<<flit_interval<<endl;
    for (int i = WARMUP; i <= TG_NUM; i++)
    {
        // compute number of flits in packet (function of pkt size and flit size)
        num_flits = (int)((ceil)((float)(frm_len[i-WARMUP]+1 - HEAD_PAYLOAD)/(DATA_PAYLOAD)) + 1);
        frame_len<<frm_len[i-WARMUP]<<endl;
        next_pkt_time = (int)next_interval();	// get next packet interval
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
double MultimediaTraffic::next_interval()
{
    //return pkt_interval;
    //return (int)((ceil)(100.0/load) * num_flits * cycles_per_flit);
    return num_flits*flit_interval+var_offtime.value();
}

///  hurst parameter calculation
/// Function taken from "http://www-nh.scphys.kyoto-u.ac.jp/~shoji/TA2005/programs/Gamma.C"
double MultimediaTraffic:: dgamma(double x)
{
    int k, n;
    double w, y;

    n = x < 1.5 ? -((int) (2.5 - x)) : (int) (x - 1.5);
    w = x - (n + 2);
    y = ((((((((((((-1.99542863674e-7 * w + 1.337767384067e-6) * w -
                   2.591225267689e-6) * w - 1.7545539395205e-5) * w +
                 1.45596568617526e-4) * w - 3.60837876648255e-4) * w -
               8.04329819255744e-4) * w + 0.008023273027855346) * w -
             0.017645244547851414) * w - 0.024552490005641278) * w +
           0.19109110138763841) * w - 0.233093736421782878) * w -
         0.422784335098466784) * w + 0.99999999999999999;
    if (n > 0)
    {
        w = x - 1;
        for (k = 2; k <= n; k++)
        {
            w *= x - k;
        }
    }
    else
    {
        w = 1;
        for (k = 0; k > n; k--)
        {
            y *= x - k;
        }
    }
    return w / y;
}

/// Function to compute Inverse FFT
/// Function taken from "http://www.math.wustl.edu/~victor/mfmm/"
void MultimediaTraffic:: ifft( complex *v, int n, complex *tmp )
{
    if (n>1)  			/* otherwise, do nothing and return */
    {
        int k,m;
        complex z, w, *vo, *ve;
        ve = tmp;
        vo = tmp+n/2;
        for (k=0; k<n/2; k++)
        {
            ve[k] = v[2*k];
            vo[k] = v[2*k+1];
        }
        ifft( ve, n/2, v );		/* FFT on even-indexed elements of v[] */
        ifft( vo, n/2, v );		/* FFT on odd-indexed elements of v[] */
        for (m=0; m<n/2; m++)
        {
            w.Re = cos(2*PI*m/(double)n);
            w.Im = sin(2*PI*m/(double)n);
            z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
            z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
            v[  m  ].Re = ve[m].Re + z.Re;
            v[  m  ].Im = ve[m].Im + z.Im;
            v[m+n/2].Re = ve[m].Re - z.Re;
            v[m+n/2].Im = ve[m].Im - z.Im;
        }
    }

}


///////////////////////////////////////////////////////////////////////////
/// Method to calculate FGN spectrum
/// Parameters:
/// - Initial Lambda series
/// - Hurst Parameter
/// - Number of trace points
/// returns spectrum to Noise
///////////////////////////////////////////////////////////////////////////

void MultimediaTraffic:: FGN_B_est(double *lambda, float H, int n, double *result)
{
    float d = -2*H-1;
    float dprime = -2*H;

    for (int i=0; i<=n-1; i++)
    {
        result[i] = powf(2*1*PI+lambda[i],d) + powf(2*1*PI-lambda[i],d) + powf(2*2*PI+lambda[i],d) + powf(2*2*PI-lambda[i],d) + powf(2*3*PI+lambda[i],d) + powf(2*3*PI-lambda[i],d) + (powf(2*3*PI+lambda[i],dprime) + powf(2*3*PI-lambda[i],dprime) + powf(2*4*PI+lambda[i],dprime) + powf(2*4*PI-lambda[i],dprime))/(8*PI*H);

    }

}

///////////////////////////////////////////////////////////////////////////
/// Method to generate Fractional Gaussian Noise
/// Parameters:
/// - Hurst Parameter
/// - Number of trace Points
/// - Initial Lambda series
/// returns FGN data
///////////////////////////////////////////////////////////////////////////
void MultimediaTraffic:: FGN_spectrum(double *lambda, float H, int n, double *r)
{
    double *y = new double[n];
    FGN_B_est(lambda,H,n,y);			// FGN_B_est is called.

    double c = 2*sin(PI*H)*gamma(2*H+1);
    double *x  = new double[n];
    for (int i=0; i<=n-1; i++)
    {
        x[i] = cos(lambda[i]);
        x[i] = 1-x[i];
        x[i] = x[i]*c;
    }

    double *p = new double[n];
    for (int i=0; i<=n-1; i++)
        p[i] = ((double)(powf(lambda[i],(-2*H-1))) + y[i]);

    for (int i=0; i<=n-1; i++)
        r[i] = x[i]*p[i];

    delete[] x;
    delete[] p;

}
///////////////////////////////////////////////////////////////////////////
/// Method to generate synthetic trace for a given Hurst Parameter
/// Parameters:
/// - Hurst Parameter
/// - Number of Trace Points
/// returns generated trace
///////////////////////////////////////////////////////////////////////////
void MultimediaTraffic:: hurst_cal(float H,int n,double * ret)
{
    int j;
    n = n/2;

    double *lambda = new double[n];
    for (int i=0; i<=n-1; i++)
        lambda[i] = ((float)(i+1)*PI)/n;	// Generate Initial series for N/2 numbers

    double *f = new double[n];
    FGN_spectrum(lambda,H,n,f);		// Calling FGN_spectrum

    ExponentialRandomVariable erv(1);
    for (int i=0; i<=n-1; i++)
    {
        f[i] = f[i]*erv.value();
        f[i] = sqrt(f[i]);
    }					// Output is then varied Exponentially

    double *ran = new double[n];
    UniformRandomVariable urv(0,1);

    for (int i=0; i<=n-1; i++)
//		ran[i] = 2*PI*urv.value();
        ran[i] = -PI+2*PI*urv.value();

    complex *a = new complex[n];		// Complex Numbers are generated using Uniform Random Variable
    for (int i=0; i<=n-1; i++)
    {
        a[i].Re = f[i]*cos(ran[i]);
        a[i].Im = f[i]*sin(ran[i]);
    }

    a[n-1].Re = abs(a[n-1].Re);
    a[n-1].Im = abs(a[n-1].Im);

    complex *a1 = new complex[n];
    j = n-1;
    for (int i=0; i<=n-1; i++)
    {
        a1[i].Re = a[j].Re;
        a1[i].Im = -a[j].Im;
        j--;
    }

    complex *a2 = new complex[2*n];		// N Complex Numbers are expanded using previous N/2 numbers
    for (int i=0; i<=n-1; i++)
    {
        a2[i].Re = a[i].Re;
        a2[i].Im = a[i].Im;
    }
    j=0;
    for (int i=n; i<=2*n-1; i++)
    {
        a2[i].Re = a1[j].Re;
        a2[i].Im = a1[j].Im;
        j++;
    }

    complex *temp = new complex[2*n];
    ifft( a2,2*n,temp );			// IFFT is then calculated
    for (int i=0; i<=2*n-1; i++)
        ret[i]=a2[i].Re;		// Real part of the complex numbers contains the trace

    delete[] f;
    delete[] ran;
    delete[] a;
    delete[] a1;

}

// for dynamic linking
extern "C"
{
    TrafficGenerator *maker()
    {
        return new MultimediaTraffic("MM");
    }
}
