#include "BasicMath.h"

basicMath::basicMath()
{

}
basicMath::~basicMath()
{

}
float basicMath::dot(XMFLOAT3 a, XMFLOAT3 b)
{
	float result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}
float basicMath::VectorLength(XMFLOAT3 Vector)
{
	float Length = sqrt(Vector.x*Vector.x + Vector.y*Vector.y + Vector.z*Vector.z);
	return Length;
	
}
XMFLOAT3 basicMath::Cross(XMFLOAT3 VectorA, XMFLOAT3 VectorB)
{	
	XMFLOAT3 VectorC;
	VectorC.x = VectorA.y*VectorB.z - VectorA.z*VectorB.y;
	VectorC.y = VectorA.z*VectorB.x - VectorA.x*VectorB.z;
	VectorC.z = VectorA.x*VectorB.y - VectorA.y*VectorB.x;

	return VectorC;
}
XMFLOAT3 basicMath::Normalize(XMFLOAT3 VectorA)
{
	XMFLOAT3 VectorANormalized;
	float ALength = VectorLength(VectorA);


	VectorANormalized.x = VectorA.x / ALength;
	VectorANormalized.y = VectorA.y / ALength;
	VectorANormalized.z = VectorA.z / ALength;

	return VectorANormalized;
}