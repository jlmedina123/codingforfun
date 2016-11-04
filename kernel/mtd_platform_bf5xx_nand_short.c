/* linux/drivers/mtd/nand/bf5xx_nand.c
 * Blackfin BF5xx on-chip NAND flash controller driver
 *
 * Platform devices are inherently not discoverable,
 * Platform devices are bound to drivers by matching names,
 * These devices are deemed to be connected to a virtual "platform bus"
*/


/* http://lwn.net/Articles/448499/
 *
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/bitops.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/blackfin.h>
#include <asm/dma.h>
#include <asm/cacheflush.h>
#include <asm/nand.h>
#include <asm/portmux.h>

#define DRV_NAME	"bf5xx-nand"
#define DRV_VERSION	"1.2"
#define DRV_AUTHOR	"Bryan Wu <bryan.wu@analog.com>"
#define DRV_DESC	"BF5xx on-chip NAND FLash Controller Driver"

/* NFC_STAT Masks */
#define NBUSY       0x01  /* Not Busy */
#define WB_FULL     0x02  /* Write Buffer Full */
#define PG_WR_STAT  0x04  /* Page Write Pending */
#define PG_RD_STAT  0x08  /* Page Read Pending */
#define WB_EMPTY    0x10  /* Write Buffer Empty */

/* NFC_IRQSTAT Masks */
#define NBUSYIRQ    0x01  /* Not Busy IRQ */
#define WB_OVF      0x02  /* Write Buffer Overflow */
#define WB_EDGE     0x04  /* Write Buffer Edge Detect */
#define RD_RDY      0x08  /* Read Data Ready */
#define WR_DONE     0x10  /* Page Write Done */

/* NFC_RST Masks */
#define ECC_RST     0x01  /* ECC (and NFC counters) Reset */

/* NFC_PGCTL Masks */
#define PG_RD_START 0x01  /* Page Read Start */
#define PG_WR_START 0x02  /* Page Write Start */

#ifdef CONFIG_MTD_NAND_BF5XX_HWECC
static int hardware_ecc = 1;
#else
static int hardware_ecc;
#endif

static const unsigned short bfin_nfc_pin_req[] =
	{P_NAND_CE,
	 P_NAND_RB,
	 P_NAND_D0,
	 P_NAND_D1,
	 P_NAND_D2,
	 P_NAND_D3,
	 P_NAND_D4,
	 P_NAND_D5,
	 P_NAND_D6,
	 P_NAND_D7,
	 P_NAND_WE,
	 P_NAND_RE,
	 P_NAND_CLE,
	 P_NAND_ALE,
	 0};

#ifdef CONFIG_MTD_NAND_BF5XX_BOOTROM_ECC
static struct nand_ecclayout bootrom_ecclayout = {
	.eccbytes = 24,
	.eccpos = {
		0x8 * 0, 0x8 * 0 + 1, 0x8 * 0 + 2,
		0x8 * 1, 0x8 * 1 + 1, 0x8 * 1 + 2,
		0x8 * 2, 0x8 * 2 + 1, 0x8 * 2 + 2,
		0x8 * 3, 0x8 * 3 + 1, 0x8 * 3 + 2,
		0x8 * 4, 0x8 * 4 + 1, 0x8 * 4 + 2,
		0x8 * 5, 0x8 * 5 + 1, 0x8 * 5 + 2,
		0x8 * 6, 0x8 * 6 + 1, 0x8 * 6 + 2,
		0x8 * 7, 0x8 * 7 + 1, 0x8 * 7 + 2
	},
	.oobfree = {
		{ 0x8 * 0 + 3, 5 },
		{ 0x8 * 1 + 3, 5 },
		{ 0x8 * 2 + 3, 5 },
		{ 0x8 * 3 + 3, 5 },
		{ 0x8 * 4 + 3, 5 },
		{ 0x8 * 5 + 3, 5 },
		{ 0x8 * 6 + 3, 5 },
		{ 0x8 * 7 + 3, 5 },
	}
};
#endif

