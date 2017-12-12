import sys
f = open('Big5-ZhuYin.map', 'r', encoding='big5hkscs')
dic = {}

for line in f:
	char, zhuyin = line.split()

	for i in zhuyin.split('/'):
		if i[0] in dic:
			dic[i[0]] += (char + " ")
		else:
			dic[i[0]] = char + " "

	dic[char] = line[0]

sys.stdout = open('ZhuYin-Big5.map', 'w', encoding='big5hkscs')
for key in dic:
	val = dic[key]
	print ("%s %s" % (key, val))

sys.stdout = sys.__stdout__
