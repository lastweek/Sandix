obj-y += memory.o
obj-y += memblock.o
obj-y += early_ioremap.o
obj-y += highmem.o
obj-y += page_alloc.o
obj-y += slab_common.o
obj-$(CONFIG_SPARSEMEM) += sparse.o

ifdef CONFIG_BITMAP_BOOTMEM
  obj-y += bootmem_bitmap.o
else
  obj-y += bootmem_memblk.o
endif
