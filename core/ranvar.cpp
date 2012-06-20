/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) Xerox Corporation 1997. All rights reserved.
 *
 * License is granted to copy, to use, and to make and to use derivative
 * works for research and evaluation purposes, provided that Xerox is
 * acknowledged in all documentation pertaining to any such copy or derivative
 * work. Xerox grants no other licenses expressed or implied. The Xerox trade
 * name should not be used in any advertising without its written permission.
 *
 * XEROX CORPORATION MAKES NO REPRESENTATIONS CONCERNING EITHER THE
 * MERCHANTABILITY OF THIS SOFTWARE OR THE SUITABILITY OF THIS SOFTWARE
 * FOR ANY PARTICULAR PURPOSE.  The software is provided "as is" without
 * express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this software.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /nfs/jade/vint/CVSROOT/ns-2/tools/ranvar.cc,v 1.18 2002/03/19 07:10:16 ddutta Exp $ (Xerox)";
#endif

#include <stdio.h>
#include "ranvar.h"

RandomVariable::RandomVariable()
{
    //rng_ = RNG::defaultrng();
    //rng_ = new RNG;
    rng_ = new RNG((RNG::RNGSources)2,1);
}


// Added by Debojyoti Dutta 12 October 2000
// This allows us to seed a randomvariable with
// our own RNG object. This command is called from
// expoo.cc and pareto.cc

int  RandomVariable::seed(char *x)
{

    return 1;
}


static class UniformRandomVariableClass
{
public:
    UniformRandomVariableClass() {}
} class_uniformranvar;

UniformRandomVariable::UniformRandomVariable()
{
}

UniformRandomVariable::UniformRandomVariable(double min, double max)
{
    min_ = min;
    max_ = max;
}

double UniformRandomVariable::value()
{
    return(rng_->uniform(min_, max_));
}


static class ExponentialRandomVariableClass
{
public:
    ExponentialRandomVariableClass() {}
} class_exponentialranvar;

ExponentialRandomVariable::ExponentialRandomVariable()
{
}

ExponentialRandomVariable::ExponentialRandomVariable(double avg)
{
    avg_ = avg;
}

double ExponentialRandomVariable::value()
{
    return(rng_->exponential(avg_));
}


static class ParetoRandomVariableClass
{
public:
    ParetoRandomVariableClass() {}
} class_paretoranvar;

ParetoRandomVariable::ParetoRandomVariable()
{
}

ParetoRandomVariable::ParetoRandomVariable(double avg, double shape)
{
    avg_ = avg;
    shape_ = shape;
}

double ParetoRandomVariable::value()
{
    /* yuck, user wants to specify shape and avg, but the
     * computation here is simpler if we know the 'scale'
     * parameter.  right thing is to probably do away with
     * the use of 'bind' and provide an API such that we
     * can update the scale everytime the user updates shape
     * or avg.
     */
    return(rng_->pareto(avg_ * (shape_ -1)/shape_, shape_));
}

/* Pareto distribution of the second kind, aka. Lomax distribution */
static class ParetoIIRandomVariableClass
{
public:
    ParetoIIRandomVariableClass() {}
} class_paretoIIranvar;

ParetoIIRandomVariable::ParetoIIRandomVariable()
{

}

ParetoIIRandomVariable::ParetoIIRandomVariable(double avg, double shape)
{
    avg_ = avg;
    shape_ = shape;
}

double ParetoIIRandomVariable::value()
{
    return(rng_->paretoII(avg_ * (shape_ - 1), shape_));
}

static class NormalRandomVariableClass
{
public:
    NormalRandomVariableClass() {}

} class_normalranvar;

NormalRandomVariable::NormalRandomVariable()
{

}

double NormalRandomVariable::value()
{
    return(rng_->normal(avg_, std_));
}

static class LogNormalRandomVariableClass
{
public:
    LogNormalRandomVariableClass() {}

} class_lognormalranvar;

LogNormalRandomVariable::LogNormalRandomVariable()
{

}

