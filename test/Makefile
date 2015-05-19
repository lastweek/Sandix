#make

core-y := kernel/ mm/nn

n := $(filter-out %/, $(core-y))

all:
	@echo $(obj) $(origin obj)
	@echo $(sort $(core-y))
	@echo $(n)
