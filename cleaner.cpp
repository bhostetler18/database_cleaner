#include <iostream>
#include <ctime>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include "cleaner.h"
#include "asciiGrapher.h"
#include "sqliteTools.h"

//TODO: make class to better support names and conversions?
enum TimeUnit { MINUTE = 60, HOUR = 3600, DAY = 86400, WEEK = 604800, YEAR = 31536000};

string toString(TimeUnit t) {
    switch (t) {
        case MINUTE: return "minute"; break;
        case HOUR: return "hour"; break;
        case DAY: return "day"; break;
        case WEEK: return "week"; break;
        case YEAR: return "year"; break;
    }
}

int getNumOldJobs(Db db, unsigned long cutoffTimeSeconds)
{
    string queryString = "select count(*) from job where startTime < " + to_string(cutoffTimeSeconds) + 
    " and status = 'R'" + ";";
    const char* query = queryString.c_str();
    try {
        Statement s = db.Query(query);
        s.Next();
        return s.GetField(0);
    } catch (sqdb::Exception& e) {
        cout << "DATABASE ERROR" << endl; //TODO: move to error output?
        return 0;
    }
}

//TODO: also set startTime?
void queueOldJobs(Db db, bool resetAttemptCounter, unsigned long cutoffTimeSeconds)
{
    string sqlSetStatus = "update job set status = 'Q' where startTime < " + to_string(cutoffTimeSeconds) + 
    " and status = 'R'" + ";";
    const char* updateStatus = sqlSetStatus.c_str();

    string sqlSetAttempts = "update job set attempts = 0 where startTime < " + to_string(cutoffTimeSeconds) + 
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
    // Check if database file exists
    std::ifstream infile("tmp.sqlite");
    if (!infile.good()) {
        cout << "Error: could not access database file." << endl;
        return 0;
    }

    Db database("tmp.sqlite"); //TODO: argument for selecting database?

    //getColumns(database, "job");



    // Defaults
    int cutoffTime = 24;
    bool resetCounters = true;
    bool shouldPlot = false;
    TimeUnit unit = DAY;
    string unitLabel = toString(unit) + "s";

    // Handle command line flags
    if (argc > 1) {
        if (strncmp(argv[1], "--help", 4) == 0) {
            cout << "Flags:" << endl;
            cout << "-p : show a plot of job start times" << endl;
            cout << "-t 'HOURS' : set the minimum age of jobs to search for (defaults to 24)" << endl;
            return 0;
        }

        for (unsigned int i = 1; i < argc; i++) { //TODO: gross, refactor
            bool invalid = false;
            if (strncmp(argv[i], "-p", 4) == 0) {
                shouldPlot = true;
            }
            else if (strncmp(argv[i], "-t", 4) == 0) {
                if (i + 1 >= argc) {
                    invalid = true;
                } else {
                    int time = atoi(argv[i + 1]);
                    if (time == 0) {
                        invalid = true;
                    } else {
                        cutoffTime = time;
                    }
                }
            }

            if (invalid) {
                cout << "Invalid argument(s). Try 'cleaner.out --help' for usage tips." << endl;
                return 0;
            }
        }
    }

    int cutoffTimeInSeconds = cutoffTime * unit;
    unsigned long currentTime = time(nullptr); //TODO: this could potentially break on non-UNIX systems 
    unsigned long oldestAllowableTime = currentTime - cutoffTimeInSeconds;

    int count = getNumOldJobs(database, oldestAllowableTime);
    if (count == 0) { 
        cout << "No jobs older than " << cutoffTime << " " << unitLabel << endl;
        return 0; 
    }
    if (shouldPlot) {
        vector<double> dataPoints;
        string queryString = "select startTime from job where startTime < " + to_string(oldestAllowableTime) + 
        " and status = 'R'" + ";";
        const char* query = queryString.c_str();
        try {
            Statement s = database.Query(query);
            while (s.Next()) {
                unsigned long startTime = s.GetField(0);
                unsigned long age_seconds = currentTime - startTime;
                dataPoints.push_back((double)age_seconds / (double)unit);
            }
            Grapher<double> grapher;
            grapher.setData(dataPoints);
            grapher.displayBarGraph("age (" + unitLabel + ")", "jobs", 10); //TODO: flag to set bins?

            double min = *min_element(dataPoints.begin(), dataPoints.end());
            double max = *max_element(dataPoints.begin(), dataPoints.end());

            cout << "Minimum job age: " << min << " " << unitLabel << endl;
            cout << "Maximum job age: " << max << " " << unitLabel << endl;
            cout << endl;
        } catch (sqdb::Exception& e) {
            cout << "DATABASE ERROR" << endl;
        }
    }
    cout << to_string(count) + " jobs older than " << cutoffTime << " " << unitLabel << " found." << endl;

    cout << "Queue these jobs? (Y/n) ";
    string response;
    cin >> response;
    if (response != "Y") { return 0; }
    cout << "Reset attempt counters? (Y/n) ";
    cin >> response;
    if (response == "Y") { resetCounters = true; }
    queueOldJobs(database, resetCounters, oldestAllowableTime);
    cout << "Complete!" << endl;

}
