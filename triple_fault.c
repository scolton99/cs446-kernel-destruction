#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

/**
 * Struct to describe the IDTR register structure.
 *
 * addr: Base address of Interrupt Descriptor Table (8 bytes)
 * limit: Size of Interrupt Descriptor Table (2 bytes)
 */
typedef struct __attribute__((__packed__)) idtr {
    uint16_t limit;
    uint64_t addr;
    uint64_t xs;
} idtr_t;

/**
 * Load the IDTR register into memory.
 *
 * @return A struct idtr with the value of the IDTR register.
 */
static idtr_t* get_idtr(void) {
    idtr_t* idtr = (idtr_t *) kmalloc(sizeof(idtr_t), GFP_KERNEL);
    idtr->xs = 0;
    idtr->limit = 0;
    idtr->addr = 0;
    __asm__("sidt %0" : "=m" (*idtr));

    return idtr;
}

/**
 * Set the IDTR register's value.
 *
 * @param idtr An IDTR struct with base address and limit (xs unused).
 */
static void set_idtr(idtr_t* idtr) {
    __asm__("lidt %0" : : "m" (*idtr));
}

/**
 * Print out an IDTR struct in a friendly way.
 * If xs has been modified, send a warning since this might be a sign of an overflow.
 *
 * @param idtr The IDTR value to be printed.
 */
static void print_idtr(idtr_t* idtr) {
    printk("ADDRESS: %llx, LIMIT: %x", idtr->addr, idtr->limit);
    if (idtr->xs) {
        printk("Warning: extra space in IDTR struct has been modified.");
    }
}

/**
 * Initialize the module.
 *
 * Set the IDTR's limit to 0 to cause a triple fault and an immediate restart.
 */
static int __init test_init(void) {
    idtr_t* idtr;

    printk("Loading test module...\n");

    idtr = get_idtr();

    print_idtr(idtr);

    // IDTR size = 0
    idtr->limit = 0;

    set_idtr(idtr);

    return 0;
}

/**
 * Unload the module.
 *
 * This will never run since loading the module immediately restarts the system.
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