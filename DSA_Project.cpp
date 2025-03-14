#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <map>
#include <cstdlib> // For system() call
#include <iomanip> // For formatting output
#include "Customer.h"

using namespace std;

// Constants
const int SIMULATION_TIME = 1000; // Total simulation time

// Function to save results to a CSV file
void saveResultsToCsvFile(const vector<Customer>& customers, double mean, double median, double mode, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing.\n";
        return;
    }

    // Write CSV header
    outFile << "Customer Queue Simulation Results\n";
    outFile << "ID,Arrival Time,Service Time,Departure Time\n";

    // Write customer data as CSV rows
    for (const auto& customer : customers) {
        outFile << customer.id << ","
            << fixed << setprecision(2) << customer.arrivalTime << ","
            << customer.serviceTime << ","
            << customer.departureTime << "\n";
    }

    // Write statistics
    outFile << "\nStatistics\n";
    outFile << "Mean Service Time," << mean << "\n";
    outFile << "Median Service Time," << median << "\n";
    outFile << "Mode Service Time," << mode << "\n";

    outFile.close();
    cout << "Results saved to " << filename << "\n";
}

// Function to calculate mean
double calculateMean(const vector<Customer>& customers) {
    double sum = 0.0;
    for (const auto& customer : customers) {
        sum += customer.serviceTime;
    }
    return sum / customers.size();
}

// Function to calculate median
double calculateMedian(vector<Customer> customers) {
    sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
        return a.serviceTime < b.serviceTime;
        });

    size_t size = customers.size();
    if (size % 2 == 0) {
        return (customers[size / 2 - 1].serviceTime + customers[size / 2].serviceTime) / 2.0;
    }
    else {
        return customers[size / 2].serviceTime;
    }
}

// Function to calculate mode
double calculateMode(const vector<Customer>& customers) {
    map<double, int> frequencyMap;
    for (const auto& customer : customers) {
        frequencyMap[customer.serviceTime]++;
    }

    double mode = 0.0;
    int maxFrequency = 0;
    for (const auto& pair : frequencyMap) {
        if (pair.second > maxFrequency) {
            maxFrequency = pair.second;
            mode = pair.first;
        }
    }
    return mode;
}

// Function to write service times to a file for GNUplot
void writeServiceTimesToFile(const vector<Customer>& customers, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file for writing.\n";
        return;
    }

    for (const auto& customer : customers) {
        outFile << customer.id << " " << customer.serviceTime << "\n";
    }

    outFile.close();
    cout << "Service times written to " << filename << "\n";
}

// Function to generate a graph using GNUplot
void generateGraphWithGNUplot(const string& dataFilename) {
    // GNUplot commands
    string gnuplotCommands = R"(
        set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
        set output 'service_times_graph.png'
        set title "Customer Service Times"
        set xlabel "Customer ID"
        set ylabel "Service Time"
        plot ')" + dataFilename + R"(' using 1:2 with linespoints title 'Service Time'
    )";

    // Write GNUplot commands to a temporary file
    ofstream gnuplotScript("gnuplot_script.gp");
    gnuplotScript << gnuplotCommands;
    gnuplotScript.close();

    // Call GNUplot to execute the script
    system("gnuplot gnuplot_script.gp");

    cout << "Graph generated as 'service_times_graph.png'\n";
}

// Function to display simulation results in a table
void displayResultsTable(const vector<Customer>& customers) {
    // Table header
    cout << "+-----+--------------+--------------+-----------------+\n";
    cout << "| ID  | Arrival Time | Service Time | Departure Time  |\n";
    cout << "+-----+--------------+--------------+-----------------+\n";

    // Table rows
    for (const auto& customer : customers) {
        cout << "| " << setw(3) << customer.id << " "
            << "| " << setw(12) << fixed << setprecision(2) << customer.arrivalTime << " "
            << "| " << setw(12) << customer.serviceTime << " "
            << "| " << setw(15) << customer.departureTime << " |\n";
    }

    // Table footer
    cout << "+-----+--------------+--------------+-----------------+\n";
}

// Function to display statistics attractively
void displayStatistics(double mean, double median, double mode) {
    cout << "\n+---------------------+\n";
    cout << "|      Statistics     |\n";
    cout << "+---------------------+\n";
    cout << "| Mean Service Time:  | " << setw(10) << fixed << setprecision(2) << mean << " |\n";
    cout << "| Median Service Time:| " << setw(10) << median << " |\n";
    cout << "| Mode Service Time:  | " << setw(10) << mode << " |\n";
    cout << "+---------------------+\n";
}

// Main simulation function
void runSimulation(double arrivalRate, double serviceRate, int numServers) {
    queue<Customer> customerQueue;
    vector<Customer> servedCustomers;

    // Random number generators
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<> arrivalDist(arrivalRate);
    exponential_distribution<> serviceDist(serviceRate);

    double currentTime = 0.0;
    int customerId = 0;

    while (currentTime < SIMULATION_TIME) {
        // Generate new customer
        double arrivalInterval = arrivalDist(gen);
        currentTime += arrivalInterval;
        Customer newCustomer = { customerId++, currentTime, serviceDist(gen), 0.0 };
        customerQueue.push(newCustomer);

        // Serve customers
        while (!customerQueue.empty() && numServers > 0) {
            Customer servedCustomer = customerQueue.front();
            customerQueue.pop();
            servedCustomer.departureTime = currentTime + servedCustomer.serviceTime;
            servedCustomers.push_back(servedCustomer);
            numServers--;
        }
    }

    // Display results in a table
    cout << "\nSimulation Results:\n";
    displayResultsTable(servedCustomers);

    // Calculate statistics
    double mean = calculateMean(servedCustomers);
    double median = calculateMedian(servedCustomers);
    double mode = calculateMode(servedCustomers);

    // Display statistics
    displayStatistics(mean, median, mode);

    // Write service times to a file for GNUplot
    string dataFilename = "service_times.dat";
    writeServiceTimesToFile(servedCustomers, dataFilename);

    // Generate graph using GNUplot
    generateGraphWithGNUplot(dataFilename);

    // Save results to a CSV file
    saveResultsToCsvFile(servedCustomers, mean, median, mode, "simulation_results.csv");
}

int main() {
    // Get user input for simulation parameters
    double arrivalRate, serviceRate;
    int numServers;

    cout << "Enter arrival rate (customers per unit time): ";
    cin >> arrivalRate;

    cout << "Enter service rate (services per unit time): ";
    cin >> serviceRate;

    cout << "Enter number of servers: ";
    cin >> numServers;

    // Run simulation
    runSimulation(arrivalRate, serviceRate, numServers);

    cout << "Simulation complete. Results saved to file.\n";

    // Pause to keep the console open (Windows-specific)
    system("pause");
    return 0;
}