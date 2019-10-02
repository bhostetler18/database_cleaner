#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

template<typename T>
class Grapher {
public:
	Grapher(int width = 60);
	void displayBarGraph(string xUnit, string yUnit, unsigned int bins = 10, 
		unsigned int* lowerBound = nullptr, 
		unsigned int* upperBound = nullptr) const;
	void setData(vector<T>); //TODO: template to take any numeric type

private:
	int terminalWidth; //TODO: this is used as max bar width, and graph could extend beyond due to labels
	vector<T> data;
};

template<typename T>
Grapher<T>::Grapher(int width) {
	this->terminalWidth = width;
}

template<typename T>
void Grapher<T>::setData(vector<T> data) {
	this->data = data;
}

template<typename T>
void Grapher<T>::displayBarGraph(string xUnit, string yUnit, unsigned int bins, unsigned int* lowerBound, unsigned int* upperBound) const {
	unsigned long min = *min_element(data.begin(), data.end());
	unsigned long max = *max_element(data.begin(), data.end());
	unsigned long range = max - min;
	unsigned long binWidth = range / bins; //TODO: better division of bins?
	vector<int> counts;
	vector<string> rangeLabels;
	int maxLabelWidth = 0;
	for (unsigned int i = 0; i < bins; i++) {
		unsigned long binMin = min + i * binWidth;
		unsigned long binMax = min + (i + 1) * binWidth;
		bool inclusiveMax = (i == bins - 1);
		auto isInRange = [binMin, binMax, inclusiveMax](unsigned long val) { 
			return val >= binMin && (val < binMax || (inclusiveMax && val <= binMax));
		};
		int count = count_if(data.begin(), data.end(), isInRange);
		counts.push_back(count);
		string separator = (i == bins - 1) ? " - " : " - <";
		//TODO: display decimals to avoid something like 24 - <24
		string label = to_string(binMin / 3600) + separator + to_string(binMax / 3600) + ": ";
		if (label.size() > maxLabelWidth) {
			maxLabelWidth = label.size();
		}
		rangeLabels.push_back(label);
	}
	// Calculate bar heights
	int maxCount = *max_element(counts.begin(), counts.end());
	vector<int> barHeights;
	float scalingFactor = float(this->terminalWidth)/float(maxCount);

	for (auto count : counts) {
		int normalized = count * scalingFactor; //TODO: rounding errors
		barHeights.push_back(normalized);
	}

	cout << endl << "Number of " + yUnit + " by " + xUnit << endl;
	for (unsigned int bar = 0; bar < barHeights.size(); bar++) {
		if (rangeLabels[bar].size() < maxLabelWidth) {
			cout << string(maxLabelWidth - rangeLabels[bar].size(), ' ');
		}
		cout << rangeLabels[bar];
		for (int i = 0; i < barHeights[bar]; i++) {
			cout << "█"; //TODO: use fractional bars for better accuracy?
		}
		if (barHeights[bar] != 0) {
			cout << " (" << counts[bar] << " " << yUnit << ")";
		}
		cout << endl;
	}
	cout << endl;
}
