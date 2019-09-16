#include <algorithm>
#include <iostream>
#include "asciiGrapher.h"

Grapher::Grapher(int width, int height) {
	this->terminalWidth = width;
	this->graphHeight = height;
}

void Grapher::setData(vector<unsigned long> data) {
	this->data = data;
}

void Grapher::displayBarGraph(string xAxisLabel, string yAxisLabel, unsigned int bins) {
	unsigned long min = *min_element(data.begin(), data.end());
	unsigned long max = *max_element(data.begin(), data.end());
	unsigned long range = max - min;
	unsigned long binWidth = range / bins; //TODO: better division of bins
	vector<int> counts;
	for (int i = 0; i < bins; i++) {
		unsigned long binMin = min + i * binWidth;
		unsigned long binMax = min + (i + 1) * binWidth;
		bool inclusiveMax = (i == bins - 1);
		auto isInRange = [binMin, binMax, inclusiveMax](unsigned long val) { 
			return val >= binMin && (val < binMax || (inclusiveMax && val <= binMax));
		};
		int count = count_if(data.begin(), data.end(), isInRange);
		counts.push_back(count);
	}
	// Calculate bar heights
	int barWidth = terminalWidth / bins;
	int maxCount = *max_element(counts.begin(), counts.end());
	vector<int> barHeights;
	int scalingFactor = maxCount/this->graphHeight;
	for (auto count : counts) {
		int normalized = count / scalingFactor; //TODO: rounding errors
		barHeights.push_back(normalized);
	}

	int maxLength = 0;
	vector<string> yAxisLabels;
	for (int i = 0; i < graphHeight; i++) {
		string yLabel = to_string((graphHeight - i) * scalingFactor);
		int length = yLabel.size();
		yAxisLabels.push_back(yLabel);
		if (length > maxLength) {
			maxLength = length;
		}
	}

	for (int i = 0; i < yAxisLabels.size(); i++) {
		int size = yAxisLabels[i].size();
		if (size < maxLength) {
			yAxisLabels[i] = string(maxLength - size, ' ') + yAxisLabels[i];
		}
	}

	for (int row = 0; row < graphHeight; row++) {
		cout << yAxisLabels[row] << " ";
		for (int column = 0; column < barHeights.size(); column++) {
			char fillCharacter = ' ';
			if (barHeights[column] < this->graphHeight - row) {
				cout << string(barWidth, ' ');
				continue;
			} else if (barHeights[column] == this->graphHeight - row) {
				fillCharacter = '-';
			}
			string barSegment;
			if (barWidth == 1) {
				barSegment = "|";
			} else {
				string spaces(barWidth - 2, fillCharacter);
				barSegment = "|" + spaces + "|";
			}
			cout << barSegment;
		}
		cout << endl;
	}
	cout << string(terminalWidth + maxLength + 1, '-') << " " << xAxisLabel << endl << endl;
	unsigned long currentTime = time(nullptr);
	cout << "Minimum job age: " << float(currentTime - max) / 3600.0 << " hours" << endl;
	cout << "Maximum job age: " << float(currentTime - min) / 3600.0 << " hours" << endl;
	cout << endl;
}