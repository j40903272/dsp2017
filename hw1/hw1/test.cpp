#include "hmm.h"
#include <math.h>

#define MAX_FILE 8
#define MAX_TEST_SAMPLES 2500

inline double test_on_batch(HMM &model, const char sample[]){
	// vertibi
	int sample_len = strlen(sample);
	double delta[2][MAX_STATE];

	// Init
	for(int i = 0 ; i < model.state_num ; i++)
		delta[0][i] = model.initial[i] * model.observation[ sample[0] - 'A' ][i];

	// Recursion
	for(int t = 1 ; t < sample_len ; t++){
		for(int i = 0 ; i < model.state_num ; i++){
			double max_prob = 0.0;
			for(int j = 0 ; j < model.state_num ; j++){
				double tmp = delta[0][j] * model.transition[j][i];
				if(max_prob < tmp)
					max_prob = tmp;
			}
			delta[1][i] = max_prob * model.observation[ sample[t] - 'A' ][i];
		}
		for(int i = 0 ; i < model.state_num ; i++)
			delta[0][i] = delta[1][i];
	}

	// Termination
	double max_prob = 0.0;
	for(int i = 0 ; i < model.state_num ; i++)
		if(max_prob < delta[0][i])
			max_prob = delta[0][i];
	return max_prob;
}

int main(int argc, char* argv[])
{
	if(argc != 4){
		printf("./test  modellist.txt  testing_data.txt  result.txt");
		return 0;
	}
	printf("Start testing\n");
	HMM models[MAX_FILE];
	int model_num = 0;
	char filename[MAX_LINE];

	// load model
	FILE *modellist = open_or_die(argv[1], "r");
	while(fscanf(modellist, "%s", filename) > 0)
		loadHMM(&models[model_num++], filename);
	fclose(modellist);
	printf("%d models\n", model_num);

	// load testing data
	FILE *testing_data = open_or_die(argv[2], "r");
	// output file
	FILE *result = open_or_die(argv[3], "w");


	//test
	char sample[MAX_LINE];
	int sample_num = 0;
	while(fscanf(testing_data, "%s", sample) > 0){
		double max_prob = 0.0;
		int idx = -1;
		for(int i = 0 ; i < model_num ; i++){
			double prob = test_on_batch(models[i], sample);
			if(prob > max_prob){
				idx = i;
				max_prob = prob;
			}
		}
		fprintf(result, "%s %e\n", models[idx].model_name, max_prob);
		++sample_num;
	}
	printf("%d samples\n", sample_num);
	fclose(testing_data);
	fclose(result);
	return 0;
}
