#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>


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
		Result[i] = Sum / Count;
	}
	return Result;
}

void NormalizeData(std::vector<double>& Data)
{
	auto MinMaxPair = std::minmax_element(Data.begin(), Data.end());
	auto Min = *MinMaxPair.first;
	auto Max = *MinMaxPair.second;

#	pragma omp parallel for
	for (int i = 0; i < Data.size(); ++i)
	{
		Data[i] = (Data[i] - Min) / (Max - Min);
	}
}

int main()
{
	const std::string IntensitiesFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\a\\intensities.csv";
	const std::string WavelengthsFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\a\\wavelengths.csv";
	const std::string ColorMatchingFilepath = "D:\\Dropbox\\Documentos\\Curriculos\\Thalmic Labs\\Vanhackaton Challenge\\challenge data\\ciexyz31.csv";
	
	auto Intensities = ReadCSVFile(IntensitiesFilepath);
	auto Wavelengths = ReadCSVFile(WavelengthsFilepath);
	auto ColorMatching = ReadCSVFile(ColorMatchingFilepath);

	std::vector<double> CIELambda(ColorMatching.size());
	std::vector<double> CIE_X(ColorMatching.size());
	std::vector<double> CIE_Y(ColorMatching.size());
	std::vector<double> CIE_Z(ColorMatching.size());

#	pragma omp parallel for
	for (int i = 0; i < ColorMatching.size(); ++i)
	{
		CIELambda[i] = ColorMatching[i][0];
		CIE_X[i] = ColorMatching[i][1];
		CIE_Y[i] = ColorMatching[i][2];
		CIE_Z[i] = ColorMatching[i][3];
	}
	ColorMatching.clear();
	ColorMatching.shrink_to_fit();

	std::vector<double> WaveLengthsValues(Wavelengths.size());

#	pragma omp parallel for
	for (int i = 0; i < Wavelengths.size(); ++i)
	{
		WaveLengthsValues[i] = Wavelengths[i][1];
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
	
	// Normalize the Color Matching function
	NormalizeData(CIE_X);
	NormalizeData(CIE_Y);
	NormalizeData(CIE_Z);	

	double SampledLambdaStart = 340;
	double SampledLambdaEnd = 840;	

	std::vector<std::pair<double, double>> Result{ Intensities.size() };

#	pragma omp parallel for
	for (int SampleIdx = 0; SampleIdx < Intensities.size(); ++SampleIdx)
	{
#		pragma omp critical
		std::cout << "Sample " << SampleIdx << " of " << Intensities.size() << std::endl;

		const auto& SampleData = Intensities[SampleIdx];
		auto NumberOfSpectralSamples = static_cast<double>(SampleData.size());
		int NumberOfCIESamples = CIELambda.size();

		std::vector<double> X, Y, Z, SampleDataC;

		for (int i = 0; i < NumberOfSpectralSamples; ++i)
		{
			auto di = static_cast<double>(i);
			auto WL0 = Lerp(di / NumberOfSpectralSamples, SampledLambdaStart, SampledLambdaEnd);
			auto WL1 = Lerp(di + 1.0 / NumberOfSpectralSamples, SampledLambdaStart, SampledLambdaEnd);

			X.push_back(AverageSpectrumSamples(CIELambda, CIE_X, NumberOfCIESamples, WL0, WL1));
			Y.push_back(AverageSpectrumSamples(CIELambda, CIE_Y, NumberOfCIESamples, WL0, WL1));
			Z.push_back(AverageSpectrumSamples(CIELambda, CIE_Z, NumberOfCIESamples, WL0, WL1));
			SampleDataC.push_back(AverageSpectrumSamples(WaveLengthsValues, SampleData, NumberOfSpectralSamples, WL0, WL1));
		}

		double XSum = 0.0;
		double YSum = 0.0;
		double ZSum = 0.0;
		for (int i = 0; i < NumberOfSpectralSamples; ++i)
		{
			XSum += X[i] * SampleDataC[i];
			YSum += Y[i] * SampleDataC[i];
			ZSum += Z[i] * SampleDataC[i];
		}

		double CIE_XYZ_X = XSum / (XSum + YSum + ZSum);
		double CIE_XYZ_Y = YSum / (XSum + YSum + ZSum);

		Result[SampleIdx] = { CIE_XYZ_X, CIE_XYZ_Y };
	}

	for (int i = 0; i < Result.size(); ++i) 
	{
		std::cout << Result[i].first << " " << Result[i].second << std::endl;
	}

    return 0;
}
