#pragma once
#include <vector>

#ifndef ResourceParameter_H
#define ResourceParameter_H

#include <string>

namespace std
{
	class ResourceParameter
	{
	public:
		ResourceParameter(void);
		ResourceParameter(string terrainFirstColor,string terrainSecondColor, string waterFirstColor
			, string waterSecondColor, string mountainFirstColor, string mountainSecondColor
			, float waterFraction, float radius, int seed, vector <float> frequency, vector <float> amplitude);
		~ResourceParameter(void);
		void getTerrainFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getTerrainSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getWaterFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getWaterSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getMountainFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getMountainSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		float getWaterFraction(void);
		float getRadius(void);
		int getSeed(void);
		vector <float> getFrequency(void);
		vector <float> getAmplitude(void);
        vector<pair <float, float> > getFrequencyAmplitude(void);
		void setTerrainFirstColor(string);
		void setTerrainSecondColor(string);
		void setWaterFirstColor(string);
		void setWaterSecondColor(string);
		void setMountainFirstColor(string);
		void setMountainSecondColor(string);
		void setWaterFraction(float);
		void setRadius(float);
		void setSeed(int);
		void setFrequency(float);
		void setAmplitude(float);
		void setFrequencyAmplitude(string NewfrequencyAmplitude, char delimiter);
	private:
		string terrainFirstColor;
		string terrainSecondColor;
		string waterFirstColor;
		string waterSecondColor;
		string mountainFirstColor;
		string mountainSecondColor;
		float waterFraction;
		float radius;
		int seed;
		vector <float> frequency;
		vector <float> amplitude;
        vector <pair <float, float> > frequencyAmplitude;
		unsigned char hexToBinary(const string& hexNumber);
        void splitToFrequencyAmplitude(const string& stringToSplit, char delimiter, vector<pair <float, float> >& tempVector);
	};
}
#endif

