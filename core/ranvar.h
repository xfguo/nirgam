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
 *
 * @(#) $Header: /nfs/jade/vint/CVSROOT/ns-2/tools/ranvar.h,v 1.15 2002/03/19 07:10:16 ddutta Exp $ (Xerox)
 */

#ifndef ns_ranvar_h
#define ns_ranvar_h

/* XXX still need to clean up dependencies among parameters such that
 * when one parameter is changed, other parameters are recomputed as
 * appropriate.
 */

//#include "random.h"
#include "rng.h"

class RandomVariable {
 public:
	virtual double value() = 0;
	virtual double avg() = 0;
	RandomVariable();
	// This is added by Debojyoti Dutta 12th Oct 2000
	int seed(char *);
 protected:
	RNG* rng_;
};

class UniformRandomVariable : public RandomVariable {
 public:
	virtual double value();
	virtual inline double avg() { return (max_-min_)/2; };
	UniformRandomVariable();
	UniformRandomVariable(double, double);
	double* minp()	{ return &min_; };
	double* maxp()	{ return &max_; };
	double min()	{ return min_; };
	double max()	{ return max_; };
	void setmin(double d)	{ min_ = d; };
	void setmax(double d)	{ max_ = d; };
 private:
	double min_;
	double max_;
};

class ExponentialRandomVariable : public RandomVariable {
 public:
	virtual double value();
	ExponentialRandomVariable();
	ExponentialRandomVariable(double);
	double* avgp() { return &avg_; };
	virtual inline double avg() { return avg_; };
	void setavg(double d) { avg_ = d; };
 private:
	double avg_;
};

class ParetoRandomVariable : public RandomVariable {
 public:
	virtual double value();
	ParetoRandomVariable();
	ParetoRandomVariable(double, double);
	double* avgp() { return &avg_; };
	double* shapep() { return &shape_; };
	virtual inline double avg()	{ return avg_; };
	double shape()	{ return shape_; };
	void setavg(double d)	{ avg_ = d; };
	void setshape(double d)	{ shape_ = d; };
 private:
	double avg_;
	double shape_;
	double scale_;
};

class ParetoIIRandomVariable : public RandomVariable {
 public:
        virtual double value();
        ParetoIIRandomVariable();
        ParetoIIRandomVariable(double, double);
        double* avgp() { return &avg_; };
        double* shapep() { return &shape_; };
        virtual inline double avg()   { return avg_; };
        double shape()   { return shape_; };
        void setavg(double d)  { avg_ = d; };
        void setshape(double d)  { shape_ = d; };
 private:
        double avg_;
        double shape_;
        double scale_;
};

class NormalRandomVariable : public RandomVariable {
 public:
        virtual double value();
        NormalRandomVariable();
        inline double* avgp() { return &avg_; };
        inline double* stdp() { return &std_; };
        virtual inline double avg()     { return avg_; };
        inline double std()     { return std_; };
        inline void setavg(double d)    { avg_ = d; };
        inline void setstd(double d)    { std_ = d; };
 private:
        double avg_;
        double std_;
};

class LogNormalRandomVariable : public RandomVariable {
public:
        virtual double value();
        LogNormalRandomVariable();
        inline double* avgp() { return &avg_; };
        inline double* stdp() { return &std_; };
        virtual inline double avg()     { return avg_; };
        inline double std()     { return std_; };
        inline void setavg(double d)    { avg_ = d; };
        inline void setstd(double d)    { std_ = d; };
private:
        double avg_;
        double std_;
};

class ConstantRandomVariable : public RandomVariable {
 public:
	virtual double value();
	virtual double avg(){ return val_;}
	ConstantRandomVariable();
	ConstantRandomVariable(double);
	double* valp() { return &val_; };
	double val() { return val_; };
	void setval(double d) { val_ = d; };
 private:
	double val_;
};

/*class HyperExponentialRandomVariable : public RandomVariable {
 public:
	virtual double value();
	HyperExponentialRandomVariable();
	HyperExponentialRandomVariable(double, double);
	double* avgp()	{ return &avg_; };
	double* covp()	{ return &cov_; };
	virtual double avg()	{ return avg_; };
	double cov()	{ return cov_; };
	void setavg(double d)	{ avg_ = d; };
	void setcov(double d)	{ cov_ = d; };
 private:
	double avg_;
	double cov_;
	double alpha_;
};*/


#define INTER_DISCRETE 0	// no interpolation (discrete)
#define INTER_CONTINUOUS 1	// linear interpolation
#define INTER_INTEGRAL 2	// linear interpolation and round up

struct CDFentry {
	double cdf_;
	double val_;
};

class EmpiricalRandomVariable : public RandomVariable {
public:
	virtual double value();
	virtual double interpolate(double u, double x1, double y1, double x2, double y2);
	virtual double avg(){ return value(); } // junk
	EmpiricalRandomVariable();
	double& minCDF() { return minCDF_; }
	double& maxCDF() { return maxCDF_; }
	int loadCDF(const char* filename);

protected:
	int command(int argc, const char*const* argv);
	int lookup(double u);

	double minCDF_;		// min value of the CDF (default to 0)
	double maxCDF_;		// max value of the CDF (default to 1)
	int interpolation_;	// how to interpolate data (INTER_DISCRETE...)
	int numEntry_;		// number of entries in the CDF table
	int maxEntry_;		// size of the CDF table (mem allocation)
	CDFentry* table_;	// CDF table of (val_, cdf_)
};

#endif