/*
 * Data structures for bf5xx nand flash controller driver
 */

/* bf5xx nand info */
struct bf5xx_nand_info {
	/* mtd info */
	struct nand_hw_control		controller;
	struct mtd_info			mtd;
	struct nand_chip		chip;

	/* platform info */
	struct bf5xx_nand_platform	*platform;

	/* device info */
	struct device			*device;

	/* DMA stuff */
	struct completion		dma_completion;
};

/*
 * Conversion functions
 */
static struct bf5xx_nand_info *mtd_to_nand_info(struct mtd_info *mtd)
{
	return container_of(mtd, struct bf5xx_nand_info, mtd);
}

static struct bf5xx_nand_info *to_nand_info(struct platform_device *pdev)
{
	return platform_get_drvdata(pdev);
}

static struct bf5xx_nand_platform *to_nand_plat(struct platform_device *pdev)
{
	return dev_get_platdata(&pdev->dev);
}

/*
 * struct nand_chip interface function pointers
 */

/*
 * bf5xx_nand_hwcontrol
 *
 * Issue command and address cycles to the chip
 */
static void bf5xx_nand_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	if (cmd == NAND_CMD_NONE)
		return;

	while (bfin_read_NFC_STAT() & WB_FULL)
		cpu_relax();

	if (ctrl & NAND_CLE)
		bfin_write_NFC_CMD(cmd);
	else if (ctrl & NAND_ALE)
		bfin_write_NFC_ADDR(cmd);
	SSYNC();
}

/*
 * bf5xx_nand_devready()
 *
 * returns 0 if the nand is busy, 1 if it is ready
 */
static int bf5xx_nand_devready(struct mtd_info *mtd)
{
	unsigned short val = bfin_read_NFC_STAT();

	if ((val & NBUSY) == NBUSY)
		return 1;
	else
		return 0;
}

/*
 * ECC functions
 * These allow the bf5xx to use the controller's ECC
 * generator block to ECC the data as it passes through
 */

/*
 * ECC error correction function
 */
static int bf5xx_nand_correct_data_256(struct mtd_info *mtd, u_char *dat,
					u_char *read_ecc, u_char *calc_ecc)
{
	struct bf5xx_nand_info *info = mtd_to_nand_info(mtd);
	u32 syndrome[5];
	u32 calced, stored;
	int i;
	unsigned short failing_bit, failing_byte;
	u_char data;

	calced = calc_ecc[0] | (calc_ecc[1] << 8) | (calc_ecc[2] << 16);
	stored = read_ecc[0] | (read_ecc[1] << 8) | (read_ecc[2] << 16);

	syndrome[0] = (calced ^ stored);

	/*
	 * syndrome 0: all zero
	 * No error in data
	 * No action
	 */
	if (!syndrome[0] || !calced || !stored)
		return 0;

	/*
	 * sysdrome 0: only one bit is one
	 * ECC data was incorrect
	 * No action
	 */
	if (hweight32(syndrome[0]) == 1) {
		dev_err(info->device, "ECC data was incorrect!\n");
		return 1;
	}

	syndrome[1] = (calced & 0x7FF) ^ (stored & 0x7FF);
	syndrome[2] = (calced & 0x7FF) ^ ((calced >> 11) & 0x7FF);
	syndrome[3] = (stored & 0x7FF) ^ ((stored >> 11) & 0x7FF);
	syndrome[4] = syndrome[2] ^ syndrome[3];

	for (i = 0; i < 5; i++)
		dev_info(info->device, "syndrome[%d] 0x%08x\n", i, syndrome[i]);

	dev_info(info->device,
		"calced[0x%08x], stored[0x%08x]\n",
		calced, stored);

	/*
	 * sysdrome 0: exactly 11 bits are one, each parity
	 * and parity' pair is 1 & 0 or 0 & 1.
	 * 1-bit correctable error
	 * Correct the error
	 */
	if (hweight32(syndrome[0]) == 11 && syndrome[4] == 0x7FF) {
		dev_info(info->device,
			"1-bit correctable error, correct it.\n");
		dev_info(info->device,
			"syndrome[1] 0x%08x\n", syndrome[1]);

		failing_bit = syndrome[1] & 0x7;
		failing_byte = syndrome[1] >> 0x3;
		data = *(dat + failing_byte);
		data = data ^ (0x1 << failing_bit);
		*(dat + failing_byte) = data;

		return 0;
	}

	/*
	 * sysdrome 0: random data
	 * More than 1-bit error, non-correctable error
	 * Discard data, mark bad block
	 */
	dev_err(info->device,
		"More than 1-bit error, non-correctable error.\n");
	dev_err(info->device,
		"Please discard data, mark bad block\n");

	return 1;
}

