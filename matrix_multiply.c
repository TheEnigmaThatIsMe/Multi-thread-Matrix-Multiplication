#include "matrix_multiply.h"
//Def of helper struct used in thread runner function
typedef struct helper {
    unsigned int i, j, k;
    matrix *lhs; 
    matrix *rhs; 
    matrix *result;
    int rowBegin;
    int rowEnd; 
	unsigned int threads;
} helper;

void *runner(void *param); /* the thread */

err_code matrix_multiply(matrix *lhs, matrix *rhs, matrix *result, const unsigned int threads) {
    
    //check if lhs/rhs/result matrix is NULL
    if(lhs == NULL || rhs == NULL || result == NULL){
    	//fputs("LHS PTR ERROR!\n", stderr);
        return NULL_PTR;
    }
    
    //check if rows and columns of lhs and rhs are < 1 and see if data in both matrices is NULL
    else if(lhs->rows < 1 || lhs->columns < 1 || rhs->rows < 1 || rhs->columns < 1 || lhs->data == NULL || rhs->data == NULL){
    	//fputs("LHS MATRIX INVALID ROWS/COLUMNS ERROR!\n", stderr);
    	return BAD_MATRIX;
    }
    
    //check for bad dimensions
    else if(lhs->columns != rhs->rows){
    	//fputs("DIMENSIONS ERROR!\n", stderr);
    	return BAD_DIMENSIONS;
    }
    
    //check for bad thread count
    else if(threads == 0 || threads > lhs->rows){
    	return BAD_THREAD_COUNT;  
    }
    
    //Create variables
    unsigned int i = 0, j = 0, k = 0, count = 0;
    int sum = 0;
    
    //initialize the result matrix to store data
    initialize_matrix(lhs->rows, rhs->columns, result);
    
    //error check result matrix initialization
    if(result == NULL){
    	fputs("Error initializing result matrix!\n", stderr);
    	return NULL_PTR;
    }
    
    //only using one thread
    if(threads == 1){
		for(i = 0; i < lhs->rows; i++){ //Iterate through each row of the LHS Matrix
			for(j = 0; j < rhs->columns; j++){ //Iterate through each column of the RHS Matrix
				sum = 0;
				for(k = 0; k < lhs->columns; k++){ //Iterate through each column of the LHS Matrix which is equivalent to each row of the RHS Matrix	
					sum = sum + ((lhs->data[get_index(i,k,lhs->columns)]) * (rhs->data[get_index(k,j,rhs->columns)])); /* sum +=  (LHS value) * (RHS Value) */
				}
				//store sum in result matrix
				result->data[get_index(i,j,rhs->columns)] = sum; 			
			}
		}	
	}
	//Begin multi-threading
	else {
		//create a partition size LHS rows / # of threads
		int partitionSize = ((lhs->rows) / threads);
		int start = 0, end = partitionSize;
		
		//allocate a helper structure that passes everything needed for threads
		helper *threadData = (helper *) malloc(sizeof(helper) * threads);
		
		//error check to make sure the helper structure is initialized
		if(threadData == NULL){
			fputs("Error allocating memory to threadData!\n", stderr);
    		return MEMORY_ERROR;
		}
		
		//allocate space for threads
		pthread_t *threads_created = (pthread_t *) malloc(sizeof(pthread_t) * threads);
		
		//error check that memory was allocated for threads
		if(threads_created == NULL){
			fputs("Error allocating memory to threads_created!\n", stderr);
    		return MEMORY_ERROR;
		}
		
		//Iterate through each row of the LHS Matrix
		for(i = 0; i < threads; i++){ 
	
			//Assign a row and column for each thread
			threadData[i].rhs = rhs;
			threadData[i].lhs = lhs;
			threadData[i].rowBegin = start;

			//Check for uneven thread partition
			if(i == (threads - 1)){
			/*Set the last threads end to the last row to make up for the offset caused by uneven thread partition
			If it is an even partition, the last row will be equal to the correct end for the last thread*/
			threadData[i].rowEnd = lhs->rows; 
			}
			else{
				threadData[i].rowEnd = end;
			}
			
			//set result
			threadData[i].result = result;
			
			//Create/Error check threads and pass to runner function
			if(pthread_create(threads_created + i, NULL, runner, threadData + i) != 0){
				fputs("Error creating thread!\n", stderr);
				return THREAD_ERROR;
			}

			start += partitionSize;
			end += partitionSize;

			count++;
		}
		//Join/Error check joined threads
		for(int p = 0; p < threads; p++){
			if(pthread_join(threads_created[p], NULL) != 0){
				fputs("Error joining thread!\n", stderr);
			 	return THREAD_ERROR;
			}
		}
		//free threads after they are used to prevent memory leaks
		free(threads_created);	
	}
	//Everything went according to plan ;)
    return OK;
}

