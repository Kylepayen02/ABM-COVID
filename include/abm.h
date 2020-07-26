#ifndef ABM_H
#define ABM_H

#include "abm_include.h"

/***************************************************** 
 * class: ABM
 * 
 * Interface for agent-based modeling 
 *
 * Provides operations for creation, management, and
 * progression of an agent-based model
 *
 * Stores model-related data
 *
 * NOTE: IDs of objects correspond to their positions
 * in the vectors of objects and determine the way
 * they are accessed; IDs start with 1 but are corrected
 * by -1 when accessing;   
 * 
 ******************************************************/

class ABM{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates an ABM object with default attributes
	 */
	ABM() = default;

	/**
	 * \brief Creates an ABM object with custom attributes
	 * \details Reads the infection parameters from the provided 
	 * 				file and initializes Infection object;
	 * 				The map key represents a tag to recognize which dataset
	 * 				in question is it. The only tag is mortality
	 *
	 * @param del_t - time step, days
	 * @param infile - name of the file with the input parameters
	 * @param dist_files - map of keys-tags and file names where different distribution files are stored 
	 *
	 */
	ABM(double del_t, const std::string infile, const std::map<std::string, std::string> dist_files) : 
					dt(del_t), infection(del_t) 
		{
			time = 0.0;	
			load_infection_parameters(infile); 
			load_age_dependent_distributions(dist_files);
		}

	//
	// Initialization and object construction
	//

	/**
	 * \brief Create households based on information in a file
	 * \details Constructs households based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one household 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_households(const std::string filename);

	/**
	 * \brief Create schools based on information in a file
	 * \details Constructs schools based on the ID,
	 * 				locations, and school type as defined in the file; 
	 * 				One line in the file defines one school;
	 * 				School types are "daycare", "primary", "middle",
	 * 				"high", and "college"  
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_schools(const std::string filename);

	/**
	 * \brief Create workplaces based on information in a file
	 * \details Constructs workplaces based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one workplace 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_workplaces(const std::string filename);

	/**
	 * \brief Create agents based on information in a file
	 * \details Constructs agents from demographic information
	 * 		in a file with agent per row, columns being the
	 *		information as it currently appears in the Agent
	 *		constructor; assigns agents to households, schools, and
	 *		workplaces - needs to be called AFTER creating
	 *		those places;
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_agents(const std::string filename);

	//
	// Transmission of infection
	//

	/**
	 * \brief Transmit infection according to Infection model
	 */
	void transmit_infection();

	/// \brief Count contributions of all infectious agents in each place 
	void compute_place_contributions();

	/// \brief Propagate infection and determine state transitions
	void compute_state_transitions();

	//
	// Getters
	//
	
	/// Retrieve number of infected agents at this time step
	int get_num_infected() const;

	/// Retrieve number of exposed
	int get_num_exposed() const;

	/// Retrieve number of total infected
	int get_total_infected() const { return n_infected_tot; }
	/// Retrieve number of total dead 
	int get_total_dead() const { return n_dead_tot; }
	/// Retrieve number of total recovered
	int get_total_recovered() const { return n_recovered_tot; }

	//
	// Saving simulation state
	//

	/**
	 * \brief Save infection parameter information
	 *
     * @param filename - path of the file to print to
	 */
	void print_infection_parameters(const std::string filename) const;	

	/**
	 * \brief Save age-dependent distributions 
	 *
     * @param filename - path of the file to print to
	 */
	void print_age_dependent_distributions(const std::string filename) const;

	/**
	 * \brief Save current household information 
	 * \details Outputs household information as 
	 * 		household ID, x and y location, total number
	 * 		of agents, number of infected agents
	 * 		One line per household		
	 * @param filename - path of the file to print to
	*/
	void print_households(const std::string filename) const;	

	/**
	 * \brief Save current school information 
	 * \details Outputs school information as 
	 * 		school ID, x and y location, total number
	 * 		of agents, number of infected agents
	 * 		One line per school		
	 * @param filename - path of the file to print to
	*/
	void print_schools(const std::string filename) const;	

	/**
	 * \brief Save current workplace information 
	 * \details Outputs workplace information as 
	 * 		workplace ID, x and y location, total number
	 * 		of agents, number of infected agents, ck, beta, psi
	 * 		One line per workplace		
	 * @param filename - path of the file to print to
	*/
	void print_workplaces(const std::string filename) const;	


	/**
	 * \brief Save IDs of all agents in all households
	 * \details One line per household, 0 if no agents present
	 * @param filename - path of the file to save to
	*/
	void print_agents_in_households(const std::string filename) const;
	
	/**
	 * \brief Save IDs of all agents in all schools
	 * \details One line per school, 0 if no agents present
	 * @param filename - path of the file to save to
	*/
	void print_agents_in_schools(const std::string filename) const;
	