static int bf5xx_nand_correct_data(struct mtd_info *mtd, u_char *dat,
					u_char *read_ecc, u_char *calc_ecc)
{
	struct nand_chip *chip = mtd->priv;
	int ret;

	ret = bf5xx_nand_correct_data_256(mtd, dat, read_ecc, calc_ecc);

	/* If ecc size is 512, correct second 256 bytes */
	if (chip->ecc.size == 512) {
		dat += 256;
		read_ecc += 3;
		calc_ecc += 3;
		ret |= bf5xx_nand_correct_data_256(mtd, dat, read_ecc, calc_ecc);
	}

	return ret;
}

static void bf5xx_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	return;
}

static int bf5xx_nand_calculate_ecc(struct mtd_info *mtd,
		const u_char *dat, u_char *ecc_code)
{
	struct bf5xx_nand_info *info = mtd_to_nand_info(mtd);
	struct nand_chip *chip = mtd->priv;
	u16 ecc0, ecc1;
	u32 code[2];
	u8 *p;

	/* first 3 bytes ECC code for 256 page size */
	ecc0 = bfin_read_NFC_ECC0();
	ecc1 = bfin_read_NFC_ECC1();

	code[0] = (ecc0 & 0x7ff) | ((ecc1 & 0x7ff) << 11);

	dev_dbg(info->device, "returning ecc 0x%08x\n", code[0]);

	p = (u8 *) code;
	memcpy(ecc_code, p, 3);

	/* second 3 bytes ECC code for 512 ecc size */
	if (chip->ecc.size == 512) {
		ecc0 = bfin_read_NFC_ECC2();
		ecc1 = bfin_read_NFC_ECC3();
		code[1] = (ecc0 & 0x7ff) | ((ecc1 & 0x7ff) << 11);

		/* second 3 bytes in ecc_code for second 256
		 * bytes of 512 page size
		 */
		p = (u8 *) (code + 1);
		memcpy((ecc_code + 3), p, 3);
		dev_dbg(info->device, "returning ecc 0x%08x\n", code[1]);
	}

	return 0;
}

/*
 * PIO mode for buffer writing and reading
 */
static void bf5xx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int i;
	unsigned short val;

	/*
	 * Data reads are requested by first writing to NFC_DATA_RD
	 * and then reading back from NFC_READ.
	 */
	for (i = 0; i < len; i++) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			cpu_relax();

		/* Contents do not matter */
		bfin_write_NFC_DATA_RD(0x0000);
		SSYNC();

		while ((bfin_read_NFC_IRQSTAT() & RD_RDY) != RD_RDY)
			cpu_relax();

		buf[i] = bfin_read_NFC_READ();

		val = bfin_read_NFC_IRQSTAT();
		val |= RD_RDY;
		bfin_write_NFC_IRQSTAT(val);
		SSYNC();
	}
}

static uint8_t bf5xx_nand_read_byte(struct mtd_info *mtd)
{
	uint8_t val;

	bf5xx_nand_read_buf(mtd, &val, 1);

	return val;
}

