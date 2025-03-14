#include "Customer.h"
#include <sstream>
#include <iomanip>

using namespace std;

// Convert customer data to a CSV string
string Customer::toCsv() const {
    ostringstream oss;
    oss << id << ","
        << fixed << setprecision(2) << arrivalTime << ","
        << serviceTime << ","
        << departureTime;
    return oss.str();
}