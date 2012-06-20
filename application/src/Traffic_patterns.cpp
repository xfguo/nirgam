
/*
 * Traffic_patterns.cpp
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

#include "Traffic_patterns.h"

void traffic_patterns::fixRanges(int &destx , int &desty,int num_rows,int num_cols)
{
    // Fix ranges
    if (destx<0) destx=0;
    if (desty<0) desty=0;
    if (destx>=num_rows) destx=num_rows-1;
    if (desty>=num_cols) desty=num_cols-1;
}

void traffic_patterns::setBit(int &x, int w, int v)
{
    int mask = 1 << w;

    if (v == 1)
        x = x | mask;
    else if (v == 0)
        x = x & ~mask;
    else
        assert(false);

}
int traffic_patterns::getBit(int x, int w)
{
    return (x >> w) & 1;
}
double traffic_patterns::log2ceil(double x)
{
    return ceil(log(x)/log(2.0));
}


int traffic_patterns::trafficTranspose(int tileID,int num_rows, int num_cols)
{
    int srcx;
    int srcy;
    int destx;
    int desty;
    int dnode;
    // Transpose  destination distribution
    srcx = tileID/num_cols;

    srcy = tileID%num_rows;
    destx=srcy;
    desty=srcx;
    fixRanges(destx, desty,num_rows,num_cols);
    dnode = desty+destx*num_cols;
    return dnode;
}

int traffic_patterns::trafficShuffle(int tileID, int num_rows, int num_cols)
{
    int nbits = (int)log2ceil((double)(num_rows*num_cols));
    int dnode = 0;
    for (int i=0; i<nbits-1; i++)
        setBit(dnode, i+1, getBit(tileID, i));
    setBit(dnode, 0, getBit(tileID, nbits-1));
    if (dnode>=num_rows*num_cols)
    {
        dnode=num_rows*num_cols-1;
    }
    return dnode;
}

int traffic_patterns::trafficBitReversal(int tileID, int num_rows, int num_cols)
{

    int nbits = (int)log2ceil((double)(num_rows*num_cols));
    int dnode = 0;
    for (int i=0; i<nbits; i++)
        setBit(dnode, i, getBit(tileID, nbits-i-1));

    if (dnode>=num_rows*num_cols)
    {
        dnode=num_rows*num_cols-1;
    }
    return dnode;
}
int traffic_patterns::trafficButterfly(int tileID, int num_rows, int num_cols)
{

    int nbits = (int)log2ceil((double)(num_rows*num_cols));
    int dnode = 0;
    for (int i=1; i<nbits-1; i++)
        setBit(dnode, i, getBit(tileID, i));
    setBit(dnode, 0, getBit(tileID, nbits-1));
    setBit(dnode, nbits-1, getBit(tileID, 0));
    if (dnode>=num_rows*num_cols)
    {
        dnode=num_rows*num_cols-1;
    }

    return dnode;

}