static void bf5xx_nand_write_buf(struct mtd_info *mtd,
				const uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			cpu_relax();

		bfin_write_NFC_DATA_WR(buf[i]);
		SSYNC();
	}
}

static void bf5xx_nand_read_buf16(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int i;
	u16 *p = (u16 *) buf;
	len >>= 1;

	/*
	 * Data reads are requested by first writing to NFC_DATA_RD
	 * and then reading back from NFC_READ.
	 */
	bfin_write_NFC_DATA_RD(0x5555);

	SSYNC();

	for (i = 0; i < len; i++)
		p[i] = bfin_read_NFC_READ();
}

static void bf5xx_nand_write_buf16(struct mtd_info *mtd,
				const uint8_t *buf, int len)
{
	int i;
	u16 *p = (u16 *) buf;
	len >>= 1;

	for (i = 0; i < len; i++)
		bfin_write_NFC_DATA_WR(p[i]);

	SSYNC();
}

/*
 * DMA functions for buffer writing and reading
 */
static irqreturn_t bf5xx_nand_dma_irq(int irq, void *dev_id)
{
	struct bf5xx_nand_info *info = dev_id;

	clear_dma_irqstat(CH_NFC);
	disable_dma(CH_NFC);
	complete(&info->dma_completion);

	return IRQ_HANDLED;
}

static void bf5xx_nand_dma_rw(struct mtd_info *mtd,
				uint8_t *buf, int is_read)
{
	struct bf5xx_nand_info *info = mtd_to_nand_info(mtd);
	struct nand_chip *chip = mtd->priv;
	unsigned short val;

	dev_dbg(info->device, " mtd->%p, buf->%p, is_read %d\n",
			mtd, buf, is_read);

	/*
	 * Before starting a dma transfer, be sure to invalidate/flush
	 * the cache over the address range of your DMA buffer to
	 * prevent cache coherency problems. Otherwise very subtle bugs
	 * can be introduced to your driver.
	 */
	if (is_read)
		invalidate_dcache_range((unsigned int)buf,
				(unsigned int)(buf + chip->ecc.size));
	else
		flush_dcache_range((unsigned int)buf,
				(unsigned int)(buf + chip->ecc.size));

	/*
	 * This register must be written before each page is
	 * transferred to generate the correct ECC register
	 * values.
	 */
	bfin_write_NFC_RST(ECC_RST);
	SSYNC();
	while (bfin_read_NFC_RST() & ECC_RST)
		cpu_relax();

	disable_dma(CH_NFC);
	clear_dma_irqstat(CH_NFC);

	/* setup DMA register with Blackfin DMA API */
	set_dma_config(CH_NFC, 0x0);
	set_dma_start_addr(CH_NFC, (unsigned long) buf);

	/* The DMAs have different size on BF52x and BF54x */
#ifdef CONFIG_BF52x
	set_dma_x_count(CH_NFC, (chip->ecc.size >> 1));
	set_dma_x_modify(CH_NFC, 2);
	val = DI_EN | WDSIZE_16;
#endif

#ifdef CONFIG_BF54x
	set_dma_x_count(CH_NFC, (chip->ecc.size >> 2));
	set_dma_x_modify(CH_NFC, 4);
	val = DI_EN | WDSIZE_32;
#endif
	/* setup write or read operation */
	if (is_read)
		val |= WNR;
	set_dma_config(CH_NFC, val);
	enable_dma(CH_NFC);

	/* Start PAGE read/write operation */
	if (is_read)
		bfin_write_NFC_PGCTL(PG_RD_START);
	else
		bfin_write_NFC_PGCTL(PG_WR_START);
	wait_for_completion(&info->dma_completion);
}