double LogNormalRandomVariable::value()
{
    return(rng_->lognormal(avg_, std_));
}

static class ConstantRandomVariableClass
{
public:
    ConstantRandomVariableClass() {}

} class_constantranvar;

ConstantRandomVariable::ConstantRandomVariable()
{
    //bind("val_", &val_);
}

ConstantRandomVariable::ConstantRandomVariable(double val)
{
    val_ = val;
}

double ConstantRandomVariable::value()
{
    return(val_);
}


/* Hyperexponential distribution code adapted from code provided
 * by Ion Stoica.
 */

/*static class HyperExponentialRandomVariableClass {
public:
	HyperExponentialRandomVariableClass() {}

} class_hyperexponentialranvar;

HyperExponentialRandomVariable::HyperExponentialRandomVariable()
{

	alpha_ = .95;
}

HyperExponentialRandomVariable::HyperExponentialRandomVariable(double avg, double cov)
{
	alpha_ = .95;
	avg_ = avg;
	cov_ = cov;
}

double HyperExponentialRandomVariable::value()
{
	double temp, res;
	double u = Random::uniform();

	temp = sqrt((cov_ * cov_ - 1.0)/(2.0 * alpha_ * (1.0 - alpha_)));
	if (u < alpha_)
		res = Random::exponential(avg_ - temp * (1.0 - alpha_) * avg_);
	else
		res = Random::exponential(avg_ + temp * (alpha_) * avg_);
	return(res);
}*/

/*
// Empirical Random Variable:
//  CDF input from file with the following column
//   1.  Possible values in a distrubutions
//   2.  Number of occurances for those values
//   3.  The CDF for those value
//  code provided by Giao Nguyen
*/

static class EmpiricalRandomVariableClass
{
public:
    EmpiricalRandomVariableClass() {}

} class_empiricalranvar;

EmpiricalRandomVariable::EmpiricalRandomVariable() : minCDF_(0), maxCDF_(1), maxEntry_(32), table_(0)
{

}

int EmpiricalRandomVariable::loadCDF(const char* filename)
{
    FILE* fp;
    char line[256];
    CDFentry* e;

    fp = fopen(filename, "r");
    if (fp == 0)
        return 0;


    if (table_ == 0)
        table_ = new CDFentry[maxEntry_];
    for (numEntry_=0;  fgets(line, 256, fp);  numEntry_++)
    {
        if (numEntry_ >= maxEntry_)  	// resize the CDF table
        {
            maxEntry_ *= 2;
            e = new CDFentry[maxEntry_];
            for (int i=numEntry_-1; i >= 0; i--)
                e[i] = table_[i];
            delete table_;
            table_ = e;
        }
        e = &table_[numEntry_];
        // Use * and l together raises a warning
        sscanf(line, "%lf %*f %lf", &e->val_, &e->cdf_);
    }
    fclose(fp);
    return numEntry_;
}

double EmpiricalRandomVariable::value()
{
    if (numEntry_ <= 0)
        return 0;
    double u = rng_->uniform(minCDF_, maxCDF_);
    int mid = lookup(u);
    if (mid && interpolation_ && u < table_[mid].cdf_)
        return interpolate(u, table_[mid-1].cdf_, table_[mid-1].val_,
                           table_[mid].cdf_, table_[mid].val_);
    return table_[mid].val_;
}

double EmpiricalRandomVariable::interpolate(double x, double x1, double y1, double x2, double y2)
{
    double value = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
    if (interpolation_ == INTER_INTEGRAL)	// round up
        return ceil(value);
    return value;
}

int EmpiricalRandomVariable::lookup(double u)
{
    // always return an index whose value is >= u
    int lo, hi, mid;
    if (u <= table_[0].cdf_)
        return 0;
    for (lo=1, hi=numEntry_-1;  lo < hi; )
    {
        mid = (lo + hi) / 2;
        if (u > table_[mid].cdf_)
            lo = mid + 1;
        else hi = mid;
    }
    return lo;
}
