#include "sqdb/sqdb.h"

using namespace std;
using namespace sqdb;

int getNumOldJobs(Db db, unsigned long cutoffTimeSeconds);

// Sets the status of all jobs older than the specified time to queued. 
//If resetAttemptCounter is true, this will also set these jobs' attempt count to 0.
void queueOldJobs(Db db, bool resetAttemptCounter, unsigned long cutoffTimeSeconds);
