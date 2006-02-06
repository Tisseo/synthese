#include "Geometry.h"

#include <cmath>
#include <iostream>

#include "XYPoint.h"


using namespace std;

namespace synmap
{





double calculateAngle (const XYPoint& a, const XYPoint& b, const XYPoint& c)
{
	double angle = atan2 (c.getY()-b.getY(), c.getX()-b.getX()) - atan2 (a.getY()-b.getY(), a.getX()-b.getX());

	if (angle > M_PI) angle = angle - 2*M_PI;
	if (angle < -M_PI) angle = angle + 2*M_PI;
	
	return angle;	
}



double calculateDistance (const XYPoint& a, const XYPoint& b) 
{
	return sqrt((b.getX()-a.getX())*(b.getX()-a.getX()) + (b.getY()-a.getY())*(b.getY()-a.getY()));
}


double toDegrees (double angle)
{
	return 180.0 * angle / M_PI;
}


XYPoint  
calculateSymetric (const XYPoint& a, const XYPoint& b)
{
	// Symetric of a regarding b
	double deltax = b.getX() - a.getX();
	double deltay = b.getY() - a.getY();
	return XYPoint (b.getX() + deltax, b.getY() + deltay);
} 




XYPoint 
calculateIntersection (double a1, double b1, double a2, double b2)
{
	// Intersection of :
	// y = a1.x + b1
	// y = a2.x + b2
	
	if (a1 == a2) throw "No intersection";
	
	return XYPoint (
					(b2 - b1) / (a1 -a2),
					(b2*a1 - a2*b1)	/ (a1 - a2)
					);
}



std::pair<double, double>
calculateAffineEquation (const XYPoint& p0, const XYPoint& p1) {
	// Given 2 points return a and b so that y = a.x + b
	// goes through p0 and p1
	double u0 = p1.getX() - p0.getX ();
	double v0 = p1.getY() - p0.getY ();

	double a = v0 / u0;
	double b = p0.getY () - a * p0.getX ();
	
	return std::pair<double, double> (a,b);	
}





XYPoint 
calculateIntersection (const XYPoint& p0, const XYPoint& p1, 
					   const XYPoint& p2, const XYPoint& p3)
{
	double u0 = p1.getX() - p0.getX ();
	double v0 = p1.getY() - p0.getY ();
	
	double u1 = p3.getX() - p2.getX ();
	double v1 = p3.getY() - p2.getY ();

	double a1 = v0 / u0;
	double b1 = p0.getY () - a1 * p0.getX ();
	
	double a2 = v1 / u1;
	double b2 = p2.getY () - a2 * p2.getX ();

	return 	calculateIntersection (a1, b1, a2, b2);
}




}









