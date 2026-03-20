#include "EmergencyManagement.h"
#include "Utility.h"
#include "Logger.h"
#include "BillingService.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

EmergencyManagement::EmergencyManagement() {
    loadState();
}

EmergencyManagement::~EmergencyManagement() {
    saveState();
}

void EmergencyManagement::registerEmergencyPatient(int patientID) {
    auto tempStack = oldEmergencyStack;
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        if (p->id == patientID) {
            cout << "Patient with ID " << patientID << " is already in the old records.\n";
            return;
        }
        tempStack.pop();
    }

    auto patient = make_shared<Patient>();
    patient->id = patientID;
    cout << "Enter patient name: "; getline(cin, patient->name);
    cout << "Enter patient contact: "; getline(cin, patient->contact);
    patient->age = Utility::getSafeInt("Enter patient age: ");
    cout << "Enter entry condition: "; getline(cin, patient->entryCondition);

    currentEmergencyStack.push(patient);
    Logger::getInstance().log("Registered Emergency Patient ID: " + to_string(patientID));
    cout << "Emergency Patient registered successfully.\n";
}

void EmergencyManagement::displayEmergencyPatients() const {
    if (currentEmergencyStack.empty()) { cout << "No current emergency patients.\n"; return; }
    auto tempStack = currentEmergencyStack;
    cout << "--- Current Emergency Patients ---\n";
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        cout << "ID: " << p->id << " | Name: " << p->name << " | Condition: " << p->entryCondition << "\n";
        tempStack.pop();
    }
}

void EmergencyManagement::searchEmergencyPatient(int patientID) const {
    auto tempStack = currentEmergencyStack;
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        if (p->id == patientID) { cout << "Found in Current Emergency: " << p->name << "\n"; return; }
        tempStack.pop();
    }
    tempStack = oldEmergencyStack;
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        if (p->id == patientID) { cout << "Found in Old Emergency: " << p->name << "\n"; return; }
        tempStack.pop();
    }
    cout << "Patient ID " << patientID << " not found in emergency records.\n";
}

void EmergencyManagement::leaveEmergency(int patientID) {
    stack<shared_ptr<Patient>> tempStack;
    shared_ptr<Patient> target = nullptr;

    while (!currentEmergencyStack.empty()) {
        auto p = currentEmergencyStack.top();
        currentEmergencyStack.pop();
        if (p->id == patientID) { target = p; break; }
        tempStack.push(p);
    }

    if (!target) {
        cout << "Patient ID " << patientID << " not found in current emergency.\n";
        while (!tempStack.empty()) { currentEmergencyStack.push(tempStack.top()); tempStack.pop(); }
        return;
    }

    cout << "Enter leaving health condition: "; getline(cin, target->leavingCondition);
    
    double bill = BillingService::calculateBill(target->leavingCondition, "Emergency Care", 1);
    target->totalBill = bill;
    BillingService::printInvoice(target->id, target->name, bill);

    target->hasLeftEmergency = true;
    oldEmergencyStack.push(target);

    while (!tempStack.empty()) { currentEmergencyStack.push(tempStack.top()); tempStack.pop(); }
    Logger::getInstance().log("Emergency Patient ID: " + to_string(patientID) + " discharged.");
    cout << "Patient has successfully left the emergency department.\n";
}

void EmergencyManagement::displayOldEmergencyPatients() const {
    if (oldEmergencyStack.empty()) { cout << "No old emergency patients.\n"; return; }
    auto tempStack = oldEmergencyStack;
    cout << "--- Old Emergency Patients ---\n";
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        cout << "ID: " << p->id << " | Name: " << p->name << " | Left Cond: " << p->leavingCondition << "\n";
        tempStack.pop();
    }
}

void EmergencyManagement::saveState() const {
    ofstream file("data/emergency_old.csv");
    if (!file.is_open()) return;
    auto tempStack = oldEmergencyStack;
    while (!tempStack.empty()) {
        const auto& p = tempStack.top();
        file << p->id << "," << p->name << "," << p->age << "," << p->entryCondition << "," << p->leavingCondition << "\n";
        tempStack.pop();
    }
    file.close();
}

void EmergencyManagement::loadState() {
    ifstream file("data/emergency_old.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto p = make_shared<Patient>();
        stringstream ss(line);
        string item;
        if (!getline(ss, item, ',')) continue;
        p->id = Utility::safeStoi(item);
        getline(ss, p->name, ',');
        getline(ss, item, ','); p->age = Utility::safeStoi(item);
        getline(ss, p->entryCondition, ',');
        getline(ss, p->leavingCondition, ',');
        p->hasLeftEmergency = true;
        oldEmergencyStack.push(p);
    }
    file.close();
}
