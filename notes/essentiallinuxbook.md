# 6 Serial

UART: chip to implement serial communication

Layers of serial subsystem:

 1. low-level UART driver
 2.  tty driver
 3. top-level line discipline
 
### UART driver



# 17 MTD


##MTD Subsystem

* MTD core
* Map drivers
* NOR chip drivers
* NAND chip drivers
* User modules


## Map driver

* Enable MTD device (implemented as platform device

```
// 1) define partitions
static struct mtd_partition mypartitions[] {
	.name = ,
	.size = ,
   .offset = ,
   .mask_flags = , }, { ... },};

// 2) define resources
struct resource myresource = {
	.start = ,
	.end = ,
	.flags = IORESOURCE_MEM,};
	
// 3) define platform device
struct platform_device myplatdev = {
	.name = "name",
	.resource = &myresource, };

// 4) define platform driver
struct platform_driver myplatdriver = {
	.driver = { .name = "name", },
	.prove = myprobe, }; 

static int __init module_init() {
	
	// 5) register platform device
	platform_device_register(&myplatdev); 
	
	// 6) register platform driver. Since it matches
	// platform device, probe will be called
	platform_driver_register(&mydriver);

}
```

* probe method

```
static int myprobe(struct platform_device *pdev) {

	// 1) request (reserves) memory region (so other drivers dont use it)
	struct resource *res = pdev->resource
	request_mem_region(res->start, res->end, pdev->name);
	
	// 2) map bus address to CPU address (API pci_iomap, iomap, iomap_nocache)
	struct map_info *mymap;
	void *iomem = ioremap_nocache(res->start, res->end - res->start + 1);
	
	// 3) Populate map_info struct with data and methods
	struct map_info * mymapdriver;
	mymapdriver->virt = iomem;
	...
	
	// 4) probe flash with a MTD chip driver
	do_map_probe("cfi_probe, mymydriver);
	
	// 5) register MTD partition
	struct mtd_info * mymtd;
	add_mtd_partitions(mymtd, mypartitions, <numpart>);
	
	// /dev/mtd/0, ... should show up
	
```

NOR chip drivers

* most implement specifications like CFI and JEDEC
* command-sets for compliants chips supported as kernel modules (enable in kconfig)


NAND chip drivers

* USB pen drive: controller emulates	 SCSI storage interface -> don't need NAND driver to communicate
* On-board NAND flash chips -> needs special drivers
	* Not connected to CPU via data/address lines -> uses flash controller 
	* CPU read flash -> CPU issues read command to NAND controller -> controller copies from NAND to small internal RAM
	* NAND flash chips dont support CFI
	* inform MTD about NAND chip properties -> `nand_flash_ids[]`
	* faults very common -> spare area in each flash page with OOB information
	* OOB includes ECC for error correction -> specify via struct nand_ecclayout according to chip's properties

``` 
struct nand_ecclayout {
	uint32_t eccbytes; 	  // numb of bytes that store ECC data
	uint32_t eccpos[64];  // offset to ECC data within OOB area
	uint32_t oobavail;    // unused bytes in OOB area
	struct   nand_oobfree oobfree[MAX]; 
}
```

* MTD to implement error managemenet software (if NAND controller hardware doesnt support it) 

## User modules

* Support file I/O -> flash device as disk (block device)
	* __mtdblock__: emulates harddisk -> `/dev/mtdblock/<part>`
	* __NFTL__: like mtdblock but with wear leveling -> `/dev/nftl/<part>`
	* Create filesystem:
		* ext2 
		
```
$ mkfs -t ext2 /dev/mtdblock/2
$ mount /dev/mtdblock/2 /mnt
```
* 
	* 
		* JFFS2: implements log structure -> power-down relaible, increase life span

	
* Support raw I/O -> flash device as character device

	