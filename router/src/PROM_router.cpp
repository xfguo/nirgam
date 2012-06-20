
/*
 * PROM_router.cpp
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
/// \file PROM_router.cpp
/// \brief Implements routing algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PROM_router.h"
#include "../../config/extern.h"
#include "stdlib.h"
#include "time.h"

////////////////////////////////////////////////
/// Method to set unique id
////////////////////////////////////////////////
void PROM_router::setID(UI id_tile)
{
    id = id_tile;
    initialize();
}

////////////////////////////////////////////////
/// Method that implements routing
/// inherited from base class router
/// Parameters:
/// - input direction from which flit entered the tile
/// - tileID of source tile
/// - tileID of destination tile
/// returns next hop direction
////////////////////////////////////////////////
UI PROM_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id)
{
    // insert route logic here
    //cout<<endl<<"IN PROM  "<<id<<endl<<"ip direction "<<ip_dir<<endl;
    int set_avail[2];
    int k=10;
    int xco = id / num_cols;
    int yco = id % num_cols;
    int dest_xco = dest_id / num_cols;
    int dest_yco = dest_id % num_cols;
    int source_xco = source_id / num_cols;
    int source_yco = source_id % num_cols;

    int x=dest_xco-xco;
    int y=dest_yco-yco;
    int m=dest_xco-source_xco;
    int n=dest_yco-source_yco;
    if (m<0)
        m=0-m;
    if (n<0)
        n=0-n;

    int x1,y1;
    x1=x;
    y1=y;
    if (x<0)
        x1=0-x;
    if (y<0)
        y1=0-y;
    int min_rec;
    if (m==0)
    {
        min_rec=n+1;
    }
    else if (n==0)
    {
        min_rec=m+1;
    }
    else
    {
        min_rec=(m+1)*(n+1);
    }
    float f;
    float p1,p2;
    f=(float)(k*min_rec)/(num_rows*num_cols);
    set_avail[0]=set_avail[1]=5;
    if (x1==0 && y1==0)
    {
        p1=p2=1;
        set_avail[0]=C;
        set_avail[1]=C;
        return set_avail[0];
    }

    if (id==source_id)//if current node is the source node, then ratio is x+f:y+f
    {
        if (y1+f>x1+f)//probability of going in the y direction is more than in the x direction
        {
            //avail is an array containing directions in order of their probability
            //which direction whether north or south should be selected
            p1=(y1+f)/(x1+y1+2*f);
            p2=(x1+f)/(x1+y1+2*f);
            if (yco>dest_yco && !borderW(id))
                set_avail[0]=W;
            else if (yco<dest_yco && !borderE(id))
                set_avail[0]=E;
            else if (yco==dest_yco)
            {
                if (xco>dest_xco && !borderN(id))
                    set_avail[0]=N;
                else if (xco<dest_xco && !borderS(id))
                    set_avail[0]=S;
                else if (xco==dest_xco)
                    set_avail[0]=C;
            }
            if (xco>dest_xco && !borderN(id))
                set_avail[1]=N;
            else if (xco<dest_xco && !borderS(id))
                set_avail[1]=S;
            else
                set_avail[1]=set_avail[0];
        }
        else//probability of going in x direction is more than in the y direction
        {
            p1=(x1+f)/(x1+y1+2*f);

            p2=(y1+f)/(x1+y1+2*f);
            if (xco>dest_xco && !borderN(id))
                set_avail[0]=N;
            else if (xco<dest_xco && !borderS(id))
                set_avail[0]=S;
            else if (xco==dest_xco)
            {
                if (yco>dest_yco && !borderW(id))
                    set_avail[0]=W;
                else if (yco<dest_yco && !borderE(id))
                    set_avail[0]=E;
                else if (yco==dest_yco)
                    set_avail[0]=C;
            }
            if (yco>dest_yco && !borderW(id))
                set_avail[1]=W;
            else if (yco<dest_yco && !borderE(id))
                set_avail[1]=E;
            else
                set_avail[1]=set_avail[0];
        }
    }

    else//current node is an intermediate node
    {
        //two possibilities: 1. packet arrives on the Y-axis ingress, 2.packet arrives on the X-axis ingress

        ///1. Y-axis ingress
        if (ip_dir==E)//arrives on the Y-axis ingress from the east direction
        {
            if (y1==0)
            {
                p1=p2=1;
                if (xco>dest_xco && !borderN(id))
                {
                    set_avail[0]=N;
                    set_avail[1]=N;
                }
                else if (xco<dest_xco && !borderS(id))
                {
                    set_avail[0]=S;
                    set_avail[1]=S;
                }
                else
                {
                    set_avail[0]=C;
                    set_avail[1]=C;
                }
            }
            else
            {
                if (y1+f>=x1)
                {
                    p1=(y1+f)/(y1+f+x1);
                    p2=(x1)/(y1+f+x1);
                    if (!borderW(id))
                        set_avail[0]=W;
                    if (xco>dest_xco && !borderN(id))
                        set_avail[1]=N;
                    else if (xco<dest_xco && !borderS(id))
                        set_avail[1]=S;
                    else
                        set_avail[1]=set_avail[0];
                }
                else
                {
                    p1=(x1)/(y1+f+x1);
                    p2=(y1+f)/(y1+f+x1);
                    if (!borderW(id))
                        set_avail[1]=W;
                    if (xco>dest_xco && !borderN(id))
                        set_avail[0]=N;
                    else if (xco<dest_xco && !borderS(id))
                        set_avail[0]=S;
                    else
                        set_avail[0]=set_avail[1];
                }
            }
        }
        else if (ip_dir==W)//arrives on the Y-axis ingress from the west direction
        {
            if (y1==0)
            {
                p1=p2=1;
                if (xco>dest_xco && !borderN(id))
                {
                    set_avail[0]=N;
                    set_avail[1]=N;
                }
                else if (xco<dest_xco && !borderS(id))
                {
                    set_avail[0]=S;
                    set_avail[1]=S;
                }
                else
                {
                    set_avail[0]=C;
                    set_avail[1]=C;
                }
            }
            else
            {
                if (y1+f>=x1)
                {
                    p1=(y1+f)/(y1+f+x1);
                    p2=(x1)/(y1+f+x1);
                    if (!borderE(id))
                        set_avail[0]=E;
                    if (xco>dest_xco && !borderN(id))
                        set_avail[1]=N;
                    else if (xco<dest_xco && !borderS(id))
                        set_avail[1]=S;
                    else
                        set_avail[1]=set_avail[0];
                }
                else
                {
                    p1=(x1)/(y1+f+x1);
                    p2=(y1+f)/(y1+f+x1);
                    if (!borderE(id))
                        set_avail[1]=E;
                    if (xco>dest_xco && !borderN(id))
                        set_avail[0]=N;
                    else if (xco<dest_xco && !borderS(id))
                        set_avail[0]=S;
                    else
                        set_avail[0]=set_avail[1];
                }
            }
        }

        ///2. X-axis ingress
        else if (ip_dir==N)//arrives on the X-axis ingress from the north direction
        {

            if (x1==0)
            {
                p1=p2=1;
                //ADD
                if (yco>dest_yco && !borderW(id))
                {
                    set_avail[0]=W;
                    set_avail[1]=W;
                }
                else if (yco<dest_yco && !borderE(id))
                {
                    set_avail[0]=E;
                    set_avail[1]=E;
                }
                else
                {
                    set_avail[0]=C;
                    set_avail[1]=C;
                }
            }
            else
            {
                if (x1+f>=y1)
                {
                    p1=(x1+f)/(x1+y1+f);
                    p2=(y1)/(x1+y1+f);
                    if (!borderS(id))
                        set_avail[0]=S;
                    if (yco>dest_yco && !borderW(id))
                        set_avail[1]=W;
                    else if (yco<dest_yco && !borderE(id))
                        set_avail[1]=E;
                    else
                        set_avail[1]=set_avail[0];
                }
                else
                {
                    p1=(y1)/(x1+y1+f);
                    p2=(x1+f)/(x1+y1+f);
                    if (!borderS(id))
                        set_avail[1]=S;
                    if (yco>dest_yco && !borderW(id))
                        set_avail[0]=W;
                    else if (yco<dest_yco && !borderE(id))
                        set_avail[0]=E;
                    else
                        set_avail[0]=set_avail[1];
                }
            }
        }
        else if (ip_dir==S)//arrives on the X-axis ingress from the south direction
        {
            if (x1==0)
            {
                p1=p2=1;

                if (yco>dest_yco && !borderW(id))
                {
                    set_avail[0]=W;
                    set_avail[1]=W;
                }
                else if (yco<dest_yco && !borderE(id))
                {
                    set_avail[0]=E;
                    set_avail[1]=E;
                }
                else
                {
                    set_avail[0]=C;
                    set_avail[1]=C;
                }
            }
            else
            {
                if (x1+f>=y1)
                {
                    p1=(x1+f)/(x1+y1+f);
                    p2=(y1)/(x1+y1+f);
                    if (!borderN(id))
                        set_avail[0]=N;
                    if (yco>dest_yco && !borderW(id))
                        set_avail[1]=W;
                    else if (yco<dest_yco && !borderE(id))
                        set_avail[1]=E;
                    else
                        set_avail[1]=set_avail[0];
                }
                else
                {
                    p1=(y1)/(x1+y1+f);
                    p2=(x1+f)/(x1+y1+f);
                    if (!borderN(id))
                        set_avail[1]=N;
                    if (yco>dest_yco && !borderW(id))
                        set_avail[0]=W;
                    else if (yco<dest_yco && !borderE(id))
                        set_avail[0]=E;
                    else
                        set_avail[0]=set_avail[1];
                }
            }
        }
    }
    //int a;
    //cout<<endl<<power[set_avail[0]]<<" "<<power[set_avail[1]]<<endl;
    //cin>>a;
    //srand(time(0));
    //float i;
    //i=(float)rand()/RAND_MAX;
    prng=new UniformRandomVariable(0,1);
    if (p1==p2)
    {
        if ((prng->value())<.5)
            return set_avail[0];
        else
            return set_avail[1];
    }

    if ((prng->value())<p1)
    {
        return set_avail[0];
    }
    else
    {
        return set_avail[1];
    }
}

////////////////////////////////////////////////
/// Method containing any initializations
/// inherited from base class router
////////////////////////////////////////////////
// may be empty
// definition must be included even if empty, because this is a virtual function in base class
void PROM_router::initialize()
{

}

void PROM_router::update_power(int i,double p)
{
    power[i]=p;
}

// for dynamic linking
extern "C"
{
    router *maker()
    {
        return new PROM_router;
    }
}
