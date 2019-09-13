#include <iostream>
#include <ctime>
#include <utility>
#include <vector>
#include "cleaner.h"
#include "asciiGrapher.h"

int getNumFailedJobs(Db db, unsigned long cutoffTime)
{
    string queryString = "select count(*) from job where startTime < " + to_string(cutoffTime) + 
    " and status = 'R'" + ";";
    const char* query = queryString.c_str();
    try {
        Statement s = db.Query(query);
        s.Next();
        return s.GetField(0);
    } catch (sqdb::Exception& e) {
        cout << "DATABASE ERROR" << endl;
        return 0;
    }
}

//TODO: also set startTime?
void queueFailedJobs(Db db, bool resetAttemptCounter, unsigned long cutoffTime)
{
    string sqlSetStatus = "update job set status = 'Q' where startTime < " + to_string(cutoffTime) + 
    " and status = 'R'" + ";";
    const char* updateStatus = sqlSetStatus.c_str();

    string sqlSetAttempts = "update job set attempts = 0 where startTime < " + to_string(cutoffTime) + 
    " and status = 'R'" + ";";
    const char* updateAttemptCount = sqlSetAttempts.c_str();

    try {
        db.Query("BEGIN EXCLUSIVE;").Next();
        db.Query(updateStatus).Next();
        if (resetAttemptCounter) {
            db.Query(updateAttemptCount).Next();
        }
        db.CommitTransaction();
    } catch (sqdb::Exception& e) {
        cout << "DATABASE ERROR" << endl;
    }
}

int main(int argc, char** argv) {
    Db database("tmp.sqlite"); //TODO: argument for selecting database?

    // Defaults
    int cutoffHours = 24;
    bool resetCounters = true;
    bool shouldPlot = false;

    // Handle command line flags
    if (argc > 1) {
        if (strncmp(argv[1], "--help", 4) == 0) {
            cout << "Flags:" << endl;
            cout << "-p : show a plot of job start times" << endl;
            cout << "-t 'HOURS' : set the age in hours at which jobs are considered failed (defaults to 24)" << endl;
            return 0;
        }

        for (int i = 1; i < argc; i++) { //TODO: gross, refactor
            bool invalid = false;
            if (strncmp(argv[i], "-p", 4) == 0) {
                shouldPlot = true;
            }
            else if (strncmp(argv[i], "-t", 4) == 0) {
                if (i + 1 >= argc) {
                    invalid = true;
                } else {
                    int hours = atoi(argv[i + 1]);
                    if (hours == 0) {
                        invalid = true;
                    } else {
                        cutoffHours = hours;
                    }
                }
            }

            if (invalid) {
                cout << "Invalid argument(s). Try 'cleaner.out --help' for usage tips." << endl;
                return 0;
            }
        }
    }

    int cutoffTimeInSeconds = cutoffHours * 3600;
    unsigned long currentTime = time(nullptr); //TODO: this could potentially break on non-UNIX systems 
    unsigned long oldestAllowableTime = currentTime - cutoffTimeInSeconds;

    int count = getNumFailedJobs(database, oldestAllowableTime);
    if (count == 0) { 
        cout << "No failed jobs." << endl;
        return 0; 
    }
    if (shouldPlot) {
        vector<unsigned long> dataPoints;
        string queryString = "select startTime from job where startTime < " + to_string(oldestAllowableTime) + 
        " and status = 'R'" + ";";
        const char* query = queryString.c_str();
        try {
            Statement s = database.Query(query);
            while (s.Next()) {
                unsigned long startTime = s.GetField(0);
                dataPoints.push_back(startTime);
            }
            Grapher grapher;
            grapher.setData(dataPoints);
            grapher.displayBarGraph(20);
        } catch (sqdb::Exception& e) {
            cout << "DATABASE ERROR" << endl;
        }
    }
    cout << to_string(count) + " failed jobs found." << endl;

    cout << "Queue these jobs? (Y/n) ";
    string response;
    cin >> response;
    if (response != "Y") { return 0; }
    cout << "Reset attempt counters? (Y/n) ";
    cin >> response;
    if (response == "Y") { resetCounters = true; }
    queueFailedJobs(database, resetCounters, oldestAllowableTime);
    cout << "Complete!" << endl;

}
