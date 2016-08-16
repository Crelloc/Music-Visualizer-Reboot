#include "dataprocessing.h"

enum {left, right}; //left and right
enum {re, im};

/*void setupDFTForStereo(Visualizer_Pkg_ptr vis_pkg_ptr, double val, int* index){

	if(*index%2 == 0){
			vis_pkg_ptr->fftw_ptr[left].in[*index][re];
			vis_pkg_ptr->fftw_ptr[left].in[*index][im];

	}
	else{
			vis_pkg_ptr->fftw_ptr[right].in[*index][re];
			vis_pkg_ptr->fftw_ptr[right].in[*index][im];
			(*index)++;
	}

}
*/
void processWAVFile(Uint32 wavLength, int buffer_size, Visualizer_Pkg_ptr vis_pkg_ptr){

	printf("FILENAME: %s\n", vis_pkg_ptr->filename);


}
