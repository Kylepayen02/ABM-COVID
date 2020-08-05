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
	 * @param psi_e - absenteeism correction for school employee
	 * @param psi - absenteeism corrent for student
	 * @param beta_e - infection transmission rate for school employee, 1/time
	 * @param beta_s - infection transmission rate for student, 1/time
	 */
	School(const int school_ID, const double xi, const double yi, 
			const double severity_cor, const double psi_e, const double psi, const double beta_e, const double beta_s) :
		psi_emp(psi_e), psi_j(psi), beta_emp(beta_e), Place(school_ID, xi, yi, severity_cor, beta_s){ }

	//
	// Infection related computations
	//

    /**
      * \brief Include symptomatic contribution in the sum
      * \brief Include exposed employee contribution in the sum
      *	@param inf_var - agent infectiousness variability factor
      */
    void add_exposed_employee(double inf_var) { lambda_sum += inf_var*beta_emp; }

    /**
     * \brief Include symptomatic employee contribution in the sum
     * @param inf_var - agent infectiousness variability factor
     */
    void add_symptomatic_employee(double inf_var) { lambda_sum += inf_var*ck*beta_emp*psi_emp; }

    /**
     * \brief Include symptomatic student contribution in the sum
     * @param inf_var - agent infectiousness variability factor
     */
    void add_symptomatic_student(double inf_var) { lambda_sum += inf_var*ck*beta_j*psi_j; }

    //
    // Setters
    //
    void change_employee_transmission_rate (const double new_rate) { beta_emp = new_rate; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a School object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | beta_s | psi_s
	 * 		Delimiter is a space.
	 * 	@param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

private:
	// Absenteeism correction - employee, students
	double psi_emp = 0.0;
	double psi_j = 0.0;
	// Employee infection transmission rate , 1/time
	double beta_emp = 0.0;
};

#endif
