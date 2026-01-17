#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>
using namespace std;

class SystemInterface {
public:
    virtual void welcomeMessage() = 0;
};

class FineRecord {
protected:
    string pName, pRank, vName, lNum, cNum, vType, mobNum, station, area, transID, date;
    double amount;
    int isPaid;

public:
    FineRecord(string pN, string pR, string vN, string lN, string cN, string vT, string mN, string st, string ar, double amt, int paid = 0, string tid = "N/A", string dt = "") {
        pName = pN; pRank = pR; vName = vN; lNum = lN; cNum = cN;
        vType = vT; mobNum = mN; station = st; area = ar;
        amount = amt; isPaid = paid; transID = tid;

        if(dt == "") {
            time_t now = time(0);
            date = ctime(&now);
            date.erase(date.length()-1);
        } else {
            date = dt;
        }
    }

    virtual void displaySummary() {
        cout << left << setw(15) << lNum << " | " << setw(15) << vType << " | " << setw(8) << amount << " | " << (isPaid ? "PAID" : "UNPAID") << " | " << date << endl;
    }

    void displayFullDetails() {
        cout << endl;
        cout << "CASE DETAILS [" << (isPaid ? "PAID" : "UNPAID") << "]" << endl;
        cout << "Date          : " << date << endl;
        cout << "Violator      : " << vName << " | Mobile: " << mobNum << endl;
        cout << "License       : " << lNum << " | Vehicle: " << cNum << endl;
        cout << "Crime         : " << vType << " | Fine: " << amount << " BDT" << endl;
        cout << "Location      : " << area << " (" << station << ")" << endl;
        cout << "Officer       : " << pName << " (" << pRank << ")" << endl;
        if(isPaid)
            cout << "Transaction ID: " << transID << endl;

    }

    string getMobile() { return mobNum; }
    string getLicense() { return lNum; }
    int getStatus() { return isPaid; }
    double getAmount() { return amount; }
    void setPaid(string tid) { isPaid = 1; transID = tid; }

    friend ofstream& operator<<(ofstream& ofs, const FineRecord& fr) {
        ofs << fr.pName << "|" << fr.pRank << "|" << fr.vName << "|" << fr.lNum << "|" << fr.cNum << "|"
            << fr.vType << "|" << fr.mobNum << "|" << fr.station << "|" << fr.area << "|"
            << fr.amount << "|" << fr.isPaid << "|" << fr.transID << "|" << fr.date << endl;
        return ofs;
    }
};

class TrafficControl : public SystemInterface {
private:
    vector<FineRecord> records;
    string adminPass = "1120120";
    string fileName = "fines.txt";

    void loadFromFile() {
        ifstream file(fileName);
        if (!file) return;
        records.clear();
        string pN, pR, vN, lN, cN, vT, mN, st, ar, amtStr, paidStr, tid, dt;
        while (getline(file, pN, '|')) {
            getline(file, pR, '|'); getline(file, vN, '|'); getline(file, lN, '|');
            getline(file, cN, '|'); getline(file, vT, '|'); getline(file, mN, '|');
            getline(file, st, '|'); getline(file, ar, '|');
            getline(file, amtStr, '|'); getline(file, paidStr, '|');
            getline(file, tid, '|'); getline(file, dt);
            if (!pN.empty()) {
                records.push_back(FineRecord(pN, pR, vN, lN, cN, vT, mN, st, ar, atof(amtStr.c_str()), atoi(paidStr.c_str()), tid, dt));
            }
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(fileName);
        for (const auto& r : records) file << r;
        file.close();
    }

public:
    TrafficControl() { loadFromFile(); }

    void welcomeMessage() override {
        cout << endl;
        cout << "HELLO! Welcome To Traffic Fine Management System" << endl;
    }

    void showDashboard() {
        double collected = 0, pending = 0;
        int pCount = 0, uCount = 0;
        for (auto& r : records) {
            if (r.getStatus()) { collected += r.getAmount(); pCount++; }
            else { pending += r.getAmount(); uCount++; }
        }
        cout << endl;
        cout << "\nSYSTEM DASHBOARD" << endl;
        cout << "Total Collected Fine: " << collected << " BDT (" << pCount << " cases)" << endl;
        cout << "Total Pending Fine  :   " << pending << " BDT (" << uCount << " cases)" << endl;
    }

    void adminModule() {
        string pass;
        cout << "Admin Login: "; cin >> pass;
        if (pass != adminPass) { cout << "Wrong Password!\n"; return; }

        while(true) {
            showDashboard();
            cout << "\n1. Issue New Fine\n2. View All Records\n3. Search by License\n4. Logout\nSelection: ";
            int adminChoice; cin >> adminChoice;

            if (adminChoice == 1) {
                string pN, pR, vN, lN, cN, vT, mN, st, ar;
                double amt; cin.ignore();
                cout << "Police Name  : "; getline(cin, pN);
                cout << "Rank         : "; getline(cin, pR);
                cout << "Violator Name: "; getline(cin, vN);
                cout << "License      : "; getline(cin, lN);
                cout << "Car No       : "; getline(cin, cN);
                cout << "Crime Type   : "; getline(cin, vT);
                cout << "Mobile No    : "; getline(cin, mN);
                cout << "Station      : "; getline(cin, st);
                cout << "Area         : "; getline(cin, ar);
                cout << "Amount       : "; cin >> amt;
                records.push_back(FineRecord(pN, pR, vN, lN, cN, vT, mN, st, ar, amt));
                saveToFile();
                cout << "Case Filed Successfully.\n";
            }
            else if (adminChoice == 2) {
                cout << left << setw(15) << "License" << " | " << setw(15) << "Crime" << " | " << setw(8) << "Amount" << " | " << "Status" << " | Date" << endl;
                for (auto& r : records) r.displaySummary();
            }
            else if (adminChoice == 3) {
                string sL; cout << "Enter License: "; cin >> sL;
                bool f = false;
                for (auto& r : records) if(r.getLicense() == sL) { r.displayFullDetails(); f = true; }
                if(!f) cout << "No record found.\n";
            }
            else if (adminChoice == 4) break;
        }
    }

    void paymentModule() {
        string mob;
        cout << "\nEnter Mobile Number: "; cin >> mob;
        double total = 0; bool found = false;
        vector<FineRecord*> pending;

        for (auto& r : records) {
            if (r.getMobile() == mob && !r.getStatus()) {
                if(!found) cout << "\nPENDING CASES:\n";
                r.displaySummary();
                total += r.getAmount();
                pending.push_back(&r);
                found = true;
            }
        }

        if (found) {
            cout << "TOTAL PAYABLE: " << total << " BDT" << endl;
            cout << "Pay now? (y/n): "; char c; cin >> c;
            if (c == 'y' || c == 'Y') {
                string tid; cout << "Enter Transit ID: "; cin >> tid;
                for (auto rPtr : pending) rPtr->setPaid(tid);
                saveToFile();
                cout << "Payment Successful.\n";
            }
        } else cout << "No unpaid fines found.\n";
    }
};

int main() {
    TrafficControl system;
    int choice;
    while (true) {
        system.welcomeMessage();
        cout << "1. Admin Login\n2. Pay Fine\n3. Exit\nSelection: ";
        if (!(cin >> choice)) { cin.clear(); string d; cin >> d; continue; }
        if (choice == 1) system.adminModule();
        else if (choice == 2) system.paymentModule();
        else if (choice == 3) break;
    }
    return 0;
}

