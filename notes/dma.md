
https://www.kernel.org/doc/Documentation/DMA-API-HOWTO.txt

* Addresses involved in DMA
    * virtual: 
        * used by CPU, from k/vmalloc(), void *
    * physical:
        * virtual translated to physical by virtual memory system (TLB, page tables)
        * physical addr cannot be used by driver, use ioremap() to produce virtual
    * bus address:
        * used by devices with register as MMIO addr, or DMA
        * can be same as physical
        * IOMMU produces arbitrary mapping between physical and bus
* dynamic dma mapping: dma addresses mapped only when used


* memory DMA-able:
    * if aquired with page allocator (get_free_page, kmalloc, kmem_cache_alloc...)
    * not from vmalloc (possible but you'd have to walk page tables, get physical addr, and translate each to kernel addr with `__va()`)
    * no data/text/bss or stack (can be mapped outside rest of physical memory). would have to ensure I/O buffers are cacheline-aligned to avoid cacheline sharing problems
    * no address from kmap

* Check if DMA controller can support the device DMA capabilities in probe routine:
    * coherent: dma_set_mask_and_coherent(dev, mask)
    * streaming: dma_set_mask(dev, mask)


* Types of DMA
	* consistent (synchronous, coherent) 
		* mapped at driver initialization, unmapped at end
		* hardware guarantees device and CPU can access data in parallel and they'll see each other's updates without explicit software flusing 
		* CPU store to memory immediately visible to device, and vice versa -> guaranteed by consisten mappings
		* CPU may still reorder stores -> still need memory barriers
		* cpu_addr = dma_alloc_coherent(dev, size, &dma_handle, gfp): one large memory region
		* struct dma_pool *pool = dma_pool_create(name, dev, size, align, boundary): for many small memory regions
		
		
		
	* streaming (asynchronouse, streaming)
		* mapped for one DMA transfer, unmapped right after 
		* dma_map_page
		* dma_map_single
		* dma_map_sg
		* To reuse buffer, it needs to sync so CPU and device see update copy: dma_sync_single_for_cpu, dma_sync_sg_for_cpu, dma_sync_single_for_device, dma_sync_sg_for_device	 		

    
