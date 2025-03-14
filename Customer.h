#pragma once
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

struct Customer {
    int id;
    double arrivalTime;
    double serviceTime;
    double departureTime;

    // Function to convert customer data to a CSV string
    std::string toCsv() const;
};

#endif // CUSTOMER_H

