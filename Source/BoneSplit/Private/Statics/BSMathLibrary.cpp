// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Statics/BSMathLibrary.h"

FVector UBSMathLibrary::Slerp(const FVector& A, const FVector& B, const float T)
{
	const float Omega = 
		FGenericPlatformMath::Acos(FVector::DotProduct(A.GetSafeNormal(), B.GetSafeNormal()));
	
	const float SinOmega = FGenericPlatformMath::Sin(Omega);
	
	const FVector TermOne = A * (FGenericPlatformMath::Sin(Omega * (1.0 - T)) / SinOmega);
	const FVector TermTwo =B * (FGenericPlatformMath::Sin(Omega * T) / SinOmega);
	
	return TermOne + TermTwo;
}
