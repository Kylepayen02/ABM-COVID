#ifndef REGULAR_TRANSITIONS_H
#define REGULAR_TRANSITIONS_H

//
// Places
//

#include "../places/place.h"
#include "../places/household.h"
#include "../places/school.h"
#include "../places/workplace.h"
#include "../places/hospital.h"

//
// Other
//

#include "../common.h"
#include "../agent.h"
#include "../infection.h"
#include "../states_manager/regular_states_manager.h"


/***************************************************** 
 * class: RegularTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for a most general
 * case of agent 
 * 
 ******************************************************/

class RegularTransitions{
public:

	//
	// Constructors
	//
	
	/// Creates a RegularTransitions object with default attributes
	RegularTransitions() = default;

	/// \brief Implement transitions relevant to susceptible
	/// \details Returns 1 if the agent got infected 
	int susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,
				const std::map<std::string, double>& infection_parameters, std::vector<Agent>& agents);

	/// \brief Implement transitions relevant to exposed
	/// \details Return 1 if recovered without symptoms 
	int exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, const std::map<std::string, double>& infection_parameters);



	/// \brief Transitions of a symptomatic agent 
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> symptomatic_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,
				const std::map<std::string, double>& infection_parameters);

private:

	// For changing agent states
	RegularStatesManager states_manager;

	/// \brief Return total lambda of susceptible agent
	double compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces);

	/// \brief Compte and set agent properties related to recovery without symptoms and incubation
	void recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				                const std::map<std::string, double>& infection_parameters);


	/// \brief Verifies and manages removal of an agent from the model
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);

	/// \brief Remove agent's ID from places where they are registered
	void remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);

	/// \brief Remove agent's index from all workplaces and schools that have them registered
	void remove_from_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces);

	/// \brief Add agent's ID back to the places where they are registered
	/// \details This is done to keep realistic numbers of people in different places
	/// 	which influences the probability
	void add_agent_to_all_places(const Agent& agent,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces);

	/// \brief Add agent's index from all workplaces and schools that have them registered
	void add_to_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces);
};

#endif