static void bf5xx_nand_dma_read_buf(struct mtd_info *mtd,
					uint8_t *buf, int len)
{
	struct bf5xx_nand_info *info = mtd_to_nand_info(mtd);
	struct nand_chip *chip = mtd->priv;

	dev_dbg(info->device, "mtd->%p, buf->%p, int %d\n", mtd, buf, len);

	if (len == chip->ecc.size)
		bf5xx_nand_dma_rw(mtd, buf, 1);
	else
		bf5xx_nand_read_buf(mtd, buf, len);
}

static void bf5xx_nand_dma_write_buf(struct mtd_info *mtd,
				const uint8_t *buf, int len)
{
	struct bf5xx_nand_info *info = mtd_to_nand_info(mtd);
	struct nand_chip *chip = mtd->priv;

	dev_dbg(info->device, "mtd->%p, buf->%p, len %d\n", mtd, buf, len);

	if (len == chip->ecc.size)
		bf5xx_nand_dma_rw(mtd, (uint8_t *)buf, 0);
	else
		bf5xx_nand_write_buf(mtd, buf, len);
}

static int bf5xx_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
		uint8_t *buf, int oob_required, int page)
{
	bf5xx_nand_read_buf(mtd, buf, mtd->writesize);
	bf5xx_nand_read_buf(mtd, chip->oob_poi, mtd->oobsize);

	return 0;
}

static int bf5xx_nand_write_page_raw(struct mtd_info *mtd,
		struct nand_chip *chip,	const uint8_t *buf, int oob_required)
{
	bf5xx_nand_write_buf(mtd, buf, mtd->writesize);
	bf5xx_nand_write_buf(mtd, chip->oob_poi, mtd->oobsize);

	return 0;
}

/*
 * System initialization functions
 */

/*
 * BF5XX NFC hardware initialization
 *  - pin mux setup
 *  - clear interrupt status
 */
static int bf5xx_nand_hw_init(struct bf5xx_nand_info *info)
{
	int err = 0;
	unsigned short val;
	struct bf5xx_nand_platform *plat = info->platform;

	/* setup NFC_CTL register */
	val = (1 << NFC_PG_SIZE_OFFSET) |
		(plat->data_width << NFC_NWIDTH_OFFSET) |
		(plat->rd_dly << NFC_RDDLY_OFFSET) |
		(plat->wr_dly << NFC_WRDLY_OFFSET);
	bfin_write_NFC_CTL(val);

	/* clear interrupt status */
	bfin_write16(NFC_IRQMASK, 0x0);
	val = bfin_read16(NFC_IRQSTAT);
	bfin_write16(NFC_IRQSTAT, val);

	/* 4)
     * DMA initialization  */
    init_completion(&info->dma_completion); // init_waitqueue_head
    
    /* Request NFC DMA channel 
     *      * For ISA devices, the original DMA controller used in the PC could manage four "channels," each associated with one set of DMA registers. Four devices could store their DMA information in the controller at the same time*/
    request_dma(CH_NFC, "BF5XX NFC driver");

    set_dma_callback(CH_NFC, bf5xx_nand_dma_irq, info);  // request_irq

    /* Turn off the DMA channel first */
    disable_dma(CH_NFC);    
    

	return err;
}


/*
 * bf5xx_nand_probe
 *
 * called by device layer when it finds a device matching
 * one our driver can handled. This code checks to see if
 * it can allocate all necessary resources then calls the
 * nand layer to look for devices
 */
