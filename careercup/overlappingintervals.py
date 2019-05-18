'''
There is an unordered interval stream, write a method, returns the total length that all intervals cover by now
class Interval{
 	int start;
	int end;
}
public List<Integer> countCoverLength(Iterator<Interval> stream){

}
'''


def overlappingintervals(intervals):
	# intervals = [[start, end], ...]
	bigdict = {}
	for interval in intervals:
		for i in range(interval[0], interval[1]):
			bigdict[i] = True
	return len(bigdict.keys())

# https://stackoverflow.com/questions/43600878/merging-overlapping-intervals-python
def overlappingintervals2(temp_tuple):
	temp_tuple.sort(key=lambda interval: interval[0])
	merged = [temp_tuple[0]]
	for current in temp_tuple:
    		previous = merged[-1]
    		if current[0] <= previous[1]:
        		previous[1] = max(previous[1], current[1])
    		else:
        		merged.append(current)	
