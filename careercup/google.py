"""
https://www.careercup.com/question?id=5655707613921280
Given a list of files. Return all the unique lines from all files.
"""

def uniquelines(files = []):
	lines = {}
	for f in files:
		linenum=0
		for line in f.readline():
			linenum+=1
			if line not in lines.keys():
				lines[line] = (f, linenum)
	return lines.keys()


"""
Give you a text file, remove duplicated lines.
"""
def removeduplicatelines(filename):
	lines_seen = set()
	outfile = open(outfilename, 'w')
	with open(filename, 'r') as fp:
		for line in fp.readline():
			if line not in lines_seen:
				outfile.write(line)
				lines_seen.add(line)
	outfile.close()
	
