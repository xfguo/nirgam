
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

#include "XY_router.h"
#include "../../config/extern.h"

////////////////////////////////////////////////
/// Method to set unique id
////////////////////////////////////////////////
void XY_router::setID(UI id_tile)
{
    id = id_tile;
    initialize();
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
UI XY_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id)
{
    int xco = id / num_cols;
    int yco = id % num_cols;
    int dest_xco = dest_id / num_cols;
    int dest_yco = dest_id % num_cols;
    int dirx= ND,diry=ND;
    int id_E = id +1, id_W = id -1, id_N = id - num_cols, id_S = id + num_cols;
    int choose=0; // 0 for X-dir and 1 for Y-dir
    int markx=0, marky=0;
    if (dest_xco == xco && dest_yco == yco)
        return C;

    if (dest_yco > yco)
        dirx= E;
    else if (dest_yco < yco)
        dirx= W;

    if (dest_xco < xco)
        diry= N;
    else if (dest_xco > xco)
        diry= S;

    //if the node itself is a marked node
    for (int i=0; i<failno ; i++)
    {
        if (id == fail[i][0])
        {
            if (id_E == fail[i][1] && dirx == E)
            {
                dirx=ND;
            }
            if (id_S == fail[i][1] && diry == S)
            {
                diry=ND;
            }
        }
        if (id == fail[i][1])
        {
            if (id_W == fail[i][0] && dirx == W)
            {
                dirx=ND;
            }
            if (id_N == fail[i][0] && diry == N)
            {
                diry=ND;
            }
        }
    }

    // end self marked node check

    if (dirx== ND)
        return diry;
    if (diry== ND)
        return dirx;

    // Mark nodes in the x direction
    if (dirx == E)
    {
        for (int i=0; i<failno ; i++)
        {
            if (id_E == fail[i][0] || id_E == fail[i][1])
            {
                markx = 1;
            }

        }
    }
    else if (dirx == W)
    {
        for (int i=0; i<failno ; i++)
        {
            if (id_W == fail[i][0] || id_W == fail[i][1])
            {
                markx = 1;
            }

        }
    }

    // Mark nodes in the y direction

    if (diry == N)
    {
        for (int i=0; i<failno ; i++)
        {
            if (id_N == fail[i][0] || id_N == fail[i][1])
            {
                marky = 1;
            }
        }
    }
    else if (diry == S)
    {
        for (int i=0; i<failno ; i++)
        {
            if (id_S == fail[i][0] || id_S == fail[i][1])
            {
                marky = 1;
            }

        }
    }

    if (markx==0 && marky==0)
    {
        choose=(id%2);
    }

    if (markx==0 && marky==1)
    {
        choose=0;
    }
    if (markx==1 && marky==0)
    {
        choose=1;
    }
    if (markx==1 && marky==1)
    {
        choose=(id%2);
    }
//	cout<<"\nTile ID "<<id<<"CHOSEN DIR "<< choose<< " dirX= "<< dirx << " dirY= " << diry<<"\n";

    switch (choose)
    {
    case 0:
        return dirx;
        break;
    case 1:
        return diry;
        break;
    }
    return 0;
}

////////////////////////////////////////////////
/// Method containing any initializations
/// inherited from base class router
////////////////////////////////////////////////
// may be empty
// definition must be included even if empty, because this is a virtual function in base class
void XY_router::initialize()
{

}

// for dynamic linking
extern "C"
{
    router *maker()
    {
        return new XY_router;
    }
}
