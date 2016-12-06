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

#include "ResourceParameter.h"
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

ResourceParameter::ResourceParameter(void)
{
    /*waterFraction = 0.0;
        radius = 0.0;
        seed = 0;*/
}
ResourceParameter::ResourceParameter(string newTerrainFirstColor,string newTerrainSecondColor,
                                     string newWaterFirstColor, string newWaterSecondColor,
                                     string newMountainFirstColor, string newMountainSecondColor,
                                     float newWaterFraction, float newRadius, unsigned int newSeed,
                                     string newFrequencyAmplitude,
                                     vector <pair <string, int> > p_meshLocObjAmount)
{
    terrainFirstColor = newTerrainFirstColor;
    waterFirstColor = newWaterFirstColor;
    mountainFirstColor = newMountainFirstColor;
    terrainSecondColor = newTerrainSecondColor;
    waterSecondColor = newWaterSecondColor;
    mountainSecondColor = newMountainSecondColor;
    waterFraction = newWaterFraction;
    radius = newRadius;
    seed = newSeed;
    setFrequencyAmplitude(newFrequencyAmplitude,' ');
    meshLocObjAmount = p_meshLocObjAmount;
}
ResourceParameter::~ResourceParameter(void)
{
}
void ResourceParameter::getTerrainFirstColor(unsigned char &red,
                                             unsigned char &green,
                                             unsigned char &blue)
{
    terrainFirstColor.erase(remove(terrainFirstColor.begin(), terrainFirstColor.end(), '#'),
                            terrainFirstColor.end());
    red = hexToBinary(terrainFirstColor.substr (0,2));
    green = hexToBinary(terrainFirstColor.substr (2,2));
    blue = hexToBinary(terrainFirstColor.substr (4,2));
}
void ResourceParameter::getTerrainSecondColor(unsigned char &red,
                                              unsigned char &green,
                                              unsigned char &blue)
{
    terrainSecondColor.erase(remove(terrainSecondColor.begin(), terrainSecondColor.end(), '#'),
                             terrainSecondColor.end());
    red = hexToBinary(terrainSecondColor.substr (0,2));
    green = hexToBinary(terrainSecondColor.substr (2,2));
    blue = hexToBinary(terrainSecondColor.substr (4,2));
}
void ResourceParameter::getWaterFirstColor(unsigned char &red,
                                           unsigned char &green,
                                           unsigned char &blue)
{
    waterFirstColor.erase(remove(waterFirstColor.begin(), waterFirstColor.end(), '#'),
                          waterFirstColor.end());
    red = hexToBinary(waterFirstColor.substr (0,2));
    green = hexToBinary(waterFirstColor.substr (2,2));
    blue = hexToBinary(waterFirstColor.substr (4,2));
}
void ResourceParameter::getWaterSecondColor(unsigned char &red,
                                            unsigned char &green,
                                            unsigned char &blue)
{
    waterSecondColor.erase(remove(waterSecondColor.begin(), waterSecondColor.end(), '#'),
                           waterSecondColor.end());
    red = hexToBinary(waterSecondColor.substr (0,2));
    green = hexToBinary(waterSecondColor.substr (2,2));
    blue = hexToBinary(waterSecondColor.substr (4,2));
}
void ResourceParameter::getMountainFirstColor(unsigned char &red,
                                              unsigned char &green,
                                              unsigned char &blue)
{
    mountainFirstColor.erase(remove(mountainFirstColor.begin(), mountainFirstColor.end(), '#'),
                             mountainFirstColor.end());
    red = hexToBinary(mountainFirstColor.substr (0,2));
    green = hexToBinary(mountainFirstColor.substr (2,2));
    blue = hexToBinary(mountainFirstColor.substr (4,2));
}
void ResourceParameter::getMountainSecondColor(unsigned char &red,
                                               unsigned char &green,
                                               unsigned char &blue)
{
    mountainSecondColor.erase(remove(mountainSecondColor.begin(), mountainSecondColor.end(), '#'),
                              mountainSecondColor.end());
    red = hexToBinary(mountainSecondColor.substr (0,2));
    green = hexToBinary(mountainSecondColor.substr (2,2));
    blue = hexToBinary(mountainSecondColor.substr (4,2));
}
string ResourceParameter::getTerrainFirstColor(void)
{
    return terrainFirstColor;
}
string ResourceParameter::getTerrainSecondColor(void)
{
    return terrainSecondColor;
}
string ResourceParameter::getWaterFirstColor(void)
{
    return waterFirstColor;
}
string ResourceParameter::getWaterSecondColor(void)
{
    return waterSecondColor;
}
string ResourceParameter::getMountainFirstColor(void)
{
    return mountainFirstColor;
}
string ResourceParameter::getMountainSecondColor(void)
{
    return mountainSecondColor;
}
float ResourceParameter::getWaterFraction(void)
{
    return waterFraction;
}
float ResourceParameter::getRadius(void)
{
    return radius;
}
unsigned int ResourceParameter::getSeed(void)
{
    return seed;
}
vector <float>& ResourceParameter::getFrequency(void)
{
    return frequency;
}
vector <float>& ResourceParameter::getAmplitude(void)
{
    return amplitude;
}
vector<pair <float, float> >& ResourceParameter::getFrequencyAmplitude(void)
{
    return frequencyAmplitude;
}
vector <string>& ResourceParameter::getMeshLocations(void)
{
    return meshLocation;
}
vector <int>& ResourceParameter::getObjectAmount(void)
{
    return objectAmount;
}
vector <pair <string, int> >& ResourceParameter::getMeshLocObjAmount(void)
{
    return meshLocObjAmount;
}
void ResourceParameter::setTerrainFirstColor(string newTerrainFirstColor)
{
    terrainFirstColor = newTerrainFirstColor;
}
void ResourceParameter::setTerrainSecondColor(string newTerrainSecondColor)
{
    terrainSecondColor = newTerrainSecondColor;
}
void ResourceParameter::setWaterFirstColor(string newWaterFirstColor)
{
    waterFirstColor = newWaterFirstColor;
}
void ResourceParameter::setWaterSecondColor(string newWaterSecondColor)
{
    waterSecondColor = newWaterSecondColor;
}
void ResourceParameter::setMountainFirstColor(string newMountainFirstColor)
{
    mountainFirstColor = newMountainFirstColor;
}
void ResourceParameter::setMountainSecondColor(string newMountainSecondColor)
{
    mountainSecondColor = newMountainSecondColor;
}
void ResourceParameter::setWaterFraction(float newWaterFraction)
{
    waterFraction = newWaterFraction < 0 ? 0 : newWaterFraction > 1 ? 1 : newWaterFraction;
}
void ResourceParameter::setRadius(float newRadius)
{
    radius = newRadius < 0 ? 1 : newRadius;
}
void ResourceParameter::setSeed(unsigned int newSeed)
{
    seed = newSeed;
}
void ResourceParameter::setFrequency(float newFrequency)
{
    newFrequency = newFrequency < 0 ? 0 : newFrequency;
    frequency.push_back(newFrequency);
}
void ResourceParameter::setAmplitude(float newAmplitude)
{
    newAmplitude = newAmplitude < 0 ? 0 : newAmplitude;
    amplitude.push_back(newAmplitude);
}
void ResourceParameter::setFrequencyAmplitude(string NewfrequencyAmplitude, char delimiter)
{
    splitToFrequencyAmplitude(NewfrequencyAmplitude, delimiter, frequencyAmplitude);
}
void ResourceParameter::setFrequencyAmplitude(float p_frequency, float p_amplitude)
{
    p_frequency = p_frequency < 0 ? 0 : p_frequency;
    p_amplitude = p_amplitude < 0 ? 0 : p_amplitude;
    setFrequency(p_frequency);
    setAmplitude(p_amplitude);
    frequencyAmplitude.push_back(make_pair(p_frequency, p_amplitude));
}
void ResourceParameter::setMeshLocation(string p_location)
{
    meshLocation.push_back(p_location);
}
void ResourceParameter::setObjectAmount(int p_objAmount)
{
    objectAmount.push_back(p_objAmount);
}
void ResourceParameter::setMeshLocObjAmount(string p_location, int p_objAmount)
{
    meshLocObjAmount.push_back(make_pair(p_location, p_objAmount));
}
unsigned char ResourceParameter::hexToBinary(const string& hexNumber)
{
    stringstream stringstream1;
    stringstream1 << hex << hexNumber;
    int number;
    stringstream1 >> number;
    unsigned char color = number;
    return color;
}
/* if the number of the pairs it not even, the function will ignore the last one
 * to have pair values. the first value is frequency and the second is amplitude.*/
