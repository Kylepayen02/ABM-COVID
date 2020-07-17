#ifndef CONTRIBUTIONS_H
#define CONTRIBUTIONS_H

//
// Places
//

#include "places/place.h"
#include "places/household.h"
#include "places/school.h"
#include "places/workplace.h"

//
// Other
//

#include "common.h"
#include "agent.h"


/***************************************************** 
 * class: Contributions
 *
 * Functionality for computing of infection probability
 * contributions from agents to places and eventually 
 * mobility 
 * 
 ******************************************************/

class Contributions{
public:

	//
	// Constructors
	//

	/// \brief Default constructor only
	Contributions() = default;
	
	//
	// Contributions of agents to places 
	//
	
	/** 
	 * \brief Count contributions of an exposed agent
	 * @param agent - reference to Agent object
	 * @param time - current time
	 * @param households... - references to vectors of places 
	 */
	void compute_exposed_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);

	/** 
	 * \brief Count contributions of a symptomatic agent
	 * @param agent - reference to Agent object
	 * @param time -  current time
	 * @param households... - references to vectors of places
	 */
	void compute_symptomatic_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);

	/// \brief Compute the total contribution to infection probability at every place
	void total_place_contributions(std::vector<Household>& households, 
					std::vector<School>& schools, std::vector<Workplace>& workplaces);

	/// \brief Set contributions/sums from all agents in places to 0.0 
	void reset_sums(std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);



private:

	//
	// Specific contribution types
	//
	
	/// \brief Count contributions of a untreated and not tested symptomatic agent
	void compute_regular_symptomatic_contributions(const Agent& agent,  
				const double inf_var, std::vector<Household>& households, 
				std::vector<School>& schools, std::vector<Workplace>& workplaces);

};
#endif













