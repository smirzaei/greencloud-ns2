/*
 * resdemand.cc
 *
 *  @date Sep 5, 2013
 *  @author Guzek:Mateusz
 */

#include "resdemand.h"
#include "dcresource.h"

ResDemand::ResDemand(Resource & res) : Resource( res){
	capacity_location = std::vector <Capacity *>(res.capacity.size(),NULL) ;
	current_performance = std::vector <double>(res.capacity.size(),0);
	supported_virtual_resource = NULL;

}

ResDemand::ResDemand(Resource & res,DcResource* svr) : Resource( res){
	capacity_location = std::vector <Capacity *>(res.capacity.size(),NULL) ;
	current_performance = std::vector <double>(res.capacity.size(),0);
	if(svr != NULL){
		std::vector<Capacity>::iterator i_res;
		std::vector<Capacity>::iterator i_dem;
		for(i_res = svr->capacity.begin(), i_dem = this->capacity.begin();
				i_res != svr->capacity.end();
				i_res++, i_dem++){
			i_dem->virtual_capacities.push_back(&(*i_res));
		}
	}
	supported_virtual_resource = svr;
}

