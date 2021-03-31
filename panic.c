#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

const int IRQ_NUMBER = 2;

// Not sure exactly why this works, but for IRQ=2, Vector=0x30 + IRQ=48 + 2=50
const int ASM_INTERRUPT = 50;

/**
 * Causes a kernel oops by dereferencing the null pointer.
 */
static void cause_oops(void) {
    int* a = (int*) 0;
    printk("%d\n", *a);
}

/**
 * Causes a panic by dereferencing a null pointer mid-interrupt.
 *
 * @param irq The IRQ number.
 * @param dev_id The context in which this handler was registered.
 * @return IRQ_HANDLED - The IRQ was successfully handled by default if it reaches the return.
 */
static irqreturn_t cause_panic(int irq, void* dev_id) {
    cause_oops();
    return IRQ_HANDLED;
}

/**
 * Load the module.
 *
 * Causes a kernel panic by specifying an interrupt handler that causes a kernel oops.
 */
static int __init test_init(void) {
    int irq_request_result;

    printk("Loading test module...\n");

    printk("Requesting interrupt %d (0x%x)\n", IRQ_NUMBER, IRQ_NUMBER);
    irq_request_result = request_irq(IRQ_NUMBER, cause_panic, 0, "panic", 0);

    // If we weren't able to get the IRQ, notify the log.
    if (irq_request_result) {
        printk("Interrupt request failed: %d\n", irq_request_result);
        return -1;
    }

    // Cause the interrupt that we got an IRQ for.
    asm("int %0" : : "i" (ASM_INTERRUPT));

    return 0;
}

/**
 * Unload the module.
 *
 * This will never run since loading the module causes a panic.
 */
static void __exit test_exit(void) {
    printk("Goodbye World\n");
    free_irq(IRQ_NUMBER, 0);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Spencer Colton");
MODULE_DESCRIPTION("Destroys the entire kernel");
MODULE_VERSION("1.0");