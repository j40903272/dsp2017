#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <iterator>
#include <map>
#include <Ngram.h>
using namespace std;

typedef map<string, vector<string> > MAP;
typedef struct{
	int idx;
	string str;
	double prob;
}DPNode;

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
	vector<vector<DPNode> > table;
	vector<DPNode> last;

	last.push_back((DPNode){0, "<s>", 1.0});
	table.push_back(last);
	
	for(auto i = chars.begin() ; i != chars.end() ; i++){
		int ii = distance(chars.begin(), i);
		last.clear();
		for(auto j = Map[*i].begin() ; j != Map[*i].end() ; j++){
			double maxprob = -100000.0;
			int maxidx = 0;
			for(int k = 0 ; k < table[ii].size() ; k++){
				double prob = table[ii][k].prob + ngramProb(voc, lm, table[ii][k].str.data(), j->data());
				if(prob > maxprob){
					maxprob = prob;
					maxidx = k;
				}
			}
			last.push_back((DPNode){maxidx, *j, maxprob});
		}
		table.push_back(last);
	}

	vector<string> output;
	int idx = 0;
	for(int i = table.size()-1 ; i >= 0 ; i--){
		output.push_back(table[i][idx].str);
		idx = table[i][idx].idx;
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
		for(int len = output.size()-1 ; len >= 0 ; len--)
			printf("%s%c", output[len].data(), " \n"[len==0]);
	}
}