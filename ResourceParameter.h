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

class ResourceParameter
{
public:
    ResourceParameter(void);
    ResourceParameter(std::string terrainFirstColor, std::string terrainSecondColor,
                      std::string waterFirstColor, std::string waterSecondColor,
                      std::string mountainFirstColor, std::string mountainSecondColor,
                      float waterFraction, float radius, unsigned int seed,
                      std::string frequencyAmplitude,
                      std::vector <std::pair <std::string, int> > p_meshLocObjAmount);
    ~ResourceParameter(void);
    void getTerrainFirstColor(unsigned char &red,
                              unsigned char &green,
                              unsigned char &blue);
    void getTerrainSecondColor(unsigned char &red,
                               unsigned char &green,
                               unsigned char &blue);
    void getWaterFirstColor(unsigned char &red,
                            unsigned char &green,
                            unsigned char &blue);
    void getWaterSecondColor(unsigned char &red,
                             unsigned char &green,
                             unsigned char &blue);
    void getMountainFirstColor(unsigned char &red,
                               unsigned char &green,
                               unsigned char &blue);
    void getMountainSecondColor(unsigned char &red,
                                unsigned char &green,
                                unsigned char &blue);
    std::string getTerrainFirstColor(void);
    std::string getTerrainSecondColor(void);
    std::string getWaterFirstColor(void);
    std::string getWaterSecondColor(void);
    std::string getMountainFirstColor(void);
    std::string getMountainSecondColor(void);
    float getWaterFraction(void);
    float getRadius(void);
    unsigned int getSeed(void);
    std::vector <float>& getFrequency(void);
    std::vector <float>& getAmplitude(void);
    std::vector<std::pair <float, float> >& getFrequencyAmplitude(void);
    std::vector <std::string>& getMeshLocations(void);
    std::vector <int>& getObjectAmount(void);
    std::vector <std::pair <std::string, int> >& getMeshLocObjAmount(void);
    void setTerrainFirstColor(std::string);
    void setTerrainSecondColor(std::string);
    void setWaterFirstColor(std::string);
    void setWaterSecondColor(std::string);
    void setMountainFirstColor(std::string);
    void setMountainSecondColor(std::string);
    void setWaterFraction(float);
    void setRadius(float);
    void setSeed(unsigned int);
    void setFrequencyAmplitude(std::string NewfrequencyAmplitude, char delimiter);
    void setFrequencyAmplitude(float p_frequency, float p_amplitude);
    void setMeshLocation(std::string p_location);
    void setObjectAmount(int p_objAmount);
    void setMeshLocObjAmount(std::string p_location, int p_objAmount);
    void emptyFrequencyAmplitude();
    void emptyMeshLocObjAmount();
private:
    std::string terrainFirstColor;
    std::string terrainSecondColor;
    std::string waterFirstColor;
    std::string waterSecondColor;
    std::string mountainFirstColor;
    std::string mountainSecondColor;
    float waterFraction;
    float radius;
    unsigned int seed;
    std::vector <float> frequency;
    std::vector <float> amplitude;
    std::vector <std::pair <float, float> > frequencyAmplitude;
    unsigned char hexToBinary(const std::string& hexNumber);
    void splitToFrequencyAmplitude(const std::string& stringToSplit,
                                   char delimiter,
                                   std::vector<std::pair <float, float> >& tempVector);
    std::vector <std::string> meshLocation;
    std::vector <int> objectAmount;
    std::vector <std::pair <std::string, int> > meshLocObjAmount;
    void setFrequency(float);
    void setAmplitude(float);
};

#endif

