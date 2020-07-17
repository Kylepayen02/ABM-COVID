#ifndef STATESMANAGER_H
#define STATESMANAGER_H

//
// Other
//

#include "../common.h"
#include "../io_operations/abm_io.h"
#include "../agent.h"

/***************************************************** 
 * class: StatesManager 
 *
 * Functionality for modifying agents states  
 * 
 ******************************************************/

class StatesManager{
public:

	//
	// Constructors
	//

	StatesManager() = default;

	/// Set all states for transition from susceptible to exposed
	void set_susceptible_to_exposed(Agent& agent);
	/// Set all states for transition from susceptible to exposed that will never become symptomatic
	void set_susceptible_to_exposed_never_symptomatic(Agent& agent);
	/// Set exposed that never developed symptoms to removed
	void set_exposed_never_symptomatic_to_removed(Agent& agent);

	/// Set all states for transition from exposed to general symptomatic
	void set_exposed_to_symptomatic(Agent& agent);
	/// Set all states relevant to agent that will die 
	void set_dying_symptomatic(Agent& agent);
	/// Set all states relevant to agent that will recover 
	void set_recovering_symptomatic(Agent& agent);
	


	/// Set all removed related states
	void set_any_to_removed(Agent& agent);



private:

};

#endif
