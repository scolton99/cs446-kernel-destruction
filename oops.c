#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>

/**
 * Causes a kernel oops by dereferencing the null pointer.
 */
static void cause_oops(void) {
    int* a = (int*) 0;
    printk("%d\n", *a);
}

/**
 * Load the module.
 *
 * Causes a kernel oops by dereferencing the null pointer.
 */
static int __init test_init(void) {
    printk("Loading test module...\n");

    cause_oops();

    return 0;
}

/**
 * Unload the module.
 *
 * This will never run since loading the module causes an oops.
 */
static void __exit test_exit(void) {
    printk("Goodbye World\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Spencer Colton");
MODULE_DESCRIPTION("Destroys the entire kernel");
MODULE_VERSION("1.0");