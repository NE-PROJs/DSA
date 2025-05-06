#include<iostream>
#include<string>
#include<queue>
#include<iomanip>
#include<algorithm>
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

    void registerDoctor(){
        int id, maxAppointments;
        string name, specialization;
        string dob;
        cout<<"\n-----Register Doctor-----\n";
        cout<<"Enter Doctor ID: ";
        cin>>id;
        if(findDoctorById(id) != nullptr){
            cout<<"Doctor with ID "<<id<<" already exists.\n";
            return;
        };
        cin.ignore();
        cout<<"Enter Doctor Name: ";
        getline(cin, name);

        cout<<"Enter Doctor DOB (YYYY-MM-DD): ";
        cin>>dob;
        cin.ignore();
        cout<<"Enter Doctor Specialization: ";  
        getline(cin, specialization);
        cout<<"Enter Max Appointments per day: ";
        cin>>maxAppointments;

        Doctor* newDoctor= new Doctor(id, name, dob, specialization, maxAppointments);
        if(doctorsHead == nullptr){
            doctorsHead= newDoctor;
        }else{
            Doctor* current= doctorsHead;
            while(current->next != nullptr){
                current= current->next;
            };
            current->next= newDoctor;
        };
        cout<< "doctor registered successfully.\n";
    };
    void registerPatient(){
        int id;
        string name, dob, gender;
        
        cout<<"\n -----Register Patient -----\n";
        cout<<"Enter Patient ID: ";
        cin>>id;

        if(findPatientById(id) !=nullptr){
            cout<<"Patient with ID "<<id<<" already exists.\n";
            return;
        };
    };

    void registerAppointment(){
        int id, patientId, doctorId;
        string date;
        cout<<"\n-----Register Appointment-----\n";
        cout<<"Enter Appointment ID: ";
        cin>>id;

        if(findAppointmentById(id) != nullptr){
            cout<<"Appointment with ID "<<id<<" already exists.\n";
            return;
        };
        cout<<"Enter Patient ID: ";
        cin>>patientId;

        //check if entered ID exists
        if(findPatientById(patientId) == nullptr){
            cout<<"Patient with ID "<<patientId<<" does not exist.\n";
            return;
        };

        cout<<"Enter Doctor ID: ";
        cin>>doctorId;

        Doctor* doctor= findDoctorById(doctorId);
        if(doctor == nullptr){
            cout<<"Doctor with ID "<<doctorId<<" does not exist.\n";
            return;
        };

        if(doctor->current_appointments >= doctor-> max_appointments){
            cin.ignore();
            cout<< "Doctor is fully booked. Wd you like to be added to the waiting list? (y/n): ";
            char choice;
            cin>> choice;

            if(toupper(choice) == 'Y'){
                cin.ignore();
                cout<<"Enter requested date (YYYY-MM-DD): ";
                getline(cin, date);

                waitingList.push(WaitingPatient(patientId, doctorId, date));
                cout<<"You have been added to the waiting list.\n";
        };
        return;
    };
    cin.ignore();
    cout<<"Enter Appointment Date (YYYY-MM-DD): ";
    getline(cin, date);

    Appointment* newAppointment= new Appointment(id, patientId, doctorId, date);
    if(appointmentsHead == nullptr){
        appointmentsHead= newAppointment;}
        else{
            Appointment* current= appointmentsHead;
            while(current->next != nullptr){
                current= current->next;
            };
            current->next= newAppointment;
        };
        doctor->current_appointments++;

        cout<<"Appointment registered successfully.\n";
};

    Patient* findPatientById(int id){
        Patient* current= patientsHead;
        while(current != nullptr){
            if(current->patient_id == id){
                return current;
            }
            current= current->next;
        }
        return nullptr;
    };

    Doctor* findDoctorById(int id){
        Doctor* current= doctorsHead;
        while(current != nullptr){
            if(current-> doctor_id == id){
                return current;
            };
            current= current->next;
        };
        return nullptr;
    };

    Appointment* findAppointmentById(int id){
        Appointment* current= appointmentsHead;
        while(current != nullptr){
            if(current-> appointment_id == id){
                return current;
            };
            current= current->next;
        };
        return nullptr;
    };

    void searchPatientByName(){
        if(patientsHead == nullptr){
            cout<<"No patients registered yet.\n";
            return;
        };
        string searchName;
        cout<<"\n ----Search patient by name---\n";
        cin.ignore();
        cout<<"Enter patient name: ";
        getline(cin, searchName);

        //convert to lowercase
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

        bool found= false;
        Patient* current= patientsHead;

        cout<<"......search results......\n";
        while(current !=nullptr){
            string currname= current-> name;
            transform(currname.begin(), currname.end(), currname.begin(), ::tolower);

            if(currname.find(searchName)!= string::npos){
                cout<<"Patient ID: "<<current->patient_id<<endl;
                cout<<"Name: "<<current->name<<endl;
                cout<<"DOB: "<<current->dob<<endl;
                found= true;
            };
            current= current->next;
        };
        if(!found){
            cout<<"No patients found with name "<<searchName<<endl;
        };
        cout<<"......end of search results......\n";
    };

    void sortDoctorBySpecialization(){
        if(doctorsHead== nullptr || doctorsHead->next== nullptr){
            cout<<"No doctors registered yet.\n";
            displayDoctors();
            return;
        };

        //converting to array for easy sorting

        vector<Doctor*> doctorsArray;
        Doctor* current= doctorsHead;
        while(current != nullptr){
            doctorsArray.push_back(current);
            current= current->next;
        };

        sort(doctorsArray.begin(), doctorsArray.end(), [](Doctor* a, Doctor* b){
            return a->specialization < b->specialization;
        });

        cout<<"....doctors sorted by specialization....\n";
        for(Doctor* doc : doctorsArray){
            cout<<"Doctor ID: "<<doc->doctor_id<<endl;
            cout<<"Name: "<<doc->name<<endl;
            cout<<"DOB: "<<doc->dob<<endl;
            cout<<"Specialization: "<<doc->specialization<<endl;
            cout<<"Max Appointments: "<<doc->max_appointments<<endl;
            cout<<"Current Appointments: "<<doc->current_appointments<<endl;
        };
        cout<<"....end of sorted doctors....\n";
    };

    void deletePatient(){

        if(patientsHead == nullptr){
            cout<<"No patients registered yet.\n";
            return;
        };
        int id;
        cout<<"\n-----Delete Patient-----\n";
        cout<<"Enter Patient ID: ";
        cin>>id;

        //check if entered ID exists
        if(findPatientById(id) == nullptr){
            cout<<"Patient with ID "<<id<<" does not exist.\n";
            return;
        };

        deleteAppointmentsByPatientId(id);

        if(patientsHead->patient_id == id){
            Patient* temp= patientsHead;
            patientsHead= patientsHead->next;
            delete temp;
            cout<<"Patient with ID "<<id<<" deleted successfully.\n";
            return;
        }else{
            Patient* prev= patientsHead;
            Patient* current= patientsHead->next;

            while(current != nullptr && current->patient_id !=id){
                prev= current;
                current= current->next;
            };
            if(current != nullptr){
                prev->next= current->next;
                delete current;
            };
        };
        cout<< "patient and associated appointments deleted successfully. \n";
    };

    void deleteDoctor(){
        if(doctorsHead== nullptr){
            cout<< "No doctors registered yet.\n";
            return;
        };

        int id;
        cout<<"\n-----Delete Doctor-----\n";
        cout<<"Enter Doctor ID to delete: ";
        cin>>id;

        if(findDoctorById(id) == nullptr){
            cout<<"Doctor with ID "<<id<<" does not exist.\n";
            return;
        };  

        deleteAppointmentsByDoctorId(id);

        if(doctorsHead->doctor_id == id){
            Doctor* temp= doctorsHead;
            doctorsHead= doctorsHead->next;
            delete temp;
        }else{
            Doctor* prev= doctorsHead;
            Doctor* current= doctorsHead->next;

            while(current != nullptr && current->doctor_id != id){
                prev= current;
                current= current->next;
            };
            if(current != nullptr){
                prev->next= current->next;
                delete current;
            };
        };
        cout<<"doctor with associated appointments deleted successfully.\n";   
    };

    void deleteAppointment(){
        if (appointmentsHead == nullptr) {
            cout << "\nNo appointments to delete.\n";
            return;
        }
        
        int id;
        cout << "\n--- Delete Appointment ---\n";
        cout << "Enter Appointment ID to delete: ";
        cin >> id;
        
        // Check if appointment exists
        Appointment* appointment = findAppointmentById(id);
        if (appointment == nullptr) {
            cout << "Error: Appointment with ID " << id << " does not exist!\n";
            return;
        }
        
        // Update doctor's appointment count
        Doctor* doctor = findDoctorById(appointment->doctor_id);
        if (doctor != nullptr) {
            doctor->current_appointments--;
            
            // Check if there are patients in the waiting list for this doctor
            if (!waitingList.empty() && doctor->current_appointments < doctor->max_appointments) {
                queue<WaitingPatient> tempQueue;
                bool appointmentScheduled = false;
                
                while (!waitingList.empty()) {
                    WaitingPatient patient = waitingList.front();
                    waitingList.pop();
                    
                    if (!appointmentScheduled && patient.doctor_id == doctor->doctor_id) {
                        // Create a new appointment for the waiting patient
                        int newAppointmentId = 1;
                        if (appointmentsHead != nullptr) {
                            Appointment* current = appointmentsHead;
                            while (current->next != nullptr) {
                                current = current->next;
                            }
                            newAppointmentId = current->appointment_id + 1;
                        }
                        
                        Appointment* newAppointment = new Appointment(
                            newAppointmentId, 
                            patient.patient_id, 
                            patient.doctor_id, 
                            patient.requested_date
                        );
                        
                        // Add to linked list
                        if (appointmentsHead == nullptr) {
                            appointmentsHead = newAppointment;
                        } else {
                            Appointment* current = appointmentsHead;
                            while (current->next != nullptr) {
                                current = current->next;
                            }
                            current->next = newAppointment;
                        }
                        
                        doctor->current_appointments++;
                        appointmentScheduled = true;
                        
                        cout << "Patient from waiting list has been scheduled an appointment.\n";
                    } else {
                        tempQueue.push(patient);
                    }
                }
                
                // Restore remaining patients to the waiting list
                waitingList = tempQueue;
            }
        }
        
        // Delete the appointment
        if (appointmentsHead->appointment_id == id) {
            Appointment* temp = appointmentsHead;
            appointmentsHead = appointmentsHead->next;
            delete temp;
        } else {
            Appointment* prev = appointmentsHead;
            Appointment* current = appointmentsHead->next;
            
            while (current != nullptr && current->appointment_id != id) {
                prev = current;
                current = current->next;
            }
            
            if (current != nullptr) {
                prev->next = current->next;
                delete current;
            }
        }
        
        cout << "Appointment deleted successfully!\n";
    };

    void deleteAppointmentsByPatientId(int patientId){
        if(appointmentsHead == nullptr){
            return;
        };
        Appointment* current= appointmentsHead;
        while (current !=nullptr){
            if(current -> patient_id==patientId){
                Doctor* doctor= findDoctorById(current->doctor_id);
                if(doctor != nullptr){
                    doctor->current_appointments--;
                };
            };
            current= current->next;
        };
        while (appointmentsHead != nullptr && appointmentsHead -> patient_id== patientId)
        {
            Appointment* temp= appointmentsHead;
            appointmentsHead= appointmentsHead->next;
            delete temp;
        }
        if(appointmentsHead==nullptr){
            return;
        };
        Appointment* prev= appointmentsHead;
        current= appointmentsHead->next;

        while (current != nullptr)
        {
            if(current->patient_id == patientId){
                prev-> next= current-> next;
                delete current;
                current= prev-> next;
            }else{
                prev=current;
                current=current->next;
            };
        }
    };

    void deleteAppointmentsByDoctorId(int doctorId){
        if(appointmentsHead== nullptr){
            return;
        };
        while(appointmentsHead != nullptr && appointmentsHead -> doctor_id==doctorId){
            Appointment* temp= appointmentsHead;
            appointmentsHead= appointmentsHead-> next;
            delete temp;
        };
        if(appointmentsHead==nullptr){
            return;
        };

        Appointment* prev = appointmentsHead;
        Appointment* current = appointmentsHead->next;
        
        while (current != nullptr) {
            if (current->doctor_id == doctorId) {
                prev->next = current->next;
                delete current;
                current = prev->next;
            } else {
                prev = current;
                current = current->next;
            }
        }
    };

    void displayDoctors(){
        if(doctorsHead==nullptr){
            cout<<"No doctors registered yet.\n";
            return;
        };

        cout << "\n--- Doctors List ---\n";
        cout << left << setw(12) << "Doctor ID" << setw(25) << "Name" 
             << setw(20) << "Specialization" << setw(15) << "Appointments" << endl;
        cout << string(70, '-') << endl;

        Doctor* current= doctorsHead;
        while(current!=nullptr){
            cout << left << setw(12) << current->doctor_id << setw(25) << current->name 
                 << setw(20) << current->specialization << current->current_appointments 
                 << "/" << current->max_appointments << endl;
            current = current->next;
        };
    };

    void displayPatients(){

        if(patientsHead == nullptr){
            cout<<"No patients registered yet.\n";
            return;
        };
        cout << "\n--- Patients List ---\n";
        cout << left << setw(12) << "Patient ID" << setw(25) << "Name" 
             << setw(15) << "Date of Birth" << setw(10) << "Gender" << endl;
        cout << string(60, '-') << endl;
        
        Patient* current = patientsHead;
        while (current != nullptr) {
            cout << left << setw(12) << current->patient_id << setw(25) << current->name 
                 << setw(15) << current->dob << setw(10) << current->gender << endl;
            current = current->next;
        };
    };

    void displayAppointments() {
        if (appointmentsHead == nullptr) {
            cout << "\nNo appointments registered yet.\n";
            return;
        }
        
        cout << "\n--- Appointments List ---\n";
        cout << left << setw(15) << "Appointment ID" << setw(15) << "Patient ID" 
             << setw(15) << "Doctor ID" << setw(20) << "Date" << endl;
        cout << string(65, '-') << endl;
        
        Appointment* current = appointmentsHead;
        while (current != nullptr) {
            cout << left << setw(15) << current->appointment_id << setw(15) << current->patient_id 
                 << setw(15) << current->doctor_id << setw(20) << current->appointment_date << endl;
            current = current->next;
        }
    };

    void displayWaitingList(){
        if (waitingList.empty()) {
            cout << "\nWaiting list is empty.\n";
            return;
        }
        
        cout << "\n--- Waiting List ---\n";
        cout << left << setw(15) << "Patient ID" << setw(15) << "Doctor ID" 
             << setw(20) << "Requested Date" << endl;
        cout << string(50, '-') << endl;

        queue<WaitingPatient> tempQueue = waitingList;

        while(!tempQueue.empty()){
            WaitingPatient patient= tempQueue.front();
            cout << left << setw(15) << patient.patient_id << setw(15) << patient.doctor_id 
                 << setw(20) << patient.requested_date << endl;
            tempQueue.pop();
        };
    };

    void editPatient() {
        if (patientsHead == nullptr) {
            cout << "\nNo patients to edit.\n";
            return;
        }
        
        int id;
        cout << "\n--- Edit Patient ---\n";
        cout << "Enter Patient ID to edit: ";
        cin >> id;
        
        // Find the patient
        Patient* patient = findPatientById(id);
        if (patient == nullptr) {
            cout << "Error: Patient with ID " << id << " does not exist!\n";
            return;
        }
        
        cin.ignore(); // Clear input buffer
        
        string name, dob, gender;
        cout << "Enter new name (leave blank to keep current: " << patient->name << "): ";
        getline(cin, name);
        if (!name.empty()) {
            patient->name = name;
        }
        
        cout << "Enter new date of birth (leave blank to keep current: " << patient->dob << "): ";
        getline(cin, dob);
        if (!dob.empty()) {
            patient->dob = dob;
        }
        
        cout << "Enter new gender (M/F) (leave blank to keep current: " << patient->gender << "): ";
        getline(cin, gender);
        if (!gender.empty()) {
            patient->gender = gender;
        }
        
        cout << "Patient information updated successfully!\n";
    }
    
    // Function to edit a doctor
    void editDoctor() {
        if (doctorsHead == nullptr) {
            cout << "\nNo doctors to edit.\n";
            return;
        }
        
        int id;
        cout << "\n--- Edit Doctor ---\n";
        cout << "Enter Doctor ID to edit: ";
        cin >> id;
        
        // Find the doctor
        Doctor* doctor = findDoctorById(id);
        if (doctor == nullptr) {
            cout << "Error: Doctor with ID " << id << " does not exist!\n";
            return;
        }
        
        cin.ignore(); // Clear input buffer
        
        string name, specialization;
        int maxAppointments;
        
        cout << "Enter new name (leave blank to keep current: " << doctor->name << "): ";
        getline(cin, name);
        if (!name.empty()) {
            doctor->name = name;
        }
        
        cout << "Enter new specialization (leave blank to keep current: " << doctor->specialization << "): ";
        getline(cin, specialization);
        if (!specialization.empty()) {
            doctor->specialization = specialization;
        }
        
        cout << "Enter new maximum appointments (leave 0 to keep current: " << doctor->max_appointments << "): ";
        cin >> maxAppointments;
        if (maxAppointments > 0) {
            // Ensure the new maximum is not less than current appointments
            if (maxAppointments < doctor->current_appointments) {
                cout << "Warning: New maximum is less than current appointments. ";
                cout << "Some appointments may need to be rescheduled.\n";
            }
            doctor->max_appointments = maxAppointments;
        }
        
        cout << "Doctor information updated successfully!\n";
    }
    
    // Function to edit an appointment
    void editAppointment() {
        if (appointmentsHead == nullptr) {
            cout << "\nNo appointments to edit.\n";
            return;
        }
        
        int id;
        cout << "\n--- Edit Appointment ---\n";
        cout << "Enter Appointment ID to edit: ";
        cin >> id;
        
        // Find the appointment
        Appointment* appointment = findAppointmentById(id);
        if (appointment == nullptr) {
            cout << "Error: Appointment with ID " << id << " does not exist!\n";
            return;
        }
        
        int patientId, doctorId;
        string date;
        
        cout << "Enter new Patient ID (enter 0 to keep current: " << appointment->patient_id << "): ";
        cin >> patientId;
        if (patientId > 0) {
            // Check if patient exists
            if (findPatientById(patientId) == nullptr) {
                cout << "Error: Patient with ID " << patientId << " does not exist!\n";
                return;
            }
            appointment->patient_id = patientId;
        }
        
        cout << "Enter new Doctor ID (enter 0 to keep current: " << appointment->doctor_id << "): ";
        cin >> doctorId;
        if (doctorId > 0) {
            // First, decrement current doctor's appointment count
            Doctor* currentDoctor = findDoctorById(appointment->doctor_id);
            if (currentDoctor != nullptr) {
                currentDoctor->current_appointments--;
            }
            
            // Check if new doctor exists
            Doctor* newDoctor = findDoctorById(doctorId);
            if (newDoctor == nullptr) {
                cout << "Error: Doctor with ID " << doctorId << " does not exist!\n";
                // Restore previous doctor's count
                if (currentDoctor != nullptr) {
                    currentDoctor->current_appointments++;
                }
                return;
            }
            
            // Check if new doctor has reached maximum appointments
            if (newDoctor->current_appointments >= newDoctor->max_appointments) {
                cout << "Error: The selected doctor is fully booked!\n";
                // Restore previous doctor's count
                if (currentDoctor != nullptr) {
                    currentDoctor->current_appointments++;
                }
                return;
            }
            
            // Update the appointment
            appointment->doctor_id = doctorId;
            newDoctor->current_appointments++;
        }
        
        cin.ignore(); // Clear input buffer
        
        cout << "Enter new date (leave blank to keep current: " << appointment->appointment_date << "): ";
        getline(cin, date);
        if (!date.empty()) {
            appointment->appointment_date = date;
        }
        
        cout << "Appointment updated successfully!\n";
    }


    void showMenu(){
        int choice;
        bool running = true;

        while(running){
            cout<<"\n ====== Ruhengeri Referral hospital management system=====\n"<<endl;
            cout << "1: Register Patient\n";
            cout << "2: Register Doctor\n";
            cout << "3: Register Appointment\n";
            cout << "4: Display Patients\n";
            cout << "5: Display Doctors\n";
            cout << "6: Display Appointments\n";
            cout << "7: Search Patient by Name\n";
            cout << "8: Sort Doctors by Specialization\n";
            cout << "9: Delete Patient\n";
            cout << "10: Delete Doctor\n";
            cout << "11: Delete Appointment\n";
            cout << "12: Edit Patient\n";
            cout << "13: Edit Doctor\n";
            cout << "14: Edit Appointment\n";
            cout << "15: Display Waiting List\n";
            cout << "16: Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch(choice){
                case 1:
                registerPatient();
                break;
                case 2:
                registerDoctor();
                break;
                case 3:
                registerAppointment();
                break;
                case 4:
                displayPatients();
                break;
                case 5:
                displayDoctors();
                break;
                case 6:
                displayAppointments();
                break;
                case 7:
                searchPatientByName();
                break;
                case 8:
                sortDoctorBySpecialization();
                break;
                case 9:
                deletePatient();
                break;
                case 10:
                deleteDoctor();
                break;
                case 11:
                deleteAppointment();
                break;
                case 12:
                editPatient();
                break;
                case 13:
                    editDoctor();
                    break;
                case 14:
                    editAppointment();
                    break;
                case 15:
                    displayWaitingList();
                    break;
                case 16:
                running=false;
                cout<< "Thank you for using our system \n";
                break;
                default:
                cout<<"Invalid choice. please try again. \n";
            };
        };
    };
};


int main(){
    HospitalManagementSystem hms;
    cout<<"Welcome to Ruhengeri Hospital system!!";
    hms.showMenu();

    return 0;
};

