#ifndef _PSphere_H_
#define _PSphere_H_

class PSphere
{
public:
	void create(Ogre::Real diameter, Ogre::Real seaFraction, Ogre::uint32 iters);

	void destroy();

	void pushToOgre(Ogre::ManualObject *manual);
	PSphere();

private:
	Ogre::Vector3		*vertexes;
	Ogre::Vector3		*vNorms;
	Ogre::ColourValue	*colours;
	Ogre::Vector2		*texCoords;
	Ogre::uint32		*indexes;
	Ogre::uint32		vertexCount;
	Ogre::uint32		indexCount;
	Ogre::Real			radius;
	unsigned char		*image;

	void calculate(Ogre::Vector3 vertex, Ogre::Real radius, Ogre::ColourValue colour);

	void fixTextureSeam();

	void deform(Ogre::Real seaFraction);

	void calculateNormals();

	Ogre::Vector3 calculateSphereCoordsFromTexCoords(Ogre::Vector2 *texCoord);

	Ogre::Real heightNoise(Ogre::uint32 octaves, Ogre::Real *amplitudes,
						   Ogre::Real *frequencys, Ogre::Vector3 Point);

	void generateImage(Ogre::uint32 octaves, Ogre::Real *amplitudes, Ogre::Real *frequencys,
					   Ogre::Real seaHeight, Ogre::Real top, Ogre::Real bottom);
};

#endif
