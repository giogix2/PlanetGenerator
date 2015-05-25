/* The MIT License (MIT)
 * 
 * Copyright (c) 2015 Giovanni Ortolani, Taneli Mikkonen, Pingjiang Li, Tommi Puolamaa, Mitra Vahida
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. */

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
            , float waterFraction, float radius, unsigned int seed, string frequencyAmplitude, vector <pair <string, int> > p_meshLocObjAmount);
		~ResourceParameter(void);
		void getTerrainFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getTerrainSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getWaterFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getWaterSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getMountainFirstColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		void getMountainSecondColor(unsigned char &red, unsigned char &green, unsigned char &blue);
		string getTerrainFirstColor(void);
		string getTerrainSecondColor(void);
		string getWaterFirstColor(void);
		string getWaterSecondColor(void);
		string getMountainFirstColor(void);
		string getMountainSecondColor(void);
		float getWaterFraction(void);
		float getRadius(void);
        unsigned int getSeed(void);
		vector <float>& getFrequency(void);
		vector <float>& getAmplitude(void);
		vector<pair <float, float> >& getFrequencyAmplitude(void);
        vector <string>& getMeshLocations(void);
        vector <int>& getObjectAmount(void);
        vector <pair <string, int> >& getMeshLocObjAmount(void);
		void setTerrainFirstColor(string);
		void setTerrainSecondColor(string);
		void setWaterFirstColor(string);
		void setWaterSecondColor(string);
		void setMountainFirstColor(string);
		void setMountainSecondColor(string);
		void setWaterFraction(float);
		void setRadius(float);
        void setSeed(unsigned int);
		void setFrequencyAmplitude(string NewfrequencyAmplitude, char delimiter);
		void setFrequencyAmplitude(float p_frequency, float p_amplitude);
        void setMeshLocation(string p_location);
        void setObjectAmount(int p_objAmount);
        void setMeshLocObjAmount(string p_location, int p_objAmount);
        void emptyFrequencyAmplitude();
        void emptyMeshLocObjAmount();
	private:
		string terrainFirstColor;
		string terrainSecondColor;
		string waterFirstColor;
		string waterSecondColor;
		string mountainFirstColor;
		string mountainSecondColor;
		float waterFraction;
		float radius;
        unsigned int seed;
		vector <float> frequency;
		vector <float> amplitude;
		vector <pair <float, float> > frequencyAmplitude;
		unsigned char hexToBinary(const string& hexNumber);
		void splitToFrequencyAmplitude(const string& stringToSplit, char delimiter, vector<pair <float, float> >& tempVector);
        vector <string> meshLocation;
        vector <int> objectAmount;
        vector <pair <string, int> > meshLocObjAmount;
		void setFrequency(float);
		void setAmplitude(float);
	};
}
#endif

