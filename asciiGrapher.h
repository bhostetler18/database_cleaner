#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

template<typename T>
class Grapher {
public:
	Grapher(int width = 60);
	void displayBarGraph(string xUnit, string yUnit, unsigned int bins = 10, T lowerBound = 0, T upperBound = 0) const;
	void setData(vector<T>);

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
void Grapher<T>::displayBarGraph(string xUnit, string yUnit, unsigned int bins, T lowerBound, T upperBound) const {
	T min = *min_element(data.begin(), data.end());
	T max = *max_element(data.begin(), data.end());

	if (lowerBound != 0 && upperBound != 0) {
		if (lowerBound < min || upperBound > max || lowerBound > upperBound) {
			cout << "FAILED TO SET RANGE, USING DEFAULT." << endl; //TODO: use cerr?
		}
		else {
			min = (T)lowerBound;
			max = (T)upperBound;
		}
	}

	T range = max - min;
	double binWidth = (double)range / (double)bins; //TODO: better division of bins, especially if T is integral
	vector<int> counts;
	vector<string> rangeLabels;
	int maxLabelWidth = 0;
	string separator = " - ";
	for (unsigned int i = 0; i < bins; i++) {
		double binMin = min + i * binWidth;
		double binMax = min + (i + 1) * binWidth;
		bool inclusiveMin = (i == 0);
		auto isInRange = [binMin, binMax, inclusiveMin](T val) { 
			double decimalVal = (double)val;
			return decimalVal <= binMax && (decimalVal > binMin || (inclusiveMin && decimalVal >= binMin));
		};
		int count = count_if(data.begin(), data.end(), isInRange);
		counts.push_back(count);
		stringstream ss;
		ss.precision(2);
		ss << fixed;
		ss << binMin;
		string labelMin = ss.str();
		ss.str("");
		ss << binMax;
		string labelMax = ss.str();
		string label = labelMin + separator + labelMax + ": ";
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

	//TODO: summary statistics here
}
