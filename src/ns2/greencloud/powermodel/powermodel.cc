/*
 * powermodel.cc
 *
 *  @date Sep 12, 2013
 *  @author Guzek:Mateusz
 */

#include "powermodel.h"


PowerModel::PowerModel() {

}

PowerModel::~PowerModel() {

}

void PowerModel::setName(const char* name){
	name_=name;
}
void PowerModel::print() {
	std::cout << "Abstract power model";
}

int PowerModel::command(int argc, const char*const* argv)
{

	if (argc == 2) {
		if (strcmp(argv[1], "print") == 0) {

			  /* print general info */
			  print();
			  return (TCL_OK);
			}
	}
	return (PowerModel::command(argc, argv));
}

