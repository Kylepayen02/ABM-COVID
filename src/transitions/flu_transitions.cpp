#include "../../include/transitions/flu_transitions.h"

/***************************************************** 
 * class: FluTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for an agent that
 * has a condition other than COVID and is symptomatic  
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int FluTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const double dt)
{
	double lambda_tot = 0.0;
	int got_infected = 0;

	lambda_tot = compute_susceptible_lambda(agent, time, households, schools, workplaces, hospitals);
	if (infection.infected(lambda_tot) == true){
		
		int new_flu = flu.swap_flu_agent(agent.get_ID());
		// If still available
		if (new_flu != -1){
			process_new_flu(agent, hospitals.size(), time, schools, workplaces, 
							infection, infection_parameters, flu);
		}
		states_manager.set_former_flu(agent);
	
		got_infected = 1;
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing 
		// symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
		// Determine if getting tested, how, and when
		// Remove agent from places if under home isolation
		set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, infection_parameters);
	} else {
		// If being tested
		if ((agent.tested()) && (agent.get_time_of_test() <= time)
				&& (agent.tested_awaiting_results() == true)){
			testing_transitions_flu(agent, time, infection_parameters);
		}
		// If getting test results (this may in principle happen in a
		// single step)
		if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
			testing_results_transitions_flu(agent, time, infection, households, 
						schools, workplaces, hospitals, infection_parameters);
		}
		// If recovering and leaving home isolation - reset all flags, swap with 
		// new flu and return back to regular susceptible
		if (agent.tested_false_positive() && agent.home_isolated() 
						&& agent.get_recovery_time() <= time){ 
			states_manager.reset_returning_flu(agent);
			add_to_all_workplaces_and_schools(agent, schools, workplaces);
			int new_flu = flu.swap_flu_agent(agent.get_ID());
			// If still available
			if (new_flu != -1){
				process_new_flu(agent, hospitals.size(), time, schools, workplaces, 
							infection, infection_parameters, flu);
			}
		}
	}	
	return got_infected;	
}

// Set properties related to newly created agent with flu, including testing
void FluTransitions::process_new_flu(Agent& agent, const int n_hospitals, const double time, 
			   		std::vector<School>& schools, std::vector<Workplace>& workplaces,	
					Infection& infection, const std::map<std::string, double>& infection_parameters, Flu& flu)
{
	agent.set_symptomatic_non_covid(true);
	// Testing properties
	if (flu.getting_tested()){
		// Home isolation for all tested
		remove_from_all_workplaces_and_schools(agent, schools, workplaces);
		if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
			states_manager.set_waiting_for_test_in_hospital(agent);
			int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
			// Registration will happen only upon testing time step
			agent.set_hospital_ID(hsp_ID);
		} else {
			states_manager.set_waiting_for_test_in_car(agent);
		}
		// Set testing times
		agent.set_time_to_test(infection_parameters.at("time from decision to test"));
		agent.set_time_of_test(time);
	}
}

// Return total lambda of susceptible agent 
double FluTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces, const std::vector<Hospital>& hospitals)			
{
	double lambda_tot = 0.0;

	// Regular susceptible agent
	const Household& house = households.at(agent.get_household_ID()-1);
	if (agent.student() && agent.works()){
		const School& school = schools.at(agent.get_school_ID()-1);
		const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution()+
						school.get_infected_contribution();
	} else if (agent.student()){
		const School& school = schools.at(agent.get_school_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						school.get_infected_contribution();
	} else if (agent.works()){
		const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution();
	} else {
		lambda_tot = house.get_infected_contribution();
	}	
	// Overwrite if being tested or in home isolation
	if ((agent.tested()) && (agent.tested_in_hospital()) 
			&& (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		const Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
		lambda_tot = hospital.get_infected_contribution();
	} else if ((agent.tested_awaiting_test() == true)
				|| (agent.tested_awaiting_results() == true)
				|| (agent.tested_false_positive() == true)){
		// Otherwise if waiting for test or results - home isolation
		lambda_tot =  house.get_infected_contribution();
	}
	return lambda_tot;
}

// Compte and set agent properties related to recovery without symptoms and incubation 
void FluTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				const std::map<std::string, double>& infection_parameters)
{
	// Determine if agent will recover without
	// becoming symptomatic and update corresponding states
	bool never_sy = infection.recovering_exposed();

	// Total latency period
	double latency = infection.latency();
	// Portion of latency when the agent is not infectious
	double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);

	if (never_sy){
		states_manager.set_susceptible_to_exposed_never_symptomatic(agent);
		// Set to total latency + infectiousness duration
		double rec_time = infection_parameters.at("recovery time");
		agent.set_latency_duration(latency + rec_time);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}else{
		// If latency shorter, then  not infectious during the entire latency
		states_manager.set_susceptible_to_exposed(agent);
		agent.set_latency_duration(latency);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}
}

// Determine any testing related properties
void FluTransitions::set_testing_status(Agent& agent, Infection& infection, const double time, 
										std::vector<School>& schools, std::vector<Workplace>& workplaces, 
										std::vector<Hospital>& hospitals, 
										const std::map<std::string, double>& infection_parameters)
{
	const int n_hospitals = hospitals.size();
	bool will_be_tested = false;

	// Different probability and settings for exposed
	if (agent.exposed() == true){
 		will_be_tested = infection.will_be_tested(infection_parameters.at("exposed fraction to get tested"));
		if (will_be_tested == true){
			// Determine type of testing
			if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
				states_manager.set_exposed_waiting_for_test_in_hospital(agent);
				int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
				// Registration will happen only upon testing time step
				agent.set_hospital_ID(hsp_ID);
			} else {
				states_manager.set_exposed_waiting_for_test_in_car(agent);
			}
			// Home isolation - removal from all public places 
			remove_from_all_workplaces_and_schools(agent, schools, workplaces);
			// Time to test
			agent.set_time_to_test(infection_parameters.at("time from decision to test"));
			agent.set_time_of_test(time);
		}
	} 
}

// Non-covid symptomatic testing changes
void FluTransitions::testing_transitions_flu(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters)
{
	// Determine the time agent gets results
	agent.set_time_until_results(infection_parameters.at("time from test to results"));
	agent.set_time_of_results(time);
	states_manager.set_tested_to_awaiting_results(agent);
}

// Non-covid symptomatic agent transitions upon receiving test results 
void FluTransitions::testing_results_transitions_flu(Agent& agent, 
			const double time, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	// If false positive, put under home isolation 
	double fneg_prob = infection_parameters.at("fraction false positive");
	if (infection.false_positive_test_result(fneg_prob) == true){
		states_manager.set_tested_false_positive(agent);
		agent.set_recovery_duration(infection_parameters.at("recovery time"));
		agent.set_recovery_time(time);	
	} else { 		
		// If confirmed negative, release the isolation	
		states_manager.set_tested_negative(agent);
		add_to_all_workplaces_and_schools(agent, schools, workplaces);
	}
}

// Remove agent's index from all workplaces and schools that have them registered
void FluTransitions::remove_from_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	// Else remove depending on status	
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
}

// Add to all from places where they are registered
void FluTransitions::add_to_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
}


