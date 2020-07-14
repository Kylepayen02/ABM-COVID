#ifndef SCHOOL_H
#define SCHOOL_H

#include "place.h"

class Place;

/***************************************************** 
 * class: School 
 * 
 * Defines and stores attributes of a single school
 * 
 *****************************************************/

class School : public Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a School object with default attributes
	 */
	School() = default;

	/**
	 * \brief Creates a School object
	 * \details School with custom ID, location, and infection parameters
	 *
	 * @param school_ID - ID of the school 
	 * @param xi - x coordinate of the school
	 * @param yi - y coordinate of the school
	 * @param severity_cor - severity correction for symptomatic
	 * @param psi - absenteeism correction
	 * @param beta - infection transmission rate, 1/time
	 */
	School(const int school_ID, const double xi, const double yi, 
			const double severity_cor, const double psi, const double beta) : 
		psi_j(psi), Place(school_ID, xi, yi, severity_cor, beta){ } 

	//
	// Infection related computations
	//

	/** 
	 *  \brief Include symptomatic contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic(double inf_var) override { lambda_sum += inf_var*ck*beta_j*psi_j; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a School object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | beta_j | psi_j 
	 * 		Delimiter is a space.
	 * 	@param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

private:
	// Absenteeism correction
	double psi_j = 0.0;
};

#endif
