# Generate C++ in executable form
VERILATOR_FLAGS += -cc --exe
# Warn abount lint issues; may not want this on less solid designs
# VERILATOR_FLAGS += -Wall
# Make waveforms
VERILATOR_FLAGS += --trace
# build 
VERILATOR_FLAGS += --build -j 0 --Mdir build
# Input files for Verilator
VERILATOR_INPUT = vsrc/top.v csrc/sim_main.cpp -I./vsrc

sim:
	@echo
	@echo "-- VERILATE ----------------"
	verilator $(VERILATOR_FLAGS) $(VERILATOR_INPUT)
	@echo
	@echo "-- RUN ---------------------"
	build/Vtop

	