err_code initialize_matrix(unsigned int rows, unsigned int columns, matrix *new_matrix) {
    if (new_matrix == NULL) {
        fputs("Null pointer!\n", stderr);
        return NULL_PTR;
    }
    if (rows == 0 || columns == 0) {
        fputs("Row/column size 0!\n", stderr);
        return BAD_DIMENSIONS;
    }

    new_matrix->rows = rows;
    new_matrix->columns = columns;

    const unsigned int total = rows * columns;
    new_matrix->data = malloc(total * sizeof(float));

    if (new_matrix->data == NULL) {
        perror("Malloc failure");
        return MEMORY_ERROR;
    }

    for (unsigned int idx = 0; idx < total; ++idx) {
        new_matrix->data[idx] = (((float) rand()) / (RAND_MAX)) * 100;
        //new_matrix->data[idx] = rand() % 100;
    }

    return OK;
}

void destroy_matrix(matrix *dst_matrix) {
    free(dst_matrix->data); // eh, freeing NULL is harmless
}

err_code print_matrix(matrix *mtx) {
    if (mtx == NULL) {
        fputs("Null matrix!\n", stderr);
        return NULL_PTR;
    }
    if (mtx->rows == 0 || mtx->columns == 0) {
        fputs("Row/column size 0!\n", stderr);
        return BAD_MATRIX;
    }

    if (mtx->data == NULL) {
        fputs("Null data!\n", stderr);
        return BAD_MATRIX;
    }

    const unsigned int size = mtx->rows * mtx->columns;
    const unsigned int trigger = mtx->columns - 1;

    // instead of doing rowxcolumn traversal, just walk the array
    // trigger and col_track track when it should print a newline.
    // Looks alittle gross, but it's so, so pretty

    for (unsigned int idx = 0, col_track = 0; idx < size; ++idx) {
        printf("%.3f%c", mtx->data[idx], (col_track == trigger ? (col_track = 0, '\n') : (++col_track, '\t')));
    }

    return OK;
}

//The thread will begin control in this function
void *runner(void *param) {

	//the structure that holds our data
	helper *data = (helper *) param;
	
	//the counter variables and sum
	int i = 0, j = 0, k = 0, sum = 0;

	//Row multiplied by column
	for(i = data->rowBegin; i < data->rowEnd; i++){ //Iterate through each row of the LHS Matrix
		for(j = 0; j < data->rhs->columns; j++){ //Iterate through each column of the RHS Matrix
			sum = 0;
			//Iterate through each column of the LHS Matrix which is equivalent to each row of the RHS Matrix
			for(k = 0; k < data->lhs->columns; k++) { 
				/*sum +=  (LHS value) * (RHS Value)*/
				sum = sum + ((data->lhs->data[get_index(i,k,data->lhs->columns)]) * (data->rhs->data[get_index(k,j,data->rhs->columns)]));
			}
			//set the sum in the result matrix
			data->result->data[get_index(i,j,data->rhs->columns)] = sum; 	
		}
	}
	//Exit the thread
	pthread_exit(0);
}
