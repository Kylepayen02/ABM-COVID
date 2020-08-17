#include "agent_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Agent class
 *
 *****************************************************/

// Tests
bool agent_constructor_getters_test();
bool agent_events_test();
bool agent_out_test();

int main()
{
	test_pass(agent_constructor_getters_test(), "Agent class constructor and getters");
	test_pass(agent_events_test(), "Agent class event scheduling and handling functionality");
	test_pass(agent_out_test(), "Agent class ostream operator");
}

/// Tests Agent class constructor and most of existing getters 
// This checks only one agent type, more robust type checking 
// i.e. health state, works/studies combinations are in the
// ABM test suite  
bool agent_constructor_getters_test()
{
	bool student = true, works = false, infected = true, worksSch = true;
	int age = 25, hID = 3, sID = 305, wID = 0;
	double xi = 7.009, yi = 100.5;
	int aID = 1;
	double inf_var = 0.2009;
	double cur_time = 4.0;

	Agent agent(student, works, age, xi, yi, hID, sID, worksSch, wID, infected);
	agent.set_ID(aID);
	agent.set_inf_variability_factor(inf_var);
	
	// IDs
	if (aID != agent.get_ID() || hID != agent.get_household_ID() 
			|| sID != agent.get_school_ID() || wID != agent.get_work_ID())
		return false;

	// Age
	if (age != agent.get_age())
		return false;

	// Location
	if (!float_equality<double>(xi, agent.get_x_location(), 1e-5)
			|| !float_equality<double>(yi, agent.get_y_location(), 1e-5))
		return false;

	// State
	if (student != agent.student() || works != agent.works() 
			|| worksSch != agent.school_employee() || infected != agent.infected())
		return false;	

	// Infectiousness variability
	if (!float_equality<double>(inf_var, agent.get_inf_variability_factor(), 1e-5))
		return false;

	return true;
}

/// Check the correctness of the time-depending event handling
bool agent_events_test()
{
	bool student = true, works = false, infected = true, worksSch = true;
	int age = 25, hID = 3, sID = 305, wID = 0;
	double xi = 7.009, yi = 100.5;
	int aID = 1;
	double inf_var = 0.2009;
	double cur_time = 4.0;
	double latency = 3.5, lat_end_time = cur_time + latency;
	double otd = 1.2, time_of_death = cur_time + otd;
	double recovery = 10.0, time_of_recovery = cur_time + recovery; 
	Agent agent(student, works, age, xi, yi, hID, sID, worksSch, wID, infected);
	agent.set_ID(aID);
	agent.set_inf_variability_factor(inf_var);
	
	agent.set_latency_duration(latency);
	agent.set_latency_end_time(cur_time);

	agent.set_time_to_death(otd);
	agent.set_death_time(cur_time);

	agent.set_recovery_duration(recovery);
	agent.set_recovery_time(cur_time);


	// Latency
	if (!float_equality<double>(lat_end_time, agent.get_latency_end_time(), 1e-5))
		return false;

	// Death
	if (!float_equality<double>(time_of_death, agent.get_time_of_death(), 1e-5))
		return false;
	
	// Recovery
	if (!float_equality<double>(time_of_recovery, agent.get_recovery_time(), 1e-5))
		return false;

	return true;
}

/// Tests Agent ostream operator overload/print capabilities
bool agent_out_test()
{
	bool student = false, works = true, infected = false, worksSch = false;
	int school_type = 3;
	int age = 30, hID = 50, sID = 0, wID = 10001;
	double xi = 108.009, yi = 1030.15;
	int aID = 11;

	Agent agent(student, works, age, xi, yi, hID, sID, worksSch, wID, infected);
	agent.set_ID(aID);

	// Get directly from the stream and compare
	std::stringstream agent_buff;
	agent_buff << agent;
	std::istringstream res(agent_buff.str());

	bool test_student = false, test_works = false, test_infected = false;
	bool works_at_school = false;
	int test_age = 0, test_hID = 0, test_sID = 0, test_wID = 0;
	double test_xi = 0.0, test_yi = 0.0;
	int test_aID = 0;

	res >> test_aID >> test_student >> test_works >> works_at_school >> test_age
		>> test_xi >> test_yi >> test_hID >> test_sID >> test_wID
		>> test_infected;

	// IDs
	if (aID != test_aID || hID != test_hID || sID != test_sID || wID != test_wID)
		return false;
	
	// Location
	if (!float_equality<double>(xi, test_xi, 1e-5)
			|| !float_equality<double>(yi, test_yi, 1e-5))
		return false;

	// State
	if (student != test_student || works != test_works || infected != test_infected
					|| worksSch != works_at_school)
		return false;	

	return true;
}


