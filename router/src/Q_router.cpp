
/*
 * XY_router.cpp
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
/// \file XY_router.cpp
/// \brief Implements XY routing algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Q_router.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Method to set unique id
////////////////////////////////////////////////
void Q_router::setID(UI id_tile)
{
    id = id_tile;
}

////////////////////////////////////////////////
/// Method that implements XY routing
/// inherited from base class router
/// Parameters:
/// - input direction from which flit entered the tile
/// - tileID of source tile
/// - tileID of destination tile
/// returns next hop direction
////////////////////////////////////////////////
UI Q_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id)
{

    if (ip_dir == 0)
    {
        if (dest_id == id)
            return num_nb;
        UI rand = ran_var->uniform(3000);
        UI minQ;
        UI mindir;
        UI gama = 3000* DELTA;
        if (rand > gama)
        {
            mindir= rand % num_nb;
            minQ = Q[mindir][dest_id];
            //find minimum Q value
            for (int i = 0; i < num_nb; i++)
            {
                if (minQ > Q[i][dest_id])
                {
                    minQ = Q[i][dest_id];
                    mindir = i;
                }
            }
        }
        else
        {
            if (dest_id == id)
                return num_nb;
            mindir = rand % num_nb;

        }

        return mindir;

        //return mindir;*/
    }
    else
    {
        int xco = id / num_cols;
        int yco = id % num_cols;
        int dest_xco = dest_id / num_cols;
        int dest_yco = dest_id % num_cols;
        if (dest_yco > yco)
            return E;
        else if (dest_yco < yco)
            return W;
        else if (dest_yco == yco)
        {
            if (dest_xco < xco)
                return N;
            else if (dest_xco > xco)
                return S;
            else if (dest_xco == xco)
                return C;
        }

        return 0;//*/
    }


}
void Q_router::update_estimate(UI ip_dir, UI dest_id, UI estimate, ULL q)
{
//	UI qd = (UI) ((float)q*MAX_EST/SIM_NUM);
    UI qd = q;
    //cout <<"Got q value "<< q << endl;
    UI new_estimate;
    if (qd > MAXq)//if q is greater than the allow time
        qd = MAXq;
    if (estimate + qd > MAX_EST)//if the new estimate is greater than the maximum etimate value allowed
        new_estimate = MAX_EST;
    else
        new_estimate = estimate + qd;
    Q[ip_dir][dest_id] = (UI)((1-alpha)*Q[ip_dir][dest_id] + alpha*new_estimate);//weighted average of the new and old
//	cout<<"Update estimate: dest"<<dest_id<<" dir "<<ip_dir<<" Q: "<<Q[ip_dir][dest_id]<<endl;
//	if(Q[ip_dir][dest_id] < minQ[dest_id])
//		minQ[dest_id] = Q[ip_dir][dest_id];
//	cout<<" new Q = "<<Q[ip_dir][dest_id]<<endl;
}

UI Q_router::get_estimate(UI d)
{
    UI minest;
    minest = Q[0][d];
    for (int i = 1; i < num_nb; i++)
        if (minest > Q[i][d])
            minest = Q[i][d];

    return minest;
}

////////////////////////////////////////////////
/// Method containing any initializations
/// inherited from base class router
////////////////////////////////////////////////
// may be empty
// definition must be included even if empty, because this is a virtual function in base class
void Q_router::initialize()
{
    ran_var = new RNG((RNG::RNGSources)2,1);
    alpha = ALPHA;
    if (num_nb == 0)
        cout<<"Num_ip = "<<num_nb<<" and num_tiles = "<<num_tiles<<" in Q_Router"<<endl;
    else
    {
//		cout<<"Num_ip = "<<num_nb<<" in Q_Router"<<" and num_tiles = "<<num_tiles<<endl;
        Q = new UI*[num_nb];
        for (int i = 0; i < num_nb; i++)
        {
            if (num_tiles == 0)
                cout<<" Error: Number of tiles 0"<<endl;
            else
            {
                Q[i] = new UI[num_tiles];
                for (int j = 0; j < num_tiles; j++)
                {
                    Q[i][j] = 20;
                }
                Q[i][id] = 0;
            }
        }
    }
}

// for dynamic linking
extern "C"
{
    router *maker()
    {
        return new Q_router;
    }
}
