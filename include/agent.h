#ifndef AGENT_H
#define AGENT_H

#include "common.h"
#include "infection.h"

class Infection;

/***************************************************** 
 * class: Agent
 * 
 * Defines and stores attributes of a single agent
 * 
 *****************************************************/

class Agent{
public:
	
	//
	// Constructors
	//

	/**
	 * \brief Creates an Agent object with default attributes
	 */
	Agent() = default;

	// Better keep flags and IDs clustered i.e. have worksSch 
	// right after works - I think I didn't do it this way
	// now either though ;)

	/**
 	 * \brief Creates an Agent object with custom attributes
 	 *
 	 * @param student - true (marked as 1) if Agent is a student
 	 * @param works - true (marked as 1) if Agent works
 	 * @param yrs - age of the Agent
 	 * @param xi - x coordinate of the Agent
 	 * @param yi - y coordinate of the Agent
 	 * @param houseID - household ID
 	 * @param schoolID - ID of the school Agent attends
 	 * @param wrkSch - true if agent works at a school
 	 * @param workID - ID of the workplace Agent works at
 	 * @param infected - true if Agent is infected
 	 */	
	Agent(const bool student, const bool works, const int yrs, const double xi, 
			const double yi, const int houseID, const int schoolID, const bool wrkSch,
			const int workID, const bool infected)
			: is_student(student), is_working(works), age(yrs),
		   		x(xi), y(yi), house_ID(houseID), school_ID(schoolID), worksSch(wrkSch),
				work_ID(workID), is_infected(infected) { }

	//
	// Infection related computations
	//

	//
	// Getters
	//

	/// Retrieve this agents ID
	int get_ID() const { return ID; }
	/// Agents age
	int get_age() const { return age; }
	/// House ID
	int get_household_ID() const { return house_ID; }
	/// School ID
	int get_school_ID() const { return school_ID; }
	/// Work ID
	int get_work_ID() const { return work_ID; }
	/// Location - x coordinates
	double get_x_location() const { return x; }	
	/// Location - y coordinates
	double get_y_location() const { return y; }

	/// True if infected
	bool infected() const { return is_infected; }
	/// True if student
	bool student() const { return is_student; }
	/// True if agent works
	bool works() const { return is_working; }
	///True if agent works at school
	bool school_employee() const { return worksSch; }

	/// State getters
	bool exposed() const { return is_exposed; }
	bool recovering_exposed() const { return is_recovering_exposed; }
	bool symptomatic() const { return is_symptomatic; }

	// Removal
	bool dying() const { return will_die; }
	bool recovering() const { return will_recover; }
	bool removed() const { return is_removed; }

	// I would call these dead() and recovered()
	// and the variables is_dead and is_recovered
	// I would keep the "set_" for setting though
	bool get_dead() const { return dead; }
	bool get_recovered() const { return recovered; }

	/// Get infectiousness variability factor of an agent
	double get_inf_variability_factor() const { return inf_var; }
	/// Get latency end time
	double get_latency_end_time() const { return latency_end_time; }
	/// Time when the latent-non infectious period ends
	double get_infectiousness_start_time() const { return infectiousness_start; }
	/// Get time of death if not recovering
	double get_time_of_death() const { return death_time; }
	/// Get time of recovery
	double get_recovery_time() const { return recovery_time; }
	/// Get vector of agent interactions at each timestep
	std::vector<int>& get_all_interactions() { return interactions; }
	/// Get vector of dead agent interactions at each timestep
	std::vector<int>& get_dead_interactions() { return dead_interactions; }

	//
	// Setters
	//

	/// Assign ID to an agent
	void set_ID(const int agent_ID) { ID = agent_ID; }

	/// Assign household ID
	void set_household_ID(const int ID) { house_ID = ID; }

	/// Change infection status
	void set_infected(const bool infected) { is_infected = infected; }

	// Latency
	/// Set latency duration time
	void set_latency_duration(const double ltime) { latency_duration = ltime; }
	/// Compute latency end from current time
	void set_latency_end_time(const double cur_time) 
		{ latency_end_time = cur_time + latency_duration; }
	/// Set tme when the pre-infectious period ends
	void set_infectiousness_start_time(const double cur_time, const double dt) 
		{ infectiousness_start = cur_time + dt; }

	// Death 
	/// Set onset to death duration time
	void set_time_to_death(const double dtime) { otd_duration = dtime; }
	/// Compute death time from current time
	void set_death_time(const double cur_time) 
		{ death_time = cur_time + otd_duration; }

	// Recovery
	/// Set recovery duration time
	void set_recovery_duration(const double rtime) { recovery_duration = rtime; }
	/// Compute recovery end from current time
	void set_recovery_time(const double cur_time) 
		{ recovery_time = cur_time + recovery_duration; }

	/// State setters
	void set_exposed(const bool val) { is_exposed = val; }
	void set_recovering_exposed(const bool re) { is_recovering_exposed = re; }
	void set_symptomatic(const bool val) { is_symptomatic = val; }

	void set_dying(const bool val) { will_die = val; }
	void set_recovering(const bool val) { will_recover = val; }
	void set_removed(const bool val) { is_removed = val; }

	void set_dead(const bool val) { dead = val; }
	void set_recovered(const bool val) { recovered = val; }

	/// Set infectiousness variability factor of an agent
	void set_inf_variability_factor(const double var) { inf_var = var; }


	//
	// I/O
	//

	/**
	 * \brief Print agent information 
	 * \details The information is in the same order as in the constructor,
	 *		except that it is preceeded by agent ID set separately
	 * 	@param where - output stream
	 */	
	void print_basic(std::ostream& where) const;

    /**
     * \brief Retrieve number of agent interactions at timestep
     */
    std::vector<int> collect_interactions(const std::vector<Agent>& agents);

private:

	// General demographic information
	bool is_student = false;
	bool is_working = false;
	int age = 0;

	// Latency duration in time
	double latency_duration = 0.0; 
	// Start of infectiousness
	double infectiousness_start = 0.0;
	// End of latency period within the simulation time
	double latency_end_time = 0.0;
	// Time from becoming symptomatic (onset) to death
	double otd_duration = 0.0;
	// Time of death
	double death_time = 0.0;
	// Time to recover
	double recovery_duration = 0.0;
	// Time of recovery
	double recovery_time = 0.0;

	// ID
	int ID = 0;

	// Location
	double x = 0.0, y = 0.0;

	// Household ID
	int house_ID = -1;

	// School and work IDs and types
	int school_ID = -1;
	int work_ID = -1;
	int agent_school_type = -1;
	bool worksSch = false;

	// Infection status
	bool is_infected = false;

	// Ratio of distances with infected and all distances
	double dist_ratio = 0.0;

	// State information
	bool is_exposed = false;
	// Recovering without ever developing symptoms
	bool is_recovering_exposed = false;
	bool is_symptomatic = false;

	// Testing phases and types
	bool will_die = false;
	bool will_recover = false;
	bool is_removed = false;

	bool dead = false;
	bool recovered = false;

	// Infectiousness variability parameter
	double inf_var = 1.0;

	// Number of interactions at each timestep
	std::vector<int> interactions;
	// Number of dead agent interactions
	std::vector<int> dead_interactions;


	//
	// Private member functions
	//
	
	/** 
	 * \brief Function of distance for infection propagation
	 * @param a - Infection parameter for distance scaling, units of distance
	 * @param b - Infection parameter for distance scaling, unitless
     * @param dij - distance between agents i and j 
	 *
 	 * @returns Value of the distance function
	 */	
	double distance_function(const double a, const double b, const double dij) const;


};

/// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Agent& agent);

#endif
