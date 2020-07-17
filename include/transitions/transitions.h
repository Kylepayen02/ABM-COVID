#ifndef TRANSITIONS_H
#define TRANSITIONS_H

//
// Places
//

#include "../places/place.h"
#include "../places/household.h"
#include "../places/school.h"
#include "../places/workplace.h"

//
// Transitions
//

#include "regular_transitions.h"


//
// Other
//

#include "../common.h"
#include "../agent.h"
#include "../infection.h"


/***************************************************** 
 * class: Transitions 
 *
 * Interface for computing of transitioning 
 * between different agents states 
 * 
 ******************************************************/

class Transitions{
public:

	//
	// Constructors
	//
	
	/// Creates a Transitions object with default attributes
	Transitions() = default;

	//
	// Transitioning functionality
	//

	/// \brief Implement transitions relevant to susceptible
	/// \details Returns 1 if the agent got infected 
	int susceptible_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,const std::map<std::string,
				double>& infection_parameters, std::vector<Agent>& agents);

	/// \brief Implement transitions relevant to exposed
	/// \details Return 1 if recovered without symptoms 
	int exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,const std::map<std::string,
				double>& infection_parameters);

	/// \brief Transitions of a symptomatic agent 
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> symptomatic_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,const std::map<std::string,
				double>& infection_parameters);


private:
	
	// Transition classes
	RegularTransitions regular_tr;

};

#endif
