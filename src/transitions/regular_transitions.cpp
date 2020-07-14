#include "../../include/transitions/regular_transitions.h"

/***************************************************** 
 * class: RegularTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for a most general
 * case of agent 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int RegularTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				const std::map<std::string, double>& infection_parameters, std::vector<Agent>& agents,
				Flu& flu)
{
	double lambda_tot = 0.0;
	int got_infected = 0;

	lambda_tot = compute_susceptible_lambda(agent, time, households, schools, workplaces);
	if (infection.infected(lambda_tot) == true){
		// Remove agent from potential flu population
		flu.remove_susceptible_agent(agent.get_ID());
		got_infected = 1;
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing 
		// symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
		// Determine if getting tested, how, and when
		// Remove agent from places if under home isolation
		set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, infection_parameters);
	}	
	return got_infected;	
}

// Return total lambda of susceptible agent 
double RegularTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces)			
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

	return lambda_tot;
}

// Compte and set agent properties related to recovery without symptoms and incubation 
void RegularTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
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

// Implement transitions relevant to exposed 
int RegularTransitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
										const std::map<std::string, double>& infection_parameters)
{
	// First check for testing because that holds for transition changes too
	// If being tested
	if ((agent.tested()) && (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		testing_transitions(agent, time, infection_parameters);
	}
	// If getting test results (this may in principle happen in a
	// single step)
	if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
		testing_results_transitions(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, infection_parameters);
	}

	// Check if latency time is over
	int agent_recovered = 0;
	if (agent.get_latency_end_time() <= time){
		// Conditions for retesting if becoming symptomatic
		bool retest = (agent.tested_exposed() && agent.tested_false_negative())
					|| (!agent.tested_exposed());
		// Reovering without symptoms - remove
		if (agent.recovering_exposed()){
			if (agent.home_isolated())
				add_to_all_workplaces_and_schools(agent, schools, workplaces);
			states_manager.set_exposed_never_symptomatic_to_removed(agent);
			agent_recovered = 1;
		} else {
			// Transition to symptomatic
			if (agent.home_isolated())
				add_to_all_workplaces_and_schools(agent, schools, workplaces);
			states_manager.set_exposed_to_symptomatic(agent);

			// Removal settings		
			int agent_age = agent.get_age();
			if (infection.will_die_non_icu(agent_age)){
				states_manager.set_dying_symptomatic(agent);			
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			} else {
				states_manager.set_recovering_symptomatic(agent);			
				// This may change if treatment is ICU
				agent.set_recovery_duration(infection_parameters.at("recovery time"));
				agent.set_recovery_time(time);		
			}

			// Determine if getting tested, how, and when
			// Remove agent from places if under home isolation
			if (retest == true)
				set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, infection_parameters);
			// If agent tested positive - select initial treatment for symptomatic
			// In that stage they are already IH
			if (agent.tested_covid_positive())
				select_initial_treatment(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, infection_parameters);
		}
	}
	return agent_recovered;
}

// Determine any testing related properties
void RegularTransitions::set_testing_status(Agent& agent, Infection& infection, const double time, 
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
	} else if (agent.symptomatic()) {
 		will_be_tested = infection.will_be_tested(infection_parameters.at("fraction to get tested"));
		if (will_be_tested == true){
			// If agent is getting tested - determine type and properties of testing
			if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
				states_manager.set_waiting_for_test_in_hospital(agent);
				int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
				// Registration will happen only upon testing time step
				agent.set_hospital_ID(hsp_ID);
			} else {
				states_manager.set_waiting_for_test_in_car(agent);
			}
	
			// Testing-related events - will be adjusted based on other time-dependent scenarios
			agent.set_time_to_test(infection_parameters.at("time from decision to test"));
			agent.set_time_of_test(time);
	
			// Home isolation - removal from all public places except hospitals for former
			// patients and employees (removed previously because that holds for all of them)
			remove_from_all_workplaces_and_schools(agent, schools, workplaces);
		}
	} 
}

// Transitions of a symptomatic agent 
std::vector<int> RegularTransitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					const std::map<std::string, double>& infection_parameters)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	removed = check_agent_removal(agent, time, households, schools, workplaces, hospitals);
	if (agent.removed() == true)
		return removed;
	if (agent.tested_false_negative() == true)
		return removed;

	// If being tested at this time step
	if ((agent.tested()) && (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		testing_transitions(agent, time, infection_parameters);
		return removed;
	}

	// If getting test results 	
	if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
		testing_results_transitions(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, infection_parameters);
		return removed;
	}

	// Treatment transitions (also possible in a single step)
	if (agent.being_treated())
		treatment_transitions(agent, time, dt, infection, households, 
							hospitals, infection_parameters);
	return removed;
}

// Verify if agent is to be removed at this step
std::vector<int> RegularTransitions::check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	// If dying
	if (agent.dying() == true){
		if (agent.get_time_of_death() <= time){
			removed.at(1) = 1;
			remove_agent_from_all_places(agent, households, schools, workplaces, hospitals);
			states_manager.set_any_to_removed(agent);
		}
	}
	// If recovering
	if (agent.recovering() == true){
		if (agent.get_recovery_time() <= time){
			removed.at(0) = 1;
			// If any of these categories - return to regular public places/households
			if (agent.tested() || agent.being_treated()){
				add_agent_to_all_places(agent, households, schools, workplaces, hospitals);
			}
			states_manager.set_any_to_removed(agent);
		}
	}
	return removed;
}

// Agent transitions related to testing time
void RegularTransitions::testing_transitions(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters)
{
	// Determine the time agent gets results
	agent.set_time_until_results(infection_parameters.at("time from test to results"));
	agent.set_time_of_results(time);
	states_manager.set_tested_to_awaiting_results(agent);
}

// Agent transitions upon receiving test results 
void RegularTransitions::testing_results_transitions(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	// If false negative, remove testing, put back to exposed
	// No false negative symptomatic 
	double fneg_prob = infection_parameters.at("fraction false negative");
	if (infection.false_negative_test_result(fneg_prob) == true
		 && agent.exposed() == true){
		states_manager.set_tested_false_negative(agent);
		add_to_all_workplaces_and_schools(agent, schools, workplaces);
	} else {
		// If confirmed positive
		// Exposed - keep in home isolation until symptomatic
		if (agent.exposed()){
			states_manager.set_home_isolation(agent);
			agent.set_tested_covid_positive(true);		
		} else {
			// Symptomatic - identify treatment
			select_initial_treatment(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, infection_parameters);
			agent.set_tested_covid_positive(true);
		}
	}			
}

// Determine type of intial treatement and its properties
void RegularTransitions::select_initial_treatment(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	if (infection.agent_hospitalized(agent.get_age()) == true){
		
		// Remove agent from all places, then add to a random
		// hospital for treatment
		remove_agent_from_all_places(agent, households, schools, workplaces, hospitals);
		
		// Set hospital ID and add
		int hID = infection.get_random_hospital_ID(hospitals.size());
		agent.set_hospital_ID(hID);
		hospitals.at(hID-1).add_agent(agent.get_ID());

		// ICU
		if (infection.agent_hospitalized_ICU(agent.get_age()) == true){
			// Retest for dying
			if (infection.will_die_ICU()){
				states_manager.set_icu_dying(agent);
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			}else{
				// If recovering - set times and transitions
				states_manager.set_icu_recovering(agent);
				// Reset the recovery time to > ICU + hospitalization
				double t_icu = infection_parameters.at("time in ICU");
				double t_hsp_icu = infection_parameters.at("time in hospital after ICU");
				agent.set_time_icu_to_hsp(time + t_icu);
			   	agent.set_time_hsp_to_ih(time + t_icu + t_hsp_icu);	
				agent.set_recovery_duration(t_icu + t_hsp_icu);
				agent.set_recovery_time(time);
			}
		}else{
			// Hospitalized
			states_manager.set_hospitalized(agent);
			// If dying, set transition to ICU
			if (agent.dying() == true){
				double dt_icu = infection_parameters.at("time before death to ICU");
				double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
				agent.set_time_hsp_to_icu(t_icu);	
			}else{
				// If recovering, set transition to home
				double t_rh = agent.get_recovery_time();
				double del_t_hsp = infection_parameters.at("time in hospital");
				double t_hsp = time + del_t_hsp; 
				if (t_rh > t_hsp){
					agent.set_time_hsp_to_ih(t_hsp);
				}else{
					agent.set_time_hsp_to_ih(t_hsp);
					agent.set_recovery_duration(del_t_hsp);
					agent.set_recovery_time(time);
				}
			}
		}
	}else{
		// If home isolated
		states_manager.set_home_isolation(agent);
		// If dying, set transition to ICU
		if (agent.dying() == true){
			double dt_icu = infection_parameters.at("time before death to ICU");
			double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
			agent.set_time_ih_to_icu(t_icu);	
		}else{
			// If recovering, determine possible transition to hospital
			double t_rh = agent.get_recovery_time();
			double t_hsp = time + infection.get_onset_to_hospitalization();
			// Only if transition is later than a step away and earlier than
			// the recovery time
			if ((t_rh > t_hsp) && (t_hsp > time + dt)){
				agent.set_time_ih_to_hsp(t_hsp);
			}else{
				// Set to past recovery
				agent.set_time_ih_to_hsp(2.0*t_rh);
			}
		}
	}
}

// Determine treatment changes 
void RegularTransitions::treatment_transitions(Agent& agent, const double time, 
			const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	// ICU - can only transition to hospitalization
	// if not dying
	if (agent.recovering() && agent.hospitalized_ICU()){
		if (agent.get_time_icu_to_hsp() <= time){
			agent.set_hospitalized_ICU(false);
			agent.set_hospitalized(true);
		}
	}else if (agent.hospitalized()){
		if (agent.dying()){
			if (agent.get_time_hsp_to_icu() <= time){
				// Transition to ICU
				agent.set_hospitalized(false);
				agent.set_hospitalized_ICU(true);
			}	
		}else{		
			if (agent.get_time_hsp_to_ih() <= time){
				// Transition to home isolation
				agent.set_hospitalized(false);
				agent.set_home_isolated(true);
				// Remove from hospital and add to household
				int agent_ID = agent.get_ID();
		        households.at(agent.get_household_ID()-1).add_agent(agent_ID);
		        // Remove agent from hospital
		        hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
			}
		}
	}else if (agent.home_isolated()){
		if (agent.dying()){
			// Will end up in ICU
			if (agent.get_time_ih_to_icu() <= time){
				// Set hospital ID, add to hospital 
				int hID = infection.get_random_hospital_ID(hospitals.size());
				agent.set_hospital_ID(hID);
				hospitals.at(hID-1).add_agent(agent.get_ID());
				// Remove from home
				households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				agent.set_home_isolated(false);
				agent.set_hospitalized(false);
				agent.set_hospitalized_ICU(true);	
			}
		} else {
			double t_hsp = agent.get_time_ih_to_hsp();
			// Comparing to dt too, it is currently minimum 
			// for hospitalization; for now assuming
			// no ICU for recovering - hospitalized
			if ( (t_hsp >= dt) && (t_hsp <= time) ){
				agent.set_home_isolated(false);
				agent.set_hospitalized(true);
				// Set hospital ID 
				int hID = infection.get_random_hospital_ID(hospitals.size());
				agent.set_hospital_ID(hID);
				hospitals.at(hID-1).add_agent(agent.get_ID());
				// Remove from home
				households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				// Set transition back
				double t_rh = agent.get_recovery_time();
				double del_t_hsp = infection_parameters.at("time in hospital");
				double t_hsp = time + del_t_hsp; 
				if (t_rh > t_hsp){
					agent.set_time_hsp_to_ih(t_hsp);
				}else{
					agent.set_time_hsp_to_ih(t_hsp);
					agent.set_recovery_duration(del_t_hsp);
					agent.set_recovery_time(time);
				}
			}
		}		
	}	
}

// Remove agent's ID from places where they are registered
void RegularTransitions::remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals)
{
	// No error if already removed due to hospitalization
	int agent_ID = agent.get_ID();

	int hs_ID = agent.get_household_ID();
	if (hs_ID > 0){
		households.at(hs_ID-1).remove_agent(agent_ID);
	} else {
		throw std::runtime_error("Regular symptomatic agent does not have a valid household ID");
	}
	
	if (agent.hospitalized() || agent.hospitalized_ICU())
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);

	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
}

// Add to all from places where they are registered
void RegularTransitions::add_to_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
}

// Remove agent's index from all workplaces and schools that have them registered
void RegularTransitions::remove_from_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
}

// Add agent's ID back to the places where they are registered
// This is done to keep realistic numbers of people in different places
// which influences the probability
void RegularTransitions::add_agent_to_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals)
{
	int agent_ID = agent.get_ID();

	// ICU should always transition to hospital first
	if (agent.hospitalized_ICU())
		throw std::runtime_error("Attempting recovery of an agent directly from ICU");
	
	// Remove from all not to count twice
	if (agent.hospitalized())
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
		
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
	// Hospitalized was the only possibility where infected agent was not
	// associated with a household
	if (agent.hospitalized())
		households.at(agent.get_household_ID()-1).add_agent(agent_ID);
}


