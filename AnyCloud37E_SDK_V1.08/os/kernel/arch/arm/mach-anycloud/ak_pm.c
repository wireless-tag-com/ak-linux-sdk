/*
 * arch/arm/mach-anycloud/pm.c
 *
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/suspend.h>
#include <asm/cacheflush.h>
#include <mach/ak_l2_exebuf.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <asm-generic/gpio.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>


#ifdef CONFIG_WAKELOCK
#include <linux/wakelock.h>
  struct wake_lock pc_wake_lock;
#endif

struct pm_wakeup_info ak_pm_info;


#if 0
static ak_pm_debug_init(void)
{
}
#else
#define ak_pm_debug_init() do { } while(0)
#endif

static int ak_pm_begin(suspend_state_t state)
{
	printk(KERN_INFO"%s ...\n", __func__);
	return 0;
}

static void ak_pm_end(void)
{
	printk(KERN_INFO"%s ...\n", __func__);
	return;
}

static int ak_pm_valid_state(suspend_state_t state)
{
	switch(state) {
		case PM_SUSPEND_ON:
		case PM_SUSPEND_STANDBY:
		case PM_SUSPEND_MEM:
			return 1;
		default:
			return 0;
	}
}

#if 0

void L2_LINK(standby) L2FUNC_NAME(standby)(unsigned long param1,
	unsigned long param2,unsigned long param3, unsigned long param4)
{
	// invalidate and disable mmu
	/* unsigned long value;*/

	DISABLE_CACHE_MMU();

	/* is standby, wait for wakeup by other mode, example prees gpio-key, rtc, ... etc */

	/* enable ICache & DCache, mmu */
	ENABLE_CACHE_MMU();
}

#else

void L2_LINK(standby) L2FUNC_NAME(standby)(unsigned long param1,
	unsigned long param2,unsigned long param3, unsigned long param4)
{
	// invalidate and disable mmu
	unsigned long value;
	
	DISABLE_CACHE_MMU();

	/* close ddr2 auto refresh */
	value = __raw_readl((void __iomem *)0x2100000c);
	value &= ~(0x1<<0);
	__raw_writel(value, (void __iomem *)0x2100000c); 

	/* send precharge command */
	value = __raw_readl((void __iomem *)0x21000010);
	value &= ~(0xFFFFFFFF<<0);
	value |= 0x12A00400;  /* 0x02A00400; */
	__raw_writel(value, (void __iomem *)0x21000010);

	PM_DELAY(2);

	/* enter ram self-refresh £¬ 0x04C00000 */
	value = __raw_readl((void __iomem *)0x21000010);
	value &= ~(0xFFFFFFFF<<0);
	value |= 0x04C00000;
	__raw_writel(value, (void __iomem *)0x21000010);	
	PM_DELAY(3);

	/* close ddr2 */
	value = __raw_readl((void __iomem *)0x21000078);
	value &= ~(0x80000000);
	value |= 0x80000000;
	__raw_writel(value, (void __iomem *)0x21000078);
	__asm__ __volatile__("andeq r0, r0, r0");	 

	/* enter standby mode */
	value = __raw_readl((void __iomem *)0x21800000);
	value |= 0x1;
	__raw_writel(value, (void __iomem *)0x21800000);
	PM_DELAY(0x5000);

	/* is standby, wait for wakeup by other mode, example prees gpio-key, rtc, ... etc */
	/* open ddr2 */
	value = __raw_readl((void __iomem *)0x21000078);
	value |= 0x80000000;
	value &= ~0x80000000;
	__raw_writel(value, (void __iomem *)0x21000078);
	__asm__ __volatile__("andeq r0, r0, r0");

	/* open auto refresh */
	value = __raw_readl((void __iomem *)0x2100000c);
	value |= 0x1;
	__raw_writel(value, (void __iomem *)0x2100000c); 
	__asm__ __volatile__("andeq r0, r0, r0");

	/* NOP, set CLK hight and exit self-refresh, txsnr = 55*2 */
	value = __raw_readl((void __iomem *)0x21000010);
	value |= (0x1<<25);
	__raw_writel(value, (void __iomem *)0x21000010);

	/* send NOP command */
	value = __raw_readl((void __iomem *)0x21000010);
	value &= ~(0xFFFFFFFF<<0);
	value |= 0x02F00000;
	__raw_writel(value, (void __iomem *)0x21000010);

	PM_DELAY(300); 

	/* send NOP command */
	value = __raw_readl((void __iomem *)0x21000010);
	value &= ~(0xFFFFFFFF<<0);
	value |= 0x02F00000;
	__raw_writel(value, (void __iomem *)0x21000010);

	/* NOP , txsrd= 200 tck*/
	PM_DELAY(200);

	/* enable ICache & DCache, mmu */ 
	ENABLE_CACHE_MMU();
} 

#endif



static int ak_pm_enter(suspend_state_t state)
{
#ifdef CONFIG_WAKELOCK
	wake_lock(&pc_wake_lock);
#endif
	printk(KERN_INFO"%s.,line:%d..\n", __func__,__LINE__);
	flush_cache_all();

	/* copy the code that enter standby to l2 memory, then move pc to l2 */
	SPECIFIC_L2BUF_EXEC(standby, 0,0,0,0);

	return 0;
}

static struct platform_suspend_ops ak_pm_ops = {
	.valid = ak_pm_valid_state,
	.begin = ak_pm_begin,
	.enter = ak_pm_enter,
	.end   = ak_pm_end,
};



