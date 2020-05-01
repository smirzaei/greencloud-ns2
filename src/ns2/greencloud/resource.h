/*
 * resource.h
 *
 *  @date Mar 8, 2013
 *  @author Guzek:Mateusz
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <stdlib.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <algorithm>


/*Update the last element if a new position is added to enum*/
enum res_type {
	Computing,
	Memory,
	Storage,
	Networking,
	FirstResType = Computing,
	LastResType = Networking
};

class Capacity{
public:
	Capacity();
	Capacity(int i);
	Capacity(double d);
	double value;
	std::vector<Capacity*> virtual_capacities;
	Capacity& operator=(const double& val);
	Capacity& operator=(const Capacity& c);
	operator double();
	Capacity& operator+=(const Capacity& rhs);
	Capacity& operator-=(const Capacity& rhs);
	Capacity& operator+=(const double& rhs);
	Capacity& operator-=(const double& rhs);

	double getValueRecursive();


};
inline bool operator==(const Capacity& lhs, const Capacity& rhs){ return (lhs.value ==rhs.value);}
inline bool operator!=(const Capacity& lhs, const Capacity& rhs){return !operator==(lhs,rhs);}
inline bool operator< (const Capacity& lhs, const Capacity& rhs){ return (lhs.value < rhs.value);}
inline bool operator> (const Capacity& lhs, const Capacity& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const Capacity& lhs, const Capacity& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const Capacity& lhs, const Capacity& rhs){return !operator< (lhs,rhs);}

inline Capacity operator+(Capacity lhs, const Capacity& rhs)
{
  lhs += rhs;
  return lhs;
}
inline Capacity operator-(Capacity lhs, const Capacity& rhs)
{
  lhs -= rhs;
  return lhs;
}
inline bool operator==(const Capacity& lhs, const double& rhs){ return (lhs.value ==rhs);}
inline bool operator!=(const Capacity& lhs, const double& rhs){return !operator==(lhs,rhs);}
inline bool operator< (const Capacity& lhs, const double& rhs){ return (lhs.value < rhs);}
inline bool operator> (const Capacity& lhs, const double& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const Capacity& lhs, const double& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const Capacity& lhs, const double& rhs){return !operator< (lhs,rhs);}

inline Capacity operator+(Capacity lhs, const double& rhs)
{
  lhs += rhs;
  return lhs;
}
inline Capacity operator-(Capacity lhs, const double& rhs)
{
  lhs -= rhs;
  return lhs;
}

class Resource {
public:
	Resource(res_type t,double a ,std::vector <Capacity> cap);
	void print();

	std::vector <Capacity> capacity; /**< Capacities of resources, e.g. MIPS for each core, B for each disk in an array */
	virtual ~Resource();
	res_type getType() {return type;};
	double getArch() {return arch;};
	static res_type translateType(const char* t);
protected:
	Resource();

	res_type type; /**< Type of resource, see enum ( e.g. CPU, Memory, Disk, Network Interface) */
	/* Architecture, ordering of architectures shall be listed separately.
	 * For now higher number means more advanced architecture.
	 * (TODO: efficient orders implementation)*/
	double arch;

	Resource& operator=(const Resource& r);
	int setType(const char* t);
	int setCapacity(std::vector <Capacity> cap);
	void sortCapacity();
};
#endif /* RESOURCE_H_ */
