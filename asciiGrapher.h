#include <vector>
#include <string>

using namespace std;

class Grapher {
public:
	Grapher(int width = 80);
	void displayBarGraph(string xAxisLabel, string yAxisLabel, unsigned int bins = 10);
	void setData(vector<unsigned long>); //TODO: template to take any numeric type

private:
	int terminalWidth;
	vector<unsigned long> data;
};

