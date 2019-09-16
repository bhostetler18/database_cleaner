#include <vector>
#include <string>

using namespace std;

class Grapher {
public:
	Grapher(int width = 80);
	void displayBarGraph(string xAxisUnit, string yAxisUnit, unsigned int bins = 10);
	void setData(vector<unsigned long>); //TODO: template to take any numeric type

private:
	int terminalWidth; //TODO: this is used as max bar width, and graph could extend beyond due to labels
	vector<unsigned long> data;
};

