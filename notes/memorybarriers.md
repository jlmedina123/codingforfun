https://www.kernel.org/doc/Documentation/memory-barriers.txt

* expected guarantees
	* dependent memory accesses are issued in order
	* overlapping loads and stores within CPU will apear in order 
	* overlapping memory accesses may be merged or discarded
* not expected
	* independent loads and stores are issued in order  	

* independent memory operations are performed in random order -> problem for CPU-CPU or CPU-device interaction
* memory barriers impose ordering over memory operations on either side of barrier

* types of memory barriers
	* write (store)
	* data dependency 
	* read (load)
	* general	