#include<iostream>
#include<string>
#include<queue>
using namespace std;
struct Patient{
    int patient_id;
    string name, dob, gender;
    Patient* next;
    Patient(int id, string n, string d, string g): 
    patient_id(id), name(n), dob(d), gender(g), next(nullptr) {};
};

struct Doctor{
    int doctor_id;
    string name, dob, specialization;
    int max_appointments;
    int current_appointments;
    Doctor* next;
    Doctor(int id, string n, string d, string s, int max=10):
    doctor_id(id), name(n), dob(d), specialization(s), max_appointments(max),
    current_appointments(0), next(nullptr) {};
};

struct Appointment{
    int appointment_id;
    int patient_id;
    int doctor_id;
    string appointment_date;
    Appointment* next;
    Appointment(int a_id, int p_id, int d_id, string date):
    appointment_id(a_id), patient_id(p_id), doctor_id(d_id), appointment_date(date), next(nullptr) {};
};

struct WaitingPatient{
    int patient_id;
    int doctor_id;
    string requested_date;
    WaitingPatient* next;
    WaitingPatient(int p_id, int d_id, string date):
    patient_id(p_id), doctor_id(d_id), requested_date(date), next(nullptr) {};
};

class HospitalManagementSystem{
    private :
    Patient* patientsHead;
    Doctor* doctorsHead;
    Appointment* appointmentsHead;
    queue<WaitingPatient> waitingList;

    public :
    HospitalManagementSystem(): 
    patientsHead(nullptr), doctorsHead(nullptr), appointmentsHead(nullptr) {};

    ~HospitalManagementSystem(){
        Patient* currentPatient= patientsHead;

        while(currentPatient != nullptr){
            Patient* temp= currentPatient;
            currentPatient= currentPatient-> next;
            delete temp;
        };

        Doctor* currentDoctor= doctorsHead;
        while(currentDoctor != nullptr){
            Doctor* temp= currentDoctor;
            currentDoctor= currentDoctor->next;
            delete temp;
        };

        Appointment* currentAppointment= appointmentsHead;
        while(currentAppointment != nullptr){
            Appointment* temp= currentAppointment;
            currentAppointment= currentAppointment->next;
            delete temp;
        };
    };


};
