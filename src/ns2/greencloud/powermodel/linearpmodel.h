/*
 * linearpmodel.h
 *
 *  @date Feb 25, 2014
 *  @author Guzek:Mateusz
 */

#ifndef LINEARPMODEL_H_
#define LINEARPMODEL_H_

#include "powermodel.h"

class LinearPModel: public PowerModel {
public:
	LinearPModel();
	virtual ~LinearPModel();
	virtual int command(int argc, const char*const* argv);
	virtual double estimate(int size, double* predictors);
	virtual double getMaxPower();
	virtual void addComponent(DcResource* component);
	virtual void print();
private:
	double * coefficients; /**< Slopes as the resource types. The last element is the intercept */
	bool * initialized; /**< Initialization flag */
	bool ready; /**< Model initialized and ready for usage */
	void setCoefficient(const char* coef,double value);
	void setCoefficientNumeric(const char* coef,double value);
	void updateInit();
	int coef_number;
	void setCoefNumber(int number);
};

#endif /* LINEARPMODEL_H_ */