	/**
	 * \brief Save IDs of all agents in all workplaces
	 * \details One line per workplace, 0 if no agents present
	 * @param filename - path of the file to save to
	*/
	void print_agents_in_workplaces(const std::string filename) const;


	/**
	 * \brief Save current agent information 
	 * \details Outputs agent information as 
	 *		indicated in Agent constructor 
	 * 		One line per agent		
	 * @param filename - path of the file to print to
	*/
	void print_agents(const std::string filename) const;

    /**
     *  \brief Collect all interactions for each agent
     */
    void collect_all_interactions();

    /**
     *  \brief Collect all dead interactions for each agent
     */
     void collect_dead_interactions();

    /**
     * \brief Output number of agent interactions for each agent into a txt file
     * @param fil
     */

    void output_interactions(std::string filename);

    /**
    * \brief Output number of dead agent interactions for each agent into a txt file
    */

    void output_dead_interactions(std::string filename);
	
	//
	// Functions mainly for testing
	//
	
	/// Return a const reference to a House object vector
	const std::vector<Household>& get_vector_of_households() const { return households; }
	/// Return a const reference to a School object vector
	const std::vector<School>& get_vector_of_schools() const { return schools; }
	/// Return a const reference to a Workplace object vector
	const std::vector<Workplace>& get_vector_of_workplaces() const { return workplaces; }
	/// Return a const reference to an Agent object vector
	const std::vector<Agent>& get_vector_of_agents() const { return agents; }
	/// Return a reference to an Agent object vector
	std::vector<Agent>& get_vector_of_agents_non_const()  { return agents; }
	/// Return a copy of a House object vector
	std::vector<Household> get_copied_vector_of_households() const { return households; }
	/// Return a copy of a School object vector
	std::vector<School> get_copied_vector_of_schools() const { return schools; }
	/// Return a copy of a Workplace object vector
	std::vector<Workplace> get_copied_vector_of_workplaces() const { return workplaces; }
	/// Return a copy of Infection object
	Infection get_copied_infection_object() const { return infection; }
	/// Return a const reference to parameter map
	const std::map<std::string, double> get_infection_parameters() const
		{ return infection_parameters; }
private:

	// General model attributes
	// Time step
	double dt = 1.0;
	// Time - updated continuously throughout the simulation
	double time = 0.0;

	// Total number of infected, dead and recovered
	int n_infected_tot = 0.0;
	int n_dead_tot = 0.0;
	int n_recovered_tot = 0.0;

	// Infection parameters
	std::map<std::string, double> infection_parameters = {};

	// Age-dependent distributions
	std::map<std::string, std::map<std::string, double>> age_dependent_distributions = {};

	// Infection properties and transmission model
	Infection infection;
	// Class for computing infection contributions
	Contributions contributions;
	// Class for computing agent transitions
	Transitions transitions;
	// Class for setting agent state transitions
	StatesManager states_manager;

	// Vectors of individual model objects
	std::vector<Agent> agents;
	std::vector<Household> households;
	std::vector<School> schools;
	std::vector<Workplace> workplaces;


	// Private methods

	/// Load infection parameters, store in a map
	void load_infection_parameters(const std::string);

	/// Load age-dependent distributions as vectors stored in a map
	void load_age_dependent_distributions(const std::map<std::string, std::string>);

	/**
	 * \brief Read object information from a file	
	 * @param filename - path of the file to print to
	 */
	std::vector<std::vector<std::string>> read_object(std::string filename);

	/// \brief Set properties of initially infected - exposed
	void initial_exposed(Agent&);

//Might use, might not
    void initial_exposed_with_never_sy(Agent&);

	// Increasing time
	void advance_in_time() { time += dt; }

	/**
	 * \brief Print basic places information to a file
	 */
	template <typename T>
	void print_places(std::vector<T> places, const std::string fname) const;

	/// \brief Print all agent IDs in a particular type of place to a file
	template <typename T>
	void print_agents_in_places(std::vector<T> places, const std::string fname) const;

	/**
	 * \brief Retrieve information about agents from a file and store all in a vector
	 */
	void load_agents(const std::string fname);

	/**
	 * \brief Assign agents to households, schools, and workplaces
	 */
	void register_agents();
};

// Write Place objects
template <typename T>
void ABM::print_places(std::vector<T> places, const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// Write data to file
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<T>(places);
}

// Write agent IDs in Place objects
template <typename T>
void ABM::print_agents_in_places(std::vector<T> places, const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// First collect the data into a nested vector
	std::vector<std::vector<int>> agents_all_places;
	for (const auto& place : places){
		std::vector<int> agent_IDs = place.get_agent_IDs();
		// If no agents, store a 0
		if (agent_IDs.empty())
			agents_all_places.push_back({0});
		else
			agents_all_places.push_back(agent_IDs);
	}

	// Then write data to file, one line per place
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<int>(agents_all_places);
}



#endif
