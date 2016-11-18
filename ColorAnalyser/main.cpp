#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "CommandLineParser.h"

#include "ColorUtils/ColorUtils.h"

auto ReadCSVFile(const std::string& filepath, char Separator = ',')
{
	std::ifstream CSVStream{ filepath };	

	if (!CSVStream.is_open())
	{
		throw std::runtime_error("Cannot open file " + filepath);
	}

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
		Result[i] = Sum / Count;
	}
	return Result;
}

int main(int ArgC, char* ArgV[])
{
	CommandLineParser CmdParser(ArgC, ArgV);

	if (!CmdParser.OptionExists("--intensities") &&
		!CmdParser.OptionExists("--wavelengths") &&
		!CmdParser.OptionExists("--color-matching"))
	{
		std::cout << "USAGE: " << ArgV[0] << "--intensities [CSV Filepath] --wavelengths [CSV Filepath] --color-matching [CSV Filepath]" << std::endl;
		return EXIT_FAILURE;
	}

	bool bPrintColorStats = CmdParser.OptionExists("--stats");

	auto IntensitiesFilepath = CmdParser.GetOptionValue("--intensities");
	auto WavelengthsFilepath = CmdParser.GetOptionValue("--wavelengths");
	auto ColorMatchingFilepath = CmdParser.GetOptionValue("--color-matching");

	try
	{
		auto Intensities = ReadCSVFile(IntensitiesFilepath);
		auto Wavelengths = ReadCSVFile(WavelengthsFilepath);
		auto ColorMatching = ReadCSVFile(ColorMatchingFilepath);

		std::vector<double> CIELambda(ColorMatching.size());
		std::vector<double> CIE_X(ColorMatching.size());
		std::vector<double> CIE_Y(ColorMatching.size());
		std::vector<double> CIE_Z(ColorMatching.size());

#		pragma omp parallel for
		for (int i = 0; i < ColorMatching.size(); ++i)
		{
			CIELambda[i] = ColorMatching[i][0];
			CIE_X[i] = ColorMatching[i][1];
			CIE_Y[i] = ColorMatching[i][2];
			CIE_Z[i] = ColorMatching[i][3];
		}
		ColorMatching.clear();
		ColorMatching.shrink_to_fit();

		std::vector<double> WavelenghtsValues(Wavelengths.size());

#		pragma omp parallel for
		for (int i = 0; i < Wavelengths.size(); ++i)
		{
			WavelenghtsValues[i] = Wavelengths[i][1];
		}

		// Remove Noise from data
		{
			decltype(Intensities) FilteredIntensities;
			for (auto IntensityValues : Intensities)
			{
				FilteredIntensities.emplace_back(MovingAverageFilter(IntensityValues));
			}
			Intensities.swap(FilteredIntensities);
		}

		// Define the visible spectrum
		const double SampledLambdaStart = 340;
		const double SampledLambdaEnd = 840;

		std::vector<xyYColor> Result{ Intensities.size() };

		// Finally process the samples
#		pragma omp parallel for
		for (int SampleIdx = 0; SampleIdx < Intensities.size(); ++SampleIdx)
		{
			const auto& SampleData = Intensities[SampleIdx];
			auto NumberOfSpectralSamples = static_cast<int>(SampleData.size());
			auto NumberOfCIESamples = static_cast<int>(CIELambda.size());

			std::vector<double> X, Y, Z, Lambdas;

			// For each sample do an a linear interpolation on the function to calculate the XY coordinates
			for (int i = 0; i < NumberOfSpectralSamples; ++i)
			{
				auto di = static_cast<double>(i);
				auto WL0 = Lerp(di / NumberOfSpectralSamples, SampledLambdaStart, SampledLambdaEnd);
				auto WL1 = Lerp(di + 1.0 / NumberOfSpectralSamples, SampledLambdaStart, SampledLambdaEnd);

				X.push_back(AverageSpectrumSamples(CIELambda, CIE_X, NumberOfCIESamples, WL0, WL1));
				Y.push_back(AverageSpectrumSamples(CIELambda, CIE_Y, NumberOfCIESamples, WL0, WL1));
				Z.push_back(AverageSpectrumSamples(CIELambda, CIE_Z, NumberOfCIESamples, WL0, WL1));

				Lambdas.push_back(AverageSpectrumSamples(WavelenghtsValues, SampleData, NumberOfSpectralSamples, WL0, WL1));
			}

			// Finally we calculate the chromatic response of the observer ...
			double XSum = 0.0;
			double YSum = 0.0;
			double ZSum = 0.0;
			for (int i = 0; i < NumberOfSpectralSamples; ++i)
			{
				XSum += X[i] * Lambdas[i];
				YSum += Y[i] * Lambdas[i];
				ZSum += Z[i] * Lambdas[i];
			}

			// .. and then we normalize the tristimulus values to get the XY coordinates of the CIE1931 diagram			

			Result[SampleIdx] = XYZToxyY({ XSum, YSum, ZSum });
		}

		for (int i = 0; i < Result.size(); ++i)
		{
			std::cout << Result[i].x << " " << Result[i].y << " " << Result[i].Y << std::endl;
		}		

		if (bPrintColorStats)
		{
			xyYColorStatistics ColorStats{ Result.data(), static_cast<int>(Result.size()) };
			std::cout << "Avg        : x: " << ColorStats.Average.x   << " y: " << ColorStats.Average.y   << std::endl;
			std::cout << "StdDev     : x: " << ColorStats.StdDev.x    << " y: " << ColorStats.StdDev.y    << std::endl;
			std::cout << "Rel. StdDev: x: " << ColorStats.RelStdDev.x << " y: " << ColorStats.RelStdDev.y << std::endl;
			std::cout << "Distance to White: " << ColorStats.DistanceToWhite << std::endl;
		}
	}
	catch (const std::runtime_error& Exception)
	{
		std::cerr << Exception.what() << std::endl;
	}

    return 0;
}
