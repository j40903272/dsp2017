import sys

if len(sys.argv) < 4 :
	print ("python3 acc.py result.txt testing_answer.txt acc.txt")
	exit()

my_ans = []
test_ans = []

for line in open(sys.argv[1]):
	ans = line.split()[0]
	my_ans.append(ans)

#print ("%d lines in %s" % (len(my_ans), sys.argv[1]))

for line in open(sys.argv[2]):
	ans = line.split()[0]
	test_ans.append(ans)

#print ("%d lines in %s" % (len(test_ans), sys.argv[2]))

if len(my_ans) != len(test_ans):
	print ("line number doesnt match")
	exit()

correct = 0;
for i, j in zip(my_ans, test_ans):
	if i == j :
		correct += 1

with open(sys.argv[3], "w") as f:
	out = str( float(correct) / len(my_ans) ) + "\n"
	print (out)
	f.write(out)