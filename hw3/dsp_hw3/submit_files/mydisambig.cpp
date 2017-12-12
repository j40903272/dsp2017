#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <iterator>
#include <map>
#include <Ngram.h>
using namespace std;

typedef map<string, vector<string> > MAP;

inline vector<string> sentence_preprocess(char buf[]){
	vector<string> chars;
	char *token = strtok(buf, " ");
	while(token != NULL){
		chars.push_back(token);
		token = strtok(NULL, " ");
	}
	return chars;
}

inline void buildmap(MAP& Map){
	FILE *fp = fopen("ZhuYin-Big5.map", "r");
	char buf[65536];
	while(fgets(buf, sizeof(buf), fp) != NULL){
		char *token = strtok(buf, " ");
		string key(token);
		token = strtok(NULL, " \n");
		while(token != NULL){
			Map[key].push_back(token);
			token = strtok(NULL, " \n");
		}
	}
	Map["\n"].push_back("</s>");
}

inline double ngramProb(Vocab& voc, Ngram& lm, const char *a, const char *b){
	VocabIndex wid1 = voc.getIndex(a);
	VocabIndex wid2 = voc.getIndex(b);
	if(wid1 == Vocab_None)
		wid1 = voc.getIndex(Vocab_Unknown);
	if(wid2 == Vocab_None)
		wid2 = voc.getIndex(Vocab_Unknown);
	VocabIndex context[] = {wid1, Vocab_None};
	return lm.wordProb(wid2, context);
}

inline double ngramProb(Vocab& voc, Ngram& lm, const char *a, const char *b, const char *c){
	VocabIndex wid1 = voc.getIndex(a);
	VocabIndex wid2 = voc.getIndex(b);
	VocabIndex wid3 = voc.getIndex(c);
	if(wid1 == Vocab_None)
		wid1 = voc.getIndex(Vocab_Unknown);
	if(wid2 == Vocab_None)
		wid2 = voc.getIndex(Vocab_Unknown);
	if(wid3 == Vocab_None)
		wid3 = voc.getIndex(Vocab_Unknown);
	VocabIndex context[] = {wid2, wid1, Vocab_None};
	return lm.wordProb(wid3, context);
}

vector<string> viterbi(const vector<string>& chars, Vocab& voc, Ngram& lm, MAP& Map){
	vector<vector<string> > chartab;
	vector<vector<int> > idxtab;
	vector<double> lastprobs, probs;
	vector<string> lastchars;

	lastprobs.push_back(1);
	lastchars.push_back("<s>");
	for(auto i = chars.begin() ; i != chars.end() ; i++)
	{
		vector<string> currentchars;
		vector<int> maxidxs;
		for(auto j = Map[*i].begin() ; j != Map[*i].end() ; j++)
		{
			double maxprob = -100000.0;
			int maxidx = 0;
			for(auto k = lastchars.begin() ; k != lastchars.end() ; k++)
			{
				int idx = distance(lastchars.begin(), k);
				double prob = lastprobs[idx] + ngramProb(voc, lm, k->data(), j->data());
				if(prob > maxprob){
					maxprob = prob;
					maxidx = idx;
				}
			}
			probs.push_back(maxprob);
			currentchars.push_back(*j);
			maxidxs.push_back(maxidx);
		}
		lastprobs = probs;
		lastchars = currentchars;
		chartab.push_back(currentchars);
		idxtab.push_back(maxidxs);
		probs.clear();
	}

	vector<string> output;
	int idx = idxtab.size()-1, maxidx = idxtab[idx][0];
	for(--idx ; idx >= 0 ; idx--){
		output.push_back(chartab[idx][maxidx]);
		maxidx = idxtab[idx][maxidx];
	}
	return output;
}

int main(int argc, char* argv[]){
	if(argc != 4){
		printf("Usage : ./mydisambig [input file] [lm] [order]");
		exit(0);
	}
	
	int order = atoi(argv[3]);
	Vocab voc;
	Ngram lm(voc, order);
	File lmfile(argv[2], "r");
	lm.read(lmfile);
	lmfile.close();
	MAP Map;
	buildmap(Map);
	
	char buf[512];
	FILE *input = fopen(argv[1], "r");
	while(fgets(buf, sizeof(buf), input) != NULL){
		vector<string> chars = sentence_preprocess(buf);
		vector<string> output = viterbi(chars, voc, lm, Map);
		printf("<s> ");
		for(int len = output.size()-1 ; len >= 0 ; len--)
			printf("%s ", output[len].data());
		printf("</s>\n");
	}
}