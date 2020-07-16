#include "../../include/states_manager/regular_states_manager.h"

/***************************************************** 
 * class: RegularStatesManager 
 *
 * Functionality for modifying agents states
 * for most general agent case  
 * 
 ******************************************************/

// Set all states for transition from susceptible to exposed
void RegularStatesManager::set_susceptible_to_exposed(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);
	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(false);
	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set all states for transition from susceptible to exposed that will never become symptomatic
void RegularStatesManager::set_susceptible_to_exposed_never_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);
	agent.set_recovering_exposed(true);	
	agent.set_symptomatic(false);
	
	agent.set_dying(false);
	agent.set_recovering(true);	
	agent.set_removed(false);
}

// Set exposed that never developed symptoms to removed
void RegularStatesManager::set_exposed_never_symptomatic_to_removed(Agent& agent)
{
	set_any_to_removed(agent);
}

// Set all states for transition from exposed to general symptomatic
void RegularStatesManager::set_exposed_to_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	

	agent.set_symptomatic(true);
	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set all states relevant to agent that will die 
void RegularStatesManager::set_dying_symptomatic(Agent& agent)
{
	agent.set_dying(true);
	agent.set_recovering(false);	
}

// Set all states relevant to agent that will recover 
void RegularStatesManager::set_recovering_symptomatic(Agent& agent)
{
	agent.set_dying(false);
	agent.set_recovering(true);	
}

// Set all removed related states
void RegularStatesManager::set_any_to_removed(Agent& agent)
{
	agent.set_removed(true);

	agent.set_dying(false);
	agent.set_recovering(false);	

	agent.set_infected(false);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(false);

}
