#include "hmm.h"
#include <string.h>

#define MAX_TRAIN_SAMPLES 10000
static double alpha[MAX_TRAIN_SAMPLES][MAX_SEQ][MAX_STATE];
static double beta[MAX_TRAIN_SAMPLES][MAX_SEQ][MAX_STATE];
static double _gamma[MAX_TRAIN_SAMPLES][MAX_SEQ][MAX_STATE];
static double epsilon[MAX_TRAIN_SAMPLES][MAX_SEQ][MAX_STATE][MAX_STATE];
static char samples[MAX_TRAIN_SAMPLES][MAX_SEQ];


inline void update(const int sample_cnt, HMM& model){
#pragma omp parallel
{
// update model.initial
	#pragma omp for nowait
	for(int i = 0 ; i < model.state_num ; i++){
		double tmp = 0.0;
		for(int j = 0 ; j < sample_cnt ; j++)
			tmp += _gamma[j][0][i];
		model.initial[i] = tmp / sample_cnt;
	}

// update model.transition
	#pragma omp for nowait collapse(2)
	for(int i = 0 ; i < model.state_num ; i++){
		for(int j = 0 ; j < model.state_num ; j++){
			double epsilon_sum = 0.0, gamma_sum = 0.0;
			for(int k = 0 ; k < sample_cnt ; k++){
				int len = strlen(samples[k]);
				for(int l = 0 ; l < len-1 ; l++){
					epsilon_sum += epsilon[k][l][i][j];
					gamma_sum += _gamma[k][l][i];
				}
			}
			model.transition[i][j] = epsilon_sum / gamma_sum;
		}
	}

// update model.observation
	#pragma omp for nowait collapse(2)
	for(int i = 0 ; i < model.state_num ; i++){
		for(int j = 0 ; j < model.observ_num ; j++){
			double sum1 = 0.0, sum2 = 0.0;
			for(int k = 0 ; k < sample_cnt ; k++){
				int len = strlen(samples[k]);
				for(int l = 0 ; l < len ; l++){
					if(samples[k][l] - 'A' == j)
						sum1 += _gamma[k][l][i];
					sum2 += _gamma[k][l][i];
				}
			}
			model.observation[j][i] = sum1 /sum2;
		}
	}
}//pragma

}

inline void train_on_batch(const int s, HMM &model, const char * const sample, const int sample_len){

// Init
	for(int i = 0 ; i < model.state_num ; i++){
		alpha[s][0][i] = model.initial[i] * model.observation[ sample[0] - 'A' ][i];
		beta[s][sample_len-1][i] = 1;
	}

// calculate alpha
	// Induction
	for(int t = 1 ; t < sample_len ; t++){
		for(int i = 0 ; i < model.state_num ; i++){
			double tmp = 0.0;
			for(int j = 0 ; j < model.state_num ; j++)
				tmp += alpha[s][t-1][j] * model.transition[j][i];
			alpha[s][t][i] = tmp * model.observation[ sample[t] - 'A' ][i];
		}
	}

// calculate beta
	// Induction
	for(int t = sample_len-2 ; t >= 0 ; t--)
		for(int i = 0 ; i < model.state_num ; i++)
			for(int j = 0 ; j < model.state_num ; j++)
				beta[s][t][i] += model.transition[i][j] * //
								 model.observation[ sample[t+1] - 'A' ][j] * //
								 beta[s][t+1][j];
	
// calculate epsilon
	for(int t = 0 ; t < sample_len-1 ; t++){
		double tmp = 0.0;
		for(int i = 0 ; i < model.state_num ; i++){
			for(int j = 0 ; j < model.state_num ; j++){
				epsilon[s][t][i][j] = alpha[s][t][i] * //
									  model.transition[i][j] * //
									  model.observation[ sample[t+1] - 'A' ][j] * //
									  beta[s][t+1][j];
				tmp += epsilon[s][t][i][j];
			}
		}
		for(int i = 0 ; i < model.state_num ; i++)
			for(int j = 0 ; j < model.state_num ; j++)
				epsilon[s][t][i][j] /= tmp;
	}

// calculate gamma
	for(int t = 0 ; t < sample_len ; t++){
		double tmp = 0.0;
		for(int i = 0 ; i < model.state_num ; i++)
			tmp += ( _gamma[s][t][i] = alpha[s][t][i] * beta[s][t][i] );
		for(int i = 0 ; i < model.state_num ; i++)
			_gamma[s][t][i] /= tmp;
	}
}


void trainHMM(HMM &model, const int iters, FILE *fin){
	printf("Start training\n");
	int sample_cnt = 0;
	while(fscanf(fin, "%s", samples[sample_cnt++]) > 0);
	printf("%d samples\n", --sample_cnt);
	for(int iter = 0 ; iter < iters ; iter++){
		#pragma omp parallel for
		for(int s = 0 ; s < sample_cnt ; s++){
			int len = strlen(samples[s]);
			train_on_batch(s, model, samples[s], len);
		}
		update(sample_cnt, model);
		printf("Iteration %d completed!\n", iter+1);
	}
}

int main(int argc, char* argv[])
{
	if(argc != 5){
		printf("./train  iteration  model_init.txt  seq_model_01.txt  model_01.txt");
		return 0;
	}

	HMM model;
	int iter = atoi(argv[1]);
	loadHMM(&model, argv[2]);
	FILE *fin = open_or_die(argv[3], "r");		//training data
	trainHMM(model, iter, fin);
	FILE *fout = open_or_die(argv[4], "w");		//output model
	dumpHMM(fout, &model);
	printf("dump model to %s\n", argv[4]);
	return 0;
}