static int bf5xx_nand_probe(struct platform_device *pdev)
{
	struct bf5xx_nand_platform *plat = to_nand_plat(pdev);
	struct bf5xx_nand_info *info = NULL;
	struct nand_chip *chip = NULL;
	struct mtd_info *mtd = NULL;
	int err = 0;

    // use pin as perhipheral
	peripheral_request_list(bfin_nfc_pin_req, DRV_NAME);

    // 1) allocate private data and initialize
	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (info == NULL) {
		dev_err(&pdev->dev, "no memory for flash info\n");
		err = -ENOMEM;
		goto out_err_kzalloc;
	}

    // attach private data to platform device
	platform_set_drvdata(pdev, info);

	spin_lock_init(&info->controller.lock);
	init_waitqueue_head(&info->controller.wq);

	info->device     = &pdev->dev;
	info->platform   = plat;

	// 2) initialise nand_chip data struct 
	chip = &info->chip;

	chip->options |= NAND_BUSWIDTH_16;
	chip->options |= NAND_CACHEPRG | NAND_SKIP_BBTSCAN;

	chip->read_buf = (plat->data_width) ?
		bf5xx_nand_read_buf16 : bf5xx_nand_read_buf;
	chip->write_buf = (plat->data_width) ?
		bf5xx_nand_write_buf16 : bf5xx_nand_write_buf;
	chip->read_byte    = bf5xx_nand_read_byte;
	chip->cmd_ctrl     = bf5xx_nand_hwcontrol;
	chip->dev_ready    = bf5xx_nand_devready;
    if (hardware_ecc) {
        chip->ecc.layout = &bootrom_ecclayout;
        chip->read_buf      = bf5xx_nand_dma_read_buf;
        chip->write_buf     = bf5xx_nand_dma_write_buf;
        chip->ecc.calculate = bf5xx_nand_calculate_ecc;
        chip->ecc.correct   = bf5xx_nand_correct_data;
        chip->ecc.mode      = NAND_ECC_HW;
        chip->ecc.hwctl     = bf5xx_nand_enable_hwecc;
        chip->ecc.read_page_raw = bf5xx_nand_read_page_raw;
        chip->ecc.write_page_raw = bf5xx_nand_write_page_raw;
    } else {
        chip->ecc.mode      = NAND_ECC_SOFT;
    }
    chip->priv	   = &info->mtd;
	chip->controller   = &info->controller;
	chip->IO_ADDR_R    = (void __iomem *) NFC_READ;
	chip->IO_ADDR_W    = (void __iomem *) NFC_DATA_WR;
	chip->chip_delay   = 0;

	// 3) initialise mtd info data struct 
	mtd 		= &info->mtd;
	mtd->priv	= chip;
	mtd->owner	= THIS_MODULE;

	// 4) initialise the hardware 
	err = bf5xx_nand_hw_init(info);


    // 5) Scan nand chips
	/* scan hardware nand chip and setup mtd info data struct 
       This fills out all the uninitialized function pointers with the defaults.
       The flash ID is read and the mtd/chip structures are filled with the
       appropriate values.
    */
    nand_scan(mtd, 1, NULL)


	// 6) register add NAND partition
    struct mtd_partition *parts = info->platform->partitions;
    int nr = info->platform->nr_partitions;
    mtd_device_register(mtd, parts, nr);


	return 0;

}


/* driver device registration 
 * Platform devices can use 2 methods to register
 *  1) use device tree and match device tree compatible to platform driver name
 *  2) use struct resource array, that is refered by platform_device
 *  */
static struct device_driver bf5xx_nand_device {
    .name = DRV_NAME,
    .owner = THIS_MODULE,    
}
/*
 * struct platform_device_id {
 *      char name[PLATFORM_NAME_SIZE];
 *      kernel_ulong_t driver_data;
 *  };
 */

/* if ID table present, platform bus code will scan it every time it has 
 * to find a driver for a new platform device, checking the name
 * Most platform devices just use a name in struct device_driver
 */
static struct platform_driver bf5xx_nand_driver = {
	.probe		= bf5xx_nand_probe,
	//.remove		= bf5xx_nand_remove,
	//.suspend	= bf5xx_nand_suspend,
	//.resume		= bf5xx_nand_resume,
	.driver		= bf5xx_nand_device,  // struct device_driver
    //.id_table = &id,                // const struct platform_device_id *
    //num_resources = 5,              //u32     num_resources;
    //.resource = &r,                 //struct resource *resource;
};


/* 
 * calls platform_driver_register 
 */
module_platform_driver(bf5xx_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_ALIAS("platform:" DRV_NAME);
