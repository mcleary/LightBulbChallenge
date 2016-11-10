#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

auto GetFileSize(std::fstream Filestream)
{
	Filestream.seekg(0, std::ios::end);
	auto FileSize = Filestream.tellg();
	Filestream.seekg(0, std::ios::beg);
	return FileSize;
}

auto ReadCSVFile(const std::string& filepath, char Separator = ',')
{
	std::ifstream CSVStream{ filepath };	

	std::vector<std::vector<double>> CSVContents;
	
	std::string Line;
	while (std::getline(CSVStream, Line))
	{
		if (Line[0] == '#')
		{
			continue;
		}

		std::replace(Line.begin(), Line.end(), Separator, ' ');
		std::istringstream LineStream{ Line };
		std::vector<double> Tokens{ std::istream_iterator<double>{ LineStream }, std::istream_iterator<double>{} };

		CSVContents.emplace_back(Tokens);
	}

	return CSVContents;
}


inline double Lerp(double Alpha, double A, double B)
{
	return A + Alpha * (B - A);
}

inline double Interp(double W, int I, const std::vector<double>& L, const std::vector<double>& V)
{
	return Lerp((W - L[I]) / (L[I + 1] - L[I]), V[I], V[I + 1]);
}

inline double SegAvg(double WL0, double WL1, int I, const std::vector<double>& L, const std::vector<double>& V)
{
	return 0.5 * Interp(WL0, I, L, V) + Interp(WL1, I, L, V);
}

double AverageSpectrumSamples(const std::vector<double>& Lambda, const std::vector<double> Vals, int N, double LambdaStart, double LambdaEnd)
{
	if (LambdaEnd < Lambda[0])
	{
		return Vals[0];
	}
	if (LambdaStart >= Lambda[N - 1])
	{
		return Vals[N - 1];
	}
	if (N == 1)
	{
		return Vals[0];
	}

	double Sum = 0.0;

	if (LambdaStart < Lambda[0])
	{
		Sum += Vals[0] * (Lambda[0] - LambdaStart);
	}
	if (LambdaEnd > Lambda[N - 1])
	{
		Sum += Vals[N - 1] * (LambdaEnd - Lambda[N - 1]);
	}

	int Index = 0;
	while (LambdaStart > Lambda[Index + 1])
	{
		Index++;
	}

	while (Index + 1 < N && LambdaEnd > Lambda[Index])
	{
		double SegStart = std::max(LambdaStart, Lambda[Index]);
		double SegEnd = std::min(LambdaEnd, Lambda[Index + 1]);
		Sum += SegAvg(SegStart, SegEnd, Index, Lambda, Vals) * (SegEnd - SegStart);
		Index++;
	}

	return Sum / (LambdaEnd - LambdaStart);
}

auto MovingAverageFilter(const std::vector<double>& Values, int N = 10)
{
	std::vector<double> Result(Values.size());
	for (int i = 0; i < Values.size(); ++i)
	{
		double Sum = 0;
		int Count = 0;
		for (int j = i - N / 2; j < i + N / 2; ++j)
		{
			if (0 <= j && j < Values.size())
			{
				Sum += Values[j];
				Count++;
			}
		}
		Result[i] = Sum / static_cast<double>(Count);
	}
	return Result;
}

int main()
{
	const std::string IntensitiesFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\a\\intensities.csv";
	const std::string WavelengthsFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\a\\wavelengths.csv";
	const std::string ColorMatchingFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\a\\ciexyz31.csv";
	
	auto Intensities = ReadCSVFile(IntensitiesFilepath);
	auto Wavelengths = ReadCSVFile(WavelengthsFilepath);
	auto ColorMatching = ReadCSVFile(ColorMatchingFilepath);

	for (auto IntensityValues : Intensities)
	{

	}

    return 0;
}