/**
* @BRIEF		pm proble function
* @AUTHOR		cao_donghua
* @DATE 		2020-02-26
* @PARAM[in] 	*pdev:pointer to platform device.
* @RETURN		int
* @RETVAL		if probe ok,return 0, else return -n
* @NOTES
*/
static int ak_pm_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	int ret = 0;
	int number;
	int index;
	int gpio_index;
	
	number = of_property_count_u32_elems(node, "wakeup-mode");
	if(number < 0){
		printk(KERN_ERR"%s, line:%d, err: no set wakeup-mode!\n", __func__,__LINE__);
		ret = -1;
		return ret;
	}

	ak_pm_info.wakeup_mode_cnt = number;
	for (index = 0; index < number; index++) {
		/* get wakeup mode cnt */
		ret = of_property_read_u32_index(node, "wakeup-mode", index, &ak_pm_info.wakeup_mode[index]);
		if (ret < 0){
			printk(KERN_ERR"%s, line:%d, err: get no wakeup-mode cnt!\n", __func__,__LINE__);
			return ret;
		}

		if (ak_pm_info.wakeup_mode[index] == 0x1){ /* gpio wakeup */	
			/* get wakup gpio mode , gpio config cnt */
			ak_pm_info.wakeup_gpio_cnt = of_gpio_named_count(node, "wakeup-gpio");
			if(ak_pm_info.wakeup_gpio_cnt < 0){
				printk(KERN_ERR"%s, line:%d, err: no set wakeup-gpio!\n", __func__,__LINE__);
				continue;
			}

			/* get wakeup gpio index */
			for (gpio_index = 0; gpio_index < ak_pm_info.wakeup_gpio_cnt; gpio_index++) {
				ret = of_get_named_gpio(node, "wakeup-gpio", gpio_index);
				ak_pm_info.wakeup_gpio_index[gpio_index] = ret;
			}

			/* get wakup gpio edge config cnt,because every gpio must be had one edge config */
			ak_pm_info.wakeup_gpio_edge_cnt = of_property_count_u32_elems(node, "wakeup-gpio-edge");
			if(ak_pm_info.wakeup_gpio_edge_cnt < 0){
				printk(KERN_ERR"%s, line:%d, err: get no wakeup-gpio-edge cnt!\n", __func__,__LINE__);
				continue;
			}

			/* because every gpio must be had one edge config,so gpio index cnt must be equ edge cnt */
			if (ak_pm_info.wakeup_gpio_cnt != ak_pm_info.wakeup_gpio_edge_cnt){
				printk(KERN_ERR"%s, line:%d, err: set wakeup-gpio and wakeup-gpio-edge cnt not equal!\n", __func__,__LINE__);
				continue;
			}

			/* get wakup gpio edge config */
			for (gpio_index = 0; gpio_index < ak_pm_info.wakeup_gpio_edge_cnt; gpio_index++) {
				ret = of_property_read_u32_index(node, "wakeup-gpio-edge", gpio_index, &ak_pm_info.wakeup_gpio_edge[gpio_index]);
				if (ret < 0){
					printk(KERN_ERR"%s, line:%d, err: get no wakeup-gpio-edge!\n", __func__,__LINE__);
					break;
				}
			}			
		}

		if (ak_pm_info.wakeup_mode[index] == 0x3){/* ain0 wakeup */
			ret = of_property_read_u32(node, "wakeup-ain0-edge",&ak_pm_info.wakeup_ain0_edge);
			if (ret < 0){
				ak_pm_info.wakeup_ain0_edge = 0x0;
				printk(KERN_ERR"%s, line:%d, err: get no wakeup_ain0_edge!\n", __func__,__LINE__);
			}
		}
	}

	suspend_set_ops(&ak_pm_ops);

	printk(KERN_ERR"%s , line:%d , pm Driver probe\n", __func__, __LINE__);

	return 0;
}


/**
* @BRIEF		pm remove function
* @AUTHOR		cao_donghua
* @DATE 		2020-02-26
* @PARAM[in] 	*pdev:pointer to platform device.
* @RETURN		int
* @RETVAL		if probe ok,return 0, else return -n
* @NOTES
*/
static int ak_pm_remove(struct platform_device *pdev)
{

	printk(KERN_ERR"%s , line:%d , pm Driver remove \n", __func__, __LINE__);

	return 0;
}


static const struct of_device_id ak_pm_of_ids[] = {
	{ .compatible = "anyka,ak37e-pm" },
	{},
};
MODULE_DEVICE_TABLE(of, ak_pm_of_ids);

static struct platform_driver ak_pm_driver = {
	.driver	= {
		.name    = "ak_pm",
		.of_match_table = of_match_ptr(ak_pm_of_ids),
		.owner	 = THIS_MODULE,
	},
	.probe   = ak_pm_probe,
	.remove  = ak_pm_remove,
};


static int __init ak_pm_init(void)
{
	printk("AK Power Management, (c) 2021 ANYKA\n");
#ifdef CONFIG_WAKELOCK
	wake_lock_init(&pc_wake_lock, WAKE_LOCK_SUSPEND, "pc_wakelock");
#endif
	return platform_driver_register(&ak_pm_driver);
}

module_init(ak_pm_init);


/**
* @BRIEF		pm drv exit function
* @AUTHOR		cao_donghua
* @DATE 		2020-02-26
* @PARAM[in] 	void.
* @RETURN		void
* @RETVAL		none
* @NOTES
*/
static void __exit ak_pm_exit(void)
{
	printk(KERN_ERR"%s , line:%d , pm Driver\n", __func__, __LINE__);
	platform_driver_unregister(&ak_pm_driver);
}

module_exit(ak_pm_exit);

MODULE_DESCRIPTION("Anyka PM driver");
MODULE_AUTHOR("Anyka Microelectronic Ltd.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.00");