void ResourceParameter::splitToFrequencyAmplitude(const string& stringToSplit,
                                                  char delimiter,
                                                  vector<pair <float, float> >& tempVector)
{
    string::size_type i = 0;
    string::size_type j = stringToSplit.find(delimiter);
    std::pair<float ,float> pair;
    float frequency,amplitude;
    string stringTemp1,stringTemp2;
    while (j != string::npos)
    {
        stringTemp1= stringToSplit.substr(i, j-i);
        i = ++j;
        j = stringToSplit.find(delimiter, j);
        if (j != string::npos)
        {
            stringTemp2 = stringToSplit.substr(i, j-i);
            i = ++j;
            j = stringToSplit.find(delimiter, j);
        }
        else if(j == string::npos)
        {
            stringTemp2 = stringToSplit.substr(i, stringToSplit.length());
        }
        /* std::atof(string.c_str()) returned zero with perfectly good string.
         * Using istringstream instead. */
        istringstream convStream1(stringTemp1);
        convStream1 >> frequency;
        istringstream convStream2(stringTemp2);
        convStream2 >> amplitude;

        //control the range
        frequency = frequency < 0 ? 0 : frequency;
        amplitude = amplitude < 0 ? 0 : amplitude;

        setFrequency(frequency);
        setAmplitude(amplitude);
        pair = std::make_pair(frequency,amplitude);
        tempVector.push_back(pair);
    }
}
//empty the vector
void ResourceParameter::emptyFrequencyAmplitude()
{
    frequencyAmplitude.clear();
    frequency.clear();
    amplitude.clear();
}
void ResourceParameter::emptyMeshLocObjAmount()
{
    meshLocObjAmount.clear();
    meshLocation.clear();
    objectAmount.clear();
}
