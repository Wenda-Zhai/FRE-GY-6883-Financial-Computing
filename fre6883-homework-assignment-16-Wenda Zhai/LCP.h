#pragma once
#include "ParabPDE.h"
class LCP
{
public:
ParabPDE* PtrPDE;
virtual double g(double t,double x)=0;
};