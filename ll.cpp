#include<iostream>
using namespace std;

class Patient{
    public:
    int patient_id;
    string name;
    string dob;
    string gender;
    Patient *next;
    Patient(int patient_id, string name, string dob, string gender){
        this->patient_id = patient_id;
        this->name = name;
        this->dob = dob;
        next = NULL;
    }
};

class Doctor{
    public:
    int doctor_id;
    string name;
    string dob;
    string specialization;
    Doctor *next;
    Doctor(int doctor_id, string name, string dob, string specialization){
        this->doctor_id = doctor_id;
        this->name = name;
        this->dob = dob;
        this->specialization = specialization;
        next = NULL;
    }
};

class Appointment{
    public:
    int appointment_id;
    int patient_id;
    int doctor_id;
    string appointment_date;
    Appointment *next;
    Appointment(int appointment_id, int patient_id, int doctor_id, string appointment_date){
        this->appointment_id = appointment_id;
        this->patient_id = patient_id;
        this->doctor_id = doctor_id;
        this->appointment_date = appointment_date;
        next = NULL;
    }
};
