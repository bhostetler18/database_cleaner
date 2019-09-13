#include "sqdb/sqdb.h"

using namespace std;
using namespace sqdb;

int getNumFailedJobs(Db db, unsigned long cutoffTime);

// Sets the status of all jobs older than the specified time to queued. 
//If resetAttemptCounter is true, this will also set these jobs' attempt count to 0.
void queueFailedJobs(Db db, bool resetAttemptCounter, unsigned long cutoffTime);
