
/*
 * NED.cpp
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
/// \file NED_traffic.cpp
/// \brief Implements NED traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NED_traffic.h"
#include "../../config/extern.h"




////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
NEDTraffic::NEDTraffic(int num_row, int num_col, int tile_id)
{
    num_rows=num_row;
    num_cols=num_col;
    tileID=tile_id;

    dst_vect=new int[num_rows*num_rows-2];
    MatR=new int[num_rows*num_rows];
    Probability = new float[num_rows*num_rows-2];
    m=(float)1/num_rows;
    for (int i=0; i<num_rows*num_rows-2; i++)
    {
        dst_vect[i]=0;
    }
}

void NEDTraffic::get_NEDTraffic(float *tiles)
{
    int  i, j, k, a, b;
    float Q, x, p, pr, sum=0.0, term=0.0;

    *Sr=(tileID/num_rows)+1;
    *(Sr+1)=(tileID%num_cols)+1;
    //Calculating the Distance matrix  Rij and Distance frequency (DF) vector dst_vect.
    makeR();
    Q=calculateQ(0, 1);

    p=pow(Q, num_rows);
    for (i=0; i<dst_K; i++)
    {
        pr=pow(p, (i*m+1));
        term=pr;
        sum+=term*dst_vect[i+1];
        Probability[i]=term;
    }
    //ProbMatrix();
    a=Sr[0]-1;
    b=Sr[1]-1;
//	cout<<"inside ned"<<endl;
    for (i=0; i<num_rows; i++)

    {
//	cout<<"\n\n";

        for (j=0; j<num_rows; j++)
        {
            tiles[i*num_rows+j]=Probability[(int)MatR[i*num_rows+j]-1];
//			cout<<tiles[i*num_rows+j]<<"\t\t";
        }
    }

}

/////////////////////////////////////////////////
//calculate the pobability equation. and find Q ie. (Probability=Q^num_rows)
/////////////////////////////////////////////////
float NEDTraffic::calculateQ(float q1, float q2)
{
    float Q=q1+((q2-q1)/2);
    float y=0.0;
    for (int i=0; i<dst_K; i++)
    {
        y += dst_vect[i+1]*(pow(Q, i+(1/m)));
    }
    y=y-1;
    if (y>=-0.000005 && y<=0.000005)
        return Q;
    if (y<0)
        Q=calculateQ(Q, q2);
    else
        Q=calculateQ(q1, Q);

    return Q;
}

/////////////////////////////////////////////////
// Making the distance matrix R
/////////////////////////////////////////////////
void NEDTraffic::makeR()
{
    //for( int i=0; i<num_rows*num_rows-2; i++) cout<<"\ninmakeR===== dst_vect= "<<dst_vect[i];
    int  a=Sr[0]-1, b=Sr[1]-1;
    int dst, x, y;
    dst_K=0;

    for (int i=0; i<num_rows; i++)
        for (int j=0; j<num_cols; j++)
        {
            x=i-a;
            y=j-b;
            if (x<0) x = x * (-1);
            if (y<0) y = y * (-1);
            dst = x+y;
            MatR[i*num_rows+j]=dst;
            if (dst > dst_K)
                dst_K = dst;
        }
    //printf("\n from sourcr %d  %d  the distance matrix is : ", x, y);

    //cout<<"\n Max Rij dst_K = "<<dst_K<<endl;
    for (int i=0; i<num_rows; i++)
        for (int j=0; j<num_cols; j++)
        {
            dst_vect[MatR[i*num_rows+j]]+=1;
        }
}

