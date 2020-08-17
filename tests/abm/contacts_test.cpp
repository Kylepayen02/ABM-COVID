#include "abm_tests.h"


/*****************************************************
 *
 * Test to check the collection of agent interactions
 *
 ******************************************************/

void test_contacts();
//May implement for better formalize
//bool compare_files();

int main(){
    //
    test_contacts();
}

// Output:
// "dead_interactions.txt" - File containing the number of interactions between agents for all agents
// "interactions.txt" - File containing the number of dead interactions between agents for all agents
// Use "diff" to compare with correct output files
void test_contacts()
{
    // Time in days, space in km
    double dt = 0.25;
    // Max number of steps to simulate
    int tmax = 10;


    // Input files
//	std::string fin("input_data/NR_agents.txt");
    std::string fin("test_data/contacts_input_data/contacts.txt");
    std::string hfile("test_data/contacts_input_data/NR_households.txt");
    std::string sfile("test_data/contacts_input_data/NR_schools.txt");
    std::string wfile("test_data/contacts_input_data/NR_workplaces.txt");


    // File with infection parameters
    std::string pfname("test_data/contacts_input_data/infection_parameters.txt");
    // Files with age-dependent distributions;
    std::string dmort_name("test_data/contacts_input_data/age_dist_mortality.txt");
    // Map for abm loading of distributions
    std::map<std::string, std::string> dfiles =
            { {"mortality", dmort_name} };

    ABM abm(dt, pfname, dfiles);

    // First the places
    abm.create_households(hfile);
    abm.create_schools(sfile);
    abm.create_workplaces(wfile);

    // Then the agents
    abm.create_agents(fin, 0);

    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();

    for (int ti = 0; ti<=tmax; ++ti){

        //Get the number of interactions for each agent
        if (ti == 2){
            agents.at(0).set_dead(true);
        }
        if (ti == 3){
            agents.at(3).set_dead(true);
        }
        if (ti == 4){
            agents.at(9).set_dead(true);
        }


        abm.collect_all_interactions();
        abm.collect_dead_interactions();
    }



    // Output interactions
    abm.output_interactions("interactions.txt");

    //Output dead interactions
    abm.output_dead_interactions("dead_interactions.txt");


}


